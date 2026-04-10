//$Id$
//------------------------------------------------------------------------------
//                         ClassName
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/ /
//
/**
 * File description here.
 */
//------------------------------------------------------------------------------


#include "Covariance.hpp"
#include "GmatBase.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CONSTRUCTION
//#define DEBUG_ACCESS
//#define DEBUG_INCREASING_ELEMENT_SIZE

Covariance::Covariance(GmatBase *owner) :
   covarianceOwner   (owner),
   subCovariance     (NULL),
   theCovariance     (NULL),
   dimension         (0)
{
   theCovariance = new Rmatrix();
}

Covariance::~Covariance()
{
   if (subCovariance != NULL)
      delete subCovariance;
   if (theCovariance != NULL)
      delete theCovariance;
}


Covariance::Covariance(const Covariance &cov) :
   covarianceOwner   (cov.covarianceOwner),
   subCovariance     (NULL),
   elementNames      (cov.elementNames),
   elementIDs        (cov.elementIDs),
   elementSizes      (cov.elementSizes),
   elementOwners     (cov.elementOwners),
   dimension         (cov.dimension)
{
   //if (cov.theCovariance.IsSized())
   //   theCovariance = cov.theCovariance;

   theCovariance = new Rmatrix();
   if (cov.theCovariance->IsSized())
      *theCovariance     = *(cov.theCovariance);
}


Covariance& Covariance::operator=(const Covariance &cov)
{
   if (&cov != this)
   {
      covarianceOwner   = cov.covarianceOwner;

      //if (cov.theCovariance.IsSized())
      //   theCovariance     = cov.theCovariance;

      if (cov.theCovariance->IsSized())
      {
         if (theCovariance != NULL)
            delete theCovariance;
         theCovariance = new Rmatrix();
         *theCovariance = *(cov.theCovariance);
      }

      elementNames      = cov.elementNames;
      elementIDs        = cov.elementIDs;
      elementSizes      = cov.elementSizes;
      elementOwners     = cov.elementOwners;

      dimension         = cov.dimension;
   }

   return *this;
}


Real& Covariance::operator()(const Integer r, const Integer c)
{
   if ((r < 0 ) || (r >= dimension))
      throw GmatBaseException("Covariance row index out of bounds");

   if ((c < 0 ) || (c >= dimension))
      throw GmatBaseException("Covariance column index out of bounds");

   //return theCovariance(r,c);
   return (*theCovariance)(r, c);
}


Real Covariance::operator()(const Integer r, const Integer c) const
{
   if ((r < 0 ) || (r >= dimension))
      throw GmatBaseException("Covariance row index out of bounds");

   if ((c < 0 ) || (c >= dimension))
      throw GmatBaseException("Covariance column index out of bounds");

   //return theCovariance(r,c);
   return (*theCovariance)(r,c);
}


