//------------------------------------------------------------------------------
//                         ConwaySpiralPointObject
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
// Created: 2017.02.13
//
/**
 * Developed based on ConwaySpiralPointObject.m
 */
//------------------------------------------------------------------------------
#ifndef ConwaySpiralPointObject_hpp
#define ConwaySpiralPointObject_hpp

#include "UserPointFunction.hpp"

/**
 * ConwaySpiralPointObject class
 */
class ConwaySpiralPointObject : public UserPointFunction
{
public:
   
   /// default constructor
   ConwaySpiralPointObject();
   /// copy constructor
   ConwaySpiralPointObject(const ConwaySpiralPointObject &copy);
   /// operator=
   ConwaySpiralPointObject& operator=(const ConwaySpiralPointObject &copy);
   /// default destructor
   virtual ~ConwaySpiralPointObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
};

#endif // ConwaySpiralPointObject_hpp