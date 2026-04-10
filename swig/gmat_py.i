%module gmat_py

#define SWIG_GMAT
%include "PythonSwigUtil.swg"

// ArrayTemplate and TableTemplate have to be handled manually instead of
// through the macro due to implementation differences, mainly the required
// cast and for TableTemplate, the multiplication of the table dimensions
%typemap(out) double * ArrayTemplate<Real>::GetDataVector()
%{{
int arrSize = (Integer)((ArrayTemplate< Real > const *) arg1)->GetSize();

if (arrSize < 0) {
   PyErr_SetString(PyExc_IndexError, "Length of array is < 0");
   SWIG_fail;
}

$result = SWIG_PyArrayOutDouble($1, arrSize);
}%}

%typemap(out) double * TableTemplate<Real>::GetDataVector()
%{{
int rowSize = (Integer)((TableTemplate< Real > const *) arg1)->GetNumRows();
int colSize = (Integer)((TableTemplate< Real > const *) arg1)->GetNumColumns();
int arrSize = rowSize*colSize;

if (arrSize < 0) {
   PyErr_SetString(PyExc_IndexError, "Length of array is < 0");
   SWIG_fail;
}

$result = SWIG_PyArrayOutDouble($1, arrSize);
}%}



// Set the GetInternalObject to downcast the returned pointer
%typemap(out) Component * Moderator::GetInternalObject {
    const std::string lookup_typename = *arg2 + " *";
    swig_type_info * const outtype = SWIG_TypeQuery(lookup_typename.c_str());
    $result = SWIG_NewPointerObj(SWIG_as_voidptr($1), outtype, $owner);
}

// Use Python style for ToString()
%rename(__str__) *::ToString;

%rename(__getitem__) *::operator();
%rename(__getitem__) *::operator[];

// Add support for slices for ArrayTemplate<Real> and TableTemplate<Real>
%feature("shadow") ArrayTemplate<Real>::__setitem__(Integer index, const Real& value) %{
def __setitem__(self, index, value):
    if isinstance(index, slice):
        index_list = list(range(self.GetSize())[index])
        out = []
        for ii in range(len(index_list)):
            out.append(_gmat_py.ArrayTemplateReal___setitem__(self, index_list[ii], value[ii]))
        return out
    elif isinstance(index, list):
        out = []
        for ii in range(len(index)):
            out.append(_gmat_py.ArrayTemplateReal___setitem__(self, index[ii], value[ii]))
        return out
    else:
        if index < 0:
            index += self.GetSize()
        return _gmat_py.ArrayTemplateReal___setitem__(self, index, value)
%}

%feature("shadow") ArrayTemplate<Real>::__getitem__(Integer index) const %{
def __getitem__(self, index):
    if isinstance(index, slice): # Handle slice indexing
        index_list = list(range(self.GetSize())[index])
        return [_gmat_py.ArrayTemplateReal___getitem__(self, idx) for idx in index_list]
    elif isinstance(index, list):
        return [_gmat_py.ArrayTemplateReal___getitem__(self, idx) for idx in index]
    else:
        if index < 0: # Handle negative index
            index += self.GetSize()
        return _gmat_py.ArrayTemplateReal___getitem__(self, index)
%}

%feature("shadow") TableTemplate<Real>::__setitem__(Integer indices[2], const Real& value) %{
def __setitem__(self, index, value):
    if isinstance(index, tuple): # Handle tuple indexing
        r, c = index
        return self.__setitem__([r,c], value)

    if isinstance(index[0], slice): # Handle slice indexing
        row_list = list(range(self.GetNumRows())[index[0]])
    elif isinstance(index[0], list): # Handle list indexing
        row_list = index[0]
    else:
        if index[0] < 0: # Handle negative row index
            index[0] += self.GetNumRows()
        row_list = [index[0]]

    if isinstance(index[1], slice): # Handle slice indexing
        col_list = list(range(self.GetNumColumns())[index[1]])
    else:
        if index[1] < 0: # Handle negative column index
            index[1] += self.GetNumColumns()
        col_list = [index[1]]

    if not isinstance(value, list): # Handle scalar value
        value = [[value]]
    elif isinstance(index[1], list): # Handle list indexing
        col_list = index[1]
    elif not isinstance(value[0], list): # Handle 1D list
        if len(col_list) == 1:
            value = [[val] for val in value]
        else:
            value = [value]

    out = []
    for r_idx in range(len(row_list)):
        r = row_list[r_idx]
        col_out = []
        for c_idx in range(len(col_list)):
            c = col_list[c_idx]
            col_out.append(_gmat_py.TableTemplateReal___setitem__(self, [r, c], value[r_idx][c_idx]))
        out.append(col_out)

    # Flatten output if of size 1
    if len(out[0]) == 1:
        out = [l[0] for l in out]

    if len(out) == 1:
        out = out[0]

    return out
%}

