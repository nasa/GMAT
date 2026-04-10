//$Id$
//------------------------------------------------------------------------------
//                              MathFactory
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
 * Implementation code for the MathFactory class, responsible for creating 
 * MathNode objects.
 */
//------------------------------------------------------------------------------

#include "MathFactory.hpp"
#include "StringUtil.hpp"          // for Capitalize()
#include "MessageInterface.hpp"

// include list of MathNode classes here
#include "MathElement.hpp"
#include "Abs.hpp"
#include "Acos.hpp"
#include "Asin.hpp"
#include "Atan.hpp"
#include "Atan2.hpp"
#include "Add.hpp"
#include "Ceil.hpp"
#include "Cos.hpp"
#include "Cross3.hpp"
#include "DegToRad.hpp"
#include "Determinant.hpp"
#include "Diag.hpp"
#include "Divide.hpp"
#include "Exp.hpp"
#include "Fix.hpp"
#include "Floor.hpp"
#include "Inverse.hpp"
#include "Log.hpp"
#include "Log10.hpp"
#include "Min.hpp"
#include "Mod.hpp"
#include "Multiply.hpp"
#include "Negate.hpp"
#include "Norm.hpp"
#include "Power.hpp"
#include "RadToDeg.hpp"
#include "Rand.hpp"
#include "Randn.hpp"
#include "Sin.hpp"
#include "Sprintf.hpp"
#include "Sqrt.hpp"
#include "Strcat.hpp"
#include "Strcmp.hpp"
#include "Strfind.hpp"
#include "Strlen.hpp"
#include "Strrep.hpp"
#include "Substr.hpp"
#include "Subtract.hpp"
#include "Tan.hpp"
#include "Transpose.hpp"
#include "FunctionRunner.hpp"      // for inline GmatFunction

//#define DEBUG_MATH_FACTORY 1

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested MathNode class 
 *
 * @param <ofType>   the MathNode object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