void Covariance::AddCovarianceElement(const std::string &name,
      GmatBase* owner)
{
   Integer parmID;
   Integer covSize;

   //size_t pos = name.find_last_of('.');
   //if (pos != name.npos)
   //{
   //   std::string childObjFullName = owner->GetName() + "." + name.substr(0, pos);
   //   std::string paramName = name.substr(pos + 1);
   //   GmatBase* childObj = owner->GetRefObject(Gmat::UNKNOWN_OBJECT, childObjFullName);
   //   if (childObj == NULL)
   //      throw GmatBaseException("Error: Object with name '" + childObjFullName + "' was not defined in script.");

   //   parmID = childObj->GetParameterID(paramName);
   //   covSize = childObj->HasParameterCovariances(parmID);
   //}
   //else
   //{
      parmID = owner->GetParameterID(name);
      covSize = owner->HasParameterCovariances(parmID);
   //}

   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Adding covariance element %s with id %d "
            "to object named %s\n", name.c_str(), owner->GetParameterID(name),
            owner->GetName().c_str());
   #endif

   if (covSize > 0)
   {
      // Check to see if element already exists for this object; if not, add it
      Integer index = -1;
      for (UnsignedInt i = 0; i < elementNames.size(); ++i)
      {
         if (name == elementNames[i])
         {
            // if (elementOwners[i] == owner)
            // It needs to compare 2 objects by name in order to avoid cloned object
            if (elementOwners[i]->GetName() == owner->GetName())
            {
               index = i;
               break;
            }
         }
      }

      if (index == -1)
      {
         elementNames.push_back(name);
         elementIDs.push_back(parmID);
         elementSizes.push_back(covSize);
         elementOwners.push_back(owner);
         dimension += covSize;

         // It needs to set dimension for Rmatrix before using it
         //if (theCovariance.GetNumRows() != 0)
         if ((*theCovariance).GetNumRows() != 0)
         {
            //Rmatrix temp = theCovariance;
            //theCovariance.SetSize(dimension, dimension, true);
            Rmatrix temp = *theCovariance;
            (*theCovariance).SetSize(dimension, dimension, true);
            for (Integer row = 0; row < temp.GetNumRows(); ++row)
            {
               for (Integer col = 0; col < temp.GetNumColumns(); ++col)
               {
                  //theCovariance(row, col) = temp(row, col);
                  (*theCovariance)(row, col) = temp(row, col);
               }
            }
         }
         else
         {
            //theCovariance.SetSize(dimension, dimension, true);
            (*theCovariance).SetSize(dimension, dimension, true);
         }
      }
   }
   else
      throw GmatBaseException("Covariance handling for " + name +
            " is not implemented");

   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Covariance dimension is now %d\n",
            dimension);
   #endif
}

bool Covariance::ConstructLHS(const std::string& lhs)
{
   bool retval = false;

   if (lhs != "")
   {

   }

   PrepareMatrix();

   return retval;
}


bool Covariance::ConstructRHS(const std::string& rhs)
{
   bool retval = false;

   // Parse the input string here

   return retval;
}

bool Covariance::ConstructRHS(Rvector diagonal, Integer start)
{
   bool retval = false;

   if (diagonal.IsSized() == false)
      throw GmatBaseException("Diagonal covariance vector is not properly "
            "initialized");

   Integer length = diagonal.GetSize();
   if (start + length > dimension)
      throw GmatBaseException("Diagonal covariance vector is will not fit in "
            "the allocated covariance matrix");

   length += start;
   for (Integer i = start; i < length; ++i)
      for (Integer j = start; j < length; ++j)
         if (i == j)
         {
            //theCovariance(i, j) = diagonal(i - start);
            (*theCovariance)(i, j) = diagonal(i - start);
         }
         else
         {
            //theCovariance(i, j) = 0.0;
            (*theCovariance)(i, j) = 0.0;
         }
   return retval;
}


bool Covariance::ConstructRHS(Rmatrix data, Integer start)
{
   bool retval = false;

   if (data.IsSized() == false)
      throw GmatBaseException("Input covariance matrix is not properly "
            "initialized");

   Integer length = data.GetNumRows();
   if (data.GetNumColumns() != length)
      throw GmatBaseException("Input covariance matrix is not square");

   if (start + length > dimension)
      throw GmatBaseException("Input covariance matrix is will not fit in "
            "the allocated covariance matrix");

   for (Integer i = 0; i < length; ++i)
      for (Integer j = i; j < length; ++j)
         if (i == j)
         {
            //theCovariance(i + start, j + start) = data(i, j);
            (*theCovariance)(i + start, j + start) = data(i, j);
         }
         else
         {
            // Symmetrize as we go
            //theCovariance(i+start, j+start) = theCovariance(j+start, i+start)
            //                                = (data(i, j) + data(j, i)) / 2.0;
            (*theCovariance)(i + start, j + start) = (*theCovariance)(j + start, i + start)
               = (data(i, j) + data(j, i)) / 2.0;
         }

   return retval;
}


