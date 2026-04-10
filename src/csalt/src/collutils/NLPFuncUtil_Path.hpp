//------------------------------------------------------------------------------
//                         NLPFuncUtil_Path
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Created: 2016.05.20
//
/**
 * NOTE: currently, this class is empty - keeping as placeholder and
 *       for possible future use
 */
//------------------------------------------------------------------------------
#ifndef NLPFuncUtil_Path_hpp
#define NLPFuncUtil_Path_hpp

#include "NLPFuncUtil.hpp"

/**
 * NLPFuncUtil_Path class
 */
class NLPFuncUtil_Path : public NLPFuncUtil
{
public:
   
   
   /// default constructor
   NLPFuncUtil_Path();
   
   /// copy constructor
   NLPFuncUtil_Path(const NLPFuncUtil_Path &copy);
   
   /// assignment operator
   NLPFuncUtil_Path&   operator=(const  NLPFuncUtil_Path &copy);
   
   /// virtual destructor
   virtual ~NLPFuncUtil_Path();
   
   
protected:
   
};
#endif // NLPFuncUtil_Path_hpp