GmatBase* MathFactory::CreateObject(const std::string &ofType,
                                    const std::string &withName)
{
   return CreateMathNode(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateMathNode(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested MathNode class 
 *
 * @param <ofType>   the MathNode object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
MathNode* MathFactory::CreateMathNode(const std::string &ofType,
                                      const std::string &withName)
{
   MathNode *mathNode = NULL;

   // The FIRST letter of function name can be either lower or upper case,
   // so capitalize the first letter of the type before checking.
   // eg) cos, Cos, add, Add.
   
   std::string newType = GmatStringUtil::Capitalize(ofType);
   
   #if DEBUG_MATH_FACTORY
   MessageInterface::ShowMessage
      ("MathFactory::CreateMathNode() ofType=%s, newType=%s, withName=%s\n",
       ofType.c_str(), newType.c_str(), withName.c_str());
   #endif
   
   // Leaf node
   if (ofType == "MathElement")
      mathNode = new MathElement(ofType, withName);
   
   // Simple math operations
   else if (newType == "Add")         // Add(x,y) or x+y
      mathNode = new Add(withName);
   else if (newType == "Subtract")    // Subtract(x,y) or x-y
      mathNode = new Subtract(withName);
   else if (newType == "Multiply")    // Multiply(x,y) or x*y
      mathNode = new Multiply(withName);
   else if (newType == "Divide")      // Divide(x,y) or x/y
      mathNode = new Divide(withName);
   else if (newType == "Negate")      // Negate(x)
      mathNode = new Negate(withName);
   else if (newType == "Sqrt")        // Sqrt(x)
      mathNode = new Sqrt(withName);
   else if (newType == "Abs")         // Abs(x)
      mathNode = new Abs(withName);
   
   // Real math functions
   else if (newType == "Ceil")        // ceil(x)
      mathNode = new Ceil(withName);
   else if (newType == "Exp")         // exp(x)
      mathNode = new Exp(withName);
   else if (newType == "Floor")       // floor(x)
      mathNode = new Floor(withName);
   else if (newType == "Fix")         // fix(x)
      mathNode = new Fix(withName);
   else if (newType == "Log")         // log(x)
      mathNode = new Log(withName);
   else if (newType == "Log10")       // log10(x)
      mathNode = new Log10(withName);
   else if (newType == "Min")         // Min(a1, .., an)
      mathNode = new Min(withName);
   else if (newType == "Mod")         // mod(x)
      mathNode = new Mod(withName);
   else if (newType == "Power")       // power(x,y) or x^y
      mathNode = new Power(withName);
   
   // Matrix, Vector functions
   else if (newType == "Cross")       // cross(a, b)
      mathNode = new Cross3(withName);
   else if (newType == "Det")         // det(m)
      mathNode = new Determinant(withName);
   else if (newType == "Diag")        // diag([n1 n2 n3, ...])
      mathNode = new Diag(withName);
   else if (newType == "Inv")         // inv(m)
      mathNode = new Inverse(withName);
   else if (newType == "Norm")        // norm(m)
      mathNode = new Norm(withName);
   else if (newType == "Rand")        // rand(m)
      mathNode = new Rand(withName);
   else if (newType == "Randn")       // randn(m)
      mathNode = new Randn(withName);
   else if (newType == "Transpose")   // transpose(m) or m'
      mathNode = new Transpose(withName);
   
   // Trigonometric functions
   else if (newType == "Sin")         // sin(x)
      mathNode = new Sin(withName);
   else if (newType == "Cos")         // cos(x)
      mathNode = new Cos(withName);
   else if (newType == "Tan")         // tan(x)
      mathNode = new Tan(withName);
   else if (newType == "Asin")        // asin(x)
      mathNode = new Asin(withName);
   else if (newType == "Acos")        // acos(x)
      mathNode = new Acos(withName);
   else if (newType == "Atan")        // atan(x)
      mathNode = new Atan(withName);
   else if (newType == "Atan2")       // atan2(y,x)
      mathNode = new Atan2(withName);
   
   // Unit conversion functions
   else if (newType == "DegToRad" || newType == "Deg2Rad")
      mathNode = new DegToRad(withName);
   else if (newType == "RadToDeg" || newType == "Rad2Deg")
      mathNode = new RadToDeg(withName);
   
   else if (newType == "FunctionRunner")
      mathNode = new FunctionRunner(withName);
   
   // String functions
   else if (newType == "Sprintf")
      mathNode = new Sprintf(withName);
   else if (newType == "Strcat")
      mathNode = new Strcat(withName);
   else if (newType == "Strcmp")
      mathNode = new Strcmp(withName);
   else if (newType == "Strfind")
      mathNode = new Strfind(withName);
   else if (newType == "Strrep")
      mathNode = new Strrep(withName);
   else if (newType == "Strlen")
       mathNode = new Strlen(withName);
   else if (newType == "Substr")
       mathNode = new Substr(withName);
   
   return mathNode;
}


//------------------------------------------------------------------------------
//  MathFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MathFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
MathFactory::MathFactory()
   : Factory(Gmat::MATH_NODE)
{
   isCaseSensitive = true;
   
   if (creatables.empty())
      BuildCreatables();

   GmatType::RegisterType(Gmat::MATH_NODE, "MathNode");
   GmatType::RegisterType(Gmat::MATH_TREE, "MathTree");
}


//------------------------------------------------------------------------------
//  MathFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MathFactory.
 *
 * @param <createList> list of creatable MathNode objects
 */
//------------------------------------------------------------------------------
MathFactory::MathFactory(StringArray createList)
   : Factory(createList, Gmat::MATH_NODE)
{
   isCaseSensitive = true;
}


//------------------------------------------------------------------------------
//  MathFactory(const MathFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MathFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
MathFactory::MathFactory(const MathFactory &fact)
   : Factory(fact)
{
   isCaseSensitive = true;
   
   if (creatables.empty())
      BuildCreatables();
}


//------------------------------------------------------------------------------
//  MathFactory& operator= (const MathFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the MathFactory base class.
 *
 * @param <fact> the MathFactory object whose data to assign to "this" factory.
 *
 * @return "this" MathFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
MathFactory& MathFactory::operator=(const MathFactory &fact)
{
   Factory::operator=(fact);
   isCaseSensitive = true;
   return *this;
}


//------------------------------------------------------------------------------
// ~MathFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the MathFactory class.
 */
//------------------------------------------------------------------------------
MathFactory::~MathFactory()
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
void MathFactory::BuildCreatables()
{
   // The FIRST letter of function name can be either lower or upper case.
   
   // Math element
   creatables.push_back("MathElement");
   
   // Simple math operations
   creatables.push_back("Add");
   creatables.push_back("Divide");
   creatables.push_back("Multiply");
   creatables.push_back("Negate");
   creatables.push_back("Subtract");
   
   // Real math functions
   creatables.push_back("Abs");
   creatables.push_back("Ceil");
   creatables.push_back("Exp");
   creatables.push_back("Fix");
   creatables.push_back("Floor");
   creatables.push_back("Log");
   creatables.push_back("Log10");
   creatables.push_back("Min");
   creatables.push_back("Mod");
   creatables.push_back("Power");
   creatables.push_back("Sqrt");
   
   // Matrix and vector functions
   creatables.push_back("Cross");
   creatables.push_back("Det");
   creatables.push_back("Diag");
   creatables.push_back("Inv");
   creatables.push_back("Norm");
   creatables.push_back("Rand");
   creatables.push_back("Randn");
   creatables.push_back("Transpose");
   
   // Trigonometric functions
   creatables.push_back("Sin");
   creatables.push_back("Cos");
   creatables.push_back("Tan");
   creatables.push_back("Asin");
   creatables.push_back("Acos");
   creatables.push_back("Atan");
   creatables.push_back("Atan2");
   
   // Unit conversion functions
   creatables.push_back("DegToRad");  
   creatables.push_back("RadToDeg");
   creatables.push_back("Deg2Rad");  
   creatables.push_back("Rad2Deg");
   
   // String functions
   creatables.push_back("Sprintf"); // For formatted string output using sprintf function
   creatables.push_back("Strcat");  // For string concatenation eg, strcat(a, b, 'string literal')
   creatables.push_back("Strcmp");  // For string comparison eg, strcmp(a, 'string literal')
   creatables.push_back("Strfind"); // For string find eg, strfind(a, 'string literal')
   creatables.push_back("Strrep");  // For string replacement eg, strrep(a, 'string literal')
   creatables.push_back("Substr");  // For substring eg, substr(a, start, end=strlen)
   creatables.push_back("Strlen");  // For string length eg, strlen(a)
   
   // GmatFunction
   creatables.push_back("FunctionRunner");  
}

