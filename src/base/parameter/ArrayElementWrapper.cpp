//$Id$
//------------------------------------------------------------------------------
//                                  ArrayElementWrapper
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number NNG04CC06P
//
// Author: Wendy C. Shoan
// Created: 2007.01.18
//
/**
 * Implementation of the ArrayElementWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ArrayElementWrapper.hpp"
#include "ParameterException.hpp"
#include "RealUtilities.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_AE_WRAPPER

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ArrayElementWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs ArrayElementWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
ArrayElementWrapper::ArrayElementWrapper() :
   ElementWrapper(),
   array         (NULL),
   row           (NULL),
   column        (NULL),
   arrayName     (""),
   rowName       (""),
   columnName    ("")
{
   wrapperType = Gmat::ARRAY_ELEMENT_WT;
}

//---------------------------------------------------------------------------
//  ArrayElementWrapper(const ArrayElementWrapper &aew);
//---------------------------------------------------------------------------
/**
 * Constructs base ArrayElementWrapper structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param <aew>  ArrayElementWrapper instance to copy to create "this" 
 * instance.
 */
//---------------------------------------------------------------------------
ArrayElementWrapper::ArrayElementWrapper(const ArrayElementWrapper &aew) :
   ElementWrapper(aew),
   array         (NULL),
   arrayName     (aew.arrayName),
   rowName       (aew.rowName),
   columnName    (aew.columnName)
{
   if (aew.row != NULL)
      row = aew.row->Clone();
   else
      row          = NULL;

   if (aew.column != NULL)
      column = aew.column->Clone();
   else
      column       = NULL;

   SetupWrapper();
}

//---------------------------------------------------------------------------
//  ArrayElementWrapper& operator=(const ArrayElementWrapper &aew)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ArrayElementWrapper structures.
 *
 * @param <aew> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ArrayElementWrapper& ArrayElementWrapper::operator=(
                             const ArrayElementWrapper &aew)
{
   if (&aew == this)
      return *this;

   ElementWrapper::operator=(aew);
   array        = NULL;  //

   if (aew.row != NULL)
      row = aew.row->Clone();
   else
      row          = NULL;

   if (aew.column != NULL)
      column = aew.column->Clone();
   else
      column       = NULL;

   arrayName    = aew.arrayName;
   rowName      = aew.rowName;
   columnName   = aew.columnName;

   SetupWrapper();

   return *this;
}

//---------------------------------------------------------------------------
//  ~ArrayElementWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ArrayElementWrapper::~ArrayElementWrapper()
{
   if (row)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (row, row->GetDescription(), "ArrayElementWrapper::~ArrayElementWrapper()",
          "deleting row");
      #endif
      delete row;
   }
   
   if (column)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (column, column->GetDescription(), "ArrayElementWrapper::~ArrayElementWrapper()",
          "deleting column");
      #endif
      delete column;
   }
}


