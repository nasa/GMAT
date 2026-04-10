// Utilities for the Java SWIG wrapper for GMAT
%include "exception.i"
%include "enumsimple.swg"
%include "arrays_java.i"

%include "std_string.i"
%include "cpointer.i"
#ifdef SWIG_GMAT
%pointer_class(double, Realp);
%pointer_class(int, Integerp);
%pointer_class(std::string, stringp);
%pointer_class(bool, boolp);
#else
%header %{
typedef double Realp;
typedef int Integerp;
typedef std::string stringp;
typedef bool boolp;
%}
#endif

%runtime %{
SWIGINTERN void GMAT_JavaException(JNIEnv *jenv, const char *exception, const char *msg) {
  jclass excep;
  jenv->ExceptionClear();
  excep = jenv->FindClass(exception);
  if (excep)
    jenv->ThrowNew(excep, msg);
}
%}

// ---------- Define macros below here: ----------

// This macro creates a function to downcast the wrapped object
%define DOWNCAST(DerivedClass, BaseClass)
%exception DerivedClass::SetClass(BaseClass *base) {
  $action
    if (!result) {
      jclass excep = jenv->FindClass("java/lang/ClassCastException");
      if (excep) {
        jenv->ThrowNew(excep, "SetClass exception, unable to set class as a DerivedClass");
      }
    }
}
%extend DerivedClass {
  static DerivedClass *SetClass(BaseClass *base) {
    return dynamic_cast<DerivedClass *>(base);
  }
};
%enddef


// This macro defines how exceptions are carried over from C++ to Java
%define EXCEPTION()
%exception {
  try {
    $action
  }
  catch(BaseException &e) {
    GMAT_JavaException(jenv, "gmat/APIException", e.GetFullMessage().c_str());
    return $null;
  }
}
%enddef


// This macro defines how to get the size of a returned array
%define ARRAYRETURN(TYPE, SWIGNAME, ClassName, FunctionName, LengthFunctionName)
%typemap(out) TYPE * ClassName::FunctionName
%{
int arrSize = (Integer)(arg1)->LengthFunctionName;

if (arrSize < 0) {
   SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException,
                          "Length of array is < 0");
   return $null;
}

if (!$1 && arrSize != 0) {
   SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException,
                          "Invalid array returned");
   return $null;
}

$result = SWIG_JavaArrayOut##SWIGNAME##(jenv, (TYPE *)$1, arrSize);
%}
%enddef

// This macro defines how to get the fixed size of a returned array
%define ARRAYRETURNSIZE(TYPE, SWIGNAME, ClassName, FunctionName, SIZE)
%typemap(out) TYPE * ClassName::FunctionName
%{
if (!$1) {
   SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException,
                          "Invalid array returned");
   return $null;
}

$result = SWIG_JavaArrayOut##SWIGNAME##(jenv, (TYPE *)$1, SIZE);
%}
%enddef


// This macro creates the converters for std::vectors
%define VECTORCONVERT(CTYPE, JAVATYPE, SWIGCLASS)

%template(SWIGCLASS) std::vector<CTYPE>;

%extend std::vector<CTYPE> {
#if defined(SWIG)
%proxycode %{
  // Functions added from SWIG proxycode below:

  public JAVATYPE[] ToArray() {
    int size = (int) size();
    JAVATYPE[] arr = new JAVATYPE[size];

    for (int ii = 0; ii < size; ii++) {
      arr[ii] = get(ii);
    }

    return arr;
  }
%}
#endif
}
%enddef
