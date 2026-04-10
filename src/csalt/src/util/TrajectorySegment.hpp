//------------------------------------------------------------------------------
//                             TrajectorySegment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Jeremy Knittel / NASA
// Created: 2017.02.23
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
    
#ifndef TrajectorySegment_hpp
#define TrajectorySegment_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"

struct TrajectoryDataStructure
{
   Real    time;
   Rvector states;
   Rvector controls;
   Rvector integrals;
};

class TrajectorySegment
{
public:
   /// default constructor
   TrajectorySegment();
   /// copy constructor
   TrajectorySegment(const TrajectorySegment &copy);
   /// operator=
   TrajectorySegment& operator=(const TrajectorySegment &copy);
   
   /// destructor
   virtual ~TrajectorySegment(); 

   /// access (get/set) methods
   virtual void    SetNumControlParams(Integer input);
   virtual void    SetNumStateParams(Integer input);
   virtual void    SetNumIntegralParams(Integer input);

   virtual Integer GetNumControls();
   virtual Integer GetNumStates();
   virtual Integer GetNumIntegrals();
   virtual Integer GetNumDataPoints();

   virtual Real    GetTime(Integer index);
   virtual Real    GetControl(Integer index, Integer dimension);
   virtual Real    GetState(Integer index, Integer dimension);
   virtual Real    GetIntegral(Integer index, Integer dimension);
   virtual Integer GetControlSize(Integer index);
   virtual Integer GetStateSize(Integer index);
   virtual Integer GetIntegralSize(Integer index);

   virtual void    AddDataPoint(TrajectoryDataStructure inputData);

protected:

   /// Number of data points in this segment
   Integer numDataPoints;   

   /// Number of state parameters in the problem
   Integer numStates;

   /// Number of control parameters in the problem
   Integer numControls;

   /// Number of integral parameters in the problem
   Integer numIntegrals; 

   /// The vector of data
   std::vector<TrajectoryDataStructure> trajData;

   /// Copy the array data from the input object
   void CopyArrays(const TrajectorySegment &copy);      
};

#endif
