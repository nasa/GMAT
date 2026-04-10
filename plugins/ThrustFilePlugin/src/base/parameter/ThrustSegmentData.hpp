//------------------------------------------------------------------------------
//                                  ThrustSegmentData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Peter Candell
// Created: 2022.09.06
//
/**
 * Declares ThrustSegment related data class.
 */
 //------------------------------------------------------------------------------
#ifndef ThrustSegmentData_hpp
#define ThrustSegmentData_hpp

#include "ThrustFileDefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "ThrustSegment.hpp"

class THRUSTFILE_API ThrustSegmentData : public RefData
{
public:

   ThrustSegmentData(const std::string &name = "");
   ThrustSegmentData(const ThrustSegmentData &data);
   ThrustSegmentData& operator= (const ThrustSegmentData& right);
   virtual ~ThrustSegmentData();

   Real GetReal(Integer item);
   Real SetReal(Integer item, Real val);

   std::string GetString(Integer item);
   Real SetString(Integer item, std::string &val);

   Rvector GetRvector(Integer item);
   Rvector SetRvector(Integer item, Rvector &val);

   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;

   static const Real THRUST_REAL_UNDEFINED;


protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);

   ThrustSegment *mThrustSegment;

   Rvector UNDEFINED_RETURN_RVECTOR = Rvector(3, 1, 1, THRUST_REAL_UNDEFINED);
   std::string UNDEFINED_RETURN_STRING = "UNDEFINED_RETURN_STRING";

   enum
   {
      THRUSTSEGMENTSCALEFACTOR,
      TSF_SIGMA,
      TSF_MASSFLOW,
      MASSFLOWSCALEFACTOR,
      MASSSOURCE,
      THRUST_ANGLE_CONSTRAINT,
      THRUST_ANGLE_1,
      THRUST_ANGLE_2,
      THRUST_ANGLE_SIGMA_1,
      THRUST_ANGLE_SIGMA_2,
      TSF_EPSILON,
      START_EPOCH,
      END_EPOCH,
      ThrustSegmentParamCount,
   };

   enum
   {
      THRUSTSEGMENT = 0,
      ThrustSegmentDataObjectCount
   };

   static const std::string VALID_OBJECT_TYPE_LIST[ThrustSegmentDataObjectCount];

private:

};

#endif /*ThrustSegmentData_hpp*/

