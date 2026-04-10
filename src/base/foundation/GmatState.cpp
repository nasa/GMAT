//$Id$
//------------------------------------------------------------------------------
//                                  GmatState
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/12/15
//
/**
 * Implementation of the GmatState class.  This is the class for state data used 
 * in GMAT's propagators and solvers.
 */
//------------------------------------------------------------------------------

#include "GmatState.hpp"
#include "GmatBaseException.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"

#include <sstream>
#include "MessageInterface.hpp"

//#define DEBUG_STATE_LOADING


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// GmatState(Integer size)
//------------------------------------------------------------------------------
/**
 * Default constructor for the GmatState.
 *
 * @param size Size of the requested state vector.
 */
//------------------------------------------------------------------------------
GmatState::GmatState(Integer size) :
   theEpoch          (GmatTimeConstants::MJD_OF_J2000),
   theEpochGT        (GmatTimeConstants::MJD_OF_J2000),
   hasPrecisionTime  (false),
   stateSize         (size)   
{
   if (stateSize == 0)
   {
      theData = NULL;

      // Adding first derivative of state w.r.t. time t 
      theDataDot = NULL;

      dataIDs = NULL;
      associatedElements = NULL;

   }
   else
   {
      theData = new Real[stateSize];
      theDataDot = new Real[stateSize];

      Zero();
      dataIDs = new Integer[stateSize];
      associatedElements = new Integer[stateSize];
      dataTypes.assign(stateSize, "");
   }
}

//------------------------------------------------------------------------------
// ~GmatState()
//------------------------------------------------------------------------------
/**
 * Destructor for the GmatState.
 */
//------------------------------------------------------------------------------
GmatState::~GmatState()
{
   if (theData != NULL)
      delete [] theData;

   if (theDataDot != NULL)
      delete[] theDataDot;

   if (dataIDs != NULL)
      delete [] dataIDs;
   if (associatedElements != NULL)
      delete [] associatedElements;
}

//------------------------------------------------------------------------------
// GmatState(const GmatState& gs)
//------------------------------------------------------------------------------
/**
 * Copy constructor for the GmatState.
 * 
 * @param gs The state that is copied here
 */
//------------------------------------------------------------------------------
GmatState::GmatState(const GmatState& gs) :
   theEpoch          (gs.theEpoch),
   theEpochGT        (gs.theEpochGT),
   hasPrecisionTime  (gs.hasPrecisionTime),
   stateSize         (gs.stateSize)
{
   dataTypes = gs.dataTypes;

   if (stateSize == 0)
   {
      theData = NULL;
      theDataDot = NULL;
      dataIDs = NULL;
      associatedElements = NULL;
   }
   else
   {
      theData = new Real[stateSize];
      theDataDot = new Real[stateSize];
      dataIDs = new Integer[stateSize];
      associatedElements = new Integer[stateSize];
      
      memcpy(theData, gs.theData, stateSize * sizeof(Real));
      memcpy(theDataDot, gs.theDataDot, stateSize * sizeof(Real));
      memcpy(dataIDs, gs.dataIDs, stateSize * sizeof(Integer));
      memcpy(associatedElements, gs.associatedElements, stateSize * sizeof(Integer));
      
      dataTypes.assign(stateSize, "");
      for (Integer i = 0; i < stateSize; ++i)
         dataTypes[i] = gs.dataTypes[i];
   }
}

//------------------------------------------------------------------------------
// GmatState& operator=(const GmatState& gs)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the GmatState.
 * 
 * @param gs The state that is copied here
 */
