//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeData
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
// Developed further jointly by NASA/GSFC, Thinking Systems, Inc., and 
// Schafer Corp., under AFRL NOVA Contract #FA945104D03990003
//
// Author: Daniel Hunter
// Created: 2006/6/26
// Modified:  Dunn Idle (added MRPs)
// Date:      2010/08/24
//
/**
 * Declares Attitude related data class.
 */
//------------------------------------------------------------------------------
#ifndef AttitudeData_hpp
#define AttitudeData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"

class GMAT_API AttitudeData : public RefData
{
public:

   AttitudeData(const std::string name = "");
   AttitudeData(const AttitudeData &data);
   AttitudeData& operator= (const AttitudeData& right);
   virtual ~AttitudeData();
   
   Real GetReal(Integer item);
   void SetReal(Integer item, Real value);
   Rvector GetRvector(Integer item);
   void SetRvector(Integer item, const Rvector &value);
   Rmatrix33 GetRmatrix33(Integer item);
   std::string GetString(Integer item);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   static const Real ATTITUDE_REAL_UNDEFINED;
   
protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);
   
   Spacecraft *mSpacecraft;
   Integer mEpochId;
   
   enum 
   {
      DCM_11,
      DCM_12,
      DCM_13,
      DCM_21,
      DCM_22,
      DCM_23,
      DCM_31,
      DCM_32,
      DCM_33,
      DIRECTION_COSINE_MATRIX,
      QUAT_1, 
      QUAT_2, 
      QUAT_3, 
      QUAT_4,
      QUATERNION,
      EULER_ANGLE_1,
      EULER_ANGLE_2,
      EULER_ANGLE_3,
      MRP_1,  
      MRP_2,  
      MRP_3,  
      ANGULAR_VELOCITY_X, 
      ANGULAR_VELOCITY_Y, 
      ANGULAR_VELOCITY_Z,
      EULER_ANGLE_RATE_1,
      EULER_ANGLE_RATE_2,
      EULER_ANGLE_RATE_3,
      BODY_ALIGNMENT_VECTOR_X,    
      BODY_ALIGNMENT_VECTOR_Y,    
      BODY_ALIGNMENT_VECTOR_Z,    
      BODY_CONSTRAINT_VECTOR_X,   
      BODY_CONSTRAINT_VECTOR_Y,   
      BODY_CONSTRAINT_VECTOR_Z,   
      ATTITUDE_PARAM,             
      ATTITUDE_REFERENCE_BODY_ID, 
      ATTITUDE_CONSTRAINT_TYPE_ID,
   };
   
   enum
   {
      SPACECRAFT = 0,
      AttitudeDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[AttitudeDataObjectCount];
};

#endif /*AttitudeData_hpp*/
