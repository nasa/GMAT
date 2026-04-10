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
 * Definition of the GmatState class.  This is the class for state data used in 
 * GMAT's propagators and solvers.
 */
//------------------------------------------------------------------------------

#ifndef GmatState_hpp
#define GmatState_hpp

#include "gmatdefs.hpp"
#include "GmatTime.hpp"


/**
 * A GmatState is a vector on numbers and an associated epoch.  
 * 
 * This class defines the core data set used in GMAT's propagation and solver 
 * subsystems when state data at a defined epoch is needed.
 */
class GMAT_API GmatState
{
public:
	GmatState(Integer size = 0);
	virtual ~GmatState();
   GmatState(const GmatState& gs);
   GmatState& operator=(const GmatState& gs);
   
   // Operator overloads
   Real&             operator[](const Integer index);
   Real              operator[](const Integer index) const;

   // Sizing manipulation
   void              SetSize(const Integer size);
   
   // Access methods
   Integer           GetSize() const;
   Real*             GetState();
   bool              SetState(const Real *data, const Integer size, 
                              const Integer start = 0);

   Real*             GetStateDot(); 
   bool              SetStateDot(const Real *dataDot, const Integer size, 
                                 const Integer start = 0);

   GmatEpoch         GetEpoch() const;
   GmatTime          GetEpochGT() const;
   GmatEpoch         SetEpoch(const GmatEpoch ep);
   GmatTime          SetEpochGT(const GmatTime ep);
   bool              SetPrecisionTimeFlag(bool onOff = true);
   
   bool              SetElementProperties(const Integer index, const Integer id, 
                           const std::string &textId, const Integer associate);
   const StringArray&
                     GetElementDescriptions();
   Integer           GetAssociateIndex(const Integer id);

   bool              HasPrecisionTime() const;

protected:
   /// The epoch of the state data
   GmatEpoch         theEpoch;
   GmatTime          theEpochGT;
   bool              hasPrecisionTime;

   /// The state data
   Real              *theData;

   /// The state data's first derivative w.r.t. time 
   Real              *theDataDot; 

   Integer           *dataIDs;
   Integer           *associatedElements;
   StringArray       dataTypes;
   
   /// Length of the state vector
   Integer           stateSize;
   
   void              Resize(Integer newSize, bool withCopy = true);
   void              Zero(Integer begin = 0, UnsignedInt length = 0);

};

#endif /*GmatState_hpp*/
