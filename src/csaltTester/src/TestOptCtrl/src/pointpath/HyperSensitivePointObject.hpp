//------------------------------------------------------------------------------
//                         HyperSensitivePointObject
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
// Author: Wendy Shoan
// Created: 2016.10.18
//
/**
 * Developed based on HyperSensitivePointObject.m
 */
//------------------------------------------------------------------------------
#ifndef HyperSensitivePointObject_hpp
#define HyperSensitivePointObject_hpp

#include "UserPointFunction.hpp"

/**
 * HyperSensitivePointObject class
 */
class HyperSensitivePointObject : public UserPointFunction
{
public:
   
   /// default constructor
   HyperSensitivePointObject();
   /// copy constructor
   HyperSensitivePointObject(const HyperSensitivePointObject &copy);
   /// operator=
   HyperSensitivePointObject& operator=(const HyperSensitivePointObject &copy);
   /// default destructor
   virtual ~HyperSensitivePointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // HyperSensitivePointObject_hpp