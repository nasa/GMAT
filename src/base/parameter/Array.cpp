//$Id$
//------------------------------------------------------------------------------
//                                  Array
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/20
//
/**
 * Implements Array class which handles one or two dimensional array.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Array.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"          // for SeparateBy(), GetArrayIndexVar()
#include "GmatGlobal.hpp"          // for IsWritingGmatKeyword()
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_ARRAY 1
//#define DEBUG_ARRAY_GET 1
//#define DEBUG_ARRAY_SET 1
//#define DEBUG_INITIAL_VALUE 1

//---------------------------------
// static data
//---------------------------------
const std::string
Array::PARAMETER_TEXT[ArrayParamCount - ParameterParamCount] =
{
   "NumRows",
   "NumCols",
   "RmatValue",
   "SingleValue",
   "RowValue",
   "ColValue",
   "InitialValue",
   "InitialValueType",
}; 

const Gmat::ParameterType
Array::PARAMETER_TYPE[ArrayParamCount - ParameterParamCount] =
{
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::RMATRIX_TYPE,
   Gmat::REAL_ELEMENT_TYPE,  // was Gmat::REAL_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Array(const std::string &name = "", const std::string &desc = "",
//       const std::string &unit = "")
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> parameter name
 * @param <desc> parameter description
 * @param <unit> parameter unit
 */
//------------------------------------------------------------------------------
Array::Array(const std::string &name, const std::string &desc,
             const std::string &unit)
   : Parameter(name, "Array", GmatParam::USER_PARAM, NULL, desc, unit,
               GmatParam::NO_DEP, Gmat::UNKNOWN_OBJECT, false, false, false, true)
{
   mNumRows = 0;
   mNumCols = 0;
   mInitialValueType = 1;
   mSizeSet = false;
   
   // GmatBase data
   objectTypes.push_back(Gmat::ARRAY);
   objectTypeNames.push_back("Array");
   blockCommandModeAssignment = false;
   mReturnType = Gmat::RMATRIX_TYPE;
   parameterCount = ArrayParamCount;
}


//------------------------------------------------------------------------------
// Array(const Array &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
Array::Array(const Array &copy)
   : Parameter(copy)
{
   // We don't want to change the name when copy
   std::string thisName = GetName();
   
   mNumRows = copy.mNumRows;
   mNumCols = copy.mNumCols;
   mInitialValueType = copy.mInitialValueType;
   mSizeSet = copy.mSizeSet;
   mRmatValue = copy.mRmatValue;
   
   SetName(thisName);
}


