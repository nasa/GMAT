//$Id$
//------------------------------------------------------------------------------
//                                 StringUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
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
// Created: 2006/1/6
//
/**
 * This file defines string utility functions.
 */
//------------------------------------------------------------------------------
#ifndef StringUtil_hpp
#define StringUtil_hpp

#include "utildefs.hpp"
#include "GmatGlobal.hpp"

namespace GmatStringUtil
{
   enum StripType
   {
      LEADING = 1,
      TRAILING = 2,
      BOTH = 3,
   };
   
   GMATUTIL_API std::string RemoveAll(const std::string &str, char ch, Integer start = 0);
   GMATUTIL_API std::string RemoveAll(const std::string &str, const std::string &removeStr,
                           Integer start = 0);
   GMATUTIL_API std::string RemoveAllBlanks(const std::string &str, bool ignoreSingleQuotes = false);
   GMATUTIL_API std::string RemoveLastNumber(const std::string &str, Integer &lastNumber);
   GMATUTIL_API std::string RemoveLastString(const std::string &str, const std::string &lastStr,
                           bool removeAll = false);
   GMATUTIL_API std::string RemoveSpaceInBrackets(const std::string &str,
                           const std::string &bracketPair);
   GMATUTIL_API std::string RemoveMultipleSpaces(const std::string &str);
   GMATUTIL_API std::string RemoveTrailingZeros(Real val, const std::string &valStr,
                           Integer iterCount = 0);
   GMATUTIL_API std::string RemoveScientificNotation(const std::string &str);
   GMATUTIL_API std::string RemoveMathSymbols(const std::string &str, bool removeMathOperator = false);
   
   GMATUTIL_API std::string PadWithBlanks(const std::string &str, Integer toSize, StripType whichEnd = TRAILING);
   GMATUTIL_API std::string BuildNumber(Real value, bool useExp = false, Integer length = 17);


   GMATUTIL_API std::string Trim(const std::string &str, StripType stype = BOTH,
                           bool removeSemicolon = false, bool removeEol = false);
   GMATUTIL_API std::string Strip(const std::string &str, StripType stype = BOTH);
   GMATUTIL_API std::string ToUpper(const std::string &str, bool firstLetterOnly = false);
   GMATUTIL_API std::string ToLower(const std::string &str, bool firstLetterOnly = false);
   GMATUTIL_API std::string Capitalize(const std::string &str);
   GMATUTIL_API std::string ReplaceFirst(const std::string &str, const std::string &from,
                           const std::string &to, std::string::size_type startIndex = 0);
   GMATUTIL_API std::string Replace(const std::string &str, const std::string &from,
                           const std::string &to, std::string::size_type startIndex = 0);
   GMATUTIL_API std::string ReplaceName(const std::string &str, const std::string &from,
                           const std::string &to);
   GMATUTIL_API std::string ReplaceNumber(const std::string &str, const std::string &from,
                           const std::string &to);
   GMATUTIL_API std::string ReplaceChainedUnaryOperators(const std::string &str);
   
   GMATUTIL_API std::string RealToString(const Real &val, bool useCurrentFormat = true,
      bool scientific = false, bool showPoint = true,
      Integer precision = GmatGlobal::DATA_PRECISION,
      Integer width = GmatGlobal::DATA_WIDTH);
   GMATUTIL_API std::string RealToString(const Real &val, Integer precision, bool showPoint = false,
      Integer width = 1);

   GMATUTIL_API std::string ToString(const bool &val);
   GMATUTIL_API std::string ToString(const Real &val, Integer precision, bool showPoint = false,
                           Integer width = 1);
   GMATUTIL_API std::string ToString(const Integer &val, Integer width);
   
   GMATUTIL_API std::string ToString(const Real &val, bool useCurrentFormat = true,
                           bool scientific = false, bool showPoint = true, 
                           Integer precision = GmatGlobal::DATA_PRECISION,
                           Integer width = GmatGlobal::DATA_WIDTH);
   GMATUTIL_API std::string ToString(const Integer &val, bool useCurrentFormat = true,
                           Integer width = GmatGlobal::INTEGER_WIDTH);
   GMATUTIL_API std::string ToStringNoZeros(const Real &val);
   
   GMATUTIL_API std::string ToOrdinal(Integer i, bool textOnly = false);

