//------------------------------------------------------------------------------
//                              Substr
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
// Developed jointly by NASA/GSFC and Pearl River Technologies
//
// Author: Peter Candell
// Created: 2024.07.10
//
/**
 * Declares Substr class.
 */
 //------------------------------------------------------------------------------

#include "Substr.hpp"
#include "MathException.hpp"
#include "ElementWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Substr()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
 //------------------------------------------------------------------------------
Substr::Substr(const std::string& name)
    : StringFunctionNode("Substr", name)
{
}


//------------------------------------------------------------------------------
// ~Substr()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
 //------------------------------------------------------------------------------
Substr::~Substr()
{
}


//------------------------------------------------------------------------------
//  Substr(const Substr &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Substr object (copy constructor).
 *
 * @param <copy> Object that is copied
 */
 //------------------------------------------------------------------------------
Substr::Substr(const Substr& copy) :
    StringFunctionNode(copy)
{
}

//------------------------------------------------------------------------------
// Substr& Substr::operator=(const Substr& ss)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ss The Substr object that supplies properties this one needs
 *
 * @return A reference to this instance
 */
 //------------------------------------------------------------------------------
Substr& Substr::operator=(const Substr& ss)
{
    if(this != &ss)
        StringFunctionNode::operator=(ss);
    return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Substr operation.
 *
 * @return clone of the Substr operation.
 *
 */
 //------------------------------------------------------------------------------
GmatBase* Substr::Clone() const
{
    return (new Substr(*this));
}

//------------------------------------------------------------------------------
// std::string EvaluateString()
//------------------------------------------------------------------------------
/**
 *  Substr(String s1, int start, int end = strlen)
 * Takes in a string, start, and end. If no end, end = strlen
 * Returns a substring of the string starting at index start and ending at 
 * index end. substring goes from [start, end]
 * 
 *
 */
 //------------------------------------------------------------------------------
std::string Substr::EvaluateString()
{
#ifdef DEBUG_EVALUATE
    MessageInterface::ShowMessage
    ("Substr::EvaluateString() <%p>'%s' entered\n", this, GetName().c_str());
#endif
    std::string result;
    std::string s1;
    int start;
    int end;

    //Running all of these checks are slow, we could instead just put the functionality inside a try, catch and throw an error...

    BuiltinFunctionNode::ValidateWrappers();

    if (inputArgWrappers.size() == 2 || inputArgWrappers.size() == 3)
    {

        if (inputArgWrappers[0]->GetDataType() != Gmat::STRING_TYPE)
            throw MathException(GetTypeName() + "() function requires two or three input arguments. Issue found with s1. substr(String s1, int start, int end = strlen).\n");

        s1 = inputArgWrappers[0]->EvaluateString();
        
        if(inputArgWrappers[1]->GetDataType() == Gmat::INTEGER_TYPE)
            start = inputArgWrappers[1]->EvaluateInteger();
        else if (inputArgWrappers[1]->GetDataType() == Gmat::REAL_TYPE)
            start = inputArgWrappers[1]->EvaluateReal();
        else
            throw MathException(GetTypeName() + "() function requires two or three input arguments. Issue found with start. substr(String s1, int start, int end = strlen).\n");

        if (inputArgWrappers.size() == 3)
        {
            if (inputArgWrappers[2]->GetDataType() == Gmat::INTEGER_TYPE)
                end = inputArgWrappers[2]->EvaluateInteger();
            else if (inputArgWrappers[2]->GetDataType() == Gmat::REAL_TYPE)
                end = inputArgWrappers[2]->EvaluateReal();
            else
                throw MathException(GetTypeName() + "() function requires two or three input arguments. Issue found with end. substr(String s1, int start, int end = strlen).\n");
        }
        else
            end = s1.length();
    }
    else
    {
        throw MathException(GetTypeName() + "() function requires two or three input arguments. String s1, int start, int end = strlen. \n");
    }

    if(start >= s1.length())
        throw MathException(GetTypeName() + "() error: start index must be less than the length of the string. \n");

    if (start >= end)
        throw MathException(GetTypeName() + "() error: start index must be less than end index. \n");

    result = s1.substr(start, end-start);

#ifdef DEBUG_EVALUATE
    MessageInterface::ShowMessage
    ("Substr::EvaluateString() <%p>'%s' returning '%s'\n", this,
        GetName().c_str(), result.c_str());
#endif
    return result;
}



