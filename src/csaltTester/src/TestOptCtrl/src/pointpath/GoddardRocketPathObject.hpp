//------------------------------------------------------------------------------
//                         GoddardRocketPathObject
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
// Created: 2017.02.21
//
/**
 * Developed based on GoddardRocketPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef GoddardRocketPathObject_hpp
#define GoddardRocketPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * GoddardRocketPathObject class
 */
class GoddardRocketPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   GoddardRocketPathObject();
   /// copy constructor
   GoddardRocketPathObject(const GoddardRocketPathObject &copy);
   /// operator=
   GoddardRocketPathObject& operator=(const GoddardRocketPathObject &copy);
   /// default destructor
   virtual ~GoddardRocketPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
   
protected:
   Real gravity;
};

#endif // GoddardRocketPathObject_hpp