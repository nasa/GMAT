//$Id$
//------------------------------------------------------------------------------
//                              FunctionFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2006/04/05
//
/**
 * Implementation code for the FunctionFactory class, responsible for creating 
 * Function objects.
 */
//------------------------------------------------------------------------------

#include "FunctionFactory.hpp"
#include "StringUtil.hpp"          // for Capitalize()
#include "MessageInterface.hpp"

// include list of Function classes here
#include "GetLastState.hpp"
#include "GetEphemStates.hpp"
#include "SetSeed.hpp"
#include "Pause.hpp"
#include "SystemTime.hpp"
#include "ConvertTime.hpp"
#include "Sign.hpp"
#include "Str2num.hpp"
#include "Num2str.hpp"
#include "RotationMatrix.hpp"
#include "Angle.hpp"
#include "QuaternionProduct.hpp"
#include "QuaternionRotation.hpp"
#include "QuaternionToDCM.hpp"

//#define DEBUG_MATH_FACTORY 1

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Function class 
 *
 * @param <ofType>   the Function object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
Function* FunctionFactory::CreateObject(const std::string &ofType,
                                        const std::string &withName)
{
   return CreateFunction(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateFunction(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Function class 
 *
 * @param <ofType>   the Function object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
Function* FunctionFactory::CreateFunction(const std::string &ofType,
                                      const std::string &withName)
{
   Function *builtinFunction = NULL;
   
   #if DEBUG_MATH_FACTORY
   MessageInterface::ShowMessage
      ("FunctionFactory::CreateFunction() ofType=%s, withName=%s\n",
       ofType.c_str(), withName.c_str());
   #endif
   
   // Create builtin function
   if (ofType == "GetLastState")
      builtinFunction = new GetLastState(ofType, withName);
   else if (ofType == "GetEphemStates")
      builtinFunction = new GetEphemStates(ofType, withName);
   else if (ofType == "SetSeed")
      builtinFunction = new SetSeed(ofType, withName);
   else if (ofType == "Pause")
      builtinFunction = new Pause(ofType, withName);
   else if (ofType == "pause")
      builtinFunction = new Pause(ofType, withName);
   else if (ofType == "SystemTime")
      builtinFunction = new SystemTime(ofType, withName);
   else if (ofType == "ConvertTime")
      builtinFunction = new ConvertTime(ofType, withName);
   else if (ofType == "Sign")
      builtinFunction = new Sign(ofType, withName);
   else if (ofType == "sign")
      builtinFunction = new Sign(ofType, withName);
   else if (ofType == "Str2num")
      builtinFunction = new Str2num(ofType, withName);
   else if (ofType == "str2num")
      builtinFunction = new Str2num(ofType, withName);
   else if (ofType == "Num2str")
      builtinFunction = new Num2str(ofType, withName);
   else if (ofType == "num2str")
      builtinFunction = new Num2str(ofType, withName);
   else if (ofType == "RotationMatrix")
      builtinFunction = new RotationMatrix(ofType, withName);
   else if (ofType == "Angle")
      builtinFunction = new Angle(ofType, withName);
   else if (ofType == "angle")
      builtinFunction = new Angle(ofType, withName);
   else if (ofType == "QuaternionProduct")
      builtinFunction = new QuaternionProduct(ofType, withName);
   else if (ofType == "QuaternionRotation")
      builtinFunction = new QuaternionRotation(ofType, withName);
   else if (ofType == "QuaternionToDCM")
      builtinFunction = new QuaternionToDCM(ofType, withName);

   return builtinFunction;
}


//------------------------------------------------------------------------------
//  FunctionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FunctionFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
FunctionFactory::FunctionFactory()
   : Factory(Gmat::FUNCTION)
{
   isCaseSensitive = true;
   
   if (creatables.empty())
      BuildCreatables();

   GmatType::RegisterType(Gmat::FUNCTION, "Function");
}


//------------------------------------------------------------------------------
//  FunctionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FunctionFactory.
 *
 * @param <createList> list of creatable Function objects
 */
//------------------------------------------------------------------------------
FunctionFactory::FunctionFactory(StringArray createList)
   : Factory(createList, Gmat::FUNCTION)
{
   isCaseSensitive = true;
}


//------------------------------------------------------------------------------
//  FunctionFactory(const FunctionFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FunctionFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
FunctionFactory::FunctionFactory(const FunctionFactory &fact)
   : Factory(fact)
{
   isCaseSensitive = true;
   
   if (creatables.empty())
      BuildCreatables();
}


//------------------------------------------------------------------------------
//  FunctionFactory& operator= (const FunctionFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the FunctionFactory base class.
 *
 * @param <fact> the FunctionFactory object whose data to assign to "this" factory.
 *
 * @return "this" FunctionFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
FunctionFactory& FunctionFactory::operator=(const FunctionFactory &fact)
{
   Factory::operator=(fact);
   isCaseSensitive = true;
   return *this;
}


//------------------------------------------------------------------------------
// ~FunctionFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the FunctionFactory class.
 */
//------------------------------------------------------------------------------
FunctionFactory::~FunctionFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
/**
 * Fills in creatable objects
 */
//------------------------------------------------------------------------------
void FunctionFactory::BuildCreatables()
{
   // The FIRST letter of function name can be either lower or upper case.
   
   creatables.push_back("GetLastState");
   creatables.push_back("GetEphemStates");
   creatables.push_back("SetSeed");
   creatables.push_back("Pause");
   creatables.push_back("pause");
   creatables.push_back("SystemTime");
   creatables.push_back("ConvertTime");
   creatables.push_back("Sign");
   creatables.push_back("sign");
   creatables.push_back("Str2num");
   creatables.push_back("str2num");
   creatables.push_back("Num2str");
   creatables.push_back("num2str");
   creatables.push_back("RotationMatrix");
   creatables.push_back("Angle");
   creatables.push_back("QuaternionProduct");
   creatables.push_back("QuaternionRotation");
   creatables.push_back("QuaternionToDCM");
}

