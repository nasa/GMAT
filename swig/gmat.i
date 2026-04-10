%module gmat

#define SWIG_GMAT
%include "JavaSwigUtil.swg"

// ArrayTemplate and TableTemplate have to be handled manually instead of
// through the macro due to implementation differences, mainly the required
// cast and for TableTemplate, the multiplication of the table dimensions
%typemap(out) double * ArrayTemplate<Real>::GetDataVector()
%{
int arrSize = (Integer)((ArrayTemplate< Real > const *) arg1)->GetSize();

if (arrSize < 0) {
   SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException,
                          "Length of array is < 0");
   return $null;
}

$result = SWIG_JavaArrayOutDouble(jenv, (double *)$1, arrSize);
%}

%typemap(out) double * TableTemplate<Real>::GetDataVector()
%{
int rowSize = (Integer)((TableTemplate< Real > const *) arg1)->GetNumRows();
int colSize = (Integer)((TableTemplate< Real > const *) arg1)->GetNumColumns();
int arrSize = rowSize*colSize;

if (arrSize < 0) {
   SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException,
                          "Length of array is < 0");
   return $null;
}

$result = SWIG_JavaArrayOutDouble(jenv, (double *)$1, arrSize);
%}


// Add function to set the ownership of the pointer
%typemap(javacode) SWIGTYPE %{
  public void setSwigOwnership(boolean swigOwnedFlag) {
    swigCMemOwn = swigOwnedFlag;
  }

  public boolean getSwigOwnership() {
    return swigCMemOwn;
  }
%}

// Use Java style for ToString()
%rename(toString) *::ToString;

// Rename operators that can't be overridden in Java
// following Python naming convention
%rename(lt) operator<;
%rename(le) operator<=;
%rename(eq) operator==;
%rename(ne) operator!=;
%rename(ge) operator>=;
%rename(gt) operator>;

%rename(add) operator+;
%rename(sub) operator-;
%rename(mul) operator*;
%rename(div) operator/;
%rename(iadd) operator+=;
%rename(isub) operator-=;
%rename(imul) operator*=;
%rename(idiv) operator/=;

%rename(getitem) operator();
%rename(getitem) operator[];

%rename(rshift) operator>>;
%rename(lshift) operator<<;

%typemap(javabase) BaseException "java.lang.Exception";
%rename(getMessage) BaseException::GetFullMessage;

%include "gmat.swg"