   GMATUTIL_API std::string RemoveExtraParen(const std::string &str, bool ignoreComma = false,
                                         bool ignoreSingleQuotes = false);
   GMATUTIL_API std::string RemoveOuterParen(const std::string &str);
   GMATUTIL_API std::string RemoveOuterString(const std::string &str, const std::string &start,
                           const std::string &end);
   GMATUTIL_API std::string RemoveEnclosingString(const std::string &str, const std::string &enStr);
   GMATUTIL_API std::string RemoveInlineComment(const std::string &str, const std::string &cmStr);
   GMATUTIL_API std::string MakeCommentLines(const std::string &str, bool breakAtCr = false);
   GMATUTIL_API std::string ParseFunctionName(const std::string &str, std::string &argStr);
   GMATUTIL_API StringArray ParseFunctionCall(const std::string &str);
   GMATUTIL_API std::string AddEnclosingString(const std::string &str, const std::string &enStr);
   GMATUTIL_API std::string GetInvalidNameMessageFormat();
   
   GMATUTIL_API char GetClosingBracket(const char &openBracket);
   
   GMATUTIL_API StringArray SeparateBrackets(const std::string &chunk,
                          const std::string &bracketPair,
                          const std::string &delim,
                          bool checkOuterBracket = true);
   GMATUTIL_API StringArray SeparateBy(const std::string &str, const std::string &delim,
                           bool putBracketsTogether = false, bool insertDelim = false,
                           bool insertComma = true);
   GMATUTIL_API StringArray SeparateByComma(const std::string &str, bool checkSingleQuote = true);
   GMATUTIL_API StringArray SeparateDots(const std::string &str);
   GMATUTIL_API StringArray DecomposeBy(const std::string &str, const std::string &delim);
   