bool Covariance::FillMatrix(const Rmatrix& rhs, bool overrideAndFill)
{
   bool retval = true;

   // Check sizes of the matrices
   if (!overrideAndFill && (dimension != rhs.GetNumRows()))
      throw GmatBaseException("Covariance assignment dimensions do not match");
   if (rhs.GetNumRows() != rhs.GetNumColumns())
      throw GmatBaseException("Input covariance matrix is not square");

   // Fill in the matrix info
   if (dimension != rhs.GetNumRows())
   {
      // Must be in override and fill mode; names, indices, sizes & owners are
      // all invalid; clear & set as unknown
      elementNames.clear();
      elementIDs.clear();
      elementSizes.clear();
      elementOwners.clear();

      elementNames.push_back("GenericCovariance");
      elementIDs.push_back(-1);
      // set dimension to the input size
      dimension = rhs.GetNumRows();
      elementSizes.push_back(dimension);
      elementOwners.push_back(NULL);
   }
   //theCovariance = rhs;
   (*theCovariance) = rhs;

   return retval;
}

void Covariance::SetDimension(Integer size)
{
   dimension = size;
   PrepareMatrix();
}


//---------------------------------------------------------------------------
// Integer GetDimension()
//---------------------------------------------------------------------------
/**
* Get dimension of covariance matrix
*
* @return     Dimension of covariance matrix
*/
//---------------------------------------------------------------------------
Integer Covariance::GetDimension()
{
   return dimension;
}


//---------------------------------------------------------------------------
// Rmatrix* GetCovariance()
//---------------------------------------------------------------------------
/**
* Get covariance matrix
*
* @return     Covariance matrix
*/
//---------------------------------------------------------------------------
Rmatrix *Covariance::GetCovariance()
{
   //return &theCovariance;
   return theCovariance;
}


//--------------------------------------------------------------------------
// Integer GetElementIndex(const std::string elementName) const
//--------------------------------------------------------------------------
/**
* This function is used to get the index associated to an element
*
* @param elementName    name of a given element 
*
* @return               index of the element when it found. 
*                       return -1 for case the element is not found.
*/
//--------------------------------------------------------------------------
Integer Covariance::GetElementIndex(const std::string elementName) const
{
   Integer index = -1;
   for (Integer i = 0; i < elementNames.size(); ++i)
   {
      if (elementNames[i] == elementName)
      {
         index = i;
      }
   }

   return index;
}


//--------------------------------------------------------------------------
// Integer GetElementIndex(const Integer forParameterID) const
//--------------------------------------------------------------------------
/**
* This function is used to get the index associated to an element
*
* @param forParameterID    parameter ID of the element
*
* @return               index of the element when it found.
*                       return -1 for case the element is not found.
*/
//--------------------------------------------------------------------------
Integer Covariance::GetElementIndex(const Integer forParameterID) const
{
   Integer index = -1;
   for (Integer i = 0; i < elementIDs.size(); ++i)
   {
      if (elementIDs[i] == forParameterID)
      {
         index = i;
      }
   }

   return index;
}


//--------------------------------------------------------------------------
// Integer GetElementID(const std::string elementName) const
//--------------------------------------------------------------------------
/**
* This function is used to get parameterID associated to an element
*
* @param elementName    name of a given element
*
* @return               parameterID of the element when it found.
*                       return -1 for case the element is not found.
*/
//--------------------------------------------------------------------------
Integer Covariance::GetElementID(const std::string elementName) const
{
   Integer parameterID = -1;
   for (Integer i = 0; i < elementNames.size(); ++i)
   {
      if (elementNames[i] == elementName)
      {
         parameterID = elementIDs[i];
      }
   }

   return parameterID;
}


//--------------------------------------------------------------------------
// Integer Covariance::GetElementSize(const std::string elementName) const
//--------------------------------------------------------------------------
/**
* This function is used to get size of covariance associated to the input
* element
*
* @param elementName    name of a given element
*
* @return               size of covariance associated to the element.
*                       return -1 if element not found
*/
//--------------------------------------------------------------------------
Integer Covariance::GetElementSize(const std::string elementName) const
{
   Integer index = GetElementIndex(elementName);
   Integer size = -1;
   if (index != -1)
      size = elementSizes[index];

   return size;
}


