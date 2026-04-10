// Custom implementation of arrays_java.i for Python lists

// Array support functions declarations macro
%define PYTHON_ARRAYS_DECL(CTYPE, PYCTYPE, PYTYPE, PYFUNCNAME)
%{
static int SWIG_PyArrayIn##PYFUNCNAME (CTYPE **carr, PyObject *input);
static void SWIG_PyArrayArgout##PYFUNCNAME (CTYPE *carr, PyObject *input);
static PyObject* SWIG_PyArrayOut##PYFUNCNAME (CTYPE *result, int sz);
%}
%enddef

// Array support functions macro
%define PYTHON_ARRAYS_IMPL(CTYPE, PYCTYPE, PYTYPE, PYFUNCNAME)
%{
/* CTYPE[] support */
static int SWIG_PyArrayIn##PYFUNCNAME (CTYPE **carr, PyObject *input) {
  int i;
  int sz;
  if (!input) {
    PyErr_SetString(PyExc_ValueError, "null array");
    return 0;
  }
  sz = PyList_Size(input);
  *carr = new CTYPE[sz];
  if (!*carr) {
    PyErr_SetString(PyExc_MemoryError, "array memory allocation failed");
    return 0;
  }
  for (i=0; i<sz; i++)
    (*carr)[i] = Py##PYTYPE##_As##PYCTYPE##(PyList_GetItem(input, i));
  return 1;
}

static void SWIG_PyArrayArgout##PYFUNCNAME (CTYPE *carr, PyObject *input) {
  int i;
  int sz = PyList_Size(input);
  for (i=0; i<sz; i++)
    PyList_SetItem(input, i, Py##PYTYPE##_From##PYCTYPE##(carr[i]));
}

static PyObject* SWIG_PyArrayOut##PYFUNCNAME (CTYPE *result, int sz) {
  int i;
  PyObject *pyresult = PyList_New(sz);
  if (!pyresult)
    return NULL;
  for (i=0; i<sz; i++)
    PyList_SetItem(pyresult, i, Py##PYTYPE##_From##PYCTYPE##(result[i]));
  return pyresult;
}
%}
%enddef


PYTHON_ARRAYS_DECL(int, Long, Long, Int)                 /* int[] */
PYTHON_ARRAYS_DECL(double, Double, Float, Double)     /* double[] */

PYTHON_ARRAYS_IMPL(int, Long, Long, Int)                 /* int[] */
PYTHON_ARRAYS_IMPL(double, Double, Float, Double)     /* double[] */


// Arrays of primitive types use the following macro. The array typemaps use support functions.
%define PYTHON_ARRAYS_TYPEMAPS(CTYPE, PYCTYPE, PYTYPE, PYFUNCNAME)

%typemap(in) CTYPE[]
%{  if (!SWIG_PyArrayIn##PYFUNCNAME((CTYPE **)&$1, $input)) return NULL; %}
%typemap(in) CTYPE[ANY]
%{  if ($input && PyList_Size($input) != $1_size) {
    PyErr_SetString(PyExc_IndexError, "incorrect array size");
    return NULL;
  }
  if (!SWIG_PyArrayIn##PYFUNCNAME((CTYPE **)&$1, $input)) return NULL; %}
%typemap(argout) CTYPE[ANY], CTYPE[] 
%{ SWIG_PyArrayArgout##PYFUNCNAME((CTYPE *)$1, $input); %}
%typemap(out) CTYPE[ANY]
%{$result = SWIG_PyArrayOut##PYFUNCNAME((CTYPE *)$1, $1_dim0); %}
%typemap(out) CTYPE[] 
%{$result = SWIG_PyArrayOut##PYFUNCNAME((CTYPE *)$1, FillMeInAsSizeCannotBeDeterminedAutomatically); %}
%enddef


PYTHON_ARRAYS_TYPEMAPS(int, Long, Long, Int)                 /* int[ANY] */
PYTHON_ARRAYS_TYPEMAPS(double, Double, Float, Double)     /* double[ANY] */


// Add typechecks for checking that the inputs are valid lists containing the coorrect types
%typecheck(SWIG_TYPECHECK_INT32_ARRAY) int*, int[] {
  $1 = PyList_Check($input) ? 1 : 0;

  if ($1) {
     int size = PyList_Size($input);
     for (int ii = 0; ii < size; ii++) {
        $1 = PyLong_Check(PyList_GetItem($input, ii)) ? $1 : 0;
     }
  }
}

%typecheck(SWIG_TYPECHECK_DOUBLE_ARRAY) double*, double[] {
  $1 = PyList_Check($input) ? 1 : 0;

  if ($1) {
     int size = PyList_Size($input);
     for (int ii = 0; ii < size; ii++) {
        $1 = PyFloat_Check(PyList_GetItem($input, ii)) ? $1 : 0;
     }
  }
}