   GMATUTIL_API bool IsNumber(const std::string &str);
   GMATUTIL_API bool IsValidReal(const std::string &str, Real &value, Integer &errorCode,
                             bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToReal(const char *str, Real *value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToReal(const std::string &str, Real *value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToReal(const char *str, Real &value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToReal(const std::string &str, Real &value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToInteger(const std::string &str, Integer *value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToInteger(const char *str, Integer &value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToInteger(const std::string &str, Integer &value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToUnsignedInt(const std::string &str, UnsignedInt *value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToUnsignedInt(const std::string &str, UnsignedInt &value, bool trimParens = false, bool allowOverflow = true);
   GMATUTIL_API bool ToBoolean(const std::string &str, bool *value, bool trimParens = false);
   GMATUTIL_API bool ToBoolean(const std::string &str, bool &value, bool trimParens = false);
   GMATUTIL_API bool ToOnOff(const std::string &str, std::string &value, bool trimParens = false);
   
   GMATUTIL_API RealArray ToRealArray(const std::string &str, bool allowOverflow = true, bool allowSemicolon = false);
   GMATUTIL_API IntegerArray ToIntegerArray(const char *str, bool allowOverflow = true);
   GMATUTIL_API IntegerArray ToIntegerArray(const std::string &str, bool allowOverflow = true);
   GMATUTIL_API UnsignedIntArray ToUnsignedIntArray(const std::string &str, bool allowOverflow = true);
   GMATUTIL_API StringArray ToStringArray(const std::string &str);
   GMATUTIL_API BooleanArray ToBooleanArray(const std::string &str);

   GMATUTIL_API void ParseParameter(const std::string &str, std::string &type,
                           std::string &owner, std::string &dep);
   GMATUTIL_API void GetArrayCommaIndex(const std::string &str, Integer &comma,
                           const std::string &bracketPair = "()");
   GMATUTIL_API void GetArrayIndexVar(const std::string &str, std::string &rowStr,
                           std::string &colStr, std::string &name,
                           const std::string &bracketPair = "()");
   GMATUTIL_API void GetArrayIndex(const std::string &str, Integer &row, Integer &col,
                           std::string &name, const std::string &bracketPair = "()");
   GMATUTIL_API void GetArrayIndex(const std::string &str, std::string &rowStr,
                           std::string &colStr, Integer &row, Integer &col,
                           std::string &name, const std::string &bracketPair = "()");
   GMATUTIL_API std::string GetArrayName(const std::string &str,
                           const std::string &bracketPair = "()");
   GMATUTIL_API bool IsOneElementArray(const std::string &str);
   GMATUTIL_API bool IsSimpleArrayElement(const std::string &str);
   
   GMATUTIL_API void FindFirstAndLast(const std::string &str, char ch, Integer &first,
                           Integer &last);
   GMATUTIL_API void FindParenMatch(const std::string &str, Integer &open, Integer &close,
                           bool &isOuterParen);
   GMATUTIL_API void FindMatchingParen(const std::string &str, Integer &openParen,
                           Integer &closeParen, bool &isOuterParen,
                           Integer start = 0);
   GMATUTIL_API void FindMatchingBracket(const std::string &str, Integer &openBracket,
                           Integer &closeBracket, bool &isOuterBracket,
                           const std::string &bracket, Integer start = 0);
   GMATUTIL_API void FindLastParenMatch(const std::string &str, Integer &openParen,
                           Integer &closeParen, Integer start = 0);
   
   GMATUTIL_API bool IsEnclosedWith(const std::string &str, const std::string &enclosingStr);
   GMATUTIL_API bool IsEnclosedWithExtraParen(const std::string &str, bool checkOps = true,
                                          bool ignoreComma = false);
   GMATUTIL_API bool IsEnclosedWithBraces(const std::string &str);
   GMATUTIL_API bool IsEnclosedWithBrackets(const std::string &str);
   GMATUTIL_API bool IsBracketBalanced(const std::string &str, const std::string &bracketPair);
   GMATUTIL_API bool IsParenBalanced(const std::string &str);
   GMATUTIL_API bool IsParenEmpty(const std::string &str);
   GMATUTIL_API bool AreAllBracketsBalanced(const std::string &str, const std::string &allPairs);
   GMATUTIL_API bool AreAllNamesValid(const std::string &str, bool blankNameIsOk = false);
   GMATUTIL_API bool IsValidFunctionCall(const std::string &str);
   GMATUTIL_API bool IsOuterParen(const std::string &str);
   GMATUTIL_API bool IsCommaPartOfArray(const std::string &str, Integer start = 0);
   GMATUTIL_API bool IsBracketPartOfArray(const std::string &str, const std::string &bracketPairs,
                           bool checkOnlyFirst);
   GMATUTIL_API bool IsParenPartOfArray(const std::string &str);
   GMATUTIL_API bool IsThereEqualSign(const std::string &str);
   GMATUTIL_API bool IsThereMathSymbol(const std::string &str);
   GMATUTIL_API bool HasNoBrackets(const std::string &str, bool parensForArraysAllowed = true);
   GMATUTIL_API bool IsSingleItem(const std::string &str);
   GMATUTIL_API bool StartsWith(const std::string &str, const std::string &value);
   GMATUTIL_API bool EndsWith(const std::string &str, const std::string &value);
   GMATUTIL_API bool EndsWithPathSeparator(const std::string &str);
   GMATUTIL_API bool IsValidNumber(const std::string &str, bool allowOverflow = true);
   GMATUTIL_API bool IsValidName(const char *str, bool ignoreBracket = false,
                             bool blankNameIsOk = false);
   GMATUTIL_API bool IsValidName(const std::string &str, bool ignoreBracket = false,
                             bool blankNameIsOk = false);
   GMATUTIL_API bool IsValidParameterName(const std::string &str);
   GMATUTIL_API bool IsLastNumberPartOfName(const std::string &str);
   GMATUTIL_API bool IsBlank(const std::string &str, bool ignoreEol = false);
   GMATUTIL_API bool HasMissingQuote(const std::string &str, const std::string &quote,
                                 bool ignoreSpaceAfterQuote = true);
   GMATUTIL_API bool IsStringInsideSymbols(const std::string &str, const std::string &reqStr,
                                       const std::string &symbol, std::string::size_type &reqStrPos);
   GMATUTIL_API bool IsMathEquation(const std::string &str, bool checkInvalidOpOnly = false,
                                bool blankNameIsOk = false);
   GMATUTIL_API bool IsMathOperator(const char &ch);
   
   GMATUTIL_API Integer NumberOfOccurrences(const std::string &str, const char c);
   GMATUTIL_API Integer NumberOfScientificNotation(const std::string &str);
   
   GMATUTIL_API StringArray GetVarNames(const std::string &str);
   GMATUTIL_API void WriteStringArray(const StringArray &strArray,
                                  const std::string &desc = "",
                                  const std::string &prefix = "");

   GMATUTIL_API bool IsValidIdentity(const std::string &str);
   GMATUTIL_API bool IsValidExtendedIdentity(const std::string &str);
   GMATUTIL_API bool IsValidFileName(const std::string &str);
   GMATUTIL_API bool IsValidFullFileName(const std::string &str, Integer &error);

   GMATUTIL_API StringArray ParseName(const std::string& extendedName);

   enum AlignmentType
   {
      LEFT,
      RIGHT,
      CENTER,
   };
   
   GMATUTIL_API std::string GetAlignmentString(const std::string inputString, UnsignedInt len, AlignmentType alignment = LEFT);
   
   // Wide string handling
   GMATUTIL_API std::wstring StringToWideString(const std::string &str);
   GMATUTIL_API std::string  WideStringToString(const std::wstring &wstr);
   GMATUTIL_API std::string  WideStringToString(const wchar_t *wchar);
}

#endif // StringUtil_hpp