//--------------------------------------------------------------------------
// Integer Covariance::GetElementSize(const Integer forParameterID) const
//--------------------------------------------------------------------------
/**
* This function is used to get size of covariance associated to the input
* element
*
* @param forParameterID    Parameter ID of the element
*
* @return               size of covariance associated to the element.
*                       return -1 if element not found
*/
//--------------------------------------------------------------------------
Integer Covariance::GetElementSize(const Integer forParameterID) const
{
   Integer index = GetElementIndex(forParameterID);
   Integer size = -1;
   if (index != -1)
      size = elementSizes[index];

   return size;
}


//--------------------------------------------------------------------------
// bool Covariance::SetElementSize(const std::string elementName, 
//                                    const Integer elementSize)
//--------------------------------------------------------------------------
/**
* This function is used to resize of covariance associated to the input
* element.
*
* @param elementName       Name of the input element
*
* @return                  true if set size have no error, false otherwise
*/
//--------------------------------------------------------------------------
bool Covariance::SetElementSize(const std::string elementName,
   const Integer elementSize)
{
   Integer forParameterID = GetElementID(elementName);
   Integer index = GetElementIndex(forParameterID);
   Integer startLoc = GetSubMatrixLocationStart(forParameterID);
   Integer size = GetElementSize(forParameterID);
   if (size == elementSize)
      return true;
   else if (elementSize > size)
      return IncreasingElementSize(elementNames[index], startLoc + size, elementSize - size);
   else
      return DecreasingElementSize(elementNames[index], startLoc + size, size - elementSize);
}


//--------------------------------------------------------------------------
// bool Covariance::SetElementSize(const Integer forParameterID, 
//                                    const Integer elementSize)
//--------------------------------------------------------------------------
/**
* This function is used to resize of covariance associated to the input
* element.
*
* @param forParameterID    Parameter ID of the element
*
* @return                  true if set size have no error, false otherwise
*/
//--------------------------------------------------------------------------
bool Covariance::SetElementSize(const Integer forParameterID, 
                                const Integer elementSize)
{
   Integer index = GetElementIndex(forParameterID);
   Integer startLoc = GetSubMatrixLocationStart(forParameterID);
   Integer size = GetElementSize(forParameterID);
   if (size == elementSize)
      return true;
   else if (elementSize > size)
      return IncreasingElementSize(elementNames[index], size, elementSize - size);
   else
      return DecreasingElementSize(elementNames[index], size, size - elementSize);
}


//--------------------------------------------------------------------------
// GmatBase* Covariance::GetElementOwner(const std::string elementName)
//--------------------------------------------------------------------------
/**
* This function is used to get owned object of the element
*
* @param elementName    name of a given element
*
* @return               the owned object of the element.
*                       return NULL if element not found
*/
//--------------------------------------------------------------------------
GmatBase* Covariance::GetElementOwner(const std::string elementName)
{
   Integer index = GetElementIndex(elementName);
   GmatBase* owner = NULL;
   if (index != -1)
      owner = elementOwners[index];

   return owner;
}


//--------------------------------------------------------------------------
// GmatBase* Covariance::GetElementOwner(const Integer forParameterID)
//--------------------------------------------------------------------------
/**
* This function is used to get owned object of the element
*
* @param forParameterID    parameter ID of the element
*
* @return               the owned object of the element.
*                       return NULL if element not found
*/
//--------------------------------------------------------------------------
GmatBase* Covariance::GetElementOwner(const Integer forParameterID)
{
   Integer index = GetElementIndex(forParameterID);
   GmatBase* owner = NULL;
   if (index != -1)
      owner = elementOwners[index];

   return owner;
}


