#include "tcni.h"
#include "tcvm.h"
#include <ffi.h>
#include <dlfcn.h>

ffi_type getffiType(Context context, TCObject * arg) {
    TCClass c = (TCClass)OBJ_CLASS(arg);

    if(areClassesCompatible (context, c, "java.lang.String") == COMPATIBLE) {
        return ffi_type_pointer;
    }
    if(areClassesCompatible (context, c, "java.lang.Integer") == COMPATIBLE) {
        return ffi_type_sint32;
    }
    if(areClassesCompatible (context, c, "java.lang.Double") == COMPATIBLE) {
        return ffi_type_double;
    }
    if(areClassesCompatible (context, c, "java.lang.Float") == COMPATIBLE) { 
        return ffi_type_float;
    }

    if(areClassesCompatible (context, c, "java.lang.Long") == COMPATIBLE) { 
        return ffi_type_float;
    }
    
    return ffi_type_void;
}



TC_API void tnTCNI_invokeMethod_sscO (NMParams p) {

    // for static calls args begin from 0
    char * module = String2CharP(p->obj[0]);
    char * method = String2CharP(p->obj[1]);
    char* className = p->obj[2] == NULL ? NULL :String2CharP(p->obj[2]);
    void* handle;
    handle = htGetPtr(&htLoadedLibraries, hashCode(module));
    printf("lib addres: %d", handle);
    // TODO implement for null 
    TCObject *tcArgs = ARRAYOBJ_START(p->obj[3]);
    int argArrLen = ARRAYLEN(tcArgs);
    ffi_type **argTypes;
    argTypes = xmalloc((argArrLen)* sizeof(ffi_type *));
    void **args;
    args = xmalloc((argArrLen + 1) * sizeof(void *));

    for (size_t i = 0; i < argArrLen; i++)
    {
        TCObject o = tcArgs[i];
        ffi_type argType = getffiType(p->currentContext, o);
        
        if(argType.type == ffi_type_pointer.type) {
            argTypes[i] = &ffi_type_pointer;
            char *strArg = String2CharP(o);
            printf("\n");
            args[i] = &strArg;
        }
        if(argType.type == ffi_type_float.type) {
            argTypes[i] = &ffi_type_float;
            float *fValue = xmalloc(sizeof(float));
            (*fValue) = Float_v(o); 
            args[i] = fValue;
        }
        if(argType.type == ffi_type_double.type) {
            argTypes[i] = &ffi_type_double;
            double *dValue = xmalloc(sizeof(double));
            (*dValue) = Double_v(o);
            args[i] = dValue;

        }
        if(argType.type == ffi_type_sint.type) {
            argTypes[i] = &ffi_type_sint;
            int *iValue = xmalloc(sizeof(int));
            (*iValue) = Integer_v(o);
            args[i] = iValue;
        }
    }
    
    void* add_data_fn = dlsym(handle, method);
    char* err = dlerror();
    if (err) {
        fprintf(stderr, "dlsym failed: %s\n", err);
        exit(1);
    }
    
    // Describe the interface of add_data to libffi.
    

    if(className == NULL) {
        ffi_cif cif;
        ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argArrLen, &ffi_type_void, argTypes);
        ffi_call(&cif, FFI_FN(add_data_fn), NULL, args);
        p->retO = NULL;
    }
    else if(strEq(className, "java/lang/Integer")) {
        ffi_cif cif;
        ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argArrLen, &ffi_type_sint, argTypes);
        if (status != FFI_OK) {
            fprintf(stderr, "ffi_prep_cif failed: %d\n", status);
            exit(1);
        }
        TCObject o = createObject(p->currentContext, "java.lang.Integer");
        ffi_call(&cif, FFI_FN(add_data_fn), &Integer_v(o), args);
        p->retO = o;
    }
    else if(strEq(className, "java/lang/Double")) {
        ffi_cif cif;
        ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argArrLen, &ffi_type_double, argTypes);
        TCObject o = createObject(p->currentContext, "java.lang.Double");
        ffi_call(&cif, FFI_FN(add_data_fn), &Double_v(o), args);
        p->retO = o;
    }
    else if(strEq(className, "java/lang/String")) {
        ffi_cif cif;
        ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argArrLen, &ffi_type_pointer, argTypes);
        char * strRet; 
        ffi_call(&cif, FFI_FN(add_data_fn), &strRet, args);
        int len = xstrlen(strRet);
        TCObject ret = createStringObjectFromCharP(p->currentContext, strRet, len);
        p->retO = ret;
        setObjectLock(ret, UNLOCKED);
    }
    else if(strEq(className, "java/lang/Float")) {
        ffi_cif cif;
        ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argArrLen, &ffi_type_float, argTypes);
        float * f = xmalloc(sizeof(float));
        TCObject o = createObject(p->currentContext, "java.lang.Float");
        ffi_call(&cif, FFI_FN(add_data_fn), f, args);
        Float_v(o) = *f;
        p->retO = o;    
    }
}