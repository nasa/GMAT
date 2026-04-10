//------------------------------------------------------------------------------
//                         RayleighPointObject
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
// Author: Claire Conway
// Created: 2017.02.03
//
/**
 * Developed based on RayleighPointObject.m
 */
//------------------------------------------------------------------------------
#ifndef RayleighPointObject_hpp
#define RayleighPointObject_hpp

#include "UserPointFunction.hpp"

/**
 * RayleighPointObject class
 */
class RayleighPointObject : public UserPointFunction
{
public:
   
   /// default constructor
   RayleighPointObject();
   /// copy constructor
   RayleighPointObject(const RayleighPointObject &copy);
   /// operator=
   RayleighPointObject& operator=(const RayleighPointObject &copy);
   /// default destructor
   virtual ~RayleighPointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // RayleighPointObject_hpp