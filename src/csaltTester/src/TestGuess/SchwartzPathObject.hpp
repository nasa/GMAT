//------------------------------------------------------------------------------
//                         SchwartzPathObject
//------------------------------------------------------------------------------
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 22, 2017
/**
 * Developed based on SchwartzPathObject.m
 */
//------------------------------------------------------------------------------
#ifndef SchwartzPathObject_hpp
#define SchwartzPathObject_hpp

#include "UserPathFunction.hpp"

/**
 * SchwartzPathObject class
 */
class SchwartzPathObject : public UserPathFunction
{
public:
   SchwartzPathObject();
   virtual ~SchwartzPathObject();
   SchwartzPathObject(const SchwartzPathObject &copy);
   SchwartzPathObject& operator=(const SchwartzPathObject &copy);
   
   void EvaluateFunctions();
   void EvaluateJacobians();
};

#endif // SchwartzPathObject_hpp