%feature("shadow") TableTemplate<Real>::__getitem__(Integer indices[2]) const %{
def __getitem__(self, index):
    if isinstance(index, tuple): # Handle tuple indexing
        r, c = index
        return self[[r,c]]

    if not isinstance(index, list):
        raise TypeError("Expected a row and column to be specified")
    if len(index) > 2:
        raise TypeError("Expected a row and column to be specified")

    if isinstance(index[0], slice): # Handle slice indexing
        row_list = list(range(self.GetNumRows())[index[0]])
    elif isinstance(index[0], list): # Handle list indexing
        row_list = index[0]
    else:
        if index[0] < 0: # Handle negative row index
            index[0] += self.GetNumRows()
        row_list = [index[0]]

    if isinstance(index[1], slice): # Handle slice indexing
        col_list = list(range(self.GetNumColumns())[index[1]])
    elif isinstance(index[1], list): # Handle list indexing
        col_list = index[1]
    else:
        if index[1] < 0: # Handle negative column index
            index[1] += self.GetNumColumns()
        col_list = [index[1]]

    out = []
    for r in row_list:
        col_out = []
        for c in col_list:
            col_out.append(_gmat_py.TableTemplateReal___getitem__(self, [r, c]))
        out.append(col_out)

    # Flatten output if of size 1
    if len(out[0]) == 1:
        out = [l[0] for l in out]

    if len(out) == 1:
        out = out[0]

    return out
%}

// Ignore nested struct (Warning 325) even though they are protected?
%ignore CCSDSEMSegment::EpochAndData;
%ignore Code500EphemerisFile::EphemHeader1;
%ignore Code500EphemerisFile::EphemHeader2;
%ignore Code500EphemerisFile::EphemData;
%ignore Code500EphemerisFile::DoubleByteType;
%ignore Code500EphemerisFile::DoubleUnionType;
%ignore Code500EphemerisFile::IntByteType;
%ignore Code500EphemerisFile::IntUnionType;
%ignore ODEModel::StateStructure;
%ignore ODEModel::SolveForData;
%ignore PlanetaryEphem::dcb_type;
%ignore DeFile::stateData;
%ignore DeFile::recOneData;
%ignore DeFile::recTwoData;
%ignore DeFile::headerOne;
%ignore DeFile::headerTwo;
%ignore Publisher::DataType;

// Flatten nested structs that are needed
%feature("flatnested") SolarFluxReader::FluxDataCSSI;
%rename(SolarFluxReader_FluxDataCSSI) SolarFluxReader::FluxDataCSSI;
%feature("flatnested") SolarFluxReader::FluxData;
%rename(SolarFluxReader_FluxData) SolarFluxReader::FluxData;

// Rename friend functions
%rename(Rmatrix_add) operator+(Real, const Rmatrix&);
%rename(Rmatrix_sub) operator-(Real, const Rmatrix&);
%rename(Rmatrix_mul) operator*(Real, const Rmatrix&);
%rename(Rmatrix_div) operator/(Real, const Rmatrix&);
%rename(Rmatrix33_mul) operator*(Real, const Rmatrix33&);
%rename(Rmatrix66_mul) operator*(Real, const Rmatrix66&);
%rename(Rvector_mul) operator*(Real, const Rvector&);
%rename(Rvector3_mul) operator*(Real, const Rvector3&);

%pythonappend GmatBase::Help %{
   print(val)
   val = ""
%}

%pythonappend ::Construct %{
   typestr = val.GetTypeName()
   if (typestr == "ForceModel"):
      typestr = "ODEModel"
   
   evalstr = typestr + ".SetClass(val)"
   try:
      val = eval(evalstr)
   except:
      pass
      
   val.thisown = 0
%}


%pythonappend ::Copy %{
   typestr = val.GetTypeName()
   if (typestr == "ForceModel"):
      typestr = "ODEModel"
   
   evalstr = typestr + ".SetClass(val)"
   try:
      val = eval(evalstr)
   except:
      pass
      
   val.thisown = 0
%}


%pythonappend ::GetObject %{
   typestr = val.GetTypeName()
   if (typestr == "ForceModel"):
      typestr = "ODEModel"
   
   evalstr = typestr + ".SetClass(val)"
   try:
      val = eval(evalstr)
   except:
      pass
%}


%exceptionclass BaseException;

%include "gmat.swg"

%pythoncode %{
   def Help(foritem = ""):
      helpstr = _gmat_py.HelpSystem_Instance().Help(foritem);
      print(helpstr)
      
   def GetRunSummary():
      summary = _gmat_py.GetRunSummary();
      print(summary)
      
   def ShowObjects(ofType = ""):
      olist = _gmat_py.ShowObjects(ofType);
      print(olist)
      
   def ShowClasses(ofType = ""):
      olist = _gmat_py.ShowClasses(ofType);
      print(olist)
%}
