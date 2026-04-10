//$Id$
//------------------------------------------------------------------------------
//                           Num2str
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Apr 13, 2018
/**
 * 
 */
//------------------------------------------------------------------------------


#ifndef SRC_BASE_FUNCTION_NUM2STR_HPP_
#define SRC_BASE_FUNCTION_NUM2STR_HPP_

#include "BuiltinGmatFunction.hpp"

class GMAT_API Num2str : public BuiltinGmatFunction
{
public:
   Num2str(const std::string &typeStr, const std::string &name);
   virtual ~Num2str();
   Num2str(const Num2str &f);
   Num2str& operator=(const Num2str &f);

   virtual WrapperTypeArray
                        GetOutputTypes(IntegerArray &rowCounts,
                                       IntegerArray &colCounts) const;
   virtual void         SetOutputTypes(WrapperTypeArray &outputTypes,
                                       IntegerArray &rowCounts,
                                       IntegerArray &colCounts);
   virtual bool         Initialize(ObjectInitializer *objInit, bool reinitialize = false);
   virtual bool         Execute(ObjectInitializer *objInit, bool reinitialize = false);
   virtual void         Finalize(bool cleanUp = false);

   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);

protected:

   ElementWrapper*      CreateOutputVariableWrapper(std::string outputStr, const std::string &outName);

};



#endif /* SRC_BASE_FUNCTION_NUM2STR_HPP_ */