//----------------------------------------------------------------------------
// Integer GetSubMatrixLocationStart(const Integer forParameterID) const
//----------------------------------------------------------------------------
/**
* Get the start index of a submatrix which is covariance of a given parameter.
*
* @param forParameterID    the ID of the parameter which needs to specified
*                          the start index of submatrix
*
* @return                  The start index of a submatrix to be covariance of 
*                          the given parameter
*/
//----------------------------------------------------------------------------
Integer Covariance::GetSubMatrixLocationStart(const Integer forParameterID) const
{
   Integer locationStart = 0;
   for (Integer i = 0; i < elementIDs.size(); ++i)
   {
      if (elementIDs[i] == forParameterID)
         break;
      locationStart += elementSizes[i];
   }

   return locationStart;
}


//----------------------------------------------------------------------------
// Integer GetSubMatrixLocationStart(const std::string paramName) const
//----------------------------------------------------------------------------
/**
* Get the start index of a submatrix which is covariance of a given parameter.
*
* @param paramName    name of the parameter which needs to specified
*                     the start index of submatrix
*
* @return             The start index of a submatrix to be covariance of the 
*                     given parameter
*/
//----------------------------------------------------------------------------
Integer  Covariance::GetSubMatrixLocationStart(const std::string paramName) const
{
   Integer index = 0;
   Integer locationStart = 0;
   for (; index < elementNames.size(); ++index)
   {
      if (elementNames[index] == paramName)
         break;
      locationStart += elementSizes[index];
   }
   if (index == elementNames.size())
      throw GmatBaseException("Error: cannot find covariance sub matrix for parameter " + paramName + ".\n");

   return locationStart;
}


//bool Covariance::IncreasingElementSize(const std::string elementName, const Integer atRow, const Integer increasingSize)
//{
//   Integer start = GetSubMatrixLocationStart(elementName);
//   
//   // 1. Increasing size of table on the bootom and the right side of table
//   Integer oldNumRows = theCovariance.GetNumRows();
//   Integer oldNumCols = theCovariance.GetNumColumns();
//   theCovariance.ChangeSize(oldNumRows + increasingSize, oldNumCols + increasingSize, false);
//   
//   // 2. Move all rows from the row with index 'start' down by a number of rows specified by increasingSize
//   Integer atRowAbs = atRow + start;
//   Integer toRow = theCovariance.GetNumRows()-1;
//   for (Integer row = oldNumRows - 1; row >= atRowAbs; --row)
//   {
//      for (Integer col = 0; col < theCovariance.GetNumColumns(); ++col)
//      {
//         theCovariance(toRow, col) = theCovariance(row, col);
//      }
//      --toRow;
//   }
//   
//   // 3. Fill the gap by 0
//   for (; toRow >= atRowAbs; --toRow)
//   {
//      for (Integer col = 0; col < theCovariance.GetNumColumns(); ++col)
//         theCovariance(toRow, col) = 0.0;
//   }
//   
//   // 4. Move all columns from the column with index atColABs to the right of the table by a number of columns specified by increasingSize
//   Integer atColAbs = atRow + start;
//
//   Integer toCol = theCovariance.GetNumColumns() - 1;
//   for (Integer col = oldNumCols - 1; col >= atColAbs; --col)
//   {
//      for (Integer row = 0; row < theCovariance.GetNumRows(); ++row)
//         theCovariance(row, toCol) = theCovariance(row, col);
//      --toCol;
//   }
//   
//   // 5. Fill the gap by 0
//   for (; toCol >= atColAbs; --toCol)
//   {
//      for (Integer row = 0; row < theCovariance.GetNumRows(); ++row)
//         theCovariance(row, toCol) = 0.0;
//   }
//   
//   // 6. Increasing dimension
//   Integer index = GetElementIndex(elementName);
//   elementSizes[index] = elementSizes[index] + increasingSize;
//   dimension = dimension + increasingSize;
//   
//   return true;
//}