//------------------------------------------------------------------------------
//  ElementWrapper* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new object that l.ooks like this one.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
ElementWrapper* ArrayElementWrapper::Clone() const
{
   return new ArrayElementWrapper(*this);
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return ArrayElementWrapper value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string ArrayElementWrapper::ToString()
{
   Real rval = EvaluateReal();
   return GmatStringUtil::ToString(rval);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the ArrayElementWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ArrayElementWrapper::GetDataType() const
{
   return Gmat::REAL_TYPE;
}

//------------------------------------------------------------------------------
//  const StringArray&  GetRefObjectNames() 
//------------------------------------------------------------------------------
/**
 * This method returns the list of reference object names for the ElementWrapper 
 * object.
 *
 * @return list of reference object names for the object.
 *
 */
//------------------------------------------------------------------------------
const StringArray& ArrayElementWrapper::GetRefObjectNames()
{
   refObjectNames.clear();
   // start with the array name ...
   refObjectNames.push_back(arrayName);
   
   // ... then add the reference object names from the row
   // (don't worry about duplicates for now)
   if (row)
   {
      StringArray rowRefNames = row->GetRefObjectNames();
      for (StringArray::const_iterator i = rowRefNames.begin();
           i != rowRefNames.end(); ++i)
             refObjectNames.push_back(*i);
   }

   // ... then add the reference object names from the column
   // (don't worry about duplicates for now)
   if (column)
   {
      StringArray colRefNames = column->GetRefObjectNames();
      for (StringArray::const_iterator j = colRefNames.begin();
           j != colRefNames.end(); ++j)
             refObjectNames.push_back(*j);
   }

   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage("AEWrapper:: Returning ref object names:\n");
      for (Integer ii = 0; ii < (Integer) refObjectNames.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", refObjectNames[ii].c_str());
   #endif
   
   return refObjectNames;
}

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * This method retrives a reference object for the wrapper name
 * 
 * @param <name> name of the wrapper
 *
 * @return reference for success; NULL if name not found
 *
 */
//------------------------------------------------------------------------------
GmatBase* ArrayElementWrapper::GetRefObject(const std::string &name)
{
   return array;
}

//---------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (ObjectProperty) pointer on the wrapped 
 * object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::SetRefObject(GmatBase *obj)
{
   bool isOk   = false;
   bool setRow = false;
   bool setCol = false;
   if ( (obj->IsOfType("Array")) && (obj->GetName() == arrayName) )
   {
      array = (Array*) obj;
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage("AEWrapper:: Setting array object %s\n",
            arrayName.c_str());
      #endif
      isOk = true;
   }
   if (row)    setRow = row->SetRefObject(obj);
   if (column) setCol = column->SetRefObject(obj);
   
   if (!isOk && !setRow && !setCol) return false;
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper:: Returning true from SetRefObject\n");
   #endif
   
   return true;
}

//---------------------------------------------------------------------------
//  bool RenameObject(const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Method to rename a reference object for the wrapper.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::RenameObject(const std::string &oldName, 
                                       const std::string &newName)
{
   if ((row == NULL) || (column == NULL))
   {
      throw ParameterException("Cannot rename ArrayElement - row or "
            "column element is NULL\n");
   }

   ElementWrapper::RenameObject(oldName, newName);
   // now rebuild the description string from the refObjectNames
   // first, rename things in the row and column wrappers
   row->RenameObject(oldName, newName);
   column->RenameObject(oldName, newName);
   rowName     = row->GetDescription();
   columnName  = column->GetDescription();
   // then put it all back together
   arrayName   = refObjectNames[0];
   description = arrayName + "(";
   description += rowName + ",";
   description += columnName + ")";
   return true;
}

//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped object.
 *
 * @return Real value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
Real ArrayElementWrapper::EvaluateReal() const
{
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::EvaluateReal called on array %s\n", 
         arrayName.c_str());
   #endif
   if (array == NULL)
      throw ParameterException(
      "Cannot return value of ArrayElement - object pointer is NULL\n");
   if ((row == NULL) || (column == NULL))
   {
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage("row = <%p>, column = <%p>\n", row,
               column);
      #endif

      throw ParameterException("Cannot return value of ArrayElement - row or "
            "column element is NULL\n");
   }
   Real itsValue;
   Integer rowInt    = -99;
   Integer columnInt = -99;
   try
   {
      // get the row value
      Real rowVal        = row->EvaluateReal();
      Real rowNearestInt = GmatMathUtil::NearestInt(rowVal);
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage(
            "AEWrapper::EvalReal(%s) - row evaluates to %d\n", 
            arrayName.c_str(),(Integer) rowNearestInt);
      #endif
      if ((GmatMathUtil::Mod(rowVal, rowNearestInt) != 0.0))
      {
         std::string errmsg =  "Cannot evaluate ArrayElement - ";
         errmsg += "row Element evaluates to a non-Integer value\n";
         throw ParameterException(errmsg);
      }
      rowInt = (Integer) rowNearestInt - 1;
      // get the column value
      Real colVal        = column->EvaluateReal();
      Real colNearestInt = GmatMathUtil::NearestInt(colVal);
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage(
            "AEWrapper::EvalReal(%s) - column evaluates to %d\n", 
            arrayName.c_str(),(Integer) colNearestInt);
      #endif
      if ((GmatMathUtil::Mod(colVal, colNearestInt) != 0.0))
      {
         std::string errmsg = "Cannot evaluate ArrayElement - ";
         errmsg += "column Element evaluates to a non-Integer value\n";
         throw ParameterException(errmsg);
      }
      columnInt = (Integer) colNearestInt - 1;
      
      itsValue = array->GetRealParameter("SingleValue", rowInt, columnInt);
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage(
            "AEWrapper::EvalReal(%s) - itsValue evaluates to %.12f\n", 
            arrayName.c_str(),itsValue);
      #endif
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot return Real value for array " + 
                            array->GetName(); 
      errmsg += " with row " + std::to_string(rowInt);
      errmsg += " and column " + std::to_string(columnInt);
      errmsg += " - exception thrown: " + be.GetFullMessage();
      throw ParameterException(errmsg);
   }
         
   return itsValue;
}
   
//---------------------------------------------------------------------------
//  bool SetReal(const Real toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::SetReal(const Real toValue)
{
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::SetReal called on array %s with input %.12f\n", 
         arrayName.c_str(), toValue);
   #endif
   if (array == NULL)
      throw ParameterException(
      "Cannot set value of ArrayElement - object pointer is NULL\n");
   if ((row == NULL) || (column == NULL))
      throw ParameterException(
      "Cannot return value of ArrayElement - row or column element is NULL\n");
   Integer rowInt    = -99;
   Integer columnInt = -99;

   // get the row value
   Real rowVal        = row->EvaluateReal();
   Real rowNearestInt = GmatMathUtil::NearestInt(rowVal);
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::SetReal(%s) - rowVal and rowNearestInt evaluate to %d and %d\n",
         arrayName.c_str(), (Integer) rowVal, (Integer) rowNearestInt);
   #endif
   if (rowNearestInt == 0)
   {
      std::string errmsg = "Cannot evaluate ArrayElement - ";
      errmsg += "row Element evaluates to zero\n";
      throw ParameterException(errmsg);
   }
   if ((GmatMathUtil::Mod(rowVal, rowNearestInt) != 0.0))
   {
      std::string errmsg = "Cannot evaluate ArrayElement - ";
      errmsg += "row Element evaluates to a non-Integer value\n";
      throw ParameterException(errmsg);
   }
   rowInt = (Integer) rowNearestInt - 1;
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::SetReal(%s) - and then rowInt evaluates to %d\n",
         arrayName.c_str(),(Integer) rowInt);
   #endif
   // get the column value
   Real colVal        = column->EvaluateReal();
   Real colNearestInt = GmatMathUtil::NearestInt(colVal);
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::SetReal(%s) - colVal and colNearestInt evaluate to %d and %dn",
         arrayName.c_str(), (Integer) colVal, (Integer) colNearestInt);
   #endif
   if (colNearestInt == 0)
   {
      std::string errmsg = "Cannot evaluate ArrayElement - ";
      errmsg += "column Element evaluates to zero\n";
      throw ParameterException(errmsg);
   }
   if ((GmatMathUtil::Mod(colVal, colNearestInt) != 0.0))
   {
      std::string errmsg = "Cannot evaluate ArrayElement - ";
      errmsg += "column Element evaluates to a non-Integer value\n";
      throw ParameterException(errmsg);
   }
   columnInt = (Integer) colNearestInt - 1;
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::SetReal(%s) - and then columnInt evaluates to %d\n",
         arrayName.c_str(),(Integer) columnInt);
   #endif
   try
   {

      array->SetRealParameter("SingleValue", toValue, rowInt, columnInt);
   }
   catch (BaseException &be)
   {
      std::stringstream errmsg("");
      errmsg << "Cannot set Real value for array " << array->GetName();
      errmsg << " with row " << rowInt << " and column " << columnInt;
      errmsg << " - exception thrown: " << be.GetFullMessage() << std::endl;
      throw ParameterException(errmsg.str());
   }
         
   return true;
}

