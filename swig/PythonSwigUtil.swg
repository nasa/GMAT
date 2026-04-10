// Utilities for the Python SWIG wrapper for GMAT
%include "exception.i"
%include "arrays_python.i"

%include "std_string.i"
%include "cpointer.i"
#ifdef SWIG_GMAT
%pointer_class(double, doublep);
%pointer_class(int, intp);
%pointer_class(std::string, stringp);
%pointer_class(bool, boolp);
#else
%header %{
typedef double doublep;
typedef int intp;
typedef std::string stringp;
typedef bool boolp;
%}
#endif

%runtime %{
#define STRING(x) #x
#define TOSTRING(x) STRING(x)
#define MODULE_NAME "gmatpy._py" TOSTRING(PY_MAJOR_VERSION) TOSTRING(PY_MINOR_VERSION) ".gmat_py"
SWIGINTERN void GMAT_PyException(const char *exception, const char *msg) {
  PyObject* pmodule = PyImport_ImportModule(MODULE_NAME);
  PyObject* pclass = PyObject_GetAttrString(pmodule, "APIException");
  Py_DECREF(pmodule);
  PyObject* arglist = Py_BuildValue("(s)", msg);
  PyObject* excep = PyObject_CallObject(pclass, arglist);
  PyErr_SetString((PyObject*)(excep->ob_type), msg);
  Py_DECREF(pclass);
  Py_DECREF(arglist);
  Py_DECREF(excep);
}
%}

// ---------- Define macros below here: ----------

// This macro creates a function to downcast the wrapped object
%define DOWNCAST(DerivedClass, BaseClass)
%exception DerivedClass::SetClass(BaseClass *base) {
  $action
    if (!result) {
      PyErr_SetString(PyExc_RuntimeError, "SetClass exception, unable to set class as a DerivedClass");
      SWIG_fail;
    }
}
%extend DerivedClass {
  static DerivedClass *SetClass(BaseClass *base) {
    return dynamic_cast<DerivedClass *>(base);
  }
};
%enddef


// This macro defines how exceptions are carried over from C++ to Python
%define EXCEPTION()
%exception {
  try {
    $action
  }
  catch(BaseException &e) {
    GMAT_PyException("APIException", e.GetFullMessage().c_str());
    SWIG_fail;
  }
}
%enddef


// This macro defines how to get the size of a returned array
%define ARRAYRETURN(TYPE, SWIGNAME, ClassName, FunctionName, LengthFunctionName)
%typemap(out) TYPE * ClassName::FunctionName
%{
{
int arrSize = (Integer)(arg1)->LengthFunctionName;

if (arrSize < 0) {
   PyErr_SetString(PyExc_IndexError, "Length of array is < 0");
   SWIG_fail;
}

if (!$1 && arrSize != 0) {
   PyErr_SetString(PyExc_RuntimeError, "Invalid array returned");
   SWIG_fail;
}

$result = SWIG_PyArrayOut##SWIGNAME##((TYPE *)$1, arrSize);
}
%}
%enddef

// This macro defines how to get the fixed size of a returned array
%define ARRAYRETURNSIZE(TYPE, SWIGNAME, ClassName, FunctionName, SIZE)
%typemap(out) TYPE * ClassName::FunctionName
%{
if (!$1) {
   PyErr_SetString(PyExc_RuntimeError, "Invalid array returned");
   SWIG_fail;
}

$result = SWIG_PyArrayOut##SWIGNAME##((TYPE *)$1, SIZE);
%}
%enddef


// This macro creates the converters for std::vectors
%define VECTORCONVERT(CTYPE, PYTHONTYPE, SWIGCLASS)

// For some reason, SWIGPY_SLICE_ARG is not included in every PYTHON_wrap file
// Include it here unless that reason can be found and resolved
%{
#include <iostream>

#if PY_VERSION_HEX >= 0x03020000
# define SWIGPY_SLICE_ARG(obj) ((PyObject*) (obj))
#else
# define SWIGPY_SLICE_ARG(obj) ((PySliceObject*) (obj))
#endif
%}


%template(SWIGCLASS) std::vector<CTYPE>;

%enddef