//------------------------------------------------------------------------------
// Array& operator= (const Array& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
Array& Array::operator= (const Array& right)
{
   if (this != &right)
   {
      // We don't want to change the name when copy
      std::string thisName = GetName();
      
      Parameter::operator=(right);
      mNumRows = right.mNumRows;
      mNumCols = right.mNumCols;
      mInitialValueType = right.mInitialValueType;
      mSizeSet = right.mSizeSet;
      mRmatValue = right.mRmatValue;
      
      SetName(thisName);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~Array()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Array::~Array()
{
}


//------------------------------------------------------------------------------
// bool operator==(const Array &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool Array::operator==(const Array &right) const
{
   return Parameter::operator==(right);
}


//------------------------------------------------------------------------------
// bool operator!=(const Array &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool Array::operator!=(const Array &right) const
{
   return Parameter::operator!=(right);
}


//------------------------------------------------------------------------------
// bool SetSize(const Integer row, const Integer col, bool zeroElements = true)
//------------------------------------------------------------------------------
bool Array::SetSize(const Integer row, const Integer col, bool zeroElements)
{
   #if DEBUG_ARRAY
   MessageInterface::ShowMessage
      ("Array::SetSize() row=%d, col=%d\n", row, col);
   #endif

   mNumRows = row;
   mNumCols = col;
   mRmatValue.SetSize(mNumRows, mNumCols, zeroElements);
   mSizeSet = true;
   return true;
}


//------------------------------------
// methods inherited from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// void SetRmatrix(const Rmatrix &mat)
//------------------------------------------------------------------------------
/**
 * Sets Rmatrix value of parameter.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
void Array::SetRmatrix(const Rmatrix &mat)
{
   if (mSizeSet)
   {
      mRmatValue = mat;
   }
   else
   {
      throw ParameterException("The size has not been set for " + GetName());
   }
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return parameter value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string Array::ToString()
{
   // use default global precision to convert to string (loj: 2008.03.05)
   return mRmatValue.ToString(false, false, false, GmatGlobal::DATA_PRECISION, 1,
                              false, 1, "", false);
   
}


//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Array::GetParameterList() const
{
   return PARAMETER_TEXT;
}


//------------------------------------
// methods inherited from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* Array::Clone() const
{
   return new Array(*this);
}


//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
void Array::Copy(const GmatBase* orig)
{
   operator=(*((Array *)(orig)));
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Array::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < ArrayParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return Parameter::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Array::GetParameterID(const std::string &str) const
{
   for (int i=ParameterParamCount; i<ArrayParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return Parameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Array::GetParameterType(const Integer id) const
{
   if (id >= ParameterParamCount && id < ArrayParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
   else
      return Parameter::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Array::GetParameterTypeString(const Integer id) const
{
   if (id >= ParameterParamCount && id < ArrayParamCount)
      return PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return Parameter::GetParameterTypeString(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Array::IsParameterReadOnly(const Integer id) const
{
   //loj: 11/22/06 read only parameters were wrong
   //if ((id == NUM_ROWS) || (id == NUM_COLS) || (id == SINGLE_VALUE))
   //   return true;
   
   if ((id == NUM_ROWS) || (id == NUM_COLS) || (id == RMAT_VALUE) ||
       (id == ROW_VALUE) || (id == COL_VALUE) || (id == INITIAL_VALUE) ||
       (id == INITIAL_VALUE_TYPE))
      return true;
   
   return Parameter::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterCloaked(const Integer id) const
//---------------------------------------------------------------------------
/**
* Checks to see if the requested parameter is cloaked
*
* @param <id> Description for the parameter.
*
* @return true if the parameter is cloaked
*/
//---------------------------------------------------------------------------
bool Array::IsParameterCloaked(const Integer id) const
{
   if (id == OBJECT)
      return true;

   return Parameter::IsParameterCloaked(id);
}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool Array::IsParameterCommandModeSettable(const Integer id) const
{
   // For now, turn them all on
   return true;
}



//----- Integer parameter

//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer Array::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case NUM_ROWS:
      return mNumRows;
   case NUM_COLS:
      return mNumCols;
   case INITIAL_VALUE_TYPE:
      return mInitialValueType;
   default:
      return Parameter::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Array::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 *
 * @exception <ParameterException> thrown if Row or Column already has been set.
 */
//------------------------------------------------------------------------------
Integer Array::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case NUM_ROWS:
      if (mNumRows == 0)
         mNumRows = value;
      else
         throw ParameterException("Row has already been set for " + GetName());

      if (mNumCols > 0 && !mSizeSet)
      {
         mRmatValue.SetSize(mNumRows, mNumCols);
         mSizeSet = true;
      }
      return mNumRows;
   case NUM_COLS:
      if (mNumCols == 0)
         mNumCols = value;
      else
         throw ParameterException("Column has already been set for " + GetName());
      
      if (mNumRows > 0 && !mSizeSet)
      {
         mRmatValue.SetSize(mNumRows, mNumCols);
         mSizeSet = true;
      }
      return mNumCols;
   case INITIAL_VALUE_TYPE:
      mInitialValueType = value;
      return mInitialValueType;
   default:
      return Parameter::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Array::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//----- Rvector parameter

//------------------------------------------------------------------------------
// Rvector GetRvectorParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves column or row vector from current matrix. Note that index starts
 * from 0.
 */
//------------------------------------------------------------------------------
Rvector Array::GetRvectorParameter(const Integer id, const Integer index) const
{
   #ifdef DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("Array::GetRvectorParameter() entered, id=%d, index=%d, mNumCols=%d, "
       "mNumRows=%d\n", id, index, mNumCols, mNumRows);
   MessageInterface::ShowMessage
      ("   mRmatValue.GetNumColumns()=%d, mRmatValue.GetNumRows()=%d\n   "
       "mRmatValue.ToString()=%s", mRmatValue.GetNumColumns(),
       mRmatValue.GetNumRows(), mRmatValue.ToString().c_str());
   #endif
   
   switch (id)
   {
   case ROW_VALUE:
      {
         Rvector rvec(mNumCols);
      
         for (int i=0; i<mNumCols; i++)
            rvec.SetElement(i, mRmatValue.GetElement(index, i));

         #ifdef DEBUG_ARRAY_GET
         MessageInterface::ShowMessage
            ("Array::GetRvectorParameter() returning %s\n", rvec.ToString().c_str());
         #endif
         return rvec;
      }
   case COL_VALUE:
      {
         Rvector rvec(mNumRows);
         
         for (int i=0; i<mNumRows; i++)
            rvec.SetElement(i, mRmatValue.GetElement(i, index));
         
         #ifdef DEBUG_ARRAY_GET
         MessageInterface::ShowMessage
            ("Array::GetRvectorParameter() returning %s\n", rvec.ToString().c_str());
         #endif
         return rvec;
      }
   default:
      throw ParameterException
         ("Array::GetRvectorParameter() Unknown Parameter Name " + PARAMETER_TEXT[id] + "\"");
      //return Parameter::GetRvectorParameter(id, index);
   }
}

//------------------------------------------------------------------------------
// Rvector GetRvectorParameter(const std::string &label,
//                                    const Integer index) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Rvector Array::GetRvectorParameter(const std::string &label,
                                   const Integer index) const
{
   return GetRvectorParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const Integer id, const Rvector &value,
//                                    const Integer index)
//------------------------------------------------------------------------------
const Rvector& Array::SetRvectorParameter(const Integer id, const Rvector &value,
                                          const Integer index)
{
   
   #if DEBUG_ARRAY
   MessageInterface::ShowMessage
      ("Array::SetRvectorParameter() index=%d, mNumRows=%d, mNumCols=%d\n",
       index, mNumRows, mNumCols);
   #endif
   
   switch (id)
   {
   case ROW_VALUE:
      if (value.GetSize() < mNumCols)
      {
         std::string errmsg = "Error setting Array ";
         errmsg += instanceName + "  from input Rvector: insufficient size.\n";
         throw ParameterException(errmsg);
      }
      for (int i=0; i<mNumCols; i++)
         mRmatValue.SetElement(index, i, value(i));
      
      return value;
   case COL_VALUE:
      if (value.GetSize() < mNumRows)
      {
         std::string errmsg = "Error setting Array ";
         errmsg += instanceName + "  from input Rvector: insufficient size.\n";
         throw ParameterException(errmsg);
      }
      for (int i=0; i<mNumRows; i++)
         mRmatValue.SetElement(i, index, value(i));
      
      return value;
   default:
      throw ParameterException
         ("Array::GetRvectorParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
   }
}

//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const std:string &label,
//                                    const Rvector &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& Array::SetRvectorParameter(const std::string &label,
                                          const Rvector &value,
                                          const Integer index)
{
   return SetRvectorParameter(GetParameterID(label), value, index);
}

//----- Rmatrix parameter

//------------------------------------------------------------------------------
// const Rmatrix& GetRmatrixParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 *
 * @exception <ParameterException> thrown if matrix size has not been set.
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::GetRmatrixParameter(const Integer id) const
{
   if (mSizeSet)
   {
      switch (id)
      {
      case RMAT_VALUE:
         return mRmatValue;
      default:
         return Parameter::GetRmatrixParameter(id);
      }
   }
   else
   {
      throw ParameterException("The size has not been set for " + GetName());
   }
}


//------------------------------------------------------------------------------
// const Rmatrix& GetRmatrixParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::GetRmatrixParameter(const std::string &label) const
{
   return GetRmatrixParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const Rmatrix& SetRmatrixParameter(const Integer id,
//                                    const Rmatrix &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 *
 * @exception <ParameterException> thrown if matrix size has not been set.
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::SetRmatrixParameter(const Integer id,
                                          const Rmatrix &value)
{
   switch (id)
   {
   case RMAT_VALUE:
      SetRmatrix(value);
      return value;
   default:
      return Parameter::SetRmatrixParameter(id, value);
   }
   
}


//------------------------------------------------------------------------------
// const Rmatrix& SetRmatrixParameter(const std::string &label,
//                                    const Rmatrix &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::SetRmatrixParameter(const std::string &label,
                                          const Rmatrix &value)
{
   return SetRmatrixParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index)
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const Integer id, const Integer index) const
{
   switch (id)
   {
   case SINGLE_VALUE:
      return mRmatValue.GetElement(0, index);
   default:
      throw ParameterException
         ("Array::GetRealParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
   }
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const std::string &label, const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//                       const Integer col) const
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const Integer id, const Integer row,
                             const Integer col) const
{
   #if DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("Array::GetRealParameter() entered, id=%d, row=%d, col=%d\n", id,
       row, col);
   #endif
   
   switch (id)
   {
   case SINGLE_VALUE:
      return mRmatValue.GetElement(row, col);
   default:
      throw ParameterException
         ("Array::GetRealParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
      //return Parameter::GetRealParameter(id, row, col);
   }
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer row,
//                       const Integer col) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const std::string &label, const Integer row,
                             const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer row,
//                       const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets value of array element row and col.
 *
 * @param id     The Id of the parameter
 * @param value  The real value of the array element
 * @param row    The row of the array element (starts from 0)
 * @param col    The column of the array element (starts form 0)
 */
//------------------------------------------------------------------------------
Real Array::SetRealParameter(const Integer id, const Real value,
                             const Integer row, const Integer col)
{
   switch (id)
   {
   case SINGLE_VALUE:
      #ifdef DEBUG_ARRAY_SET
         MessageInterface::ShowMessage(
         "In Array::SetRealParameter, row = %d, col = %d, value = %.12f\n",
         row, col, value);
      #endif
      mRmatValue.SetElement(row, col, value);
      return value;
   default:
      throw ParameterException
         ("Array::SetRealParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
      //return Parameter::SetRealParameter(id, value, row, col);
   }
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value,
//                       const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Array::SetRealParameter(const std::string &label, const Real value,
                             const Integer row, const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/*
 * Override this method to return array declaration string.
 * Such as Arr1[2]
 */
//------------------------------------------------------------------------------
std::string Array::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case DESCRIPTION:
      return GetArrayDefString();
   case INITIAL_VALUE:
      // Use const_cast to remove the const,
      // since GetInitialValueString() is no longer a const method
      return (const_cast<Array*>(this))->GetInitialValueString();
   default:
      return Parameter::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Array::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Array::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_ARRAY_SET
   MessageInterface::ShowMessage
      ("Array::SetStringParameter() entered, id=%d, value='%s'\n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case INITIAL_VALUE:
      {
         // Sets initial value of array elements (LOJ: 2010.09.21)
         // value should be in format of Arr(I,J)=Value, so it can be parsed
         StringArray parts = GmatStringUtil::SeparateBy(value, "=", true);
         if (parts.size() == 2)
         {
            std::string name, rowStr, colStr;
            Integer row, col;
            // parse array name and index
            GmatStringUtil::GetArrayIndex(parts[0], rowStr, colStr, row, col, name);
            std::string mapstr = rowStr + "," + colStr;
            #ifdef DEBUG_ARRAY_SET
            MessageInterface::ShowMessage
               ("   mapstr='%s', value=%s, row=%d, col=%d\n",
                mapstr.c_str(), parts[1].c_str(), row, col);
            #endif
            initialValueMap[mapstr] = parts[1];
            
            // Save initial value
            Real rval;
            #ifdef DEBUG_ARRAY_SET
            MessageInterface::ShowMessage
               ("   mNumRows=%d, mNumCols=%d, mSizeSet=%d\n", mNumRows, mNumCols, mSizeSet);
            #endif
            if ((GmatStringUtil::ToReal(parts[1], rval)) && (row != -1 && col != -1))
            {
               #ifdef DEBUG_ARRAY_SET
               MessageInterface::ShowMessage("   Setting %s to %f\n", parts[0].c_str(), rval);
               #endif
               mRmatValue(row, col) = rval;
            }
         }
         #ifdef DEBUG_ARRAY_SET
         MessageInterface::ShowMessage("Array::SetStringParameter() returning true\n");
         #endif
         return true;
      }
   default:
      return Parameter::SetStringParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool Array::SetStringParameter(const std::string &label, const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool Array::SetStringParameter(const std::string &label, const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// StringArray GetGeneratingString(Gmat::WriteMode mode,
//                const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 * 
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 * 
 * @return A string containing the text.
 */
//------------------------------------------------------------------------------
const std::string& Array::GetGeneratingString(Gmat::WriteMode mode,
                                              const std::string &prefix,
                                              const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage("Array::GetGeneratingString() entered\n");
   #endif
   
   std::stringstream data;
   bool writeGmatKeyword = GmatGlobal::Instance()->IsWritingGmatKeyword();
   
   // Crank up data precision so we don't lose anything
   data.precision(GetDataPrecision()); 
   std::string preface = "", nomme;
   
   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
       (mode == Gmat::SHOW_SCRIPT))
      inMatlabMode = false;
   if (mode == Gmat::MATLAB_STRUCT)
      inMatlabMode = true;
   
   if (useName != "")
      nomme = useName;
   else
      nomme = instanceName;
   
   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::SHOW_SCRIPT) || 
      (mode == Gmat::NO_COMMENTS) || (mode == Gmat::OBJECT_EXPORT))
   {
      std::string tname = typeName;

      // Add comment line (loj: 03/27/07)
      if ((mode != Gmat::NO_COMMENTS) && (mode != Gmat::OBJECT_EXPORT) && (GetCommentLine() != ""))
         data << GetCommentLine();
      
      data << "Create " << tname << " " << nomme 
           << "[" << mNumRows << "," << mNumCols << "];\n";
      
      // We now write out GMAT prefix on option from the startup file (see GMT-3233)
      if (writeGmatKeyword)
         preface = "GMAT ";
      else
         preface = "";
   }
   
   nomme += ".";
   
   if (mode == Gmat::OWNED_OBJECT)
   {
      preface = prefix;
      nomme = "";
   }
   
   preface += nomme;
   
   WriteParameters(mode, preface, data);
   generatingString = data.str();
   
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Array::GetGeneratingString() return\n%s\n", generatingString.c_str());
   #endif
   
   return generatingString;
}


//------------------------------------------------------------------------------
// std::string GetArrayDefString() const
//------------------------------------------------------------------------------
/*
 * Returns array declaration string.
 * Such as Arr1[2]
 */
//------------------------------------------------------------------------------
std::string Array::GetArrayDefString() const
{
   std::stringstream data;
   data << instanceName << "[" << mNumRows << "," << mNumCols << "]";
   return data.str();
}


//------------------------------------------------------------------------------
// std::string GetInitialValueString(const std::string &prefix)
//------------------------------------------------------------------------------
/*
 * Returns array initial value string including inline comments.
 *
 * Such as GMAT Arr1[1,1] = 13.34; %% initialize
 */
//------------------------------------------------------------------------------
std::string Array::GetInitialValueString(const std::string &prefix)
{
   #ifdef DEBUG_INITIAL_VALUE
   MessageInterface::ShowMessage
      ("Array::GetInitialValueString() '%s' entered\n", GetName().c_str());
   #endif
   
   std::stringstream data;
   bool writeGmatKeyword = GmatGlobal::Instance()->IsWritingGmatKeyword();
   
   Integer row = GetIntegerParameter("NumRows");
   Integer col = GetIntegerParameter("NumCols");
   
   for (Integer i = 0; i < row; ++i)
   {
      //loj: Do not write if value is zero since default is zero(03/27/07)
      for (Integer j = 0; j < col; ++j)
      {
         Real realVal = GetRealParameter(SINGLE_VALUE, i, j);
         #ifdef DEBUG_INITIAL_VALUE
         MessageInterface::ShowMessage("   value = %f\n", realVal);
         #endif
         
         if (realVal != 0.0)
         {
            //========================================================
            #ifndef __WRITE_INITIAL_VALUE_STRING__
            //========================================================
            
            // This writes out actual value
            // We now write out GMAT prefix on option from the startup file (see GMT-3233)
            if (writeGmatKeyword)
               data << "GMAT " << instanceName << "(" << i+1 << ", " << j+1 <<
                  ") = " <<  GmatStringUtil::ToString(realVal, 16, false, 1) << ";";
            else
               data << instanceName << "(" << i+1 << ", " << j+1 <<
                  ") = " <<  GmatStringUtil::ToString(realVal, 16, false, 1) << ";";
            data << GetInlineComment() + "\n";
            
            //========================================================
            #else
            //========================================================
            
            // This writes out initial value string (LOJ: 2010.09.21)
            std::string mapstr = GmatStringUtil::ToString(i+1, 1) + "," +
               GmatStringUtil::ToString(j+1, 1);
            
            #ifdef DEBUG_INITIAL_VALUE
            MessageInterface::ShowMessage
               ("Array::GetInitialValueString() mapstr='%s'\n", mapstr.c_str());
            #endif
            
            std::string initialVal = "No Initial Value";
            bool writeData = false;
            
            if (initialValueMap.find(mapstr) != initialValueMap.end())
               initialVal = initialValueMap[mapstr];
            
            #ifdef DEBUG_INITIAL_VALUE
            MessageInterface::ShowMessage("   initialVal='%s'\n", initialVal.c_str());
            #endif
            
            if (GmatStringUtil::IsNumber(initialVal))
               if (mInitialValueType == 1)
                  writeData = true;
               else
                  writeData = false;
            else
               if (mInitialValueType == 1)
                  writeData = false;
               else
                  writeData = true;
            
            if (writeData)
            {
               // We now write out GMAT prefix on option from the startup file (see GMT-3233)
               if (writeGmatKeyword)
                  data << prefix << "GMAT " << instanceName << "(" << i+1 << ", " << j+1 << ") = " << initialVal;
               else
                  data << prefix << instanceName << "(" << i+1 << ", " << j+1 << ") = " << initialVal;
               data << GetInlineComment() + "\n";
            }
            
            //========================================================
            #endif
            //========================================================
         }
      }
   }
   
   return data.str();
}