std::string ArrayElementWrapper::GetRowName()
{
   return rowName;
}

std::string ArrayElementWrapper::GetColumnName()
{
   return columnName;
}

//------------------------------------------------------------------------------
// ElementWrapper* GetRowWrapper()
//------------------------------------------------------------------------------
ElementWrapper* ArrayElementWrapper::GetRowWrapper()
{
   return row;
}

//------------------------------------------------------------------------------
// ElementWrapper* GetColumnWrapper()
//------------------------------------------------------------------------------
ElementWrapper* ArrayElementWrapper::GetColumnWrapper()
{
   return column;
}

bool ArrayElementWrapper::SetRow(ElementWrapper* toWrapper)
{
   row = toWrapper;
   return true;
}

bool ArrayElementWrapper::SetColumn(ElementWrapper* toWrapper)
{
   column = toWrapper;
   return true;
}

//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Array Element Wrapper.
 */
//---------------------------------------------------------------------------
void ArrayElementWrapper::SetupWrapper()
{
   try
   {
      GmatStringUtil::GetArrayIndexVar(description, rowName, columnName,
                                       arrayName);
   }
   catch (UtilityException &)
   {
      std::string errmsg = "Unable to set up ArrayElementWrapper \"" +
                           description;
      errmsg += "\" - does not parse correctly as an array.\n";
      throw ParameterException(errmsg);
   }
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::SetupWrapper for array named '%s' \n", arrayName.c_str());
      MessageInterface::ShowMessage("   description = %s\n", description.c_str());
      MessageInterface::ShowMessage("   rowName     = %s\n", rowName.c_str());
      MessageInterface::ShowMessage("   columnName  = %s\n", columnName.c_str());
      MessageInterface::ShowMessage("   arrayName   = %s\n", arrayName.c_str());
   #endif
   // for now, put the array name in the list of reference objects - add
   // all the other stuff when GetRefObjectNames is called
   refObjectNames.push_back(arrayName);
}