bool Covariance::IncreasingElementSize(const std::string elementName, const Integer atRow, const Integer increasingSize)
{
#ifdef DEBUG_INCREASING_ELEMENT_SIZE
   MessageInterface::ShowMessage("Start Covariance::IncreasingElementSize(elementName = %s, atRow = %d  increasingSize = %d\n", elementName.c_str(), atRow, increasingSize);
#endif

   // 1. Create a new table with size increased by increasingSize 
   Integer oldNumRows = (*theCovariance).GetNumRows();
   Integer oldNumCols = (*theCovariance).GetNumColumns();
   Integer newNumRows = oldNumRows + increasingSize;
   Integer newNumCols = oldNumCols + increasingSize;
   Rmatrix* newTable = new Rmatrix(newNumRows, newNumCols);

   // 2. Copy content from old table to new table
   // 2.1. Specify index of the starting row/column where matrix increases it size
   Integer start = GetSubMatrixLocationStart(elementName);
   Integer atAbsIndex = atRow + start;

   // 2.2. Copy content from old table to the new table
   for (Integer newTableRow = 0; newTableRow < newNumRows; ++newTableRow)
   {
      for (Integer newTableCol = 0; newTableCol < newNumCols; ++newTableCol)
      {
         if (newTableRow < atAbsIndex)
         {
            if (newTableCol < atAbsIndex)
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow, newTableCol);
            else if ((atAbsIndex <= newTableCol) && (newTableCol < atAbsIndex + increasingSize))
               (*newTable)(newTableRow, newTableCol) = 0.0;
            else
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow, newTableCol - increasingSize);
         }
         else if ((atAbsIndex <= newTableRow) && (newTableRow < atAbsIndex + increasingSize))
            (*newTable)(newTableRow, newTableCol) = 0.0;
         else
         {
            if (newTableCol < atAbsIndex)
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow - increasingSize, newTableCol);
            else if ((atAbsIndex <= newTableCol) && (newTableCol < atAbsIndex + increasingSize))
               (*newTable)(newTableRow, newTableCol) = 0.0;
            else
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow - increasingSize, newTableCol - increasingSize);
         }
      }
   }

   // 3. Delete old table and set new table to be the current table
   delete theCovariance;
   theCovariance = newTable;

   // 4. Increasing dimension
   Integer index = GetElementIndex(elementName);
   elementSizes[index] = elementSizes[index] + increasingSize;
   dimension = dimension + increasingSize;

#ifdef DEBUG_INCREASING_ELEMENT_SIZE
   MessageInterface::ShowMessage("End Covariance::IncreasingElementSize(elementName = %s, atRow = %d  increasingSize = %d\n", elementName.c_str(), atRow, increasingSize);
#endif

   return true;
}


bool Covariance::IncreasingElementSize(const Integer forParameterID, const Integer atRow, const Integer increasingSize)
{
   // 1. Create a new table with size increased by increasingSize 
   Integer oldNumRows = (*theCovariance).GetNumRows();
   Integer oldNumCols = (*theCovariance).GetNumColumns();
   Integer newNumRows = oldNumRows + increasingSize;
   Integer newNumCols = oldNumCols + increasingSize;
   Rmatrix* newTable = new Rmatrix(newNumRows, newNumCols);

   // 2. Copy content from old table to new table
   // 2.1. Specify index of the starting row/column where matrix increases it size
   Integer start = GetSubMatrixLocationStart(forParameterID);
   Integer atAbsIndex = atRow + start;

   // 2.2. Copy content from old table to the new table
   for (Integer newTableRow = 0; newTableRow < newNumRows; ++newTableRow)
   {
      for (Integer newTableCol = 0; newTableCol < newNumCols; ++newTableCol)
      {
         if (newTableRow < atAbsIndex)
         {
            if (newTableCol < atAbsIndex)
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow, newTableCol);
            else if ((atAbsIndex <= newTableCol) && (newTableCol < atAbsIndex + increasingSize))
               (*newTable)(newTableRow, newTableCol) = 0.0;
            else
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow, newTableCol - increasingSize);
         }
         else if ((atAbsIndex <= newTableRow) && (newTableRow < atAbsIndex + increasingSize))
            (*newTable)(newTableRow, newTableCol) = 0.0;
         else
         {
            if (newTableCol < atAbsIndex)
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow - increasingSize, newTableCol);
            else if ((atAbsIndex <= newTableCol) && (newTableCol < atAbsIndex + increasingSize))
               (*newTable)(newTableRow, newTableCol) = 0.0;
            else
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow - increasingSize, newTableCol - increasingSize);
         }
      }
   }

   // 3. Delete old table and set new table to be the current table
   delete theCovariance;
   theCovariance = newTable;

   // 4. Increasing dimension
   Integer index = GetElementIndex(forParameterID);
   elementSizes[index] = elementSizes[index] + increasingSize;
   dimension = dimension + increasingSize;

   return true;
}


