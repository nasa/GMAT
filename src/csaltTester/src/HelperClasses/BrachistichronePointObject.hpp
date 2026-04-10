//------------------------------------------------------------------------------ 
//                         BrachistichronePointObject 
//------------------------------------------------------------------------------ 
// GMAT: General Mission Analysis Tool. 
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
// Author: Jeremy Knittel 
// Created: 2016.11.03 
// 
/** 
 * Developed based on BrachistichronePointObject.m 
 */ 
//------------------------------------------------------------------------------ 
#ifndef BrachistichronePointObject_hpp 
#define BrachistichronePointObject_hpp 
 
#include "UserPointFunction.hpp" 
 
/** 
 * BrachistichronePointObject class 
 */ 
class BrachistichronePointObject : public UserPointFunction 
{ 
public: 
    
   /// default constructor 
   BrachistichronePointObject(); 
   /// copy constructor 
   BrachistichronePointObject(const BrachistichronePointObject &copy); 
   /// operator= 
   BrachistichronePointObject& operator=(const BrachistichronePointObject &copy); 
   /// default destructor 
   virtual ~BrachistichronePointObject(); 
    
    
   /// EvaluateFunctions 
   void EvaluateFunctions(); 
    
   /// EvaluateJacobians 
   void EvaluateJacobians(); 
    
    
protected: 
}; 
 
#endif // BrachistichronePointObject_hpp