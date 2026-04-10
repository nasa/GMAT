//$Id$
//------------------------------------------------------------------------------
//                                  ExpressionParser
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/15
//
/**
 * Declares ExpressionParser class which parses and evaluates the
 * expression. The valid expressions are composed of the following items:
 *    1. Numbers
 *    2. Operators(+,-,/,*,^)
 *    3. Parentheses
 *    4. Varialbes
 *
 * Assume this precendence of each operator:
 *    highest   + - (unary)
 *              ^ (exponentiation)
 *              * /
 *              + -
 *
 * Parser Code Reference: "The Complete Reference C++" by Herbert Schildt,
 *    4th Edition, P963-997
 *
 */
//------------------------------------------------------------------------------
#ifndef ExpressionParser_hpp
#define ExpressionParser_hpp

#include "gmatdefs.hpp"
#include "ParameterDatabase.hpp"

class GMAT_API ExpressionParser
{
public:
   ExpressionParser();
   ~ExpressionParser();
   
   Real EvalExp(const char *exp);
   void SetParameterDatabase(ParameterDatabase *pdb);
   
private:
   
   static const int NUM_VARS = 26;
   
   enum TokenType
   {
      DELIMITER = 1,
      VARIABLE,
      NUMBER
   };
   
   enum ErrorType
   {
      SYNTAX_ERROR,
      UNBALANCED_PARENTHESES,
      NO_EXPRESSION_PRESENT,
   };

   /// points to the expression
   const char *mExp;
   /// holds current token
   char mToken[80];
   /// holds token's type
   TokenType mTokenType;
   /// holds variable's values
   Real mVars[NUM_VARS];
   /// points to parameter database that holds variables
   ParameterDatabase *mParamDb;
   
   void EvalTwoTerms(Real &result);
   void EvalTwoFactors(Real &result);
   void EvalExponent(Real &result);
   void EvalUnary(Real &result);
   void EvalParenExp(Real &result);
   void GetValue(Real &result);
   Real EvalVariable(char *var);
   void GetToken();
   bool IsDelimiter(char c);
   void HandleSyntaxError(int error);
      
};
#endif // ExpressionParser_hpp