//------------------------------------------------------------------------------
GmatState& GmatState::operator=(const GmatState& gs)
{
   if (this != &gs)
   {
      theEpoch         = gs.theEpoch;
      theEpochGT       = gs.theEpochGT;
      hasPrecisionTime = gs.hasPrecisionTime;
      stateSize        = gs.stateSize;
      
      if (theData != NULL)
         delete [] theData;
      if (theDataDot != NULL)
         delete[] theDataDot;
      if (dataIDs != NULL)
         delete [] dataIDs;
      if (associatedElements != NULL)
         delete [] associatedElements;
      
      dataTypes.clear();

      if (stateSize == 0)
      {
         theData = NULL;
         theDataDot = NULL;
         dataIDs = NULL;
         associatedElements = NULL;
      }
      else
      {
         theData = new Real[stateSize];
         theDataDot = new Real[stateSize];
         dataIDs = new Integer[stateSize];
         associatedElements = new Integer[stateSize];
         memcpy(theData, gs.theData, stateSize * sizeof(Real));
         memcpy(theDataDot, gs.theDataDot, stateSize * sizeof(Real));
         memcpy(dataIDs, gs.dataIDs, stateSize * sizeof(Integer));
         memcpy(associatedElements, gs.associatedElements, stateSize * sizeof(Integer));
         
         dataTypes.assign(stateSize, "");
         for (Integer i = 0; i < stateSize; ++i)
            dataTypes[i] = gs.dataTypes[i];
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
//       OPERATOR OVERLOADS
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Real& operator[](const Integer index)
//------------------------------------------------------------------------------
/**
 * Array element accessor used to set element values
 * 
 * @param index The index to the desired element
 */
//------------------------------------------------------------------------------
Real& GmatState::operator[](const Integer index)
{
   if ((index < 0 ) || (index >= stateSize))
      throw GmatBaseException("GmatState array index out of bounds");
   
   return theData[index];
}


//------------------------------------------------------------------------------
// Real operator[](const Integer index) const
//------------------------------------------------------------------------------
/**
 * Array element accessor used to retrieve element values
 * 
 * @param index The index to the desired element
 */
//------------------------------------------------------------------------------
Real GmatState::operator[](const Integer index) const
{
   if ((index < 0 ) || (index >= stateSize))
      throw GmatBaseException("GmatState array index out of bounds");
   
   return theData[index];
}


//------------------------------------------------------------------------------
//       PUBLIC METHODS
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void SetSize(const Integer size)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
void GmatState::SetSize(const Integer size)
{
   if (size > 0)
      Resize(size);
   else
      throw GmatBaseException("State resizing to a value less than or equal "
            "to zero is not allowed");
}


//------------------------------------------------------------------------------
// Integer GetSize() const
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
Integer GmatState::GetSize() const
{
   return stateSize;
}


//------------------------------------------------------------------------------
// Real* GetState()
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
Real* GmatState::GetState()
{
   return theData;
}


//------------------------------------------------------------------------------
// bool SetState(const Real *data, const Integer size, const Integer start)

//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool GmatState::SetState(const Real *data, const Integer size, 
                           const Integer start)
{
   if (start < 0)
      throw GmatBaseException(
            "Cannot set state data -- starting index is out of range");
   if (start + size > stateSize)
      throw GmatBaseException(
            "Cannot set state data -- data span is out of range");
   
   memcpy(&(theData[start]), data, size*sizeof(Real));
   
   return true;
}


//------------------------------------------------------------------------------
// Real* GetStateDot()
//------------------------------------------------------------------------------
/**
 */
 //------------------------------------------------------------------------------
Real* GmatState::GetStateDot()
{
   return theDataDot;
}


//------------------------------------------------------------------------------
// bool SetStateDot(const Real *dataDot, const Integer size, const Integer start)

//------------------------------------------------------------------------------
/**
 */
 //------------------------------------------------------------------------------
bool GmatState::SetStateDot(const Real *dataDot, const Integer size,
   const Integer start)
{
   if (start < 0)
      throw GmatBaseException(
         "Cannot set derivative w.r.t. time of state data -- starting index is out of range");
   if (start + size > stateSize)
      throw GmatBaseException(
         "Cannot set derivative w.r.t. time of state data -- data span is out of range");

   memcpy(&(theDataDot[start]), dataDot, size * sizeof(Real));

   return true;
}


//------------------------------------------------------------------------------
// GmatEpoch GetEpoch() const
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
GmatEpoch GmatState::GetEpoch() const
{
   return theEpoch;
}


GmatTime GmatState::GetEpochGT() const
{
   return theEpochGT;
}


//------------------------------------------------------------------------------
// GmatEpoch SetEpoch(const GmatEpoch ep)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
GmatEpoch GmatState::SetEpoch(const GmatEpoch ep)
{
   theEpoch = ep;
   return theEpoch;
}


GmatTime GmatState::SetEpochGT(const GmatTime ep)
{
   theEpochGT = ep;
   return theEpochGT;
}


bool GmatState::SetPrecisionTimeFlag(bool onOff)
{
   if ((hasPrecisionTime == false)&& (onOff == true))
      if (!GmatMathUtil::IsEqual(theEpochGT.GetMjd(), theEpoch, 1e-6))
         theEpochGT = theEpoch; // only set epoch if they differ by more than 1 us
      
   hasPrecisionTime = onOff;
   return hasPrecisionTime;
}


bool GmatState::HasPrecisionTime() const
{
   return hasPrecisionTime;
}


bool GmatState::SetElementProperties(const Integer index, const Integer id, 
                                     const std::string &textId, 
                                     const Integer associate)
{
   #ifdef DEBUG_STATE_LOADING
      MessageInterface::ShowMessage(
            "Setting element %d, id = %d, desc = %s, assoc = %d\n", index, id, 
            textId.c_str(), associate);
   #endif
      
   if ((index < 0) || (index >= stateSize))
      throw GmatBaseException("Cannot set state element properties: "
                  "index out of range.");
   
   dataIDs[index] = id;
   dataTypes[index] = textId;
   associatedElements[index] = associate;
   
   return true;
}

const StringArray& GmatState::GetElementDescriptions()
{
   return dataTypes;
}

//------------------------------------------------------------------------------
// void Resize(Integer newSize, bool withCopy)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
void GmatState::Resize(Integer newSize, bool withCopy)
{
   if (newSize == stateSize)
   {
      // If size does not change, do not zero regardless of the withCopy 
      //setting and just return
      return;
   }
   
   if (newSize <= 0)
      throw GmatBaseException("GmatState Resize requested an invalid size");
   
   Real *newData = new Real[newSize];
   Real *newDataDot = new Real[newSize];
   Integer *newIDs = new Integer[newSize];
   Integer *newAssociates = new Integer[newSize];
   StringArray newTypes;
   newTypes.assign(newSize, "");

   Integer start = 0;

   if (withCopy)
   {
      // copy as much of the current state as possible into the new state 
      Integer size = (newSize > stateSize ? stateSize : newSize);
      memcpy(newData, theData, size*sizeof(Real));
      memcpy(newDataDot, theDataDot, size * sizeof(Real));
      memcpy(newIDs, dataIDs, size*sizeof(Integer));
      memcpy(newAssociates, associatedElements, size*sizeof(Integer));
  
      for (Integer i = 0; i < size; ++i)
         newTypes[i] = dataTypes[i];
         
      start = size;
   }
   
   stateSize = newSize;
   delete [] theData;
   delete[] theDataDot;
   delete [] dataIDs;
   delete [] associatedElements;
   theData = newData;
   theDataDot = newDataDot;
   dataIDs = newIDs;
   associatedElements = newAssociates;
   dataTypes.assign(newSize, "");
   dataTypes = newTypes;

   #ifdef DEBUG_STATE_LOADING
      MessageInterface::ShowMessage("dataType size is %d\n", dataTypes.size());
   #endif
      
   // Zero the unset entries in the state data
   Zero(start, newSize - start);
}


Integer GmatState::GetAssociateIndex(const Integer id)
{
   #ifdef DEBUG_ASSOCIATE_INDICES
      MessageInterface::ShowMessage("GmatState::GetAssociateIndex State "
            "Size = %d, id = %d\n", stateSize, id);
   #endif

   if ((id < 0) || (id >= stateSize))
   {
      std::stringstream msg;
      msg << "Requested element association index " << id
          << " is out of bounds for the GMAT state vector, which has size "
          << stateSize;
      throw GmatBaseException(msg.str());
   }
   return associatedElements[id];
}


//------------------------------------------------------------------------------
//       PROTECTED METHODS
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void Zero(Integer begin, UnsignedInt length)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
void GmatState::Zero(Integer begin, UnsignedInt length)
{
   if ((begin < 0) || ((Integer)(begin + length) > stateSize))
   {
      std::stringstream errmsg;
      errmsg << "GmatState request to zero " << length
             << " elements starting at element " << begin 
             << " exceeds the state size, which is " << stateSize;
      throw GmatBaseException(errmsg.str());
   }
   
   for (Integer i = begin; i < (Integer)(begin + length); ++i)
   {
      theData[i]    = 0.0;
      theDataDot[i] = 0.0;
   }
}