bool Covariance::DecreasingElementSize(const std::string elementName, const Integer atRow, const Integer decreasingSize)
{
   // 1. Create a new table with size increased by increasingSize 
   Integer oldNumRows = (*theCovariance).GetNumRows();
   Integer oldNumCols = (*theCovariance).GetNumColumns();
   
   if ((decreasingSize >= oldNumRows) || (decreasingSize >= oldNumCols))
      return false;

   Integer newNumRows = oldNumRows - decreasingSize;
   Integer newNumCols = oldNumCols - decreasingSize;
   Rmatrix* newTable = new Rmatrix(newNumRows, newNumCols);

   // 2. Copy content from old table to new table
   // 2.1. Specify index of the starting row/column where matrix increases it size
   Integer start = GetSubMatrixLocationStart(elementName);
   Integer atAbsIndex = atRow + start;

   // 2.2. Copy content from old table to the new table
   for (Integer newTableRow = 0; newTableRow < newNumRows; ++newTableRow)
   {
      for (Integer newTableCol = 0; newTableCol < newNumCols; ++newTableCol)
      {
         if (newTableRow < atAbsIndex - decreasingSize)
         {
            if (newTableCol < atAbsIndex - decreasingSize)
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow, newTableCol);
            else
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow, newTableCol + decreasingSize);
         }
         else
         {
            if (newTableCol < atAbsIndex - decreasingSize)
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow + decreasingSize, newTableCol);
            else
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow + decreasingSize, newTableCol + decreasingSize);
         }
      }
   }

   // 3. Delete old table and set new table to be the current table
   delete theCovariance;
   theCovariance = newTable;

   // 4. Increasing dimension
   Integer index = GetElementIndex(elementName);
   elementSizes[index] = elementSizes[index] - decreasingSize;
   dimension = dimension - decreasingSize;

   return true;
}


bool Covariance::DecreasingElementSize(const Integer forParameterID, const Integer atRow, const Integer decreasingSize)
{
   // 1. Create a new table with size increased by increasingSize 
   Integer oldNumRows = (*theCovariance).GetNumRows();
   Integer oldNumCols = (*theCovariance).GetNumColumns();

   if ((decreasingSize >= oldNumRows) || (decreasingSize >= oldNumCols))
      return false;

   Integer newNumRows = oldNumRows - decreasingSize;
   Integer newNumCols = oldNumCols - decreasingSize;
   Rmatrix* newTable = new Rmatrix(newNumRows, newNumCols);

   // 2. Copy content from old table to new table
   // 2.1. Specify index of the starting row/column where matrix increases it size
   Integer start = GetSubMatrixLocationStart(forParameterID);
   Integer atAbsIndex = atRow + start;

   // 2.2. Copy content from old table to the new table
   for (Integer newTableRow = 0; newTableRow < newNumRows; ++newTableRow)
   {
      for (Integer newTableCol = 0; newTableCol < newNumCols; ++newTableCol)
      {
         if (newTableRow < atAbsIndex - decreasingSize)
         {
            if (newTableCol < atAbsIndex - decreasingSize)
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow, newTableCol);
            else
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow, newTableCol + decreasingSize);
         }
         else
         {
            if (newTableCol < atAbsIndex - decreasingSize)
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow + decreasingSize, newTableCol);
            else
               (*newTable)(newTableRow, newTableCol) = (*theCovariance)(newTableRow + decreasingSize, newTableCol + decreasingSize);
         }
      }
   }

   // 3. Delete old table and set new table to be the current table
   delete theCovariance;
   theCovariance = newTable;

   // 4. Increasing dimension
   Integer index = GetElementIndex(forParameterID);
   elementSizes[index] = elementSizes[index] - decreasingSize;
   dimension = dimension - decreasingSize;

   return true;
}


