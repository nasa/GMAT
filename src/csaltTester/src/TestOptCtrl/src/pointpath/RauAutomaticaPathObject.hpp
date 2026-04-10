//------------------------------------------------------------------------------
//                         RauAutomaticaPathObject
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
// Author: Youngkwang Kim
// Created: 2016.11.07
//
/**
 * Developed based on RauAutomaticaPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef RauAutomaticaPathObject_hpp
#define RauAutomaticaPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * RauAutomaticaPathObject class
 */
class RauAutomaticaPathObject : public UserPathFunction
{
public:
   
   /// default constructor
   RauAutomaticaPathObject();
   /// copy constructor
   RauAutomaticaPathObject(const RauAutomaticaPathObject &copy);
   /// operator=
   RauAutomaticaPathObject& operator=(const RauAutomaticaPathObject &copy);
   /// default destructor
   virtual ~RauAutomaticaPathObject();
   
   
   /// EvaluateFunctions
   void EvaluateFunctions();
   
   /// EvaluateJacobians
   void EvaluateJacobians();
   
};

#endif // RauAutomaticaPathObject_hpp