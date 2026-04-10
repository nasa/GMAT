//------------------------------------------------------------------------------
//                         NLPFuncUtil
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
// Author: Youngkwang Kim
// Created: 2015/07/10 (mods 2016.05.20 WCS)
//
/**
 * header only NLPFuncUtil class. 
 * NOTE: currently, this class is empty - keeping as placeholder and
 *       for possible future use
 */
//------------------------------------------------------------------------------
#ifndef NLPFuncUtil_hpp
#define NLPFuncUtil_hpp

/**
 * NLPFuncUtil class
 */
class NLPFuncUtil
{
public:

   /// default constructor
   NLPFuncUtil();
   
   /// copy constructor
   NLPFuncUtil(const NLPFuncUtil &copy);
   
   /// assignment operator
   NLPFuncUtil&   operator=(const  NLPFuncUtil &copy);
   
   /// virtual destructor
   virtual ~NLPFuncUtil();
   

protected:

};
#endif // NLPFuncUtil_hpp