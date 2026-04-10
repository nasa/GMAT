//------------------------------------------------------------------------------
//                               QuaternionRotation
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: Aug 14, 2024
/**
 *  Vector rotations using a unit quaternion.
 */
 //------------------------------------------------------------------------------


#ifndef SRC_BASE_FUNCTION_QuaternionRot_HPP_
#define SRC_BASE_FUNCTION_QuaternionRot_HPP_

#include "BuiltinGmatFunction.hpp"

class GMAT_API QuaternionRotation : public BuiltinGmatFunction
{
public:
   QuaternionRotation(const std::string& typeStr, const std::string& name);
   virtual ~QuaternionRotation();
   QuaternionRotation(const QuaternionRotation& f);
   QuaternionRotation& operator=(const QuaternionRotation& f);

   virtual WrapperTypeArray
      GetOutputTypes(IntegerArray& rowCounts,
         IntegerArray& colCounts) const;
   virtual void         SetOutputTypes(WrapperTypeArray& outputTypes,
      IntegerArray& rowCounts,
      IntegerArray& colCounts);
   virtual bool         Initialize(ObjectInitializer* objInit, bool reinitialize = false);
   virtual bool         Execute(ObjectInitializer* objInit, bool reinitialize = false);
   virtual void         Finalize(bool cleanUp = false);

   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual void         Copy(const GmatBase* orig);

protected:
   ElementWrapper* CreateOutputArrayWrapper(Rvector3 outVec, const std::string& outName);
};



#endif /* SRC_BASE_FUNCTION_QuaternionRot_HPP_ */
