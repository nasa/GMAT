//------------------------------------------------------------------------------
//                              Strlen
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
 * Declares Strlen class. A class used to find the length of a string.
 */
 //------------------------------------------------------------------------------

#include "Strlen.hpp"
#include "ElementWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_INPUT_OUTPUT
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Strlen()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
 //------------------------------------------------------------------------------
Strlen::Strlen(const std::string& name)
    : StringFunctionNode("Strlen", name)
{
}


//------------------------------------------------------------------------------
// ~Strlen()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
 //------------------------------------------------------------------------------
Strlen::~Strlen()
{
}


//------------------------------------------------------------------------------
//  Strlen(const Strlen &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Strlen object (copy constructor).
 *
 * @param <copy> Object that is copied
 */
 //------------------------------------------------------------------------------
Strlen::Strlen(const Strlen& copy) :
    StringFunctionNode(copy)
{
}

//------------------------------------------------------------------------------
// Strlen& Strlen::operator=(const Strlen& ss)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ss The Strlen object that supplies properties this one needs
 *
 * @return A reference to this instance
 */
 //------------------------------------------------------------------------------
Strlen& Strlen::operator=(const Strlen& sl)
{
    if (this != &sl)
        StringFunctionNode::operator=(sl);
    return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Strlen operation.
 *
 * @return clone of the Strlen operation.
 *
 */
 //------------------------------------------------------------------------------
GmatBase* Strlen::Clone() const
{
    return (new Strlen(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
/**
 * Gets information on to what this class outputs
 *
 * @param type Integer gmat type that is returned
 * @param rowCount Integer number of rows that are returned
 * @param colCount Integer number of columns that are returned
 *
 */
 //------------------------------------------------------------------------------
void Strlen::GetOutputInfo(Integer& type, Integer& rowCount,
    Integer& colCount)
{
#ifdef DEBUG_INPUT_OUTPUT
    MessageInterface::ShowMessage
    ("Strlen::GetOutputInfo() <%p><%s><%s> entered\n", this,
        GetTypeName().c_str(), GetName().c_str());
#endif

    type = Gmat::REAL_TYPE;
    rowCount = 1;
    colCount = 1;

#ifdef DEBUG_INPUT_OUTPUT
    MessageInterface::ShowMessage
    ("Strlen::GetOutputInfo() <%p><%s> leaving, type=%d, "
        "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(),
        type, rowCount, colCount);
#endif
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * Strlen(s1)
 * @returns the length of string s1
 *
 */
 //------------------------------------------------------------------------------
Real Strlen::Evaluate()
{
#ifdef DEBUG_EVALUATE
    MessageInterface::ShowMessage
    ("Strlen::Evaluate() <%p>'%s' entered\n", this, GetName().c_str());
#endif

    if (inputArgWrappers.size() != 1)
        throw MathException(GetTypeName() + "() function requires one input argument");

    ValidateWrappers();

    std::string s1 = inputArgWrappers[0]->EvaluateString();

    Real result;

    result = s1.length();

#ifdef DEBUG_EVALUATE
    MessageInterface::ShowMessage
    ("Strlen::Evaluate() <%p>'%s' returning %f\n", this,
        GetName().c_str(), result);
#endif
    return result;
}