//----------------------------------------------------------------------------
// Rmatrix* GetCovariance(Integer forParameterID)
//----------------------------------------------------------------------------
/**
* Get covariance matrix associated with a given parameter.
*
* @param forParameterID    the ID of the parameter which needs to specified
*                          covariance matrix
*
* @return                  the covariance matrix for the parameter
*/
//----------------------------------------------------------------------------
Rmatrix *Covariance::GetCovariance(const Integer forParameterID)
{
   // Find the covariance elements that match up with the input ID
   Integer locationStart = 0;
   if (subCovariance != NULL)
   {
      delete subCovariance;
      subCovariance = NULL;
   }

   for (UnsignedInt i = 0; i < elementIDs.size(); ++i)
   {
      #ifdef DEBUG_ACCESS
         MessageInterface::ShowMessage("Checking if index %d == %d\n",
               elementIndices[i], forParameterID);
      #endif

      if (elementIDs[i] == forParameterID)
      {
         // Found it!
         Integer parmSize = elementSizes[i];
         subCovariance = new Rmatrix(parmSize, parmSize);

         for (Integer j = 0; j < parmSize; ++j)
         {
            for (Integer k = 0; k < parmSize; ++k)
            {
               //(*subCovariance)(j, k) = theCovariance(locationStart+j,
               //      locationStart+k);
               (*subCovariance)(j, k) = (*theCovariance)(locationStart + j,
                  locationStart + k);
            }
         }

         break;
      }
      locationStart += elementSizes[i];
   }

   return subCovariance;
}


void Covariance::PrepareMatrix()
{
   //theCovariance.SetSize(dimension, dimension);
   (*theCovariance).SetSize(dimension, dimension);

   // Initialize to the identity
   for (Integer i = 0; i < dimension; ++i)
      for (Integer j = 0; j < dimension; ++j)
      {
         //theCovariance(i, j) = (i == j ? 1.0 : 0.0);
         (*theCovariance)(i, j) = (i == j ? 1.0 : 0.0);
      }
}


void Covariance::ShowContent()
{
   MessageInterface::ShowMessage("Covariance owner = <%p,%s>\n", covarianceOwner, (covarianceOwner==NULL?"":covarianceOwner->GetName().c_str()));
   MessageInterface::ShowMessage("Covariance dimesion = %d\n", dimension);
   MessageInterface::ShowMessage("Number of elements = %d\n", elementNames.size());
   for (Integer i = 0; i < elementNames.size(); ++i)
   {
      MessageInterface::ShowMessage("%d: ElementName = <%s>   ElementID = %d  ElementOwner = <%p,%s> ElementSize = %d\n", i, elementNames[i].c_str(), elementIDs[i], elementOwners[i], elementOwners[i]->GetName().c_str(), elementSizes[i]);
   }

   MessageInterface::ShowMessage("Covariance matrix = [\n");
   //for (Integer row = 0; row < theCovariance.GetNumRows(); ++row)
   for (Integer row = 0; row < (*theCovariance).GetNumRows(); ++row)
   {
      //for (Integer col = 0; col < theCovariance.GetNumColumns(); ++col)
      for (Integer col = 0; col < (*theCovariance).GetNumColumns(); ++col)
      {
         //MessageInterface::ShowMessage("%.15le   ", theCovariance(row,col));
         MessageInterface::ShowMessage("%.15le   ", (*theCovariance)(row, col));
      }
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("]\n");

}