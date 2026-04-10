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
 * This file provides string utility functions.
 */
//------------------------------------------------------------------------------

#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"
#include "GmatGlobal.hpp"
#include "Linear.hpp"            // for ToString()
#include "StringTokenizer.hpp"   // for StringTokenizer()
#include <map>
#include <algorithm>
#include <stdio.h>               // for sprintf()
#include <cstdlib>               // Required for GCC 4.3
#include <algorithm>             // Required for GCC 4.3
#include <sstream>
#include <cerrno>
#include <limits.h>
#include <wchar.h>

//#define DEBUG_STRING_UTIL 1
//#define DEBUG_TOREAL
//#define DEBUG_ARRAY_INDEX 2
//#define DEBUG_STRING_UTIL_ARRAY 1
//#define DEBUG_STRING_UTIL_SEP 2
//#define DEBUG_SEP_BRACKETS 1
//#define DEBUG_NO_BRACKETS
//#define DEBUG_BALANCED_BRACKETS
//#define DEBUG_MATH_EQ 2
//#define DEBUG_VALID_NAME
//#define DEBUG_VALID_NUMBER
//#define DEBUG_STRING_UTIL_SEP_COMMA
//#define DEBUG_STRING_UTIL_STRING_ARRAY
//#define DEBUG_REPLACE
//#define DEBUG_REPLACE_NAME
//#define DEBUG_EXTRA_PAREN 1
//#define DEBUG_REMOVE_EXTRA_PAREN 1
//#define DEBUG_REMOVE_MATH_SYMBOLS
//#define DEBUG_NUMBER_WITH_NAME
//#define DEBUG_SCI_NOTATION
//#define DEBUG_SINGLE_ITEM 1
//#define DEBUG_MATH_SYMBOL 1
//#define DEBUG_STRING_INSIDE
//#define DEBUG_PARSE_EQUATION
//#define DEBUG_FUNCTION_NAME
//#define DEBUG_FIND_MATCHING_BRACKET

//------------------------------------------------------------------------------
// std::string RemoveAll(const std::string &str, char ch, Integer start = 0)
//------------------------------------------------------------------------------
/**
 * Removes all occurrance of input ch starting from index start.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveAll(const std::string &str, char ch,
                                      Integer start)
{
   std::string str2 = str;

   std::string::iterator iter = str2.begin();
   for (int i=0; i<start; i++)
      iter++;

   while (iter != str2.end())
   {
      if (*iter == ch)
         str2.erase(iter);
      else
         ++iter;
   }

   return str2;
}


//------------------------------------------------------------------------------
// std::string RemoveAll(const std::string &str, const std::string &removeStr, Integer start = 0)
//------------------------------------------------------------------------------
/**
 * Removes all occurance of characters in removeStr starting at start.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveAll(const std::string &str, const std::string &removeStr,
                                      Integer start)
{
   std::string str1 = str.substr(start);
   std::string::size_type removeLength = removeStr.size();
   
   for (UnsignedInt i = 0; i < removeLength; i++)
      str1 = RemoveAll(str1, removeStr[i]);
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveAllBlanks(const std::string &str, bool ignoreSingleQuotes = false)
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveAllBlanks(const std::string &str, bool ignoreSingleQuotes)
{
   std::string str1 = Trim(str);
   if (!ignoreSingleQuotes && IsEnclosedWith(str1, "'"))
      return str1;
   else
      return RemoveAll(str1, ' ');
}


//------------------------------------------------------------------------------
// std::string RemoveLastNumber(const std::string &str, Integer &lastNumber)
//------------------------------------------------------------------------------
/* This method returns string without numbers appended to string.
 * It will set lastNumber to 0 if there is no number appended or real number.
 *
 * @param str input string
 * @param lastNumber the number appended to string to output
 *
 * @return string without last number if number found
 *
 * For example,
 *    justString will return justString and set lastNumber to 0
 *    someString123 will return someString and set lastNumber to 123
 *    some1String2 will return some1String and set lastNumber to 2
 *    someString(123.567 will return someString( and set lastNumber to 0
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveLastNumber(const std::string &str,
                                             Integer &lastNumber)
{
   #ifdef DEBUG_REMOVE_LAST_NUMBER
   MessageInterface::ShowMessage
      ("GmatStringUtil::RemoveLastNumber() entered, str='%s'\n", str.c_str());
   #endif
   
   lastNumber = 0;
   // Separate name and numbers
   // (LOJ: added dot(.) for real number handling to fix GMT-3285)
   UnsignedInt index = str.find_last_not_of("0123456789.");
   if (index == str.size())
      return str;
   
   std::string str1 = str.substr(index+1);
   
   #ifdef DEBUG_REMOVE_LAST_NUMBER
   MessageInterface::ShowMessage("   Numbers='%s'\n", str1.c_str());
   #endif
   
   if (!ToInteger(str1, lastNumber))
      lastNumber = 0;
   
   str1 = str.substr(0, index+1);
   
   #ifdef DEBUG_REMOVE_LAST_NUMBER
   MessageInterface::ShowMessage
      ("GmatStringUtil::RemoveLastNumber() returning, str='%s', number=%d\n",
       str1.c_str(), lastNumber);
   #endif
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveLastString(const std::string &str, const std::string &lastStr)
//------------------------------------------------------------------------------
/* This method returns last string stripped off from the given input string if
 * last string found
 *
 * @param str input string
 * @param lastStr string to strip off
 * @param removeAll set to true if lastStr is recursively removed
 *
 * @return string without last string if last string exist
 *
 * For example, if lastStr is XXX and removeAll is false
 *    inputStringXXX will return inputString
 *    inputStringXXXXXX will return inputStringXXX
 *    inputStringXXX will return inputStringXXX
 * if lastStr is XXX and removeAll is true
 *    inputStringXXX will return inputString
 *    inputStringXXXXXX will return inputString
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveLastString(const std::string &str,
                                             const std::string &lastStr,
                                             bool removeAll)
{
   std::string str1 = str;

   if (EndsWith(str1, lastStr))
      str1 = str1.substr(0, str1.size() - lastStr.size());

   if (removeAll)
   {
      if (EndsWith(str1, lastStr))
         return RemoveLastString(str1, lastStr, removeAll);
   }

   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveSpaceInBrackets(const std::string &str,
//                                   const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * Removes spaces in array indexing.
 *   "A( 3, 3)  B(1  ,1)" gives "A(3,3)  B(1,1)"
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveSpaceInBrackets(const std::string &str,
                                                  const std::string &bracketPair)
{
   // Removed extra space in the array indexing, between bracketPair (), [], {}
   std::string str1;
   std::string::size_type index1 = 0;
   std::string::size_type closeParen;
   std::string::size_type length = str.size();
   std::string closeStr = bracketPair.substr(1,1);

   while (index1 < length)
   {
      if (str[index1] == bracketPair[0])
      {
         str1.push_back(str[index1]);
         closeParen = str.find(bracketPair[1], index1);

         if (closeParen == str.npos)
            throw UtilityException
               ("Closing bracket \"" + closeStr + "\" not found");

         // find close paren and copy non-blank char
         for (UnsignedInt j=index1+1; j<=closeParen; j++)
            if (str[j] != ' ')
               str1.push_back(str[j]);

         index1 = closeParen+1;
      }
      else
      {
         str1.push_back(str[index1++]);
      }
   }

   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveMultipleSpaces(const std::string &str)
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveMultipleSpaces(const std::string &str)
{
   std::string str1 = Trim(str);
   
   if (str1 == "")
      return str1;
   
   std::string::iterator begin = str1.begin();
   std::string::iterator pos = str1.begin();
   Integer dist = 0, length = str1.size();
   
   while (pos != str1.end())
   {
      dist = distance(begin, pos);
      length = str1.size();
      if (*pos == ' ')
      {
         if (dist < length)
         {
            if (str1[dist+1] == ' ')
               str1.erase(pos);
            else
               ++pos;
         }
      }
      else
         ++pos;
   }
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveTrailingZeros(Real val, const std::string &valStr)
//------------------------------------------------------------------------------
/**
 * Removes trailing zeros from real number string. It goes through another
 * iteration using less precisoin format for numbers ending .999999.
 *
 * @param val  Real number
 * @param valStr String of real number to remove trailing zeros
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveTrailingZeros(Real val, const std::string &valStr,
                                                Integer iterCount)
{
   #ifdef DEBUG_TRAILING_ZEROS
   MessageInterface::ShowMessage
      ("RemoveTrailingZeros() entered, val = %f, valStr = %s, iterCount = %d\n",
       val, valStr.c_str(), iterCount);
   #endif
   
   if (iterCount > 1)
      return valStr;
   
   std::string str = valStr;
   std::string str1;
   std::string::size_type point = str.find_last_of(".");
   std::string::size_type lastNonZero = str.find_last_not_of("0");
   
   if (point == lastNonZero)
   {
      str1 = str.substr(0, lastNonZero + 2);
   }
   else if (lastNonZero == str.size() - 1)
   {
      // use less precision format for case ending with .9999999
      char buffer[50];
      sprintf(buffer, "%.*f", 12 - iterCount, val);
      str1 = buffer;
      
      #ifdef DEBUG_TRAILING_ZEROS
      MessageInterface::ShowMessage
         ("   Used .%df format, str1 = %s\n", 12-iterCount, str1.c_str());
      #endif
      
      if (iterCount < 2)
         str1 = RemoveTrailingZeros(val, str1, ++iterCount);
   }
   else
   {
      str1 = str.substr(0, lastNonZero + 1);
   }
   
   return str1;
}


//------------------------------------------------------------------------------
//std::string RemoveScientificNotation(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Replaces scientific notations with zeros. If string contains equal sign,
 * it replaces scientific notations in right hand side of the first equal sign
 * to zeros.
 * For example, 1.5e+10 to 1.50010, 1.5E-11 to 1.50011.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveScientificNotation(const std::string &str)
{
   #ifdef DEBUG_SCI_NOTATION
   MessageInterface::ShowMessage
      ("GmatStringUtil::RemoveScientificNotation() entered, str=<%s>\n", str.c_str());
   #endif
   
   if (NumberOfScientificNotation(str) == 0)
   {
      #ifdef DEBUG_SCI_NOTATION
      MessageInterface::ShowMessage
         ("GmatStringUtil::() RemoveScientificNotation() no scitific notation "
          "found so returning str=<%s>\n", str.c_str());
      #endif
      return str;
   }
   
   // We don't want to remove scientific notation in order to keep size
   // of the string (for GMT-4417 fix) LOJ: 2014.03.24
   #if 0
   // Remove E+E-e+e- first
   std::string str1 = Replace(str, "E+", "``");
   str1 = Replace(str1, "E-", "``");
   str1 = Replace(str1, "e+", "``");
   str1 = Replace(str1, "e-", "``");
   str1 = RemoveAll(str1, '`');
   #else
   std::string prepend1, prepend2;
   std::string equalSign;
   std::string str1 = str;
   std::string::size_type posEqual = str1.find_first_of('=');
   if (posEqual != str1.npos)
   {
      equalSign = "=";
      prepend1 = str1.substr(0, posEqual);
      str1 = str1.substr(posEqual + 1);
      // Find first non blank
      std::string::size_type firstNonBlank = str1.find_first_not_of(' ');
      if (firstNonBlank != str1.npos)
      {
         prepend2 = str1.substr(0, firstNonBlank);
         str1 = str1.substr(firstNonBlank);
      }
   }
   
   #ifdef DEBUG_SCI_NOTATION
   MessageInterface::ShowMessage
      ("   prepend1=<%s>, prepend2=<%s>, str1=<%s>\n", prepend1.c_str(), prepend2.c_str(),
       str1.c_str());
   #endif
   
   // Replace E+E-e+e- with zeros starting from index 1 since
   // first e or E can be a variable name
   str1 = Replace(str1, "E+", "00", 1);
   str1 = Replace(str1, "E-", "00", 1);
   str1 = Replace(str1, "e+", "00", 1);
   str1 = Replace(str1, "e-", "00", 1);   
   #endif
   
   #ifdef DEBUG_SCI_NOTATION
   MessageInterface::ShowMessage("   After removing <E+E-e+e->, str=<%s>\n", str1.c_str());
   #endif
   
   // Do I need this part?
   // We don't need to remove E or e if it has no + or - sign (LOJ: 2014.03.24)
   #if 0
   // Remove E or e without + or - sign, such as 5e7
   std::string::iterator begin = str1.begin();
   std::string::iterator pos = str1.begin();
   Integer dist = 0, size = str1.length();
   char currCh;
   while (pos != str1.end())
   {
      dist = distance(begin, pos);
      currCh = str1[dist];
      #ifdef DEBUG_SCI_NOTATION_MORE
      MessageInterface::ShowMessage("   currCh='%c', dist=%d\n", currCh, dist);
      #endif
      if (dist > 0 && dist < size)
      {
         if (currCh == 'E' || currCh == 'e')
         {
            std::string str2 = str1.substr(0, dist);
            StringArray parts = SeparateBy(str2, " =(),+-*/^'$");
            Integer numParts = parts.size();
            std::string str3 = parts[numParts-1];
            
            #ifdef DEBUG_SCI_NOTATION_MORE
            MessageInterface::ShowMessage("   str2=<%s>\n", str2.c_str());
            MessageInterface::ShowMessage("   There are %d parts\n", numParts);
            for (int i = 0; i < numParts; i++)
               MessageInterface::ShowMessage("      %s\n", parts[i].c_str());
            MessageInterface::ShowMessage
               ("   str3=<%s>, IsNumber=%d\n", str3.c_str(), IsNumber(str3));
            #endif
            
            if (IsNumber(str3) && isdigit(str1[dist+1]))
               str1.erase(pos);
            else
               ++pos;
         }
         else
            ++pos;
      }
      else
         ++pos;
   }
   #endif

   
   #ifdef DEBUG_SCI_NOTATION
   MessageInterface::ShowMessage
      ("   prepend1=<%s>, equalSign=<%s>, prepend2=<%s>, str1=<%s>\n",
       prepend1.c_str(), equalSign.c_str(), prepend2.c_str(), str1.c_str());
   #endif
   
   // Prepend any saved strings
   str1 = prepend1 + equalSign + prepend2 + str1;
   
   #ifdef DEBUG_SCI_NOTATION
   MessageInterface::ShowMessage
      ("GmatStringUtil::RemoveScientificNotation() returning <%s>\n", str1.c_str());
   #endif
   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveMathSymbols(const std::string &str, bool removeMathOperator = false)
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveMathSymbols(const std::string &str, bool removeMathOperator)
{
   #ifdef DEBUG_REMOVE_MATH_SYMBOLS
   MessageInterface::ShowMessage
      ("GmatStringUtil::RemoveMathSymbols() entered, str=<%s>, removeMathOperator=%d\n",
       str.c_str(), removeMathOperator);
   #endif
   
   if (str.find_first_of(" (),*/+-^'eE") == str.npos)
   {
      #ifdef DEBUG_REMOVE_MATH_SYMBOLS
      MessageInterface::ShowMessage
         ("GmatStringUtil::RemoveMathSymbols() no math symbols found so returning str=<%s>\n",
          str.c_str());
      #endif
      return str;
   }
   
   // Remove scientific notation first
   std::string str1 = RemoveScientificNotation(str);
   #ifdef DEBUG_REMOVE_MATH_SYMBOLS
   MessageInterface::ShowMessage("   After removing <(),._>, str=<%s>\n", str1.c_str());
   #endif
   
   if (removeMathOperator)
   {
      #ifdef DEBUG_REMOVE_MATH_SYMBOLS
      MessageInterface::ShowMessage("   Now removing math operators\n");
      #endif
      std::string::iterator begin = str1.begin();
      std::string::iterator pos = str1.begin();
      Integer dist = 0;
      while (pos != str1.end())
      {
         dist = distance(begin, pos);
         if (IsMathOperator(str1[dist]))
            str1.erase(pos);
         else
            ++pos;
      }
   }
   
   #ifdef DEBUG_REMOVE_MATH_SYMBOLS
   MessageInterface::ShowMessage
      ("GmatStringUtil::RemoveMathSymbols() returning, str=<%s>\n",
       str1.c_str());
   #endif
   
   return str1;
}

//------------------------------------------------------------------------------
// std::string PadWithBlanks(const std::string &str, Integer toSize,
//                           StripType whichEnd = TRAILING)
//------------------------------------------------------------------------------
/*
 * Pads leading or trailing end of string with blanks.
 *
 * @param  str        input string to be padded
 * @param  toSize     total length desired
 * @param  whichEnd   option of padding front or end
 *
 * @note if the input string is already of size toSize or less, it will just
 *       return the string as is
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::PadWithBlanks(const std::string &str, Integer toSize,
                            StripType whichEnd)
{
   Integer     len     = (Integer) str.length();
   std::string blanks("");
   for (Integer ii= 0; ii < (toSize - len);  ii++)
      blanks += " ";
   if (whichEnd == LEADING)
   {
      return blanks + str;
   }
   else if (whichEnd == TRAILING)
   {
      return str + blanks;
   }
   else // ignore "BOTH" for now
   {
      return str;
   }
}

//------------------------------------------------------------------------------
// const std::string BuildNumber(Real value,  bool useExp = false,
//       Integer length)
//------------------------------------------------------------------------------
/**
 * Builds a formatted string containing a Real, so the Real can be serialized to
 * the display
 *
 * @param value  The Real that needs to be serialized
 * @param useExp Use scientific notation
 * @param length The size of the desired string
 *
 * @return The formatted string
 * @note   This was moved from GmatCommand
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::BuildNumber(Real value, bool useExp,
                                        Integer length)
{
   std::string retval = "Invalid number";

   if (length < 100)
   {
      char temp[100], defstr[40];
      Integer fraction = 1;

      // check for a NaN first
      if ((!GmatMathUtil::IsEqual(value, 0.0)) &&
         (GmatMathUtil::IsEqual(value, GmatRealConstants::REAL_UNDEFINED)        ||
          GmatMathUtil::IsEqual(value, GmatRealConstants::REAL_UNDEFINED_LARGE)  ||
          GmatMathUtil::IsNaN(value)))
      {
         sprintf(defstr, "%%%ds", length);
         sprintf(temp, defstr, "NaN");
      }
      else
      {
         Real shift = GmatMathUtil::Abs(value);
         if (useExp || (shift > GmatMathUtil::Exp10((Real)length-3)))
         {
            fraction = length - 8;
            sprintf(defstr, "%%%d.%de", length, fraction);
         }
         else
         {
            while (shift > 10.0)
            {
               ++fraction;
               shift *= 0.1;
            }
            fraction = length - 3 - fraction;
            sprintf(defstr, "%%%d.%dlf", length, fraction);
         }
         #ifdef DEBUG_DEFSTR
            MessageInterface::ShowMessage("defstr = %s\n", defstr);
            if (fraction < 0) MessageInterface::ShowMessage("   and fraction = %d\n", fraction);
         #endif
         sprintf(temp, defstr, value);
      }
      retval = temp;
   }

   return retval;
}



//------------------------------------------------------------------------------
// std::string Trim(const std::string &str, StripType stype = BOTH,
//                  bool removeSemicolon = false, bool removeEol = false)
//------------------------------------------------------------------------------
/*
 * Removes leading or/and trailing blanks.  On option it removes end-of-line
 * first before removing semicolon.
 *
 * @param  str  input string to be trimmed
 * @param  stype  option of trimming front, end, or both (default is BOTH)
 * @param  removeSemicolon  option of removing trailing semicolon
 * @param  removeEol  option of removing end-of-line character first before
 *                    removing semicolon
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::Trim(const std::string &str, StripType stype,
                                 bool removeSemicolon, bool removeEol)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::Trim() str=<%s>, stripType=%d, removeSemicolon=%d, "
       "removeEol=%d\n", str.c_str(), stype, removeSemicolon, removeEol);
   #endif

   if (str == "")
      return str;

   std::string str2;
   std::string whiteSpace = " \t";

   std::string::size_type index1 = str.find_first_not_of(whiteSpace);
   std::string::size_type index2 = str.find_last_not_of(whiteSpace);

   if (index1 == str.npos)
      index1 = 0;

   if (stype == LEADING)
      str2 = str.substr(index1);
   else if (stype == TRAILING)
      str2 = str.substr(0, index2 + 1);
   else if (stype == BOTH)
      str2.assign(str.substr(index1, index2-index1+1));

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("   index1=%d, index2=%d str=<%s>, str2=<%s>\n", index1, index2,
       str.c_str(), str2.c_str());
   #endif

   // remove trailing end-of-lines (\r or \n) or blanks or tabs (\t)
   // before removing semicolons
   if (removeSemicolon)
   {
      if (str2.size() > 0)
      {
         // if option is to remove eol
         if (removeEol)
         {
            // This line is no longer needed; the new code preserved tabs in
            // string literals and replaces then with spaces everywhere else
            // when the file is read.  That means that the only tab characters
            // left are in string literals, and those need to be preserved.
//            // replace all occurrence of tab with a space
//            str2 = Replace(str2, "\t", " ");

            // remove trailing \r and \n
            while (str2[str2.size()-1] == '\n' || str2[str2.size()-1] == '\r')
               str2.erase(str2.size()-1, 1);

            // remove trailing blanks
            while (str2[str2.size()-1] == ' ')
               str2.erase(str2.size()-1, 1);
         }

         // remove trailing semicolns
         while (str2.size() > 0 && str2[str2.size()-1] == ';')
            str2.erase(str2.size()-1, 1);
      }
   }

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::Trim() returning <%s>\n", str2.c_str());
   #endif

   return str2;
}


//------------------------------------------------------------------------------
// std::string Strip(const std::string &str, StripType stype = BOTH)
//------------------------------------------------------------------------------
std::string GmatStringUtil::Strip(const std::string &str, StripType stype)
{
   return Trim(str, stype);
}


//------------------------------------------------------------------------------
// std::string ToUpper(const std::string &str, bool firstLetterOnly = false)
//------------------------------------------------------------------------------
/**
 * Makes whole string or first letter upper case.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToUpper(const std::string &str, bool firstLetterOnly)
{
   Integer len = str.length();
   
   if (len == 0)
      return str;
   
   std::string upper = str;
   
   if (firstLetterOnly)
      upper[0] = toupper(str[0]);
   else
      for (int i=0; i<len; i++)
         upper[i] = toupper(str[i]);
   
   return upper;
}


//------------------------------------------------------------------------------
// std::string ToLower(const std::string &str, bool firstLetterOnly = false)
//------------------------------------------------------------------------------
/**
 * Makes whole string or first letter lower case.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToLower(const std::string &str, bool firstLetterOnly)
{
   Integer len = str.length();
   
   if (len == 0)
      return str;
   
   std::string lower = str;
   
   if (firstLetterOnly)
      lower[0] = tolower(str[0]);
   else
      for (int i=0; i<len; i++)
         lower[i] = tolower(str[i]);
   
   return lower;
}


//------------------------------------------------------------------------------
// std::string Capitalize(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Capitialize the first letter of the string
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::Capitalize(const std::string &str)
{
   Integer len = str.length();

   if (len == 0)
      return str;

   std::string newstr = str;
   if (newstr.length() == 0)
      return str;

   newstr = Trim(newstr, BOTH);
   newstr[0] = toupper(newstr[0]);

   return newstr;
}

//------------------------------------------------------------------------------
// std::string ReplaceFirst(const std::string &str, const std::string &from,
//                     const std::string &to, std::string::size_type start)
//------------------------------------------------------------------------------
/*
 * Replaces first occurenece of <from> string to <to> string from start index
 *
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ReplaceFirst(const std::string &str, const std::string &from,
                                    const std::string &to, std::string::size_type startIndex)
{
   #ifdef DEBUG_REPLACE
   MessageInterface::ShowMessage
      ("GmatStringUtil::ReplaceFirst()> str=<%s>, from=<%s>, to=<%s>, startIndex=%u\n",
       str.c_str(), from.c_str(), to.c_str(), startIndex);
   #endif
   
   // if input string is the same as string to replace, just return <to> string
   if (str == from)
   {
      #ifdef DEBUG_REPLACE
      MessageInterface::ShowMessage
         ("GmatStringUtil::ReplaceFirst()> returning <%s>, input and from string "
          "is the same\n", to.c_str());
      #endif
      return to;
   }
   
   std::string str1 = str;
   std::string prepend;
   if (startIndex > 0)
   {
      prepend = str.substr(0, startIndex);
      str1 = str.substr(startIndex);
   }
   std::string::size_type pos = str1.find(from);
   
   // if string not found, just return input string
   if (pos == str1.npos)
   {
      #ifdef DEBUG_REPLACE
      MessageInterface::ShowMessage
      ("GmatStringUtil::ReplaceFirst()> returning <%s>, the string <%s> not found\n",
       str.c_str(), from.c_str());
      #endif
      return str;
   }
   
   #ifdef DEBUG_REPLACE
   MessageInterface::ShowMessage
      ("   prepend=<%s>, str1=<%s>\n", prepend.c_str(), str1.c_str());
   #endif
   
   bool done = false;
   std::string::size_type start = 0;
   
   while (!done)
   {
      pos = str1.find(from, start);

      #ifdef DEBUG_REPLACE
      MessageInterface::ShowMessage("===> start=%u, pos=%u\n", start, pos);
      #endif

      if (pos != str1.npos)
      {
         str1.replace(pos, from.size(), to);
         start = pos + to.size();

         #ifdef DEBUG_REPLACE
         MessageInterface::ShowMessage("===> start=%u, str1=<%s>\n", start, str1.c_str());
         #endif
      }
      break;
   }
   
   str1 = prepend + str1;
   
   #ifdef DEBUG_REPLACE
   MessageInterface::ShowMessage
      ("GmatStringUtil::ReplaceFirst()> returning <%s>\n", str1.c_str());
   #endif
   
   return str1;
}

//------------------------------------------------------------------------------
// std::string Replace(const std::string &str, const std::string &from,
//                     const std::string &to, std::string::size_type start)
//------------------------------------------------------------------------------
/*
 * Replaces all occurenece of <from> string to <to> string from start index
 *
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::Replace(const std::string &str, const std::string &from,
                                    const std::string &to, std::string::size_type startIndex)
{
   #ifdef DEBUG_REPLACE
   MessageInterface::ShowMessage
      ("GmatStringUtil::Replace()> str=<%s>, from=<%s>, to=<%s>, startIndex=%u\n",
       str.c_str(), from.c_str(), to.c_str(), startIndex);
   #endif
   
   // if input string is the same as string to replace, just return <to> string
   if (str == from)
   {
      #ifdef DEBUG_REPLACE
      MessageInterface::ShowMessage
         ("GmatStringUtil::Replace()> returning <%s>, input and from string "
          "is the same\n", to.c_str());
      #endif
      return to;
   }
   
   if (from == "")
   {
      #ifdef DEBUG_REPLACE
      MessageInterface::ShowMessage
         ("GmatStringUtil::Replace()> returning <%s>, from string is empty\n",
          str.c_str());
      #endif
      return str;
   }
   
   std::string str1 = str;
   std::string prepend;
   if (startIndex > 0)
   {
      prepend = str.substr(0, startIndex);
      str1 = str.substr(startIndex);
   }
   std::string::size_type pos = str1.find(from);
   
   // if string not found, just return input string
   if (pos == str1.npos)
   {
      #ifdef DEBUG_REPLACE
      MessageInterface::ShowMessage
      ("GmatStringUtil::Replace()> returning <%s>, the string <%s> not found\n",
       str.c_str(), from.c_str());
      #endif
      return str;
   }
   
   #ifdef DEBUG_REPLACE
   MessageInterface::ShowMessage
      ("   prepend=<%s>, str1=<%s>\n", prepend.c_str(), str1.c_str());
   #endif
   
   bool done = false;
   std::string::size_type start = 0;
   
   while (!done)
   {
      pos = str1.find(from, start);

      #ifdef DEBUG_REPLACE
      MessageInterface::ShowMessage("===> start=%u, pos=%u\n", start, pos);
      #endif

      if (pos != str1.npos)
      {
         str1.replace(pos, from.size(), to);
         start = pos + to.size();

         #ifdef DEBUG_REPLACE
         MessageInterface::ShowMessage("===> start=%u, str1=<%s>\n", start, str1.c_str());
         #endif
      }
      else
      {
         break;
      }
   }
   
   str1 = prepend + str1;
   
   #ifdef DEBUG_REPLACE
   MessageInterface::ShowMessage
      ("GmatStringUtil::Replace()> returning <%s>\n", str1.c_str());
   #endif
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string ReplaceName(const std::string &str, const std::string &from,
//                         const std::string &to)
//------------------------------------------------------------------------------
/*
 * Replaces all occurenece of <from> string to <to> string if <from> is not
 * part of word. It replaces the string if character before and after <from> are
 * not alphanumeric except underscore '_'. Name with underscores is considered
 * as valid name in GMAT.
 *
 * @param  str   String to be replaced
 * @param  from  Old string to be replaced
 * @param  to    New string to replace
 *
 * @return  Return replaced string if from string found, else return original string
 *
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ReplaceName(const std::string &str, const std::string &from,
                                        const std::string &to)
{
   #ifdef DEBUG_REPLACE_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::ReplaceName() str=\"%s\", from=\"%s\", to=\"%s\"\n",
       str.c_str(), from.c_str(), to.c_str());
   #endif
   
   std::string str1 = str;
   std::string::size_type pos = str1.find(from);
   
   // if string not found, just return same string
   if (pos == str1.npos)
      return str1;
   
   // if input string is the same as string to replace, just return <to> string
   if (str == from)
      return to;
   
   bool done = false;
   std::string::size_type start = 0;
   std::string::size_type strSize = str1.size();
   std::string::size_type fromSize = from.size();
   bool replace = false;
   char underscore = '_';
   
   while (!done)
   {
      #ifdef DEBUG_REPLACE_NAME
      MessageInterface::ShowMessage("   str1='%s'\n", str1.c_str());
      #endif
      
      strSize = str1.size();
      pos = str1.find(from, start);
      
      if (pos != str1.npos)
      {
         replace = false;
         
         #ifdef DEBUG_REPLACE_NAME
         MessageInterface::ShowMessage("   start=%u, pos=%u\n", start, pos);
         #endif
         
         if (pos == 0 && fromSize < strSize)
         {
            if (!isalnum(str1[fromSize]) && str1[fromSize] != underscore)
               replace = true;
         }
         else if (pos > 0 && (pos + fromSize) < strSize)
         {
            if (!isalnum(str1[pos + fromSize]) && str1[pos + fromSize] != underscore)
               replace = true;
         }
         else if (pos == strSize-fromSize) // replace last name
         {
            replace = true;
         }
         
         #ifdef DEBUG_REPLACE_NAME
         MessageInterface::ShowMessage("   replace=%d\n", replace);
         #endif
         
         if (replace)
         {
            // Check for the system Parameter name or object property field which
            // should not be replaced, such as SMA in sat.SMA Parameter or
            // X in sat.EarthEqCS.X. If previous char is non-alphanumeric, then
            // it can be replaced, such as arr(2.2)/sat1.X. (Fix for GMT-309 LOJ:2012.08.14)
            if ((pos == 0) ||
                ((pos > 0 && str1[pos-1] == '.') && (pos-1 != str1.find_last_of('.'))) ||
                (pos > 0 && !isalnum(str1[pos-1])))
            {
               str1.replace(pos, fromSize, to);
            }
            else
            {
               #ifdef DEBUG_REPLACE_NAME
               MessageInterface::ShowMessage
                  ("   ==> name not replaced, it is an object property\n");
               #endif
            }
         }
         
         start = pos + to.size();
         
         #ifdef DEBUG_REPLACE_NAME
         MessageInterface::ShowMessage("   start=%d, str1=<%s>\n", start, str1.c_str());
         #endif
      }
      else
      {
         break;
      }
   }
   
   #ifdef DEBUG_REPLACE_NAME
   MessageInterface::ShowMessage("GmatStringUtil::ReplaceName() returning <%s>\n", str1.c_str());
   #endif
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string ReplaceNumber(const std::string &str, const std::string &from,
//                         const std::string &to)
//------------------------------------------------------------------------------
/*
 * Replaces all occurenece of <from> string to <to> string if <from> is a part
 * of number. It will replace if input string is for example 3e-, 0E+, 34234
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ReplaceNumber(const std::string &str, const std::string &from,
                                          const std::string &to)
{
   #ifdef DEBUG_REPLACE_NUMBER
   MessageInterface::ShowMessage
      ("GmatStringUtil::ReplaceNumber() str=\"%s\", from=\"%s\", to=\"%s\"\n", str.c_str(),
       from.c_str(), to.c_str());
   #endif
   
   std::string str1 = str;
   std::string::size_type pos = str1.find(from);

   // if string not found, just return same string
   if (pos == str1.npos)
      return str1;

   // if input string is the same as string to replace, just return <to> string
   if (str == from)
      return to;

   bool done = false;
   std::string::size_type start = 0;
   std::string::size_type fromSize = from.size();
   
   while (!done)
   {
      pos = str1.find(from, start);
      if (pos != str1.npos)
      {
         #ifdef DEBUG_REPLACE_NUMBER
         MessageInterface::ShowMessage("===> start=%u, pos=%u\n", start, pos);
         #endif
         
         if (pos > 0 && isdigit(str1[pos-1]))
            str1.replace(pos, fromSize, to);
         
         start = pos + to.size();
         
         #ifdef DEBUG_REPLACE_NUMBER
         MessageInterface::ShowMessage("===> start=%d, str1=<%s>\n", start, str1.c_str());
         #endif
      }
      else
      {
         break;
      }
   }
   
   #ifdef DEBUG_REPLACE_NUMBER
   MessageInterface::ShowMessage("GmatStringUtil::ReplaceNumber() returning <%s>\n", str1.c_str());
   #endif

   return str1;
}


//------------------------------------------------------------------------------
// std::string ReplaceChainedUnaryOperators(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Replaces repeated plus (+) or minus (-) signs with one sign.
 * For example "+--+abc-+--def+-+-ghi" will give "+abc-def_ghi".
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ReplaceChainedUnaryOperators(const std::string &str)
{
   #ifdef DEBUG_REPLACE_PLUSMINUS
   MessageInterface::ShowMessage
      ("MathParser::ReplaceChainedUnaryOperators() entered, str='%s'\n", str.c_str());
   #endif
   
   std::string str1 = str;
   Integer length = str1.size();
   std::string signs, nonSigns, finalStr;
   bool signFound = false, signDone = false;
   bool nonSignFound = false, nonSignDone = false;
   
   for (int i = 0; i < length; i++)
   {
      if (str1[i] == '+' || str1[i] == '-')
      {
         if (nonSignFound)
            nonSignDone = true;
         
         signFound = true;
         nonSignFound = false;
         
         if (nonSignDone)
         {
            #ifdef DEBUG_REPLACE_PLUSMINUS
            MessageInterface::ShowMessage("   nonSigns = '%s'\n", nonSigns.c_str());
            #endif
            finalStr = finalStr + nonSigns;
            nonSignDone = false;
            nonSigns = "";
            signs = str1[i];
         }
         else
         {
            signs = signs + str1[i];
         }
      }
      else
      {
         if (signFound)
            signDone = true;
         
         signFound = false;
         nonSignFound = true;
         
         if (signDone)
         {
            #ifdef DEBUG_REPLACE_PLUSMINUS
            MessageInterface::ShowMessage("   signs = '%s'\n", signs.c_str());
            #endif
            Integer numMinus = GmatStringUtil::NumberOfOccurrences(signs, '-');
            char finalSign = '-';
            if ((numMinus % 2) == 0)
               finalSign = '+';
            
            finalStr = finalStr + finalSign;
            signDone = false;
            signs = "";
            nonSigns = str1[i];
         }
         else
         {
            nonSigns = nonSigns + str1[i];
         }
      }
   }
   
   #ifdef DEBUG_REPLACE_PLUSMINUS
   MessageInterface::ShowMessage("   last signs = '%s'\n", signs.c_str());
   MessageInterface::ShowMessage("   last nonSigns = '%s'\n", nonSigns.c_str());
   #endif
   
   // Put last part
   if (signs != "")
   {
      Integer numMinus = GmatStringUtil::NumberOfOccurrences(signs, '-');
      char finalSign = '-';
      if ((numMinus % 2) == 0)
         finalSign = '+';
      
      finalStr = finalStr + finalSign;
   }
   else if (nonSigns != "")
   {
      finalStr = finalStr + nonSigns;
   }
   
   #ifdef DEBUG_REPLACE_PLUSMINUS
   MessageInterface::ShowMessage
      ("MathParser::ReplaceChainedUnaryOperators() returning '%s'\n", finalStr.c_str());
   #endif
   
   return finalStr;
}


std::string GmatStringUtil::RealToString(const Real &val, Integer precision,
   bool showPoint, Integer width)
{
   return GmatRealUtil::RealToString(val, false, false, showPoint, precision, width);
}


std::string GmatStringUtil::RealToString(const Real &val, bool useCurrentFormat,
   bool scientific, bool showPoint,
   Integer precision, Integer width)
{
   return GmatRealUtil::RealToString(val, useCurrentFormat, scientific, showPoint,
      precision, width);
}


//------------------------------------------------------------------------------
// std::string ToString(const bool &val)
//------------------------------------------------------------------------------
/*
 * Formats bool value to String. If value is not true, it returns "false".
 *
 * @param  val  boolean value
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const bool &val)
{
   if (val == true)
      return "true";
   else
      return "false";
}


//------------------------------------------------------------------------------
// std::string ToString(const Real &val, Integer precision, bool showPoint,
//                      Integer width)
//------------------------------------------------------------------------------
/*
 * Formats Real value to String.
 *
 * @param  val        Real value
 * @param  precision  Precision to be used in formatting
 * @param  showPoint  Formats using ios::showpoint if true (false)
 * @param  width      Width to be used in formatting (1)
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Real &val, Integer precision,
                                     bool showPoint, Integer width)
{
   return GmatRealUtil::ToString(val, false, false, showPoint, precision, width);
}


//------------------------------------------------------------------------------
// std::string ToString(const Integer &val, Integer width)
//------------------------------------------------------------------------------
/*
 * Formats Integer value to String.
 *
 * @param  val  Integer value
 * @param  width  Width to be used in formatting
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Integer &val, Integer width)
{
   return GmatRealUtil::ToString(val, false, width);
}


//------------------------------------------------------------------------------
// std::string ToString(const Real &val, bool useCurrentFormat, bool scientific,
//                      bool showPoint, Integer precision, Integer width)
//------------------------------------------------------------------------------
/*
 * Formats Real value to String.
 *
 * @param  val  Real value
 * @param  useCurrentFormat  Uses precision and width from GmatGlobal (true)
 * @param  scientific  Formats using scientific notation if true (true)
 * @param  showPoint if true, shows decimal point and trailing zeros (false)
 * @param  precision  Precision to be used in formatting (GmatGlobal::DATA_PRECISION)
 * @param  width  Width to be used in formatting (GmatGlobal::INTEGER_WIDTH)
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Real &val, bool useCurrentFormat,
                                     bool scientific, bool showPoint,
                                     Integer precision, Integer width)
{
   return GmatRealUtil::ToString(val, useCurrentFormat, scientific, showPoint,
                                 precision, width);
}


//------------------------------------------------------------------------------
// std::string ToString(const Integer &val, bool useCurrentFormat, Integer width)
//------------------------------------------------------------------------------
/*
 * Formats Integer value to String.
 *
 * @param  val  Integer value
 * @param  useCurrentFormat  Uses width from GmatGlobal if true (true)
 * @param  width  Width to be used in formatting (GmatGlobal::INTEGER_WIDTH)
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Integer &val, bool useCurrentFormat,
                                     Integer width)
{
   return GmatRealUtil::ToString(val, useCurrentFormat, width);
}


//------------------------------------------------------------------------------
// std::string ToStringNoZeros(const Real &val)
//------------------------------------------------------------------------------
/**
 * Formats real number to string without trailing zeros.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToStringNoZeros(const Real &val)
{
   std::string str, str1;
   char buffer[50];
   sprintf(buffer, "%.14f", val);
   str = buffer;
   
   return RemoveTrailingZeros(val, str);
}


//------------------------------------------------------------------------------
// std::string ToOrdinal(Integer i, bool textOnly = false)
//------------------------------------------------------------------------------
/**
 * Returns the ordinal number, given the input integer.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToOrdinal(Integer i, bool textOnly)
{
   std::string num  = ToString(i, 1);
   if (textOnly)
   {
      // TBD
//      return num;
   }

   // Get the digit in the ones place
   char        ones = num[num.length() - 1];
   if (ones == '1')
      return num + "st";
   else if (ones == '2')
      return num + "nd";
   else if (ones == '3')
      return num + "rd";
   else
      return num + "th";
}


//-------------------------------------------------------------------------------
// char GetClosingBracket(const char &openBracket)
//-------------------------------------------------------------------------------
char GmatStringUtil::GetClosingBracket(const char &openBracket)
{
   switch (openBracket)
   {
   case '(':
      return ')';
   case '[':
      return ']';
   case '{':
      return '}';
   case '<':
      return '>';

   default:
      UtilityException ex;
      ex.SetDetails("Found unknown open bracket: %c", openBracket);
      throw ex;
   }
}

//-------------------------------------------------------------------------------
// StringArray SeparateBrackets(const std::string &chunk,
//                              const std::string &bracketPair,
//                              const std::string &delim,
//                              bool checkOuterBracket)
//-------------------------------------------------------------------------------
/*
 * Breaks chunk into parts separated by space or comma but keeps bracket together,
 * except outer most brackets.
 *
 * For example:
 * string = "{Pert=P(1,1),MaxStep=M(1,1),Lower=L(1,1),Upper=U(1,1)}"
 * SeparateBrackets(string, "{}", ",", false) will return
 *    <Pert=P(1,1)> <MaxStep=M(1,1)> <Lower=L(1,1)> <Upper=U(1,1)>
 *
 * @param <chunk> input chunk to be break apart
 * @param <bracketPair> input bracket pair (open and close) to keep together
 *                      (), [], {}
 * @param <checkOuterBracket> true if outer bracket pair must be exist (true)
 *
 * @exception <InterpreterException> thrown
 *    If checkOuterBracket is set to true, and there is no matching bracket pair
 */
//-------------------------------------------------------------------------------
StringArray GmatStringUtil::SeparateBrackets(const std::string &chunk,
                                         const std::string &bracketPair,
                                         const std::string &delim,
                                         bool checkOuterBracket)
{
   #if DEBUG_SEP_BRACKETS
   MessageInterface::ShowMessage
      ("GmatStringUtil::SeparateBrackets() chunk='%s', bracketPair='%s', delim='%s', "
       "checkOuterBracket=%d\n", chunk.c_str(), bracketPair.c_str(), delim.c_str(),
       checkOuterBracket);
   #endif
   
   std::string str1 = chunk;
   
   // First remove blank spaces inside array bracket
   if (chunk[0] != bracketPair[0])
      str1 = RemoveSpaceInBrackets(chunk, bracketPair);
   
   #if DEBUG_SEP_BRACKETS
   MessageInterface::ShowMessage("   str1=%s\n", str1.c_str());
   #endif
   
   UnsignedInt firstOpen, lastClose;
   std::string whiteSpace = " \t";
   firstOpen = str1.find_first_not_of(whiteSpace);
   lastClose = str1.find_last_not_of(whiteSpace);
   bool bracketFound = true;
   
   if (str1[firstOpen] != bracketPair[0] || str1[lastClose] != bracketPair[1])
   {
      bracketFound = false;
      if (checkOuterBracket)
      {
         char errorMsg[1024];
         sprintf(errorMsg, "GmatStringUtil::SeparateBrackets() \"%s\" is not enclosed "
                 "with \"%s\"", str1.c_str(), bracketPair.c_str());
         
         #if DEBUG_SEP_BRACKETS
         MessageInterface::ShowMessage("*** ERROR *** %s\n", errorMsg);
         #endif

         sprintf(errorMsg, "\"%s\" is not enclosed with \"%s\"", str1.c_str(),
                 bracketPair.c_str());
         throw UtilityException(errorMsg);
      }
   }
   
   std::string str;
   
   if (bracketFound)
      str = str1.substr(firstOpen+1, lastClose-firstOpen-1);
   else
      str = str1.substr(firstOpen, lastClose-firstOpen+1);
   
   #if DEBUG_SEP_BRACKETS
   MessageInterface::ShowMessage("   str=%s\n", str.c_str());
   #endif
   
   
   StringArray parts;
   parts = SeparateBy(str, delim, true);
   
   #if DEBUG_SEP_BRACKETS
   MessageInterface::ShowMessage("   Returning:\n");
   for (unsigned int i=0; i<parts.size(); i++)
      MessageInterface::ShowMessage
         ("   parts[%d] = %s\n", i, parts[i].c_str());
   #endif
   
   return parts;
}

//------------------------------------------------------------------------------
// StringArray SeparateBy(const std::string &str, const std::string &delim,
//                        bool putBracketsTogether, bool insertDelim)
//------------------------------------------------------------------------------
/*
 * Separates string by input delimiter.
 *
 * @param  str  input string
 * @param  delim  input delimiter
 * @param  putBracketsTogether  true if putting brackets together (false)
 * @param  insertDelim  true if inserting delimiter back if only 1 delimiter (false)
 *
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::SeparateBy(const std::string &str,
                                       const std::string &delim,
                                       bool putBracketsTogether,
                                       bool insertDelim, bool insertComma)
{
   #if DEBUG_STRING_UTIL_SEP
   MessageInterface::ShowMessage
      ("GmatStringUtil::SeparateBy() str='%s', delim='%s', putBracketsTogether=%d, "
       "insertDelim=%d\n", str.c_str(), delim.c_str(), putBracketsTogether, insertDelim);
   #endif
   
   StringTokenizer st;
   if (insertDelim)
      st.Set(str, delim, insertDelim);
   else
      st.Set(str, delim);
   
   //StringTokenizer st(str, delim);
   StringArray tempParts = st.GetAllTokens();
   
   if (!putBracketsTogether)
      return tempParts;
   
   //-----------------------------------------------------------------
   // now go through each part and put brackets together
   //-----------------------------------------------------------------
   StringArray parts;
   std::string openBrackets = "([{";
   std::string::size_type index1;
   int count = tempParts.size();
   
   #if DEBUG_STRING_UTIL_SEP
   MessageInterface::ShowMessage("There are %d parts to go through\n", count);
   for (int i=0; i<count; i++)
      MessageInterface::ShowMessage
         ("   tempParts[%d]='%s'\n", i, tempParts[i].c_str());
   #endif
   
   Integer size = -1;
   bool append = false;
   
   // go through each part and put brackets together, insert back delimiter if only one
   for (int i=0; i<count; i++)
   {      
      #if DEBUG_STRING_UTIL_SEP
      MessageInterface::ShowMessage("   ==================== count = %d\n", i);
      #endif
      
      index1 = tempParts[i].find_first_of(openBrackets);
      
      if (index1 != str.npos)
      {
         #if DEBUG_STRING_UTIL_SEP
         MessageInterface::ShowMessage("   one of open brackets ([{ found\n");
         #endif
         if (append)
         {
            // if only one delimiter, insert it back in (loj: 2008.03.24)
            if (delim.size() == 1 && !insertDelim)
               parts[size] = parts[size] + delim + tempParts[i];
            else
               parts[size] = parts[size] + " " + tempParts[i];
         }
         else
         {
            #if DEBUG_STRING_UTIL_SEP > 1
            MessageInterface::ShowMessage
               ("   adding1 '%s'\n", tempParts[i].c_str());
            #endif
            
            parts.push_back(tempParts[i]);
            size++;
         }
         
         // if any bracket is not balanced, append
         if (!IsBracketBalanced(parts[size], "()") ||
             !IsBracketBalanced(parts[size], "[]") ||
             !IsBracketBalanced(parts[size], "{}"))
            append = true;
         else
            append = false;
         
         #if DEBUG_STRING_UTIL_SEP > 1
         MessageInterface::ShowMessage
            ("   parts[%d]='%s'\n", size, parts[size].c_str());
         MessageInterface::ShowMessage("   append=%d\n", append);
         #endif
      }
      else
      {
         if (append)
         {
            #if DEBUG_STRING_UTIL_SEP > 1
            MessageInterface::ShowMessage
               ("   appending '%s'\n", tempParts[i].c_str());
            #endif
            
            // if only one delimitter, insert it back in (loj: 2008.03.24)
            if (delim.size() == 1 && !insertDelim)
               parts[size] = parts[size] + delim + tempParts[i];
            else
            {
               if (insertComma)
                  parts[size] = parts[size] + "," + tempParts[i];
               else
                  parts[size] = parts[size] + tempParts[i];
            }
         }
         else
         {
            #if DEBUG_STRING_UTIL_SEP > 1
            MessageInterface::ShowMessage
               ("   adding2 '%s'\n", tempParts[i].c_str());
            #endif
            
            parts.push_back(tempParts[i]);
            size++;
         }

         // if any bracket is not balanced, append
         if (!IsBracketBalanced(parts[size], "()") ||
             !IsBracketBalanced(parts[size], "[]") ||
             !IsBracketBalanced(parts[size], "{}"))
            append = true;
         else
            append = false;

         #if DEBUG_STRING_UTIL_SEP > 1
         MessageInterface::ShowMessage
            ("   parts2[%d]='%s'\n", size, parts[size].c_str());
         MessageInterface::ShowMessage("   append=%d\n", append);
         #endif
      }
   }
   
   StringArray parts1;
   
   // add non-blank items
   for (unsigned int i=0; i<parts.size(); i++)
   {
      parts[i] = Strip(parts[i]);
      if (parts[i] != "")
         parts1.push_back(parts[i]);
   }
   
   #if DEBUG_STRING_UTIL_SEP
   MessageInterface::ShowMessage
      ("GmatStringUtil::SeparateBy() str='%s' returning:\n", str.c_str());
   for (unsigned int i=0; i<parts1.size(); i++)
      MessageInterface::ShowMessage
         ("   parts1[%d] = '%s'\n", i, parts1[i].c_str());
   #endif
   
   return parts1;
}


//------------------------------------------------------------------------------
// StringArray SeparateByComma(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Separates string by comma leaving all parentheses and single quotes intact.
 * If parentheses are not balanced, it will just return input string.
 *
 * @param  str  input string
 * @return  StringArray of separated parts
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::SeparateByComma(const std::string &str, bool checkSingleQuote)
{
   #ifdef DEBUG_STRING_UTIL_SEP_COMMA
   MessageInterface::ShowMessage
      ("GmatStringUtil::SeparateByComma() entered, str=<%s>\n", str.c_str());
   #endif
   
   StringArray parts;
   parts.push_back(str);
   
   // if no comma is found, just return input string
   std::string::size_type index1 = str.find(",");
   if (index1 == str.npos)
   {
      #ifdef DEBUG_STRING_UTIL_SEP_COMMA
      MessageInterface::ShowMessage
         ("GmatStringUtil::SeparateByComma() returning <%s>, no comma found\n", str.c_str());
      #endif
      // remove leading and trailing blanks, though
      parts[0] = Trim(parts[0], BOTH);
      return parts;
   }
   
   // Make sure that all parentheses are balanced
   if (!IsParenBalanced(str))
   {
      #ifdef DEBUG_STRING_UTIL_SEP_COMMA
      MessageInterface::ShowMessage
         ("GmatStringUtil::SeparateByComma() returning <%s>, parenthesis is not balanced\n", str.c_str());
      #endif
      return parts;
   }
   
   // Now go through each character in the string
   parts.clear();
   Integer count = str.size();
   std::string str1 = str;
   bool insideQuote = false;
   Integer openCount = 0;
   std::string part;
   
   for (int i=0; i<count; i++)
   {
      if (str1[i] == ',')
      {
         #ifdef DEBUG_STRING_UTIL_SEP_COMMA
         MessageInterface::ShowMessage
            ("   curr ch is comma, insideQuote=%d, openCount=%d\n", insideQuote, openCount);
         #endif
         if ((checkSingleQuote && insideQuote) || openCount > 0)
         {
            part = part + str1[i];
         }
         else
         {
            parts.push_back(part);
            part = "";
            insideQuote = false;
         }
      }
      else if (str1[i] == '\'')
      {
         part = part + str1[i];
         if (insideQuote)
            insideQuote = false;
         else
            insideQuote = true;
      }
      else if (str1[i] == '(')
      {
         part = part + str1[i];
         openCount++;
         #ifdef DEBUG_STRING_UTIL_SEP_COMMA
         MessageInterface::ShowMessage
            ("   curr ch is (,  part=<%s>, openCount=%d\n", part.c_str(), openCount);
         #endif
      }
      else if (str1[i] == ')')
      {
         part = part + str1[i];
         openCount--;
         #ifdef DEBUG_STRING_UTIL_SEP_COMMA
         MessageInterface::ShowMessage
            ("   curr ch is ), part=<%s>, openCount=%d\n", part.c_str(), openCount);
         #endif
      }
      else
      {
         part = part + str1[i];
      }
   }
   
   parts.push_back(part);
   
   // Let's strip off leading and trailing blanks before returning
   for (UnsignedInt i=0; i<parts.size(); i++)
   {
      parts[i] = Trim(parts[i], BOTH);
   }
   
   #ifdef DEBUG_STRING_UTIL_SEP_COMMA
   WriteStringArray(parts, "GmatStringUtil::SeparateByComma() returning:", "   ");
   #endif
   
   return parts;
}


//-------------------------------------------------------------------------------
// StringArray SeparateDots(const std::string &chunk)
//-------------------------------------------------------------------------------
/*
 * Breaks string by dots, but keep decimal point number together.
 *
 * @param <chunk> input string to be break apart
 *
 * @return string array of parts
 */
//-------------------------------------------------------------------------------
StringArray GmatStringUtil::SeparateDots(const std::string &chunk)
{
   Real rval;
   StringArray parts;

   // Separate by dots if not a number
   if (ToReal(chunk, rval))
   {
      parts.push_back(chunk);
   }
   else
   {
      StringTokenizer st(chunk, ".");
      parts = st.GetAllTokens();
   }

   #ifdef DEBUG_SEP_DOTS
   for (UnsignedInt i=0; i<parts.size(); i++)
      MessageInterface::ShowMessage
         ("   parts[%d]=%s\n", i, parts[i].c_str());
   #endif

   return parts;
}


//------------------------------------------------------------------------------
// bool IsNumber(const std::string &str)
//------------------------------------------------------------------------------
/*
 * @return  true if input string is a number.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsNumber(const std::string &str)
{
   Real rval;
   Integer ival;
   UnsignedInt uival;
   
   if (ToReal(str, rval) || ToUnsignedInt(str, uival) || ToInteger(str, ival))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsValidReal(const std::string &str, Real &value, Integer &errorCode,
//                  bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @param  str  String to check for real number and convert
 * @param  value  Output to receive real value
 * @param  errorCode  Output to receive error code
 *            0 = No error
 *           -1 = Input string is blank
 *           -2 = Input string is blank after removing leading and trailing spaces
 *           -3 = Input string contains numbers with multiple dots or invalid scientific notation
 *           -4 = First character of input string is +, -, ., or digit
 *           -5 = Input string has multiple E or e
 *           -6 = Input string has multiple + or - after E or e
 *           -7 = Input string has multiple dots
 *           -8 = Input string contains non-numeric except E or e
 *           -9 = Input string has missing numbers after + or - sign
 * @param  trimParens  Set this to true if extra layers of parenthesis to be removed before checking [false]
 * @param  
 * @return  true if input string is a number, false otherwise
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidReal(const std::string &str, Real &value, Integer &errorCode,
                                 bool trimParens, bool allowOverflow)
{
   #ifdef DEBUG_TOREAL
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidReal() entered, str='%s'\n", str.c_str());
   #endif
   
   errorCode = 0;
   
   if (str == "")
   {
      #ifdef DEBUG_TOREAL
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidReal() returning false, it is blank.\n");
      #endif
      errorCode = -1;
      return false;
   }
   
   std::string str2 = Trim(str, BOTH);
   if (trimParens)
   {
      str2 = RemoveExtraParen(str2);
      str2 = Trim(str2, BOTH);
   }
   
   if (str2.length() == 0)
   {
      #ifdef DEBUG_TOREAL
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidReal() returning false, it is blank after trim.\n");
      #endif
      errorCode = -2;
      return false;
   }
   
   // Handle infinite number first, '1.#INF' for VC++ compiler,
   // 'inf' for GCC compiler (LOJ: 2012.01.30)
   if (str2 == "1.#INF" || str2 == "inf")
   {
      value = atof(str2.c_str());
      
      #ifdef DEBUG_TOREAL
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidReal() returning true, value=%f\n", value);
      #endif
      
      return true;
   }

   // If first character is not '+', '-', '.' and digit, it's false
   if (str2[0] != '+' && str2[0] != '-' && !isdigit(str2[0]) && str2[0] != '.')
   {
      #ifdef DEBUG_TOREAL
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidReal() returning false, first char is not + - . or number.\n");
      #endif
      errorCode = -4;
      return false;
   }
   
   // Remove blanks after + or - sign
   if (str2[0] == '+' || str2[0] == '-')
   {
      std::string::size_type firstBlank = str2.find_first_of(" ", 1);
      std::string::size_type firstNonBlank = str2.find_first_not_of(" ", 1);
      #ifdef DEBUG_TOREAL
      MessageInterface::ShowMessage
         ("   firstBlank=%u, firstNonBlank=%u\n", firstBlank, firstNonBlank);
      #endif
      if (firstBlank != str2.npos)
      {
         if (firstNonBlank != str2.npos && firstBlank < firstNonBlank)
         {
            std::string str3 = str2.substr(firstNonBlank);
            str2 = str2[0] + str3;
            #ifdef DEBUG_TOREAL
            MessageInterface::ShowMessage
               ("   After spaces removed bwtween +/- and number, str2='%s'\n", str2.c_str());
            #endif
         }
      }
   }
   
   // Check for multiple E or e
   std::string::size_type index1 = str2.find_first_of("Ee");
   if (index1 != str2.npos)
      if (str2.find_first_of("Ee", index1 + 1) != str2.npos)
      {
         #ifdef DEBUG_TOREAL
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsValidReal() returning false, it has multiple Ee.\n");
         #endif
         errorCode = -5;
         return false;
      }

   // Check for multiple + or -
   index1 = str2.find_first_of("+-");
   if (index1 != str2.npos)
   {
      index1 = str2.find_first_of("+-", index1 + 1);
      if (index1 != str2.npos)
      {
         std::string::size_type firstE = str2.find_first_of("Ee");
         if (firstE != str2.npos && index1 < firstE)
         {
            #ifdef DEBUG_TOREAL
            MessageInterface::ShowMessage
               ("GmatStringUtil::IsValidReal() returning false, it has multiple + or - after Ee.\n");
            #endif
            errorCode = -6;
            return false;
         }
      }
   }
   
   // Check for multiple dots or dots in scientific notation suchas 5e-17.3
   Integer numDot = 0;
   bool sciNotationFound = false;
   for (unsigned int i=0; i<str2.length(); i++)
   {
      if (i == 0 && (str2[0] == '-' || str2[0] == '+'))
         continue;
      
      if (str2[i] == '.')
      {
         numDot++;
         
         if ((sciNotationFound && numDot > 0) ||
             (!sciNotationFound && numDot > 1))
         {
            #ifdef DEBUG_TOREAL
            MessageInterface::ShowMessage
               ("GmatStringUtil::IsValidReal() returning false, it has multiple "
                "dots or dot in the scientific notation\n");
            #endif
            
            // Check if it contains just number and/or scientific notation
            std::string str3 = RemoveScientificNotation(str2);
            str3 = RemoveAll(str3, '.');
            Real rval;
            if (ToReal(str3, rval))
               errorCode = -3;
            else
               errorCode = -7;
            return false;
         }
         
         continue;
      }
      
      if (!isdigit(str2[i]))
      {
         // Handle scientific notation
         if (str2[i] == 'e' || str2[i] == 'E')
         {
            if ((i + 1 < str2.length()) &&
                (str2[i+1] == '+' || str2[i+1] == '-' || isdigit(str2[i+1])))        
            {
               sciNotationFound = true;
               continue;
            }
         }

         
         if ((str2[i] == '+' || str2[i] == '-' || isdigit(str2[i])) &&
             (str2[i-1] == 'e' || str2[i-1] == 'E'))
         {
            if (i == str2.length()-1)
            {
               #ifdef DEBUG_TOREAL
               MessageInterface::ShowMessage
                  ("GmatStringUtil::IsValidReal() returning false, it has missing number in "
                   "the scientific notation\n");
               #endif
               errorCode = -3;
               return false;
            }
            else
               continue;
         }
         
         #ifdef DEBUG_TOREAL
         MessageInterface::ShowMessage
            ("   GmatStringUtil::IsValidReal(%s) returning false, found non-numeric except E or e\n", str2.c_str());
         #endif
         errorCode = -8;
         return false;
      }
   }
   
   // Check if there is number followed by scientific notation
   // Check if there is + or - without number
   if (str2 == "+" || str2 == "-")
   {
      #ifdef DEBUG_TOREAL
      MessageInterface::ShowMessage
         ("   GmatStringUtil::IsValidReal(%s) returning false, missing number after + or -.\n", str2.c_str());
      #endif
      errorCode = -9;
      return false;
   }
      
   #ifdef DEBUG_TOREAL
   MessageInterface::ShowMessage
      ("   Now calling atof('%s') for converting to Real\n", str2.c_str());
   #endif
   
   errno = 0;
   char *err;
   value = strtod(str2.c_str(), &err);
   if (errno == ERANGE)
   {
	   if (allowOverflow)
			MessageInterface::ShowMessage
				("GmatStringUtil::IsValidReal('%s') out of range error, value=%.12f\n", str.c_str(), value);
	   else
		   return false;
   }
   else if (strlen(err))
	   return false;
   else if (value == -0)
	   value = 0;

   #ifdef DEBUG_TOREAL
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidReal('%s') returning true, value=%g\n", str.c_str(), value);
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// bool ToReal(const char *str, Real *value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @see ToReal(const std::string &str, Real &value)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToReal(const char *str, Real *value, bool trimParens,
                            bool allowOverflow)
{
   return ToReal(std::string(str), *value, trimParens, allowOverflow);
}


//------------------------------------------------------------------------------
// bool ToReal(const std::string &str, Real *value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @see ToReal(const std::string &str, Real &value)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToReal(const std::string &str, Real *value, bool trimParens,
                            bool allowOverflow)
{
   return ToReal(str, *value, trimParens, allowOverflow);
}

//------------------------------------------------------------------------------
// bool ToReal(const char *str, Real &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @see ToReal(const std::string &str, Real &value, ...)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToReal(const char *str, Real &value, bool trimParens,
                            bool allowOverflow)
{
   return ToReal(std::string(str), value, trimParens, allowOverflow);
}

//------------------------------------------------------------------------------
// bool ToReal(const std::string &str, Real &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * This method converts string to Real using atof() after validation.
 *
 * @param  str  input string to be converted to Real
 * @param  value  output Real value
 *
 * @return true if input string represents valid Real number, false otherwise
 *
 * @note  atof() returns 100.00 for 100.00ABC, but we want it be an error.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToReal(const std::string &str, Real &value, bool trimParens,
                            bool allowOverflow)
{
   #ifdef DEBUG_TOREAL
   MessageInterface::ShowMessage
      ("GmatStringUtil::ToReal() entered, str='%s'\n", str.c_str());
   #endif
   
   Integer errCode = 0;
   if (IsValidReal(str, value, errCode, trimParens, allowOverflow))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool ToInteger(const std::string &str, Integer *value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @see ToInteger(const std::string &str, Integer &value)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToInteger(const std::string &str, Integer *value, bool trimParens, 
	bool allowOverflow)
{
   return ToInteger(str, *value, trimParens, allowOverflow);
}

//------------------------------------------------------------------------------
// bool ToInteger(const char *str, Integer &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @see ToInteger(const std::string &str, Integer &value, ...)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToInteger(const char *str, Integer &value, bool trimParens,
                               bool allowOverflow)
{
   return ToInteger(std::string(str), value, trimParens, allowOverflow);
}

//------------------------------------------------------------------------------
// bool ToInteger(const std::string &str, Integer &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * This method converts string to Integer (signed integer) using atoi() after
 * validation.
 *
 * @param  str			input string to be converted to Integer
 * @param  value		output Integer value
 * @param  trimParens	trim parentheses
 * @param  allowOverflow warn (true) or error (false) on overflow
 *
 * @return true if input string represents valid Integer number, false otherwise
 *
 * @note  atoi() returns 0 for X or 100 for 100ABC, but we want it be an error.
 *        This method returns signed interger value.
 *        The minimum value of Integer is -2147483647.
 *        The maximum value of Integer is +2147483647.
 *        The value out of this range will return complementary number
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToInteger(const std::string &str, Integer &value, bool trimParens,
                               bool allowOverflow)
{
   std::string str2 = Trim(str, BOTH);
   if (trimParens)
   {
      str2 = RemoveExtraParen(str2);
      str2 = Trim(str2, BOTH);
   }

   if (str2.length() == 0)
      return false;

   // if string is just + or - with no number, return false
   if ((str2.length() == 1) &&
	   ((str2[0] == '-') || (str2[0] == '+')))
	   return false;

   if ((str2[0] != '+') && (str2[0] != '-') && !isdigit(str2[0]))
      return false;

   // Remove blanks after + or - sign
   if (str2[0] == '+' || str2[0] == '-')
   {
      std::string::size_type firstBlank = str2.find_first_of(" ", 1);
      std::string::size_type firstNonBlank = str2.find_first_not_of(" ", 1);
      if (firstBlank != str2.npos)
      {
         if (firstNonBlank != str2.npos && firstBlank < firstNonBlank)
         {
            std::string str3 = str2.substr(firstNonBlank);
            str2 = str2[0] + str3;
         }
      }
   }

   for (unsigned int i=0; i<str2.length(); i++)
   {
      if ((i == 0) && ((str2[0] == '+') || (str2[0] == '-')))
         continue;

      if (!isdigit(str2[i]))
         return false;
   }
   
   errno = 0;
   //long int strtol ( const char * str, char ** endptr, int base );
   // Changed the value of base To 10 (Fix for GMT-3273, LOJ: 2012.11.13)
   // With base of 0, it assumes octal if string starts with 0
   //value = strtol(str2.c_str(), NULL, 0);
   value = strtol(str2.c_str(), NULL, 10);
   if (errno == ERANGE)
   {
		if (allowOverflow)
			MessageInterface::ShowMessage
				("GmatStringUtil::ToInteger('%s') out of range error, value=%d\n", str.c_str(), value);
		else
			return false;
   }
   return true;
}


//------------------------------------------------------------------------------
// bool ToUnsignedInt(const std::string &str, UnsignedInt *value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @see ToUnsignedInt(const std::string &str, UnsignedInt &value)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToUnsignedInt(const std::string &str, UnsignedInt *value, bool trimParens, 
	bool allowOverflow)
{
   return ToUnsignedInt(str, *value, trimParens, allowOverflow);
}


//------------------------------------------------------------------------------
// bool ToUnsignedInt(const std::string &str, UnsignedInt &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * This method converts string to Integer (signed integer) using atoi() after
 * validation.
 *
 * @param  str			input string to be converted to Integer
 * @param  value		output UnsignedInt value
 * @param  trimParens	trim parentheses
 * @param  allowOverflow warn (true) or error (false) on overflow
 *
 * @return true if input string represents valid Integer number, false otherwise
 *
 * @note  atoi() returns 0 for X or 100 for 100ABC, but we want it be an error.
 *        This method returns unsigned integer value.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToUnsignedInt(const std::string &str, UnsignedInt &value, bool trimParens,
	bool allowOverflow)
{
   std::string str2 = Trim(str, BOTH);
   if (trimParens)
   {
      str2 = RemoveExtraParen(str2);
      str2 = Trim(str2, BOTH);
   }

   if (str2.length() == 0)
      return false;

   // if string is just + or - with no number, return false
   if ((str2.length() == 1) &&
	   ((str2[0] == '-') || (str2[0] == '+')))
	   return false;

   if ((str2[0] != '+') && !isdigit(str2[0]))
      return false;

   // Remove blanks after + sign
   if (str2[0] == '+')
   {
      std::string::size_type firstBlank = str2.find_first_of(" ", 1);
      std::string::size_type firstNonBlank = str2.find_first_not_of(" ", 1);
      if (firstBlank != str2.npos)
      {
         if (firstNonBlank != str2.npos && firstBlank < firstNonBlank)
         {
            std::string str3 = str2.substr(firstNonBlank);
            str2 = str2[0] + str3;
         }
      }
   }

   for (unsigned int i=0; i<str2.length(); i++)
   {
      if ((i == 0) && (str2[0] == '+'))
         continue;

      if (!isdigit(str2[i]))
         return false;
   }
   
   errno = 0;
   //long int strtoul ( const char * str, char ** endptr, int base );
   // Changed the value of base To 10 (Fix for GMT-3273, LOJ: 2012.11.13)
   // With base of 0, it assumes octal if string starts with 0
   //value = strtol(str2.c_str(), NULL, 0);
   value = strtoul(str2.c_str(), NULL, 10);
   if (errno == ERANGE)
   {
		if (allowOverflow)
			MessageInterface::ShowMessage
				("GmatStringUtil::ToUnsignedInt('%s') out of range error, value=%d\n", str.c_str(), value);
		else
			return false;
   }
   return true;
}


//------------------------------------------------------------------------------
// bool ToBoolean(const std::string &str, bool *value, bool trimParens = false)
//------------------------------------------------------------------------------
bool GmatStringUtil::ToBoolean(const std::string &str, bool *value, bool trimParens)
{
   return ToBoolean(str, *value, trimParens);
}


//------------------------------------------------------------------------------
// bool ToBoolean(const std::string &str, bool &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @return true if string value is one of case insensitive [true, false, on, off].
 * @note See GMAT Software Specification on Boolean Type. (2012.06.14)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToBoolean(const std::string &str, bool &value, bool trimParens)
{
   std::string str2 = Trim(str, BOTH);
   if (trimParens)
   {
      str2 = RemoveExtraParen(str2);
      str2 = Trim(str2, BOTH);
   }

   if (str2.length() == 0)
      return false;

   std::string allLower = ToLower(str2);
   if (allLower == "true" || allLower == "on")
   {
      value = true;
      return true;
   }

   if (allLower == "false" || allLower == "off")
   {
      value = false;
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// bool ToOnOff(const std::string &str, std::string &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @return true if string value is one of case insensitive [true, false, on, off].
 * @note See GMAT Software Specification on OnOff Type. (2012.06.14)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToOnOff(const std::string &str, std::string &value, bool trimParens)
{
   std::string str2 = Trim(str, BOTH);
   if (trimParens)
   {
      str2 = RemoveExtraParen(str2);
      str2 = Trim(str2, BOTH);
   }
   
   if (str2.length() == 0)
      return false;
   
   std::string allLower = ToLower(str2);
   if (allLower == "true" || allLower == "on")
   {
      value = "On";
      return true;
   }
   
   if (allLower == "false" || allLower == "off")
   {
      value = "Off";
      return true;
   }
   
   return false;
}

//------------------------------------------------------------------------------
// RealArray ToRealArray(const std::string &str, bool allowOverflow, ...)
//------------------------------------------------------------------------------
RealArray GmatStringUtil::ToRealArray(const std::string &str, bool allowOverflow,
                                      bool allowSemicolon)
{
   #ifdef DEBUG_TO_REAL_ARRAY
   MessageInterface::ShowMessage
      ("ToRealArray() str='%s', allowSemicolon=%d\n", str.c_str(), allowSemicolon);
   #endif
   RealArray realArray;
   
   if (!IsBracketBalanced(str, "[]"))
      return realArray;

   std::string str1 = RemoveOuterString(str, "[", "]");
   str1 = Trim(str1);

   if (str1 == "")
      return realArray;
   
   std::string delimiter = " ,";
   if (allowSemicolon)
      delimiter = " ,;";
   
   #ifdef DEBUG_TO_REAL_ARRAY
   MessageInterface::ShowMessage("   delimiter='%s'\n", delimiter.c_str());
   #endif
   
   //StringArray vals = SeparateBy(str1, " ,");
   StringArray vals = SeparateBy(str1, delimiter);
   Real rval;
   
   #ifdef DEBUG_TO_REAL_ARRAY
   MessageInterface::ShowMessage("   vals.size()=%d\n", vals.size());
   #endif
   
   for (UnsignedInt i=0; i<vals.size(); i++)
   {
      if (ToReal(vals[i], rval, false, allowOverflow))
          realArray.push_back(rval);
      else
         throw UtilityException
            ("Invalid Real value \"" + vals[i] + "\" found in \"" + str + "\"");
   }

   return realArray;
}

//------------------------------------------------------------------------------
// IntegerArray ToIntegerArray(const char *str, bool allowOverflow)
//------------------------------------------------------------------------------
IntegerArray GmatStringUtil::ToIntegerArray(const char *str, bool allowOverflow)
{
   return ToIntegerArray(std::string(str), allowOverflow);
}

//------------------------------------------------------------------------------
// IntegerArray ToIntegerArray(const std::string &str)
//------------------------------------------------------------------------------
IntegerArray GmatStringUtil::ToIntegerArray(const std::string &str, bool allowOverflow)
{
   IntegerArray intArray;

   if (!IsBracketBalanced(str, "[]"))
      return intArray;

   std::string str1 = RemoveOuterString(str, "[", "]");
   str1 = Trim(str1);

   if (str1 == "")
      return intArray;

   StringArray vals = SeparateBy(str1, " ,");
   Integer ival;

   for (UnsignedInt i=0; i<vals.size(); i++)
   {
      if (ToInteger(vals[i], ival, false, allowOverflow))
          intArray.push_back(ival);
      else
         throw UtilityException
            ("Invalid Integer value \"" + vals[i] + "\" found in \"" + str + "\"");
   }

   return intArray;
}


//------------------------------------------------------------------------------
// UnsignedIntArray ToUnsignedIntArray(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Parse string to unsigned int array.
 * [0 127 255] or [0, 127, 255] to array of 0, 127, 255.
 */
//------------------------------------------------------------------------------
UnsignedIntArray GmatStringUtil::ToUnsignedIntArray(const std::string &str, bool allowOverflow)
{
   UnsignedIntArray intArray;

   if (!IsBracketBalanced(str, "[]"))
      return intArray;

   std::string str1 = RemoveOuterString(str, "[", "]");
   str1 = Trim(str1);

   if (str1 == "")

      return intArray;

   StringArray vals = SeparateBy(str1, " ,");
   UnsignedInt ival;

   for (UnsignedInt i=0; i<vals.size(); i++)
   {
      if (ToUnsignedInt(vals[i], ival, false, allowOverflow))
          intArray.push_back((UnsignedInt)ival);
      else
         throw UtilityException
            ("Invalid Unsigned Integer value \"" + vals[i] + "\" found in \"" + str + "\"");
   }

   return intArray;
}

//------------------------------------------------------------------------------
// StringArray ToStringArray(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Parse a brace-enclosed string to a string array.
 * {'str1', 'str2', 'str3']  to an array of 'str1', 'str2', 'str3'.
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::ToStringArray(const std::string &str)
{
   #ifdef DEBUG_STRING_UTIL_STRING_ARRAY
      MessageInterface::ShowMessage("Entering GmatStringUtil::ToStringArray with str = %s\n", str.c_str());
   #endif
   StringArray strArray;

   if (!IsBracketBalanced(str, "{}"))
   {
      std::string errmsg = "String array \"";
      errmsg += str + "\" does not have matching braces.\n";
      throw UtilityException(errmsg);
   }

   std::string str1 = RemoveOuterString(str, "{", "}");
   str1 = Trim(str1);
   #ifdef DEBUG_STRING_UTIL_STRING_ARRAY
      MessageInterface::ShowMessage("      after Trim, str1 = %s\n", str1.c_str());
   #endif

   if (str1 == "")
      return strArray;

   StringArray strVals = SeparateByComma(str1);
   for (UnsignedInt i=0; i<strVals.size(); i++)
   {
      std::string str2 = Trim(strVals.at(i));
      strArray.push_back(str2);
      #ifdef DEBUG_STRING_UTIL_STRING_ARRAY
         MessageInterface::ShowMessage("      extracted string = %s\n", str2.c_str());
      #endif
   }

   return strArray;
}


//------------------------------------------------------------------------------
// BooleanArray ToBooleanArray(const std::string &str)
//------------------------------------------------------------------------------
BooleanArray GmatStringUtil::ToBooleanArray(const std::string &str)
{
   BooleanArray boolArray;

   if (!IsBracketBalanced(str, "[]"))
      return boolArray;

   std::string str1 = RemoveOuterString(str, "[", "]");
   str1 = Trim(str1);
   
   if (str1 == "")
      return boolArray;

   StringArray vals = SeparateBy(str1, " ,");
   bool bval;

   for (UnsignedInt i=0; i<vals.size(); i++)
   {
      if (ToBoolean(vals[i], bval))
          boolArray.push_back(bval);
      else
         throw UtilityException
            ("Invalid Boolean value \"" + vals[i] + "\" found in \"" + str + "\"");
   }

   return boolArray;
}


//------------------------------------------------------------------------------
// void ParseParameter(const std::string &str, std::string &type,
//                     std::string &owner, std::string &dep)
//------------------------------------------------------------------------------
/*
 * Parses input string as Parameter elements.
 * Parameter is in the form of: Owner.Dependency.Type
 * Owner is the owener object of the Type and Dependency is defined by the
 * Parameter class of Type.  Dependency can be omitted.
 *    such as Sat.MJ2000Eq.X, Sat.Y, Sat.Earth.ECC, ImpBurn.B
 *
 * @see Parameter
 *
 * @param  str  input parameter string
 * @param  type  output type of parameter
 * @param  owner  ouput owner of parameter
 * @param  dep  output dependency of parameter
 */
//------------------------------------------------------------------------------
void GmatStringUtil::ParseParameter(const std::string &str, std::string &type,
                                    std::string &owner, std::string &dep)
{
   type  = "";
   owner = "";
   dep   = "";
   //find owner.dep.type
   std::string str1 = str;
   std::string::size_type pos1 = str1.find(".");
   std::string::size_type pos2 = str1.find_last_of(".");

   if (pos1 != str1.npos && pos2 != str1.npos)
   {
      owner = str1.substr(0, pos1);
      type = str1.substr(pos2+1, str1.npos-pos2);

      dep = "";
      if (pos2 > pos1)
         dep = str1.substr(pos1+1, pos2-pos1-1);
   }

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::ParseParameter() str=%s, type=%s, owner=%s, dep=%s\n",
       str.c_str(), type.c_str(), owner.c_str(), dep.c_str());
   #endif

}


//------------------------------------------------------------------------------
// void GetArrayCommaIndex(const std::string &str, Integer &comma,
//                         const std::string &bracketPair = "()")
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayCommaIndex(const std::string &str, Integer &comma,
                                        const std::string &bracketPair)
{
   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("GmatStringUtil::GetArrayCommaIndex() str=%s\n", str.c_str());
   #endif

   comma = -1;
   std::string openStr = bracketPair.substr(0,1);
   std::string::size_type openBracket = str.find(openStr);
   std::string::size_type firstComma;

   Integer length = str.size();
   std::string str1 = str.substr(openBracket+1, length-openBracket-2);

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("   openBracket=%u, str1=%s\n", openBracket, str1.c_str());
   #endif

   // if array index is empty, Arr()
   if (str1 == "")
      return;


   // if there is no opening bracket
   if (openBracket == str.npos)
   {
      comma = -1;  // wcs - 2007.11.16 - I think we just want to return -1 here ...
      //firstComma = str.find(',');
      //if (firstComma != str.npos)
      //   comma = firstComma;

      #if DEBUG_ARRAY_INDEX
      MessageInterface::ShowMessage
         ("GmatStringUtil::GetArrayCommaIndex() comma=%d\n", comma);
      #endif

      return;
   }

   Integer open, close;
   bool isOuterBracket;
   length = str1.size();

   // find matching closing parenthesis
   FindMatchingBracket(str1, open, close, isOuterBracket, bracketPair, 0);

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("   length=%d, open=%d, close=%d, isOuterBracket=%d\n",
       length, open, close, isOuterBracket);
   #endif


   firstComma = str1.find(',');


   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage("   firstComma=%u\n", firstComma);
   #endif

   // if closing paren found
   if (close != -1)
   {
      std::string::size_type commaAfterClose = str1.find(',', close);

      // if row is missing
      if (commaAfterClose == str1.npos && firstComma > UnsignedInt(open))
      {
         firstComma = str1.npos;
      }
      // if row has parenthesis
      else if (commaAfterClose != str1.npos)
      {
         firstComma = close + 1;
      }
   }

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage("   firstComma=%u\n", firstComma);
   #endif

   if (firstComma != str1.npos)
      comma = firstComma + openBracket + 1;

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("GmatStringUtil::GetArrayCommaIndex() comma=%d\n", comma);
   #endif
}


//------------------------------------------------------------------------------
// void GetArrayIndexVar(const std::string &str, std::string &rowStr,
//                       std::string &colStr, std::string &name,
//                       const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * Returns array row and column index as string. If open bracket is not found,
 * it will set "-1" as row and column string index and input string as name.
 * If row or column is missing it will "-1" to index string.
 *    Arr     will set rowStr to "-1", colStr to "-1", name to "Arr"
 *    Arr(1,) will set rowStr to  "1", colStr to "-1", name to "Arr"
 *    Arr(,1) will set rowStr to "-1", colStr to  "1", name to "Arr"
 *    Arr(,)  will set rowStr to "-1", colStr to "-1", name to "Arr"
 *    Arr()   will set rowStr to "-1", colStr to "-1", name to "Arr"
 *
 * @note It will remove all white spaces between array index
 *
 * The input array string can be a form of:
 *    array[3,3], table(1,b), array(arr1(1,1), arr2(2,2)), etc
 *
 * If input string is array1(1,1) the output will be:
 *    rowStr = "1", colStr = "1", name = "array1"
 * If input string is array2(var1, arr1(1,1)) the output will be:
 *    rowStr = "var1", colStr = "arr1(1,1)", name = "array2"
 * If input string is array2(var1, arr1(1,  c(a, b))) the output will be:
 *    rowStr = "var1", colStr = "arr1(1,c(a,b))", name = "array2"
 *
 * @param  str  input array string
 * @param  rowStr  output row string index
 * @param  colStr  output column string index
 * @param  name  output array name
 * @param  bracketPair  bracket pair used in the input array, such as "[]", "()"
 *
 * @exception UtilityException  if parentheses are not balanced, eg) Arr(a, b(3,2
 */
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndexVar(const std::string &str, std::string &rowStr,
                                      std::string &colStr, std::string &name,
                                      const std::string &bracketPair)
{
   std::string str1;
   str1 = Trim(str, BOTH, true, true);
   str1 = RemoveAll(str1, ' ');
   std::string openStr = bracketPair.substr(0,1);
   std::string closeStr = bracketPair.substr(1,1);
   
   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("GmatStringUtil::GetArrayIndexVar() str=%s\n   str1=%s\n",
       str.c_str(), str1.c_str());
   #endif

   // Check if bracket is balanced
   if (!IsBracketBalanced(str1, bracketPair))
   {
      UtilityException ex;
      ex.SetDetails("Text has unbalanced brackets: \"%s\"\n", str.c_str());
      throw ex;
   }

   rowStr = "-1";
   colStr = "-1";
   name = str1;

   std::string::size_type openBracket = str1.find(openStr);

   // if there is no opening bracket
   if (openBracket == str1.npos)
      return;

   name = str1.substr(0, openBracket);

   Integer length = str1.size();
   std::string str2 = str1.substr(openBracket+1, length-openBracket-2);

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage("   str2=%s\n", str2.c_str());
   #endif

   // if array index is empty, Arr()
   if (str2 == "")
      return;

   Integer comma;

   // get array comma index
   GetArrayCommaIndex(str1, comma, bracketPair);
      
   UnsignedInt closeBracket = str1.size() - 1;
   
   // if single array, such as a(5), b(a(5,5)), set row string as "1"
   if (comma == -1)
   {
      rowStr = "1";
      colStr = str1.substr(openBracket+1, closeBracket-openBracket-1);
   }
   else
   {
      rowStr = str1.substr(openBracket+1, comma-openBracket-1);
      colStr = str1.substr(comma+1, closeBracket-comma-1);
   }

   name = str1.substr(0, openBracket);

   if (rowStr == "")
      rowStr = "-1";

   if (colStr == "")
      colStr = "-1";

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("StringUtil::GetArrayIndexVar() rowStr=%s, colStr=%s, name=%s\n",
       rowStr.c_str(), colStr.c_str(), name.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void GetArrayIndex(const std::string &str, Integer &row, Integer &col,
//                    std::string &name, const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * Returns array integer row and column index starting from 0.  This method calls
 * GetArrayIndexVar() and if row and column string index string is not valid
 * integer, it returns -1 as array index.
 *
 * @see GetArrayIndexVar()
 *
 * @param  str  input array string
 * @param  row  output row integer index starting from 0, -1 if invalid index
 * @param  col  output column integer index starting from 0, -1 if invalid index
 * @param  name  output array name
 * @param  bracketPair  bracket pair used in the input array, such as "[]", "()"
 */
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndex(const std::string &str, Integer &row,
                                   Integer &col, std::string &name,
                                   const std::string &bracketPair)
{
   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("StringUtil::GetArrayIndex() str=%s\n", str.c_str());
   #endif

   std::string rowStr;
   std::string colStr;
   row = -1;
   col = -1;
   Integer intVal;
   
   GetArrayIndexVar(str, rowStr, colStr, name, bracketPair);
   
   // Check row string
   // @note Do not just return if row number failed, we also want to check column number
   if (rowStr != "-1")
   {
      if (ToInteger(rowStr, intVal))
      {
         if (intVal <= 0)
            row = -1;
         else if (bracketPair == "()")
            row = intVal - 1; // array index start at 0
         else
            row = intVal;
      }
   }
   
   // Check column string
   if (colStr != "-1")
   {
      if (ToInteger(colStr, intVal))
      {
         if (intVal <= 0)
            col = -1;
         if (bracketPair == "()")
            col = intVal - 1; // array index start at 0
         else
            col = intVal;
      }
   }
   
   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("StringUtil::GetArrayIndex() row=%d, col=%d, name=%s\n", row, col, name.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void GetArrayIndex(const std::string &str, std::string &rowStr,
//                    std::string &colStr, Integer &row, Integer &col,
//                    std::string &name, const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * Returns array row and column index as string and integer. This method calls
 * GetArrayIndexVar() and if row and column string index is "-1", it returns
 * -1 as array index.
 *
 * @see GetArrayIndexVar()
 *
 * @param  str  input array string
 * @param  rowStr  output row string index
 * @param  colStr  output column string index
 * @param  row  output row integer index
 * @param  col  output column integer index
 * @param  name  output array name
 * @param  bracketPair  bracket pair used in the input array, such as "[]", "()"
 */
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndex(const std::string &str, std::string &rowStr,
                                   std::string &colStr, Integer &row, Integer &col,
                                   std::string &name, const std::string &bracketPair)
{
   row = -1;
   col = -1;
   Integer intVal;

   GetArrayIndexVar(str, rowStr, colStr, name, bracketPair);

   // array index starts at 0
   if (rowStr != "-1")
      if (ToInteger(rowStr, intVal))
         row = intVal - 1;

   if (colStr != "-1")
      if (ToInteger(colStr, intVal))
         col = intVal - 1;
}


//------------------------------------------------------------------------------
// std::string GetArrayName(const std::string &str, const std::string &bracketPair)
//------------------------------------------------------------------------------
/**
 * Extracts array name form array elememnt string, ie, MyArray(1,2) returns
 * MyArray.
 *
 * @parameter  str  Array element string to be parsed for array name
 * @param  bracketPair  bracket pair used in the input array, such as [], () ["()"]
 *
 * @return  Return array name if input string is array element form;
 *              input string if input string is not a form of array 
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::GetArrayName(const std::string &str,
                                         const std::string &bracketPair)
{
   std::string rowStr, colStr, name;
   
   try
   {
      // LOJ: 2016.03.10 - Coding error
      //GetArrayIndexVar(str, rowStr, colStr, name, "()"); 
      GetArrayIndexVar(str, rowStr, colStr, name, bracketPair);
   }
   catch (UtilityException &)
   {
      return str;
   }
   
   return name;
}


//------------------------------------------------------------------------------
// bool IsOneElementArray(const std::string &str)
//------------------------------------------------------------------------------
bool GmatStringUtil::IsOneElementArray(const std::string &str)
{
   Integer row = -1, col = -1;
   std::string name;
   
   GetArrayIndex(str, row, col, name, "[]");
   
   if (row == 1 && col == 1)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsSimpleArrayElement(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Checks if string is a simple array element such a(1,1), b(c,d).
 * It will return false for a(b(1,1), c(1,1)) or a(1+1, 2*2).
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsSimpleArrayElement(const std::string &str)
{
   #if DEBUG_SIMPLE_ARRAY
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsSimpleArrayElement() entered, str='%s'\n", str.c_str());
   #endif
   
   // Check for multiple commas
   Integer numComma = NumberOfOccurrences(str, ',');
   if (numComma != 1)
   {
      #if DEBUG_SIMPLE_ARRAY
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsSimpleArrayElement() returning false, it has multiple commas\n");
      #endif
      return false;
   }
   
   // Check for math symbols
   if (IsThereMathSymbol(str))
   {
      #if DEBUG_SIMPLE_ARRAY
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsSimpleArrayElement() returning false, it has math symbols\n");
      #endif
      return false;
   }
   
   // Check for valid array element
   bool retval = false;
   std::string str1 = RemoveOuterParen(str);
   std::string rowStr, colStr, name;
   
   #if DEBUG_SIMPLE_ARRAY
   MessageInterface::ShowMessage
      ("   name='%s', rowStr='%s', colStr='%s'\n", name.c_str(), rowStr.c_str(), colStr.c_str());
   #endif
   
   GetArrayIndexVar(str1, rowStr, colStr, name, "()");
   if (IsValidName(name) && rowStr != "-1" && colStr != "-1")
      retval = true;
   
   #if DEBUG_SIMPLE_ARRAY
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsSimpleArrayElement() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void FindFirstAndLast(const std::string &str, char ch, Integer &first,
//                       Integer &last)
//------------------------------------------------------------------------------
/*
 * Finds first and last index of given chracter from the string.
 * if given chracter is not found it sets to -1
 *
 * @param  str  input string
 * @param  ch  input character to find
 * @param  first  output index of first input character found
 * @param  last  output index of last input character found
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindFirstAndLast(const std::string &str, char ch,
                                      Integer &first, Integer &last)
{
   first = str.find_first_of(ch);
   if (first == (Integer)str.npos)
      first = -1;

   last = str.find_last_of(ch);
   if (last == (Integer)str.npos)
      last = -1;
}


//------------------------------------------------------------------------------
// void FindParenMatch(const std::string &str, Integer &openParen,
//                     Integer &closeParen, bool &isOuterParen)
//------------------------------------------------------------------------------
/*
 * Finds open and close parentheses index from the string.
 * if parhenthesis is not found it sets to -1
 *
 * @param <str> input string
 * @param <openParen> set to index of first open parenthesis
 * @param <openParen> set to index of matching close parenthesis of openParen
 * @param <isOuterParen> set to true if item is enclosed with parentheses
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindParenMatch(const std::string &str, Integer &openParen,
                                    Integer &closeParen, bool &isOuterParen)
{
   //MessageInterface::ShowMessage("FindParenMatch() str=%s\n", str.c_str());

   openParen = str.find_first_of('(');
   if (openParen == (Integer)str.npos)
      openParen = -1;

   closeParen = str.find_last_of(')');
   if (closeParen == (Integer)str.npos)
      closeParen = -1;

   std::string str1 = str;
   if (openParen != -1)
      str1 = str.substr(openParen);

   isOuterParen = IsOuterParen(str1);

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::FindParenMatch() str=%s, openParen=%d, closeParen=%d, "
       "isOuterParen=%d\n", str.c_str(), openParen, closeParen, isOuterParen);
   #endif
}


//------------------------------------------------------------------------------
// void FindMatchingParen(const std::string &str, Integer &openParen,
//                        Integer &closeParen, bool &isOuterParen,
//                        Integer start = 0)
//------------------------------------------------------------------------------
/*
 * Finds matching close parenthesis of the first open parenthesis.
 * if parhenthesis is not found it sets to -1
 *
 * @param <str> input string
 * @param <openParen> set to index of first open parenthesis
 * @param <closeParen> set to index of matching close parenthesis of openParen
 * @param <isOuterParen> set to true if item is enclosed with parentheses
 * @param <start> input starting index
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindMatchingParen(const std::string &str, Integer &openParen,
                                       Integer &closeParen, bool &isOuterParen,
                                       Integer start)
{
   #ifdef DEBUG_FIND_MATCHING_BRACKET
   MessageInterface::ShowMessage
      ("FindMatchingParen() start=%d, str=%s\n", start, str.c_str());
   #endif

   // initialize output parameters
   openParen = -1;
   closeParen = -1;
   isOuterParen = false;

   int length = str.size();
   Integer openCounter = 0;

   for (int i=start; i<length; i++)
   {
      if (str[i] == '(')
      {
         openCounter++;
         if (openCounter == 1)
            openParen = i;
      }
      else if (str[i] == ')')
      {
         openCounter--;
         closeParen = i;
         if (openCounter == 0)
            break;
      }
   }
   
   if (openParen == 0 && closeParen == length-1)
      isOuterParen = true;
   
   #ifdef DEBUG_FIND_MATCHING_BRACKET
   MessageInterface::ShowMessage
      ("FindMatchingParen() str=%s, openParen=%d, closeParen=%d, isOuterParen=%d\n",
       str.c_str(), openParen, closeParen, isOuterParen);
   #endif
}


//------------------------------------------------------------------------------
// void FindMatchingBracket(const std::string &str, Integer &openBracket,
//                          Integer &closeBracket, bool &isOuterBracket,
//                          const std::string &bracket, Integer start = 0)
//------------------------------------------------------------------------------
/*
 * Finds matching closing bracket of the first open bracket.
 * if parhenthesis is not found it sets to -1
 *
 * @param <str> input string
 * @param <openBracket> set to index of first open bracket
 * @param <closeBracket> set to index of matching close bracket of openBracket
 * @param <isOuterBracket> set to true if item is enclosed with bracket
 * @param <bracket> input open/close bracket (eg: "()", "{}", "[]", "<>")
 * @param <start> input starting index
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindMatchingBracket(const std::string &str, Integer &openBracket,
                                         Integer &closeBracket, bool &isOuterBracket,
                                         const std::string &bracket, Integer start)
{
   #ifdef DEBUG_FIND_MATCHING_BRACKET
   MessageInterface::ShowMessage
      ("FindMatchingBracket() start=%d, str=%s\n", start, str.c_str());
   #endif

   // initialize output parameters
   openBracket = -1;
   closeBracket = -1;
   isOuterBracket = false;

   int length = str.size();
   Integer openCounter = 0;

   for (int i=start; i<length; i++)
   {
      if (str[i] == bracket[0])
      {
         openCounter++;
         if (openCounter == 1)
            openBracket = i;
      }
      else if (str[i] == bracket[1])
      {
         openCounter--;
         closeBracket = i;
         if (openCounter == 0)
            break;
      }
   }

   if (openBracket == 0 && closeBracket == length-1)
      isOuterBracket = true;

   #ifdef DEBUG_FIND_MATCHING_BRACKET
   MessageInterface::ShowMessage
      ("FindMatchingBracket() str=%s, openBracket=%d, closeBracket=%d, "
       "isOuterBracket=%d\n", str.c_str(), openBracket, closeBracket,
       isOuterBracket);
   #endif
}


//------------------------------------------------------------------------------
// void FindLastParenMatch(const std::string &str, Integer &openParen,
//                         Integer &closeParen, Integer start = 0)
//------------------------------------------------------------------------------
/*
 * Finds last matching close parenthesis.
 *
 * @param <str> input string
 * @param <openParen> set to index of last first open parenthesis
 * @param <closeParen> set to index of matching close parenthesis of last openParen
 * @param <start> input starting index
 *
 * @return last matching parenthesis index, -1 if close parenthesis not found
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindLastParenMatch(const std::string &str, Integer &openParen,
                                        Integer &closeParen, Integer start)
{
   //MessageInterface::ShowMessage
   //   ("FindLastParenMatch() start=%d, str=%s\n", start, str.c_str());

   Integer open1, close1;
   bool isOuterParen;
   Integer start1 = start;
   bool done = false;

   while (!done)
   {
      FindMatchingParen(str, open1, close1, isOuterParen, start1);
      //MessageInterface::ShowMessage
      //   ("   ===> start=%d, open1=%d, close1=%d\n", start1, open1, close1);

      // find next open parenthesis '('
      start1 = str.find('(', close1);

      if (start1 == -1)
         done = true;
   }

   openParen = open1;
   closeParen = close1;
}


//------------------------------------------------------------------------------
// bool IsEnclosedWith(const std::string &str, const string &enclosingStr)
//------------------------------------------------------------------------------
/*
 * Returns true if item is enclosed with given enclosing string.
 *
 * @param  str  Input string to check for enclosed with
 * @param  enclosingStr  Input enclosing string to look for enclosed with
 *
 * @return true if string is enclosed with given enclosing string
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsEnclosedWith(const std::string &str,
                                    const std::string &enclosingStr)
{
   if (str == "")
      return false;
   
   return (StartsWith(str, enclosingStr) && EndsWith(str, enclosingStr));
}


//------------------------------------------------------------------------------
// bool IsEnclosedWithExtraParen(const std::string &str, bool checkOps = true,
//                               bool ignoreComma = false)
//------------------------------------------------------------------------------
/*
 * Returns true if item is enclosed with extra parentheses
 * It will return true: ((a+b)), (a(1,1)),
 * It wiill return false: (123.456), (1,2), (a*b(1,1)), ((3+5)*2), (), (())
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsEnclosedWithExtraParen(const std::string &str, bool checkOps,
                                              bool ignoreComma)
{
   #if DEBUG_EXTRA_PAREN
   MessageInterface::ShowMessage
      ("IsEnclosedWithExtraParen() entered, str=%s, checkOps=%d, ignoreComma=%d\n",
       str.c_str(), checkOps, ignoreComma);
   #endif
   
   int length = str.size();
   bool isEnclosed = false;
   Integer openCounter = 0;
   Integer openParen = (Integer)str.find_first_of('(');
   Integer closeParen = (Integer)str.find_last_of(')');
   
   #if DEBUG_EXTRA_PAREN
   MessageInterface::ShowMessage
      ("IsEnclosedWithExtraParen() openParen=%d, closeParen=%d\n", openParen, closeParen);
   #endif
   
   if (openParen == -1 || closeParen == -1)
   {
      #if DEBUG_EXTRA_PAREN
      MessageInterface::ShowMessage
         ("IsEnclosedWithExtraParen() returning false, no open or close paren found\n");
      #endif
      return false;
   }
   
   if (openParen != 0 || closeParen != length-1)
   {
      #if DEBUG_EXTRA_PAREN
      MessageInterface::ShowMessage
         ("IsEnclosedWithExtraParen() returning false, no outer open or close paren found\n");
      #endif
      return false;
   }
   
   // Check for empty parenthesis such as () or (())
   std::string str1 = RemoveAllBlanks(str);
   std::string::size_type lastOpenParen = str1.find_last_of('(');
   std::string::size_type firstCloseParen = str1.find_first_of(')');
   if (firstCloseParen == lastOpenParen + 1)
   {
      #if DEBUG_EXTRA_PAREN
      MessageInterface::ShowMessage
         ("IsEnclosedWithExtraParen() returning false, empty parenthesis found\n");
      #endif
      return false;
   }
   
   for (int i=0; i<length; i++)
   {
      if (str[i] == '(')
      {
         openCounter++;
         if (openCounter == 1)
            openParen = i;
      }
      else if (str[i] == ')')
      {
         openCounter--;
         closeParen = i;
         if (openCounter == 0)
            break;
      }
   }
   
   // if enclosed with open and close parentheses
   if (openParen == 0 && closeParen == length-1)
   {
      // check for double parentheses
      if (str[1] == '(' && str[length-2] == ')')
      {
         Integer open2, close2;
         Integer start = 1;
         bool isOuter;
         
         FindMatchingParen(str, open2, close2, isOuter, start);
         if (close2 == length-2)
            isEnclosed = true;
      }
      
      if (!checkOps)
         isEnclosed = true;
      
      #if DEBUG_EXTRA_PAREN
      MessageInterface::ShowMessage("IsEnclosedWithExtraParen() isEnclosed=%d\n", isEnclosed);
      #endif
      
      // If double parenthesis not found or not checking operators
      if (!isEnclosed)
      {
         // check if there is any operator
         std::string substr = str.substr(1, length-2);
         Real rval;
         
         bool isRealNumber = ToReal(substr, rval);
         bool isSimpleArray = IsSimpleArrayElement(substr);
         
         #if DEBUG_EXTRA_PAREN
         MessageInterface::ShowMessage
            ("IsEnclosedWithExtraParen() substr=%s, isSimpleArray=%d, "
             "isRealNumber=%d\n", substr.c_str(), isSimpleArray, isRealNumber);
         #endif
         
         if (isRealNumber)
            isEnclosed = true;
         else if (isSimpleArray)
            isEnclosed = true;
         else
         {
            #if DEBUG_EXTRA_PAREN
            MessageInterface::ShowMessage
               ("IsEnclosedWithExtraParen() Checking for math symbol and command\n");
            #endif
            // if there is math symbol then it is not enclosed with parenthesis
            if (IsThereMathSymbol(substr))
               isEnclosed = false;
            else if (substr.find(",") != substr.npos)
               if (ignoreComma)
                  isEnclosed = true;
               else
                  isEnclosed = false;
            else
               isEnclosed = true;
         }
      }
   }
   
   #if DEBUG_EXTRA_PAREN
   MessageInterface::ShowMessage
      ("IsEnclosedWithExtraParen() returning isEnclosed=%d\n", isEnclosed);
   #endif
   
   return isEnclosed;
}

//------------------------------------------------------------------------------
// bool IsEnclosedWithBraces(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if entire string is enclosed with braces
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsEnclosedWithBraces(const std::string &str)
{
   return (StartsWith(str, "{") && EndsWith(str, "}"));
}

//------------------------------------------------------------------------------
// bool IsEnclosedWithBrackets(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if entire string is enclosed with brackets (in cases of
 * arrays of Real numbers, e.g.)
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsEnclosedWithBrackets(const std::string &str)
{
   return (StartsWith(str, "[") && EndsWith(str, "]"));
}

//------------------------------------------------------------------------------
// bool IsParenBalanced(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if parentheses are balanced (no mismatching parentheses)
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsParenBalanced(const std::string &str)
{
   int length = str.size();
   Integer openCounter = 0;
   bool retval = false;

   for (int i=0; i<length; i++)
   {
      if (str[i] == '(')
         openCounter++;
      else if (str[i] == ')')
         openCounter--;
   }

   if (openCounter == 0)
      retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// bool IsParenEmpty(const std::string &str)
//------------------------------------------------------------------------------
/**
 *  Checks if there is nothing inside parenthesis such as () or (()).
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsParenEmpty(const std::string &str)
{
   std::string str1 = RemoveAllBlanks(str);
   
   if (!IsParenBalanced(str1))
      return false;
   
   Integer length = str1.size();
   for (int i = 0; i < length; i++)
   {
      if (str1[i] != '(' && str1[i] != ')')
         return false;
   }
   
   std::string::size_type lastOpenParen = str1.find_last_of('(');
   std::string::size_type firstCloseParen = str1.find_first_of(')');
   if (firstCloseParen == (lastOpenParen + 1))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsBracketBalanced(const std::string &str, const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * @return true if brackets are balanced (no mismatching brackets) or
 *         no brackets found
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsBracketBalanced(const std::string &str,
                                       const std::string &bracketPair)
{
   char open = bracketPair[0];
   char close = bracketPair[1];
   int length = str.size();
   Integer openCounter = 0;
   bool retval = true;

   for (int i=0; i<length; i++)
   {
      if (str[i] == open)
         openCounter++;
      else if (str[i] == close)
         openCounter--;
   }

   if (openCounter != 0)
      retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool AreAllBracketsBalanced(const std::string &str,
//                             const std::string &allPairs)
//------------------------------------------------------------------------------
bool GmatStringUtil::AreAllBracketsBalanced(const std::string &str,
                                            const std::string &allPairs)
{
   #ifdef DEBUG_BALANCED_BRACKETS
      MessageInterface::ShowMessage(
          "Entering AreAllBracketsBalanced with str = %s and allPairs = %s\n",
          str.c_str(), allPairs.c_str());
   #endif
   Integer count = allPairs.size();
   if (count%2 == 1)
      throw UtilityException("Invalid number of Bracket pairs\n");
   Integer numPairs = count / 2;
   std::string openBrackets  = allPairs.substr(0, numPairs);
   std::string closeBrackets = allPairs.substr(numPairs);
   #ifdef DEBUG_BALANCED_BRACKETS
      MessageInterface::ShowMessage(
          "   openBrackets = %s and closeBrackets = %s\n",
          openBrackets.c_str(), closeBrackets.c_str());
   #endif

   Integer openCounter = 0;
   IntegerArray bracketsFound;

   int length = str.size();
   bool retval = true;

   for (int i=0; i<length; i++)
   {
      #ifdef DEBUG_BALANCED_BRACKETS
         //MessageInterface::ShowMessage(
         //    "   Now checking character %c at position %d\n", str[i], i);
      #endif
      for (Integer jj = 0; jj < numPairs; jj++)
      {
         if (str[i] == openBrackets[jj])
         {
            #ifdef DEBUG_BALANCED_BRACKETS
               MessageInterface::ShowMessage(
                   "   found an open bracket and it = %c\n", str[i]);
            #endif
            bracketsFound.push_back(jj);
            openCounter++;
         }
         else if (str[i] == closeBrackets[jj])
         {
            #ifdef DEBUG_BALANCED_BRACKETS
               MessageInterface::ShowMessage(
                   "   found a close bracket and it = %c\n", str[i]);
            #endif
            if ((openCounter > 0) && (bracketsFound.at(openCounter-1) == jj))
            {
               bracketsFound.pop_back();
               openCounter--;
            }
            else
               return false;
         }
      }
   }
   
   if (openCounter != 0)
      retval = false;
   
   #ifdef DEBUG_BALANCED_BRACKETS
   MessageInterface::ShowMessage("AreAllBracketsBalanced() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool AreAllNamesValid(const std::string &str, bool blankNameIsOk = false)
//------------------------------------------------------------------------------
/**
 * Checks if names or number are valid in the math equation.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::AreAllNamesValid(const std::string &str, bool blankNameIsOk)
{
   #ifdef DEBUG_VALID_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::AreAllNamesValid() entered, str=<%s>, blankNameIsOk=%d\n",
       str.c_str(), blankNameIsOk);
   #endif
   
   std::string str1 = Trim(str);
   
   // Remove scientific notation first where it might have + or - signs, such as e+7
   str1 = RemoveScientificNotation(str1);
   
   #ifdef DEBUG_VALID_NAME
   MessageInterface::ShowMessage("   After removing <E+E-e+e->, str=<%s>\n", str1.c_str());
   #endif
   
   StringArray parts = SeparateBy(str1, "(),+-*/^'");
   Integer numParts = parts.size();
   std::string str2;
   Real rval;
   
   #ifdef DEBUG_VALID_NAME
   MessageInterface::ShowMessage
      ("   ..... It has %d parts, start checking for any errors\n", numParts);
   #endif
   
   // Go through each part, and check for valid name
   for (Integer i=0; i<numParts; i++)
   {
      str2 = parts[i];
      
      #ifdef DEBUG_VALID_NAME
      MessageInterface::ShowMessage("   parts[%d]=<%s>\n", i, str2.c_str());
      #endif
      
      // Remove unary + or - (assuming all chained unary signs are replaced)
      if (str2[0] == '+' || str2[0] == '-')
      {
         std::string::size_type nonUnary = str2.find_first_not_of("+-");
         if (nonUnary != str2.npos)
            str2 = str2.substr(nonUnary);
      }
      
      #ifdef DEBUG_VALID_NAME
      MessageInterface::ShowMessage
         ("   parts[%d]=<%s>\n   Checking if it is a real number\n", i, str2.c_str());
      #endif
      
      if (!ToReal(str2, rval))
      {
         if (!IsValidName(str2, false, blankNameIsOk))
         {
            if (str2.find_first_of(' ') != str2.npos)
            {
               std::string str3 = RemoveAllBlanks(str2);
               
               // Check spaces between numbers such as 123 456
               if (ToReal(str3, rval))
               {
                  #ifdef DEBUG_VALID_NAME
                  MessageInterface::ShowMessage
                     ("GmatStringUtil::AreAllNamesValid() returning false, <%s> is not a valid number\n",
                      str2.c_str());
                  #endif
                  return false;
               }
               else
               {
                  // Check for invalid name such as starting with number
                  // but Parameter is ok such as sat.X
                  if (!IsValidName(str3) && !IsValidParameterName(str3))
                  {
                     #ifdef DEBUG_VALID_NAME
                     MessageInterface::ShowMessage
                        ("GmatStringUtil::AreAllNamesValid() returning false, <%s> is not a valid name\n",
                         str2.c_str());
                     #endif
                     return false;
                  }
               }
            }
         }
      }
      else
      {
         #ifdef DEBUG_VALID_NAME
         MessageInterface::ShowMessage("   It is real number <%f>\n", rval);
         #endif
      }
   }

   // Cannot determine if names with dot such as "sat .  X" is valid name here, so just return true
   #ifdef DEBUG_VALID_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::AreAllNamesValid() returning true\n");
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// bool IsValidFunctionCall(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Checks if function arguments has valid names or string literal. If input
 * is not a function call, it returns false.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidFunctionCall(const std::string &str)
{
   #ifdef DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidFunctionCall() entered, str='%s'\n", str.c_str());
   #endif
   
   StringArray callItems = ParseFunctionCall(str);
   if (callItems.size() == 0)
   {
      #ifdef DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidFunctionCall() returning false, it is not a function call\n");
      #endif
      return false;
   }
   
   bool isValid = false;
   for (unsigned int i = 0; i < callItems.size(); i++)
   {
      #ifdef DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage("   callItems[%d] = '%s'\n", i, callItems[i].c_str());
      #endif
      // If input arg is enclosed with quotes, it is OK
      if (GmatStringUtil::IsValidName(callItems[i]))
         isValid = true;
      else if (GmatStringUtil::IsEnclosedWith(callItems[i], "'"))
         isValid = true;
   }
   
   #ifdef DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidFunctionCall() returning %d\n", isValid);
   #endif
   return isValid;
}

//------------------------------------------------------------------------------
// bool IsOuterParen(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if outer parentheses is not part of ^(#) or array.
 * It ill return true for (a*b-c), (vec(3,2))
 * It ill return false for (A+b^(-1), (2,2), (abc,def)
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsOuterParen(const std::string &str)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage("IsOuterParen() str=%s\n", str.c_str());
   #endif

   int length = str.size();
   bool isOuterParen = true;

   Integer openParen = str.find_first_of('(');
   if (openParen == (Integer)str.npos)
      openParen = -1;

   Integer closeParen = str.find_last_of(')');
   if (closeParen == (Integer)str.npos)
      closeParen = -1;

   if (openParen != 0 && closeParen != length-1)
      return false;

   if (!IsEnclosedWithExtraParen(str, false))
      return false;

   if (openParen == 0 && closeParen == length-1)
   {
      // make sure ending ) is not part of ^(-1)
      std::string::size_type lastOpenParen = str.find_last_of('(');
      std::string::size_type lastCloseParen = str.find_last_of(')', closeParen-1);

      #if DEBUG_STRING_UTIL > 1
      MessageInterface::ShowMessage
         ("IsOuterParen() lastOpenParen=%d, lastCloseParen=%d\n",
          lastOpenParen, lastCloseParen);
      #endif

      std::string substr = str.substr(lastOpenParen);
      if (lastOpenParen > 0)
         if (str.find("^(", lastOpenParen-1) != str.npos &&
             (lastCloseParen == str.npos || lastCloseParen < lastOpenParen))
             isOuterParen = false;

      #if DEBUG_STRING_UTIL
      MessageInterface::ShowMessage
         ("===> IsOuterParen() isOuterParen=%d\n", isOuterParen);
      #endif

      // make sure ending ) is not part of array
      if (isOuterParen)
         if (IsParenPartOfArray(substr))
            isOuterParen = false;
   }
   else
   {
      isOuterParen = false;
   }

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("IsOuterParen() openParen=%d, closeParen=%d, length=%d, "
       "isOuterParen=%d\n", openParen, closeParen, length, isOuterParen);
   #endif

   return isOuterParen;
}


//------------------------------------------------------------------------------
// bool IsCommaPartOfArray(const std::string &str, Integer start)
//------------------------------------------------------------------------------
/*
 * Finds if first comma after start position is part of an array.
 * It first removes outer parentheses and check for comma.
 *
 * It will true for A(arr2(2,2)), A( b(x,y), I)
 * It will false for A(1,2), A(1, b(c,d))
 *
 * @param  str  input string
 * @param  start  input starting position for searching
 *
 * @return  true if comma is part of an arry, false else
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsCommaPartOfArray(const std::string &str, Integer start)
{
   // First remove parentheses around array
   std::string::size_type open = str.find_first_of("(");
   std::string::size_type close = str.find_last_of(")");
   std::string str1 = str.substr(open+1, close-open-1);

   close = str1.find(")");

   // if close paren not found, return false
   if (close == str.npos)
      return false;

   // if comma is after open paren, comma is part of array
   open = str1.find("(");
   UnsignedInt comma = str1.find(",");

   bool retval = false;

   if (comma > open)
      retval = true;

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("IsCommaPartOfArray() str1=<%s>, comma=%u, open=%u, retval=%d\n",
       str1.c_str(), comma, open, retval);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool IsBracketPartOfArray(const std::string &str,
//                           const std::string &bracketPairs,
//                           bool checkOnlyFirst)
//------------------------------------------------------------------------------
/*
 * Check if string is part of array.
 *
 * @param str  input string
 * @param bracketPairs  bracket pairs used in checking ("()", "([)]")
 * @param checkOnlyFirst  true if checking for first bracket pair
 *
 * return true if parentheses or square bracket is part of an array.
 *    For example: (2,2) or (abc,def) or [2,2], [i,j]
 *
 * return false if no parentheses found or non-alphanumeric char found inside
 * parentheses.
 *    For example: abc, abc((2,2)), (1,), (3,2]
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsBracketPartOfArray(const std::string &str,
                                          const std::string &bracketPairs,
                                          bool checkOnlyFirst)
{
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsBracketPartOfArray() entered, str=%s, bracketPairs=%s, "
       "checkOnlyFirst=%d\n", str.c_str(), bracketPairs.c_str(), checkOnlyFirst);
   #endif
   
   std::string str1 = RemoveAll(str, ' ');
   bool ret = true;
   std::string::size_type index1, index2, comma;
   std::string substr;
   Integer count = bracketPairs.size();
   char openChar = 'x';
   char closeChar = 'x';
   
   if (count%2 == 1)
      throw UtilityException("Invalid number of Bracket pair\n");
   
   std::string openBrackets = bracketPairs.substr(0, count/2);
   std::string closeBrackets = bracketPairs.substr(count/2);
   
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage
      ("   openBrackets=%s, closeBrackets=%s\n", openBrackets.c_str(),
       closeBrackets.c_str());
   #endif
   
   index1 = str1.find_first_of(openBrackets);
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   index1=%u\n", index1);
   #endif
   
   if (index1 == str1.npos)
   {
      #if DEBUG_STRING_UTIL
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsBracketPartOfArray() returning false, no open bracket found.\n");
      #endif
      return false;
   }
   
   
   openChar = str[index1];
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   openChar=%c\n", openChar);
   #endif
   
   if (checkOnlyFirst)
      index2 = str1.find_first_of(closeBrackets, index1);
   else
      index2 = str1.find_last_of(closeBrackets);
   
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   index2=%u\n", index2);
   #endif
   
   if (index2 == str1.npos)
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsBracketPartOfArray() returning false, no close bracket found\n");
      #endif
      return false;
   }
   
   closeChar = str[index2];
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   closeChar=%c\n", closeChar);
   #endif
   
   if ((openChar == '(' && closeChar == ']') ||
       (openChar == '[' && closeChar == ')'))
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsBracketPartOfArray() returning false, open and close bracket don't match.\n");
      #endif
      return false;
   }
   
   
   //-----------------------------------------------------------------
   // str1 does not have open and close bracket
   //-----------------------------------------------------------------
   std::string str2 = str1.substr(index1+1, index2-index1-1);
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   str1=<%s>\n", str1.c_str());
   #endif
   
   if (str2 == "")
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsBracketPartOfArray() returning false, it is empty sub-string.\n");
      #endif
      return false;
   }
   
   
   Integer length = str2.size();
   comma = str2.find(',');
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   comma=%u\n", comma);
   #endif
   
   //-----------------------------------------------------------------
   // if single dimension array
   //-----------------------------------------------------------------
   if (comma == str2.npos)
   {
      substr = str2.substr(0, length-1);
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage("   1st substr=%s\n", substr.c_str());
      #endif
      
      if (IsSingleItem(substr))
         ret = true;
      else
         ret = false;
      
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsBracketPartOfArray() returning false, it is single "
          "dimenstion array. returning ret=%d\n", ret);
      #endif
      
      return ret;
   }
   
   //-----------------------------------------------------------------
   // It's double dimension array
   //-----------------------------------------------------------------
   //substr = str2.substr(0, comma-1);
   substr = str2.substr(0, comma);
   
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   1st substr=%s\n", substr.c_str());
   #endif
   
   if (!IsSingleItem(substr))
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsBracketPartOfArray() returning false, it is double "
          "dimenstion array. 1st index is not a single item\n");
      #endif
      return false;
   }
   
   substr = str2.substr(comma+1, length-comma-1);
   
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   2nd substr=%s\n", substr.c_str());
   #endif
   
   if (!IsSingleItem(substr))
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsBracketPartOfArray() returning false, it is double "
          "dimenstion array. 2nd index is not a single item\n");
      #endif
      
      return false;
   }
   
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsBracketPartOfArray() returning %d, it is double "
       "dimension array\n", ret);
   #endif
   
   return ret;
}


//------------------------------------------------------------------------------
// bool IsParenPartOfArray(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if parentheses is part of an array.
 *    For example: (2,2) or (abc,def)
 *
 * return false if no parentheses found or non-alphanumeric char found inside
 * parentheses.
 *    For example: abc, abc((2,2))
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsParenPartOfArray(const std::string &str)
{
   //MessageInterface::ShowMessage
   //   ("===> GmatStringUtil::IsParenPartOfArray() str=%s\n", str.c_str());

   return IsBracketPartOfArray(str, "()", false);

}


//------------------------------------------------------------------------------
// bool IsThereEqualSign(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if there is equal sign (=) not enclosed with single quotes.
 * It will return when it finds first equal sign not in quotes.
 *
 * @param  return true if it finds equal sign not in quotes, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsThereEqualSign(const std::string &str)
{
   Integer size = str.size();
   bool inQuotes = false;

   for (Integer i=0; i<size; i++)
   {
      if (str[i] == '\'')
      {
         if (inQuotes)
            inQuotes = false;
         else
            inQuotes = true;
      }
      else if (str[i] == '=')
      {
         if (!inQuotes)
            return true;
      }
   }

   return false;
}


//------------------------------------------------------------------------------
// bool IsThereMathSymbol(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if there is any math symbos, "-,+,*,/,^,=,<,>", not enclosed with
 * single quotes. It will return when it finds first math symbol not in quotes.
 *
 * @param  return true if it finds any math symbol not in qotes, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsThereMathSymbol(const std::string &str)
{
   #if DEBUG_MATH_SYMBOL > 0
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsThereMathSymbol() entered, str = <%s>\n", str.c_str());
   #endif
   
   bool inQuotes = false;
   std::string str1 = RemoveScientificNotation(str);
   Integer size = str1.size();
   
   if (IsEnclosedWith(str1, "'"))
   {
      #if DEBUG_MATH_SYMBOL > 0
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsThereMathSymbol() returning false, string is enclosed "
          "with single quotes\n");
      #endif
      return false;
   }
   
   for (Integer i=0; i<size; i++)
   {
      #if DEBUG_MATH_SYMBOL > 1
      MessageInterface::ShowMessage("==> %c\n", str1[i]);
      MessageInterface::ShowMessage("==> inQuotes = %d\n", inQuotes);
      #endif
      
      if (str1[i] == '\'')
      {
         // If last position has first single quote, return true
         if (!inQuotes && (i == size - 1))
         {
            #if DEBUG_MATH_SYMBOL > 0
            MessageInterface::ShowMessage
               ("GmatStringUtil::IsThereMathSymbol() returning true, last position "
                "has first single quote\n");
            #endif
            return true;
         }
         
         if (inQuotes)
            inQuotes = false;
         else
            inQuotes = true;
      }
      else if (str1[i] == '+' || str1[i] == '-' || str1[i] == '*' || str1[i] == '/' ||
               str1[i] == '^' || str1[i] == '=' || str1[i] == '<' || str1[i] == '>' ||
               str1[i] == '\'')
      {
         #if DEBUG_MATH_SYMBOL > 1
         MessageInterface::ShowMessage("==> inQuotes = %d\n", inQuotes);
         #endif
         if (!inQuotes)
         {
            #if DEBUG_MATH_SYMBOL > 0
            MessageInterface::ShowMessage
               ("GmatStringUtil::IsThereMathSymbol() returning true, found math "
                "simbol '%c'\n", str1[i]);
            #endif
            return true;
         }
      }
   }
   
   #if DEBUG_MATH_SYMBOL > 0
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsThereMathSymbol() returning false, no math symbol found\n");
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// Integer NumberOfScientificNotation(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Checks if input string contains scientific notation and returns number of
 * occurrance.
 */
//------------------------------------------------------------------------------
Integer GmatStringUtil::NumberOfScientificNotation(const std::string &str)
{
   #ifdef DEBUG_SCI_NOTATION
   MessageInterface::ShowMessage
      ("GmatStringUtil::NumberOfScientificNotation() entered, str=<%s>\n", str.c_str());
   #endif
   std::string::size_type sciEIndex = str.find("E");
   std::string::size_type scieIndex = str.find("e");
   if (sciEIndex == str.npos && scieIndex == str.npos)
   {
      #ifdef DEBUG_SCI_NOTATION
      MessageInterface::ShowMessage
         ("GmatStringUtil::NumberOfScientificNotation() returning false, no E or e found\n");
      #endif
      return false;
   }
   
   // Remove spaces first
   std::string str1 = RemoveAllBlanks(str);
   std::string str2;
   Integer size;
   bool done = false;
   bool sciFound = false;
   Integer currIndex = 1;
   Integer sciCount = 0;
   char currCh, prevCh, nextCh;
   
   // Separate by math symbols except + or - for e+7 or E-10
   StringArray parts = SeparateBy(str1, "=(),*/^'");
   Integer numParts = parts.size();
   
   #ifdef DEBUG_SCI_NOTATION
   MessageInterface::ShowMessage("   There are %d parts\n", numParts);
   for (int i = 0; i < numParts; i++)
      MessageInterface::ShowMessage("      %s\n", parts[i].c_str());
   #endif
   
   // Go through each part, and check for scientific notation
   for (Integer i=0; i<numParts; i++)
   {
      str2 = parts[i];
      size = str2.size();
      done = false;
      currIndex = 1;
      
      #ifdef DEBUG_SCI_NOTATION
      MessageInterface::ShowMessage
         ("   str2=<%s>, sciFound=%d, done=%d\n", str2.c_str(), sciFound, done);
      #endif
      
      if (sciFound)
         break;
      
      while (!done)
      {
         if (currIndex > 0 && currIndex < size)
         {
            currCh = str2[currIndex];
            prevCh = str2[currIndex-1];
            // If currIndex == size-1, set currCh to null terminator
            if (currIndex == size-1)
               nextCh = '\0';
            else
               nextCh = str2[currIndex+1];
            if (currCh == 'E' || currCh == 'e')
            {
               #ifdef DEBUG_SCI_NOTATION_MORE
               MessageInterface::ShowMessage
                  ("   currCh='%c', prevCh='%c', nextCh='%c'\n", currCh, prevCh, nextCh);
               #endif
               if ((isdigit(prevCh) || prevCh == '.') &&
                   (isdigit(nextCh) || nextCh == '+' || nextCh == '-'))
               {
                  // Check if it starts with a number, + ,- and ends with number or non-alpha
                  // since non-alpha can be any math operator
                  if ((isdigit(str2[0]) || str2[0] == '+' || str2[0] == '-') &&
                      (isdigit(str2[size-1]) || !isalpha(str2[size-1])))
                  {
                     sciFound = true;
                     //done = true;
                     sciCount++;
                     ////break;
                  }
                  else
                  {
                     // Check for sci notation in another way
                     #ifdef DEBUG_SCI_NOTATION
                     MessageInterface::ShowMessage("   Check for sci notation in another way\n");
                     #endif
                     StringArray subParts = SeparateBy(str2, "+-");
                     Integer numSubParts = subParts.size();
                     for (Integer j = 0; j < numSubParts; j++)
                     {
                        std::string str3 = subParts[j];
                        #ifdef DEBUG_SCI_NOTATION_MORE
                        MessageInterface::ShowMessage("   j=%d, str3=%s\n", j, str3.c_str());
                        #endif
                        if (EndsWith(str3, "e") || EndsWith(str3, "E"))
                        {
                           #ifdef DEBUG_SCI_NOTATION_MORE
                           MessageInterface::ShowMessage("   %s ends with e or E\n", str3.c_str());
                           #endif
                           if ((j + 1) <= numSubParts)
                           {
                              if ((isdigit(str3[0]) || str3[0] == '.') && isdigit(subParts[j+1][0]))
                              {
                                 sciFound = true;
                                 sciCount++;
                                 //done = true;
                                 break;
                              }
                           }
                        }
                     }
                  }
               }
            }
            currIndex++;
         } //if (currIndex > 0 && currIndex < size)
         else
         {
            done = true;
         }
      } // while (!done)
   } // for (Integer i=0; i<numParts; i++)
   
   #ifdef DEBUG_SCI_NOTATION
   MessageInterface::ShowMessage
      ("GmatStringUtil::NumberOfScientificNotation() <%s> returning %d, sciCount=%d\n",
       str.c_str(), sciFound, sciCount);
   #endif
   return sciCount;
}


//------------------------------------------------------------------------------
// bool HasNoBrackets(const std::string &str,
//                    bool parensForArraysAllowed = true)
//------------------------------------------------------------------------------
/*
 * return true if string does not contain any brackets, braces, or parentheses,
 * or only contains parentheses as part of an arrray element, if
 * parensForArraysAllowed is true.
 *
 * return false if there are brackets, braces, or parentheses, unless
 * existing parentheses are allowed for array elements.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::HasNoBrackets(const std::string &str,
                                   bool parensForArraysAllowed)
{
   #ifdef DEBUG_NO_BRACKETS
      MessageInterface::ShowMessage("Entering HasNoBrackets with str = %s\n",
         str.c_str());
   #endif
   std::string str1 = str;
   std::string str2 = str;
   std::string left, right, arrName;
   bool hasNone = true;
   if ((str1.find('(') != str1.npos) || (str1.find(')') != str1.npos))
   {
      if (parensForArraysAllowed)
      {
         Integer open = 0, close = 0;
         bool    isOuter, done = false;
         while (!done)
         {
            FindMatchingParen(str1, open, close, isOuter);
            if (((open == -1) && (close != -1)) ||
                ((open != -1) && (close == -1)) )
                return false;
            else if ((open == -1) && (close == -1))
               done = true;
            else
            {
               str2 = str1.substr(0, close + 1);
               GetArrayIndexVar(str2, left, right, arrName, "()");
               #ifdef DEBUG_NO_BRACKETS
                  MessageInterface::ShowMessage("   left = %s, right = %s, arrName = %s\n",
                     left.c_str(), right.c_str(), arrName.c_str());
               #endif
               if ((arrName == "") || (left == "-1") || (right == "-1"))
               {
                  #ifdef DEBUG_NO_BRACKETS
                     MessageInterface::ShowMessage
                        ("   NOT a proper array ... returning false\n");
                  #endif
                  return false;
               }
               
               hasNone = HasNoBrackets(left, parensForArraysAllowed) ||
                  HasNoBrackets(right, parensForArraysAllowed) || hasNone;
               if (!hasNone)
               {
                  #ifdef DEBUG_NO_BRACKETS
                     MessageInterface::ShowMessage
                        ("   left or right contains non-array parens ... returning false\n");
                  #endif
                  return false;
               }
               str1 = str1.substr(close+1);
            }
         }
      }
      else
         return false;
   }
   if ((str.find('[') != str.npos) || (str.find(']') != str.npos)) return false;
   if ((str.find('{') != str.npos) || (str.find('}') != str.npos)) return false;

   return true;
}


//------------------------------------------------------------------------------
// bool IsSingleItem(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if string does not have arithmetic operators or '.' or
 * enclosed with double parentheses. For example -123.0, abc, (a), (sat.X), (-123.0).
 *
 * It will return false for (1,1)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsSingleItem(const std::string &str)
{
   #if DEBUG_SINGLE_ITEM
   MessageInterface::ShowMessage("IsSingleItem() entered, str=%s\n", str.c_str());
   #endif
   
   Integer length = str.size();
   bool singleItem = true;
   Integer minusSignCounter = 0;
   Real rval;
   
   // first check for number
   if (ToReal(str, rval))
   {
      #if DEBUG_SINGLE_ITEM
      MessageInterface::ShowMessage("IsSingleItem() returning true, it is a real number\n");
      #endif
      return true;
   }
   
   for (int i=0; i<length; i++)
   {
      // Added check for underscore _ (LOJ: 2015.01.21)
      //if (isalnum(str[i]) || str[i] == '.')
      if (isalnum(str[i]) || str[i] == '.' || str[i] == '_')
         continue;
      
      if (str[i] == '-')
      {
         minusSignCounter++;
         continue;
      }

      singleItem = false;
      break;
   }
   
   if (singleItem)
   {
      if (minusSignCounter > 0)
         if (str[0] != '-' || minusSignCounter != 1)
            singleItem = false;
   }
   
   #if DEBUG_SINGLE_ITEM
   MessageInterface::ShowMessage("IsSingleItem() returning %d\n", singleItem);
   #endif
   
   return singleItem;
}

//------------------------------------------------------------------------------
// std::string RemoveExtraParen(const std::string &str, bool ignoreComma = false,
//                              bool ignoreSingleQuotes = false)
//------------------------------------------------------------------------------
/*
 * This method removs extra pair of parentheses.
 * If input string is "(a(1,1) + 10.0)" it returns "a(1,1) + 10.0"
 *                    "1 + (a(1,1) + 10) * 2" returns "1 + (a(1,1) + 10) * 2"
 *                    "(())" returns "(())"
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveExtraParen(const std::string &str, bool ignoreComma,
                                             bool ignoreSingleQuotes)
{
   #if DEBUG_REMOVE_EXTRA_PAREN
   MessageInterface::ShowMessage
      ("\nRemoveExtraParen() entering, ignoreComma=%d, ignoreSingleQuotes=%d\n   str=%s\n",
       ignoreComma, ignoreSingleQuotes, str.c_str());
   #endif
   
   if (!ignoreSingleQuotes && IsEnclosedWith(str, "'"))
   {
      #if DEBUG_REMOVE_EXTRA_PAREN
      MessageInterface::ShowMessage
         ("RemoveExtraParen() returning str=<%s>, string is enclosed with "
          "single quotes\n", str.c_str());
      #endif
      return str;
   }
   
   std::string str1 = str;
   std::string substr;
   Integer length = str.size();
   Integer openCounter = 0;
   Integer openParen = 0, closeParen = 0;
   std::map<Integer, Integer> openParenMap;
   std::map<Integer, Integer> closeParenMap;
   //@note using '\b' (backspace) for parenthesis removal, is it safe to that? (LOJ: 2012.08.29)
   char charToRemove = '\b';
   
   // remove outer parentheses
   while (IsEnclosedWithExtraParen(str1, true, ignoreComma))
      str1 = str1.substr(1, str1.size() - 2);
   
   #if DEBUG_REMOVE_EXTRA_PAREN
   MessageInterface::ShowMessage("  str1=%s\n", str1.c_str());
   #endif
   
   std::string str2 = str1;
   length = str1.size();
   
   // go through each char and remove extra parentheses
   for (int i=0; i<length; i++)
   {
      if (str1[i] == '(')
      {
         openParen = i;
         openCounter++;
         openParenMap[openCounter] = i;
         
         #if DEBUG_REMOVE_EXTRA_PAREN
         MessageInterface::ShowMessage
            ("   openParen:  i=%d, openCounter=%d\n", i, openCounter);
         #endif
      }
      else if (str1[i] == ')')
      {
         closeParen = i;
         closeParenMap[openCounter] = i;
         
         #if DEBUG_REMOVE_EXTRA_PAREN
         MessageInterface::ShowMessage
            ("   closeParen: i=%d, openCounter=%d\n", i, openCounter);
         #endif
         
         //-----------------------------------------------------------
         // check one more outer parentheses
         //-----------------------------------------------------------
         
         openParen = openParenMap[openCounter];
         closeParen = closeParenMap[openCounter];
         
         substr = str1.substr(openParen, closeParen-openParen+1);
         char chBeforeOpenParen = charToRemove;
         char chAfterCloseParen = charToRemove;
         std::string strBeforeOpenParen = str1.substr(0, openParen);
         strBeforeOpenParen = RemoveAllBlanks(strBeforeOpenParen);
         Integer lenBeforeOpenParen = strBeforeOpenParen.size();
         std::string strAfterCloseParen = str1.substr(closeParen + 1);
         strAfterCloseParen = RemoveAllBlanks(strAfterCloseParen);
         Integer lenAfterCloseParen = strAfterCloseParen.size();
         
         // Check if parenthesis is part of array of function 
         bool isParenPartOfName = false;
         if (strBeforeOpenParen.size() > 0 && isalnum(strBeforeOpenParen[lenBeforeOpenParen - 1]))
            isParenPartOfName = true;
         
         // Get character before and after parenthesis
         if (openParen > 0 && lenBeforeOpenParen > 0)
            chBeforeOpenParen = strBeforeOpenParen[lenBeforeOpenParen - 1];
         if (closeParen < length && lenAfterCloseParen > 0)
            chAfterCloseParen = strAfterCloseParen[0];
         
         std::string trimmedStr = RemoveAllBlanks(substr);
         
         #if DEBUG_REMOVE_EXTRA_PAREN
         MessageInterface::ShowMessage
            ("   =====> substr='%s'\n   openParen=%d, closeParen=%d, openCounter=%d\n",
             substr.c_str(), openParen, closeParen, openCounter);
         MessageInterface::ShowMessage
            ("   strBeforeOpenParen='%s', strAfterCloseParen='%s', lenBeforeOpenParen=%d, "
             "lenAfterCloseParen=%d\n", strBeforeOpenParen.c_str(), strAfterCloseParen.c_str(),
             lenBeforeOpenParen, lenAfterCloseParen);
         MessageInterface::ShowMessage
            ("   chBeforeOpenParen='%c', chAfterCloseParen='%c', isParenPartOfName=%d\n",
             chBeforeOpenParen, chAfterCloseParen, isParenPartOfName);
         MessageInterface::ShowMessage("   trimmedStr='%s'\n", trimmedStr.c_str());
         #endif
         
         Integer removeStatus = -99;
         
         // Check if extra parenthesis can be removed. Several Else-If cases for
         // better debugging.
         if (isParenPartOfName)
         {
            removeStatus = -10;
         }
         else if (IsMathOperator(chBeforeOpenParen) && IsMathOperator(chAfterCloseParen))
         {
            removeStatus = -11;
         }
         else if (chBeforeOpenParen == '(' && chAfterCloseParen == ')')
         {
            if (IsEnclosedWithExtraParen(trimmedStr))
               removeStatus = 1;
            else
               removeStatus = -1;
         }
         else if (chBeforeOpenParen == '(' && IsMathOperator(chAfterCloseParen))
         {
            removeStatus = -2;
         }
         else if (openParen == 0 || IsMathOperator(chBeforeOpenParen))
         {
            // Check for unary - sign first
            if ((trimmedStr.size() > 1 && trimmedStr[1] != '-') &&
                (IsEnclosedWithExtraParen(trimmedStr, true, true)) &&
                (!IsMathOperator(chAfterCloseParen)) &&
                (!IsMathOperator(chBeforeOpenParen)) &&
                (!IsThereMathSymbol(trimmedStr)))
               removeStatus = 3;
            else
               removeStatus = -3;
         }
         
         #if DEBUG_REMOVE_EXTRA_PAREN
         MessageInterface::ShowMessage
            ("   -----> removeStatus=%d %s parenthesis in '%s' %d and %d\n", removeStatus,
             removeStatus > 0 ? "Removing" : "Not removing", substr.c_str(), openParen, closeParen);
         #endif
         
         if (removeStatus > 0)
         {
            str2[openParen] = charToRemove;
            str2[closeParen] = charToRemove;
         }
         
         openCounter--;
         
      }
   }
   
   #if DEBUG_REMOVE_EXTRA_PAREN
   MessageInterface::ShowMessage("   Before removing temp char = '%s'\n", str2.c_str());
   #endif
   
   str2 = RemoveAll(str2, charToRemove, 0);
   
   // Remove leading and trailing spaces again
   str2 = Trim(str2);
   
   #if DEBUG_REMOVE_EXTRA_PAREN
   MessageInterface::ShowMessage("RemoveExtraParen() exiting str2='%s'\n", str2.c_str());
   #endif
   
   return str2;
}


//------------------------------------------------------------------------------
// std::string RemoveOuterParen(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Removes outer pair of parenthesis if it has one.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveOuterParen(const std::string &str)
{
   std::string str1 = str;
   
   if (StartsWith(str, "(") && EndsWith(str, ")"))
      str1 = str.substr(1, str.size() - 2);
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveOuterString(const std::string &str, const std::string &start,
//                               const std::string end)
//------------------------------------------------------------------------------
/*
 * This method removes outer string if it has one.
 * If input string is "(a(1,1) + 10.0)" it return a(1,1) + 10.0.
 *
 * @param  str  Input string
 * @param  start  Starting string to be checked and removed
 * @param  end  Ending string to be checked and removed
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveOuterString(const std::string &str,
                                              const std::string &start,
                                              const std::string &end)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("RemoveOuterString() entering str=\"%s\", start='%s', end='%s'\n", str.c_str(),
       start.c_str(), end.c_str());
   #endif
   
   std::string str1 = str;
   
   if (StartsWith(str, start) && EndsWith(str, end))
      str1 = str.substr(1, str.size() - 2);

   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveEnclosingString(const std::string &str, const std::string &enStr)
//------------------------------------------------------------------------------
/*
 * Removes enclosing string if actually enclosed with the string.
 *
 * @param  str  Input string to remove given enclosing string
 * @param  enStr The enclosing string to check and remove
 *
 * @return string with enclosing string removed if found, original string otherwise
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveEnclosingString(const std::string &str,
                                                  const std::string &enStr)
{
   std::string str1 = str;
      
   // First check if string is enclosed with given string
   if (IsEnclosedWith(str, enStr))
   {
      str1 = str.substr(enStr.size());
      str1 = str1.substr(0, str1.size() - enStr.size());
   }
   
   #ifdef DEBUG_REMOVE_ENCLOSING_STRING
   MessageInterface::ShowMessage
      ("GmatStringUtil::RemoveEnclosingString() returning <%s>\n", str1.c_str());
   #endif
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveInlineComment(const std::string &str, const std::string &comStr)
//------------------------------------------------------------------------------
/*
 * Removes inline comments from the input string.
 * ex) "Create String srt1; % str2" will return "Create String str1;"
 *
 * @param  str  Input string to remove given enclosnig string
 * @param  cmStr The inline comment string to use for removing
 *
 * @return string with inline comments removed
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveInlineComment(const std::string &str,
                                                const std::string &cmStr)
{
   std::string str1 = str;
   std::string::size_type index = str.find(cmStr);
   if (index == str1.npos)
      return str1;
   else
      return str1.substr(0, index);
}


//------------------------------------------------------------------------------
// std::string MakeCommentLines(const std::string &str, bool breakAtCr = false)
//------------------------------------------------------------------------------
/**
 * Converts input string to comment lines by adding % at the beginning of
 * non blank lines.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::MakeCommentLines(const std::string &str, bool breakAtCr)
{
   #ifdef DEBUG_COMMENTS
   MessageInterface::ShowMessage
      ("MakeCommentLines() entered, str = '%s', length = %d\n", str.c_str(), str.size());
   #endif
   std::string line, comments;
   StringArray commentArray;
   Integer count = str.size(), lineCount = 0;
   Integer length = 0, start = 0;
   
   for (int i = 0; i < count; i++)
   {
      if (str[i] == '\n' || (str[i] == '\r' && breakAtCr))
      {
         length = i - start;
         #ifdef DEBUG_COMMENTS
         MessageInterface::ShowMessage
            ("'%c', i=%d, start=%d, length=%d\n", str[i], i, start, length);
         #endif
         // Added to comment array without adding eol
         if (length <= 0)
            commentArray.push_back("");
         else
            commentArray.push_back(str.substr(start, length));
         start = i + 1;
      }
   }
   
   #ifdef DEBUG_COMMENTS
   MessageInterface::ShowMessage("   start=%d\n", start);
   #endif
   if (start <= count)
      commentArray.push_back(str.substr(start, count-start));
   
   lineCount = commentArray.size();
   #ifdef DEBUG_COMMENTS
   MessageInterface::ShowMessage("There are %d lines\n", lineCount);
   #endif
   
   if (lineCount == 0)
   {
      comments = "% " + str;
   }
   else
   {
      for (int i = 0; i < lineCount; i++)
      {
         line = commentArray[i];
         
         // Remove first %
         if (StartsWith(line, "% "))
            line = line.substr(2);
         else if (StartsWith(line, "%"))
            line = line.substr(1);
         
         #ifdef DEBUG_COMMENTS
         MessageInterface::ShowMessage("[%d] '%s'\n", i, line.c_str());
         #endif
         if (line == "")
            comments = comments + "\n";
         else
            comments = comments + "% " + line + "\n";
      }
   }
   
   #ifdef DEBUG_COMMENTS
   MessageInterface::ShowMessage("MakeCommentLines() returning \n'%s'\n", comments.c_str());
   #endif
   
   return comments;
}


//------------------------------------------------------------------------------
// std::string ParseFunctionName(const std::string &str, std::string &argStr)
//------------------------------------------------------------------------------
/**
 * Parses function name from the following syntax:
 *    [out] = Function1(in);
 *    Function2(in);
 *    Function3;
 *
 * @param  str  Input string
 * @param  argStr output function argument including ()
 * @return  Function name or blank if name not found
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ParseFunctionName(const std::string &str, std::string &argStr)
{
   #ifdef DEBUG_FUNCTION_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::ParseFunctionName() entered, str='%s'\n", str.c_str());
   #endif

   argStr = "";
   if (str == "")
      return "";
   
   // Remove all spaces and semicolons
   std::string str1 = RemoveAll(str, ' ');
   while (str1[str1.size()-1] == ';')
      str1.erase(str1.size()-1, 1);
   
   std::string funcName;
   if (str1.find("[") != str1.npos)
   {
      std::string::size_type strPos;
      // Check if [ is part of string literal such as "sprintf('arr33 = [%s]', arr33)"
      if (IsStringInsideSymbols(str1, "[", "'", strPos))
      {
         #ifdef DEBUG_FUNCTION_NAME
         MessageInterface::ShowMessage
            ("   In GmatStringUtil::ParseFunctionName(), [ is part of string literal\n");
         #endif
      }
      else
      {
         // This code block parse function name such as [a,b] = function(x)
         std::string::size_type index1 = str1.find("=");
         #ifdef DEBUG_FUNCTION_NAME
         MessageInterface::ShowMessage("   equalSignPos = %u\n", index1);
         #endif
         std::string::size_type index2 = str1.find("(", index1 + 1);
         if (index2 == str1.npos)
            funcName = str1.substr(index1+1);
         else
            funcName = str1.substr(index1+1, index2-index1-1);
      }
   }
   else
   {
      // This code block parse function name such as a = function(x)
      std::string::size_type index1 = str1.find("=");
      #ifdef DEBUG_FUNCTION_NAME
      MessageInterface::ShowMessage("   equalSignPos = %u\n", index1);
      #endif
      
      // Parse function name if equal sign is not inside quotes
      std::string::size_type strPos;
      if (!IsStringInsideSymbols(str1, "=", "'", strPos))
      {
         std::string::size_type index2 = str1.find("(", index1 + 1);
         if (index2 == str1.npos)
            funcName = str1.substr(index1+1);
         else
            funcName = str1.substr(index1+1, index2-index1-1);
      }
   }
   
   // If function name not found above, try again (LOJ: 2016.03.07)
   if (funcName == "")
   {
      std::string::size_type index2 = str1.find("(");
      if (index2 == str1.npos)
         funcName = str1.substr(0);
      else
         funcName = str1.substr(0, index2);
   }

   // Get function arguments
   argStr = str.substr(funcName.size());
   
   #ifdef DEBUG_FUNCTION_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::ParseFunctionName() returning function name: '%s', arg: '%s'\n",
       funcName.c_str(), argStr.c_str());
   #endif
   return funcName;
}

//------------------------------------------------------------------------------
// StringArray ParseFunctionCall(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Parses function call from the following syntax and return function name and
 * function arguments.
 *    [out] = Function1(in);
 *    Function2(in);
 *    Function3;
 *    out = Function4(in, 'string literal')
 *
 * @param  str  Input string
 * @return  Function name and arguments
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::ParseFunctionCall(const std::string &str)
{
   #ifdef DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("GmatStringUtil::ParseFunctionCall() entered, str='%s'\n", str.c_str());
   #endif
   StringArray nameAndArgs;
   if (str == "")
      return nameAndArgs;
   
   std::string argStr;
   std::string fname = ParseFunctionName(str, argStr);
   #ifdef DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage("   fname='%s'\n", fname.c_str());
   #endif
   
   if (fname == "")
      return nameAndArgs;
   
   std::string::size_type firstOpenParen = str.find("(");
   
   nameAndArgs.push_back(fname);
   
   if (firstOpenParen == str.npos)
      return nameAndArgs;
   
   std::string::size_type lastCloseParen = str.find_last_of(")");
   if (lastCloseParen == str.npos)
      return nameAndArgs;
   
   std::string str1 = str.substr(firstOpenParen+1, lastCloseParen-firstOpenParen-1);
   #ifdef DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage("   substr within parenthesis = '%s'\n", str1.c_str());
   #endif
   
   // If no arguments, return
   if (str1 == "")
      return nameAndArgs;
   
   // Use SeparateBrackets() for separating array elements such as arr22(1,1) (LOJ: 2016.06.08)
   //StringArray args = SeparateByComma(str1, true);
   bool checkBrackets = false;
   if (str1.find("[") != str1.npos && str1.find("]") != str1.npos)
      checkBrackets = true;
   StringArray args = SeparateBrackets(str1, "[]", " ,;", checkBrackets);
   for (unsigned int i = 0; i < args.size(); i++)
   {
      #ifdef DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage("   args[%d] = '%s'\n", i, args[i].c_str());
      #endif
      nameAndArgs.push_back(args[i]);
   }
   
   #ifdef DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("GmatStringUtil::ParseFunctionCall() returning %d items\n", nameAndArgs.size());
   for (unsigned int i = 0; i < nameAndArgs.size(); i++)
      MessageInterface::ShowMessage("   nameAndArgs[%d] = '%s'\n", i, nameAndArgs[i].c_str());
   #endif
   return nameAndArgs;
}

//------------------------------------------------------------------------------
// std::string AddEnclosingString(const std::string &str, const std::string &enStr)
//------------------------------------------------------------------------------
/**
 * Put string in enclosing string
 *
 * @param  str  Input string
 * @param  enStr Enclosing string
 * @return  String with enclosing string
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::AddEnclosingString(const std::string &str,
                                               const std::string &enStr)
{
   if (str == "")
      return "";
   
   std::string str1 = str;
   str1 = enStr + str + enStr;
   return str1;
}


//------------------------------------------------------------------------------
// std::string GetInvalidNameMessageFormat()
//------------------------------------------------------------------------------
/**
 * Returns invalid object name message.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::GetInvalidNameMessageFormat()
{
   return ("\"%s\" is not a valid name. Please reenter a valid name.\n\n"
           "[Name cannot be a GMAT keyword or command type name, such as \"GMAT\", "
           "\"Create\", \"Propagate\", \"function\" and must begin with a letter, "
           "which may be followed by any combination of letters, digits, and underscores.]");
}


//------------------------------------------------------------------------------
// StringArray DecomposeBy(const std::string &str, const std::string &delim)
//------------------------------------------------------------------------------
/**
 * Returns the first token and the rest
 * e.g) "cd c:/my test directory" returns if delimiter is " ".
 *   cd
 *   c:/my test directory
 *
 * @param  str  Input string
 * @param  delim Delimiter
 * @return  First token and the rest or empty string array if token not found
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::DecomposeBy(const std::string &str,
                                        const std::string &delim)
{
   StringArray parts;
   if (str == "")
      return parts;
   
   std::string::size_type index1 = str.find_first_of(delim);
   if (index1 != str.npos)
   {
      parts.push_back(str.substr(0, index1));
      parts.push_back(str.substr(index1 + 1));
      return parts;
   }
   else
   {
      return parts;
   }
}


//------------------------------------------------------------------------------
// bool StartsWith(const std::string &str, const std::string &value)
//------------------------------------------------------------------------------
/*
 * Returns true if string starts with value and false if it does not.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::StartsWith(const std::string &str, const std::string &value)
{
   return (str.size() >= value.size()) &&
      (str.substr(0, value.size()) == value);
}


//------------------------------------------------------------------------------
// bool EndsWith(const std::string &str, const std::string &value)
//------------------------------------------------------------------------------
/*
 * Returns true if string ends with value and false if it does not.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::EndsWith(const std::string &str, const std::string &value)
{
   return (str.size() >= value.size()) &&
          (str.substr(str.size() - value.size(), value.size()) == value);
}


//------------------------------------------------------------------------------
// bool EndsWithPathSeparator(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Returns true if string ends with path separator (/ or \\) and false if it does not.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::EndsWithPathSeparator(const std::string &str)
{
   if (EndsWith(str, "/") || EndsWith(str, "\\"))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsValidNumber(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if string is valid integer or real number.
 *
 * @param  str  The input string to check for valid number
 * @param  allowOverflow warn (true) or error (false) on overflow
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidNumber(const std::string &str, bool allowOverflow)
{
   std::string str1 = Strip(str);
   if (str1 == "")
      return false;

   Real rval;
   Integer ival;

   if (!ToInteger(str1, ival, true, allowOverflow) && !ToReal(str1, rval, true, allowOverflow))
   {
      #ifdef DEBUG_VALID_NUMBER
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidNumber(%s) returning false\n", str1.c_str());
      #endif
      return false;
   }
   
   #ifdef DEBUG_VALID_NUMBER
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidNumber(%s) returning true\n", str1.c_str());
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// bool IsValidName(const char *str, bool ignoreBracket = false,
//                  bool blankNameIsOk = false)
//------------------------------------------------------------------------------
/*
 * @see IsValidName(const std::string &str, ...)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidName(const char *str, bool ignoreBracket,
                                 bool blankNameIsOk)
{
   return IsValidName(std::string(str), ignoreBracket, blankNameIsOk);
}

//------------------------------------------------------------------------------
// bool IsValidName(const std::string &str, bool ignoreBracket = false,
//                  bool blankNameIsOk = false)
//------------------------------------------------------------------------------
/*
 * Checks for valid name.
 *
 * Returns true if string is:
 *    1. keyword (not "GMAT", "Create", or "function") and
 *    2. does not start with a number and
 *    3. contains only alphanumeric (underscore is allowed)
 *
 * @param  str  The input string to check for valid name
 * @param  ignoreBracket  true if input string has bracket and to ignore it
 *                      when checking (false)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidName(const std::string &str, bool ignoreBracket,
                                 bool blankNameIsOk)
{
   #ifdef DEBUG_VALID_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidName() entered, str='%s', ignoreBracket=%d, blankNameIsOk=%d\n",
       str.c_str(), ignoreBracket, blankNameIsOk);
   #endif
   
   // strip blanks first (loj: 2008.08.27)
   std::string str1 = Strip(str);
   
   if (str1 == "")
   {
      if (blankNameIsOk)
      {
         #ifdef DEBUG_VALID_NAME
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsValidName(%s) returning true, string is blank after strip\n", str.c_str());
         #endif
         return true; 
      }
      else
      {
         #ifdef DEBUG_VALID_NAME
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsValidName(%s) returning false, string is blank after strip\n", str.c_str());
         #endif
         return false; 
      }
   }
   
   if (str1 == "GMAT" || str1 == "Create" || str1 == "function")
   {
      #ifdef DEBUG_VALID_NAME
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidName(%s) returning false, it is one of keywords\n", str1.c_str());
      #endif
      return false;
   }
   
   // First letter must start with alphabet
   if (!isalpha(str1[0]))
   {
      // To support setting values in vector form such as [1 2 3], check if is
      // is enclosed with []. This is needed for diagonal matrix setting syntax
      // "diag([1 2 3])"
      // LOJ: 2016.03.09
      if (IsEnclosedWithBrackets(str1))
      {
         #ifdef DEBUG_VALID_NAME
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsValidName(%s) returning true, it is vector form setting\n",
             str1.c_str());
         #endif
         return true;
      }
      else
      {
         #ifdef DEBUG_VALID_NAME
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsValidName(%s) returning false, name does not "
             "start with a character\n", str1.c_str());
         #endif
         return false;
      }
   }
   
   // if ignoring open parenthesis, remove it first
   if (ignoreBracket)
   {
      std::string::size_type openParen = str1.find_first_of("([");
      if (openParen != str1.npos)
      {
         str1 = str1.substr(0, openParen);
         str1 = Trim(str1);
      }
   }
   
   for (UnsignedInt i=1; i<str1.size(); i++)
   {
      if (!isalnum(str1[i]) && str1[i] != '_')
      {
         #ifdef DEBUG_VALID_NAME
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsValidName(%s) returning false\n", str1.c_str());
         #endif
         return false;
      }
   }
   
   #ifdef DEBUG_VALID_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidName(%s) returning true\n", str1.c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool IsValidParameterName(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Checks if input string a valid Parameter name syntax such as sat.X or sat.Earth.TA.
 * It does not check for the actual Parameter name such as X or TA.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidParameterName(const std::string &str)
{
   #ifdef DEBUG_VALID_PARAMETER_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidParameterName() entered, str=<%s>\n", str.c_str());
   #endif
   
   bool isValid = false;
   
   std::string type, owner, dep;
   ParseParameter(str, type, owner, dep);
   if (owner != "" && type != "")
      isValid = true;
   
   #ifdef DEBUG_VALID_PARAMETER_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidParameterName() returning %d\n", isValid);
   #endif
   
   return isValid;
}


//------------------------------------------------------------------------------
// bool IsLastNumberPartOfName(const std::string &str)
//------------------------------------------------------------------------------
bool GmatStringUtil::IsLastNumberPartOfName(const std::string &str)
{
   #ifdef DEBUG_NUMBER_WITH_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsLastNumberPartOfName() entered, str=<%s>\n", str.c_str());
   #endif
   
   if (str.empty())
   {
      #ifdef DEBUG_NUMBER_WITH_NAME
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsLastNumberPartOfName() returning false, "
          "input string is empty\n");
      #endif
      return false;
   }
   
   if (!isdigit(str[str.size()-1]))
   {
      #ifdef DEBUG_NUMBER_WITH_NAME
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsLastNumberPartOfName() returning false, "
          "last char is not a number\n");
      #endif
      return false;
   }
   
   Real rval;
   if (ToReal(str, rval))
   {
      #ifdef DEBUG_NUMBER_WITH_NAME
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsLastNumberPartOfName() returning false, it is a number\n");
      #endif
      return false;
   }
   
   Integer lastNumber = 0;
   bool retval = true;
   
   // Remove math operators and parenthesis
   std::string str1 = RemoveMathSymbols(str, true);
   #ifdef DEBUG_NUMBER_WITH_NAME
   MessageInterface::ShowMessage
      ("   after removing math symbols, str1=<%s>\n", str1.c_str());
   #endif
   
   // Find last number
   str1 = RemoveLastNumber(str1, lastNumber);
   #ifdef DEBUG_NUMBER_WITH_NAME
   MessageInterface::ShowMessage
      ("   str1=<%s>, lastNumber=%d\n", str1.c_str(), lastNumber);
   #endif
   
   if (str1 == "")
      retval = false;
   
   #ifdef DEBUG_NUMBER_WITH_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsLastNumberPartOfName() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool IsBlank(const std::string &text, bool ignoreEol = false)
//------------------------------------------------------------------------------
/*
 * Checks if text has only blank spaces.
 *
 * @param  text  input text
 * @param  ignoreEol  Set this to true if end-of-line char to be ignored
 *
 * @return true if text has only blank spaces
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsBlank(const std::string &text, bool ignoreEol)
{
   std::string str = Trim(text, GmatStringUtil::BOTH);

   if (ignoreEol)
   {
      str = RemoveAll(str, '\n');
      str = RemoveAll(str, '\r');
   }

   if (str == "")
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool HasMissingQuote(const std::string &str, const std::string &quote,
//                      bool ignoreSpaceAfterQuote = true)
//------------------------------------------------------------------------------
/*
 * Checks if single string item (no commans between) has missing starting or ending quote.
 *
 * @param  str    input text
 * @param  quote  quote to be used for checking
 * @param  ignoreSpace  if this flag is set, it will remove spaces after quote
 *                      before checking
 *
 * @return true if string has missing quote, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::HasMissingQuote(const std::string &str,
                                     const std::string &quote, bool ignoreSpaceAfterQuote)
{
   #ifdef DEBUG_MISSING_QUOTE
   MessageInterface::ShowMessage
      ("GmatStringUtil::HasMissingQuote() entered, str=<%s>, quote=<%s>, ignoreSpaceAfterQuote=%d\n",
       str.c_str(), quote.c_str(), ignoreSpaceAfterQuote);
   #endif
   
   std::string::size_type begQuote = str.find_first_of(quote);
   std::string::size_type endQuote = str.find_last_of(quote);
   #ifdef DEBUG_MISSING_QUOTE
   MessageInterface::ShowMessage("   begQuote=%u, endQuote=%u\n", begQuote, endQuote);
   #endif
   
   // If there is no quotes, return false
   if (begQuote == str.npos && endQuote == str.npos)
   {
      #ifdef DEBUG_MISSING_QUOTE
      MessageInterface::ShowMessage
         ("GmatStringUtil::HasMissingQuote() returning false, no quotes found\n");
      #endif
      return false;
   }
   
   // if (((!ignoreSpaceAfterQuote) &&
   //      (StartsWith(str, quote) && !EndsWith(str, quote))) ||
   //     (!StartsWith(str, quote) && EndsWith(str, quote)))
   if ((!ignoreSpaceAfterQuote) &&
       ((StartsWith(str, quote) && !EndsWith(str, quote)) ||
        (!StartsWith(str, quote) && EndsWith(str, quote))))
   {
      #ifdef DEBUG_MISSING_QUOTE
      MessageInterface::ShowMessage
         ("GmatStringUtil::HasMissingQuote() returning true, not ignoring space "
          "and it is not enclosed with quotes\n");
      #endif
      return true;
   }
   
   bool retval = true;
   if (ignoreSpaceAfterQuote)
   {
      if ((begQuote != str.npos && endQuote != str.npos) && (begQuote != endQuote))
      {
         std::string afterEndQuote = str.substr(endQuote + 1);
         afterEndQuote = RemoveAllBlanks(afterEndQuote);
         #ifdef DEBUG_MISSING_QUOTE
         MessageInterface::ShowMessage("   afterEndQuote=<%s>\n", afterEndQuote.c_str());
         #endif
         
         if (afterEndQuote == "")
            retval = false;
      }
   }
   
   #ifdef DEBUG_MISSING_QUOTE
   MessageInterface::ShowMessage
      ("GmatStringUtil::HasMissingQuote() returning %d\n", retval);
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// bool IsStringInsideSymbols(const std::string &str, const std::string &reqStr,
//                     const std::string &symbol, std::string::size_type &reqStrPos)
//------------------------------------------------------------------------------
/**
 * Checks if reqStr is inside symbols. Return true if reqStr found and it is inside
 * symbols. It updates position of reqStr.
 *
 * If symbol has two characters, it will check reqStr between first and second characters.
 * for example:
 * IsStringInsideSymbols(inputStr, ";", "[]", strPos);
 *
 * @return true if reqStr found and it is inside symbols
 *         false if reqStr not found or reqStr is not between symbols
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsStringInsideSymbols(const std::string &str, const std::string &reqStr,
                                          const std::string &symbol, std::string::size_type &reqStrPos)
{
   #ifdef DEBUG_STRING_INSIDE
   MessageInterface::ShowMessage
      ("IsStringInsideSymbols() entered, str='%s', reqStr='%s', symbol='%s'\n",
       str.c_str(), reqStr.c_str(), symbol.c_str());
   #endif
   
   // reqStrPos is output
   reqStrPos = str.find(reqStr);
   bool isStrInsideSymbols = false;
   std::string symbol1 = symbol;
   std::string symbol2 = symbol;
   
   if (symbol.size() == 2)
   {
      symbol1 = symbol.substr(0,1);
      symbol2 = symbol.substr(1,1);
   }
   
   #ifdef DEBUG_STRING_INSIDE
   MessageInterface::ShowMessage
      ("   symbol1 = '%s', symbol2 = '%s'\n", symbol1.c_str(), symbol2.c_str());
   #endif
   
   if (reqStrPos != str.npos)
   {
      // Check if reqStr is inside symbols
      //std::string::size_type symbol1Pos = str.find(symbol);
      std::string::size_type symbol1Pos = str.find(symbol1);
      if (symbol1Pos != str.npos)
      {
         //std::string::size_type symbol2Pos = str.find(symbol, symbol1Pos + 1);
         std::string::size_type symbol2Pos = str.find(symbol2, symbol1Pos + 1);
         #ifdef DEBUG_STRING_INSIDE
         MessageInterface::ShowMessage
            ("   reqStrPos=%u, symbol1Pos=%u, symbol2Pos=%u\n", reqStrPos, symbol1Pos, symbol2Pos);
         #endif
         if (symbol2Pos != str.npos)
         {
            if (reqStrPos > symbol1Pos && reqStrPos < symbol2Pos)
            {
               #ifdef DEBUG_STRING_INSIDE
               MessageInterface::ShowMessage("   '%s' is inside symbols\n", reqStr.c_str());
               #endif
               isStrInsideSymbols = true;
            }
         }
      }
   }
   
   #ifdef DEBUG_STRING_INSIDE
   MessageInterface::ShowMessage("IsStringInsideSymbols() returning %d\n", isStrInsideSymbols);
   #endif
   return isStrInsideSymbols;
}

//------------------------------------------------------------------------------
// bool IsMathEquation(const std::string &str, bool checkInvalidOpOnly = false,
//                     bool blankNameIsOk = false)
//------------------------------------------------------------------------------
/*
 * Checks if string is a math equation with numbers and variables only.
 * Math operators are "/+-*^'".
 * If string is enclosed with single quotes, it just returns false.
 * If string has function or array, it returns false.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsMathEquation(const std::string &str, bool checkInvalidOpOnly,
                                    bool blankNameIsOk)
{
   // Convert tabs to spaces as we start
   std::string strNoTab = GmatStringUtil::Replace(str, "\t", " ");

   #if DEBUG_MATH_EQ > 0
   MessageInterface::ShowMessage
      ("\nGmatStringUtil::IsMathEquation() entered, str=<%s>, checkInvalidOpOnly=%d, "
       "blankNameIsOk=%d\n", str.c_str(), checkInvalidOpOnly, blankNameIsOk);
   #endif
   
   if (IsEnclosedWith(strNoTab, "'"))
   {
      #if DEBUG_MATH_EQ
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsMathEquation(%s) returning false, it is enclosed with quotes\n",
          str.c_str());
      #endif
      return false;
   }
   
   // If check for invalid math operator only
   if (checkInvalidOpOnly)
   {
      // Remove spaces, dots, commas, underscores, and parenthesis
      std::string removeStr = " .,_()";
      std::string str1 = RemoveAll(strNoTab, removeStr);
      #if DEBUG_MATH_EQ > 1
      MessageInterface::ShowMessage
         ("   after '%s' removed '%s'\n", removeStr.c_str(), str1.c_str());
      #endif
      
      std::string::size_type length = str1.size();
      bool isValid = true;
      for (UnsignedInt i = 0; i < length; i++)
      {
         if (isalnum(str1[i]) || IsMathOperator(str1[i]))
            continue;
         else
         {
            isValid = false;
            break;
         }
      }
      
      // Check if it is string function call (LOJ: 2016.03.01)
      if (!isValid)
      {
         // Check if string literal is passed to string function
         //if (strNoTab.find("'") != strNoTab.npos)
            isValid = IsValidFunctionCall(strNoTab);
      }
      
      #if DEBUG_MATH_EQ
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsMathEquation(%s) returning %s\n", str.c_str(),
          isValid ? "true" : "false");
      #endif
      
      return isValid;
   }
   
   
   // Do more through checking
   
   StringArray parts = SeparateBy(strNoTab, "+-*/^'");
   Integer numParts = parts.size();
   
   #if DEBUG_MATH_EQ > 1
   MessageInterface::ShowMessage
      ("   ..... It has %d parts, start checking for any errors\n", numParts);
   #endif
   
   // If it has only 1 part, check if blank name is ok
   if (numParts == 1)
   {
      if (blankNameIsOk)
      {
         // Check if any spaces found
         if (strNoTab.find_first_of(' ') == strNoTab.npos)
         {
            #if DEBUG_MATH_EQ
            MessageInterface::ShowMessage
               ("GmatStringUtil::IsMathEquation(%s) returning true, no operators "
                "found but empty name is ok\n", str.c_str());
            #endif
            return true;
         }
      }
      
      #if DEBUG_MATH_EQ
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsMathEquation(%s) returning false, no operators "
          "found\n", str.c_str());
      #endif
      return false;
   }
   
   // check if it is array or function
   #if DEBUG_MATH_EQ > 1
   MessageInterface::ShowMessage("   ..... Checking if parentheses part of array\n");
   #endif
   
   std::string::size_type index = strNoTab.find_first_of("(");
   if (index != strNoTab.npos)
   {
      // check if parentheses balanced
      #if DEBUG_MATH_EQ > 1
      MessageInterface::ShowMessage("   ..... Checking if parentheses is balanced\n");
      #endif
      if (!IsParenBalanced(strNoTab))
      {
         #if DEBUG_MATH_EQ
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsMathEquation(%s) returning false\n", str.c_str());
         #endif
         return false;
      }
   }
   
   #if DEBUG_MATH_EQ > 1
   MessageInterface::ShowMessage("   ..... Checking each part\n");
   #endif
   
   bool retval = AreAllNamesValid(strNoTab);
   
   #if DEBUG_MATH_EQ
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsMathEquation(%s) returning %d\n", str.c_str(), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool IsMathOperator(const char &ch)
//------------------------------------------------------------------------------
bool GmatStringUtil::IsMathOperator(const char &ch)
{
   if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '\'')
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Integer NumberOfOccurrences(const std::string &str, const char c)
//------------------------------------------------------------------------------
/*
 * Counts the number of occurrences of the character c in the string str.
 *
 * @param  str  input string
 * @param  c    character
 *
 * @return number of occurrences of c in str
 */
//------------------------------------------------------------------------------
Integer GmatStringUtil::NumberOfOccurrences(const std::string &str, const char c)
{
   Integer count = 0;
   Integer strSz = (Integer) str.size();
   for (Integer ii = 0; ii < strSz; ii++)
      if (str[ii] == c)  count++;

   return count;
}


//------------------------------------------------------------------------------
// StringArray GetVarNames(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Searchs all variable names and returns in a std::vector array.
 *
 * @see IsValidName()
 * @param  str  input string to search for names
 * @return   item names in array
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::GetVarNames(const std::string &str)
{
   std::string str1 = str;
   std::string name;
   StringArray itemNames;
   Real rval;

   if (str == "")
      return itemNames;

   for (UnsignedInt i=0; i<str1.size(); i++)
   {
      // Make dot(.) as part of a name (Fix for GMT-1012, LOJ: 2012.10.18)
      //if (isalnum(str1[i]) || str1[i] == '_')
      if (isalnum(str1[i]) || str1[i] == '_' || str1[i] == '.')
      {
         name = name + str1[i];
      }
      else
      {
         if (name != "" && !ToReal(name, rval))
         {
            // just add new names
            if (find(itemNames.begin(), itemNames.end(), name) == itemNames.end())
               itemNames.push_back(name);
         }
         name = "";
      }
   }

   // add last item
   if (name != "" && find(itemNames.begin(), itemNames.end(), name) == itemNames.end())
      itemNames.push_back(name);

   return itemNames;
}


//------------------------------------------------------------------------------
// void WriteStringArray(const StringArray &strArray, const std::string &desc = "",
//                       const std::string &prefix = "")
//------------------------------------------------------------------------------
void GmatStringUtil::WriteStringArray(const StringArray &strArray,
                                      const std::string &desc,
                                      const std::string &prefix)
{
   Integer arrSize = strArray.size();
   MessageInterface::ShowMessage("%s\n", desc.c_str());
   MessageInterface::ShowMessage("%sThere are %d strings:\n", prefix.c_str(), arrSize);
   for (Integer i = 0; i < arrSize; i++)
   {
      MessageInterface::ShowMessage("%s'%s'\n", prefix.c_str(), strArray[i].c_str());
   }
}


std::string GmatStringUtil::GetAlignmentString(const std::string inputString, 
											  UnsignedInt len, 
											  AlignmentType adjust)
{
   std::string s1, retVal;
   s1.assign(len,' ');
   
   switch(adjust)
   {
   case LEFT:
	  retVal = (inputString + s1).substr(0,len);
	  break;

   case RIGHT:
      retVal = s1 + inputString;
	  retVal = retVal.substr(retVal.length()-len, len);
	  break;

   case CENTER:
      retVal = s1 + inputString + s1;
	  retVal = retVal.substr((len + inputString.length())/2 , len);
      break;
   }

//   MessageInterface::ShowMessage("retVal=<%s>\n", retVal.c_str());

   return retVal;
}

//------------------------------------------------------------------------------
// std::wstring StringToWideString(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Converts narrow string (std::string) to wide string (std::wstring).
 */
//------------------------------------------------------------------------------
std::wstring GmatStringUtil::StringToWideString(const std::string &str)
{
   // Convert an ASCII string to a Unicode String
   // Method 1 (cross-platform):
   std::wstring wstrTo;
   wchar_t *wszTo = new wchar_t[str.length() + 1];
   wszTo[str.size()] = L'\0';
   int num = std::mbstowcs(wszTo, str.c_str(), str.length());
   wstrTo = wszTo;
   delete[] wszTo;
   return wstrTo;
}


//------------------------------------------------------------------------------
// std::string WideStringToString(const std::wstring &wstr)
//------------------------------------------------------------------------------
/**
 * Converts wide string (std::wstring) to narrow string (std::string).
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::WideStringToString(const std::wstring &wstr)
{
   // Convert a Unicode string to an ASCII String
   // Method 1 (cross-platform):
   std::string strTo;
   char *szTo = new char[wstr.length() + 1];
   szTo[wstr.size()] = '\0';
   int num = std::wcstombs(szTo, wstr.c_str(), wstr.length());

   strTo = szTo;
   delete[] szTo;
   return strTo;
}


//------------------------------------------------------------------------------
// std::string WideStringToString(const wchar_t *wchar)
//------------------------------------------------------------------------------
/**
 * Converts wide string (wchar_t*) to narrow string (std::string).
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::WideStringToString(const wchar_t *wchar)
{
   std::wstring wstr(wchar);
   return WideStringToString(wstr);
}



//------------------------------------------------------------------------------
// bool IsValidIdentity(const std::string &str)
//------------------------------------------------------------------------------
/*
* Checks for valid identity. An identity is a name of a GMAT object. It is 
* started by a alphabet and follow by a series of alphabet, number, and 
* underscore. It has not to be GMAT keyword such as "GMAT", "Create", 
* or "function".
*
* @param  str  The input string to check for valid name
*
* Returns      true if string is satisfied that condition, false otherwise
*
*/
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidIdentity(const std::string &str)
{
   // identity has to be not empty name
   if (str == "")
      return false;

   // It has to be not GMAT key words
   if (str == "GMAT" || str == "Create" || str == "function")
      return false;

   // First letter must start with alphabet
   if (!isalpha(str[0]))
      return false;

   // follow by a series of alphabet, number, or underscore
   for (UnsignedInt i = 1; i < str.size(); i++)
   {
      if (!isalnum(str[i]) && (str[i] != '_'))
         return false;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool IsValidExtendedIdentity(const std::string &str)
//------------------------------------------------------------------------------
/*
* Checks for valid extended identity. An extended identity is an extended name 
* of a GMAT object. It is has syntax [<Identity>.]*<Identity>. An identity is 
* started by a alphabet and follow by a series of alphabet, number, and
* underscore. It has not to be GMAT keyword such as "GMAT", "Create",
* or "function".
*
* @param  str  The input string to check for valid name
*
* Returns      true if string is satisfied that condition, false otherwise
*
*/
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidExtendedIdentity(const std::string &str)
{
   std::string str1 = str;
   std::size_t pos = str1.find_first_of('.');

   while (pos != str1.npos)
   {
      std::string name = str1.substr(0, pos);
      if (!IsValidIdentity(name))
         return false;

      str1 = str1.substr(pos + 1);
      pos = str1.find_first_of('.');
   }   

   if (!IsValidIdentity(str1))
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// bool IsValidFileName(const std::string &str)
//------------------------------------------------------------------------------
/*
* Checks for valid file name. A file name is a string containing all characters
* with ASCII from 32 to 126 except /\|*":<>? characters.
*
* @param  str  The input string to check for valid name
*
* Returns      true if string is satisfied that condition, false otherwise
*
*/
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidFileName(const std::string &str)
{
   // file name has to be not empty name
   if (str == "")
      return false;

   // It contains any character with ASCII fro 32 to 127 except /\|*":<>?
   for (UnsignedInt i = 0; i < str.size(); i++)
   {
      if ((str[i] < 32) || (str[i] > 126) || 
         (str[i] == '\\') || (str[i] == '/') || (str[i] == '|') || (str[i] == '*') ||
         (str[i] == '"') || (str[i] == ':') || (str[i] == '<') || (str[i] == '>') || 
         (str[i] == '?'))
         return false;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool IsValidFileName(const std::string &str)
//------------------------------------------------------------------------------
/*
* Checks for valid full file name. A full file name is a string containing path 
* name and following by file name. 
* We cannot verify based on regular expression of full file name but we only 
* verify for characters which do not allow to be used in full file name. 
*           [<driver>:][<path>]<filename>
*
* @param  str     The input string to check for valid name
* @param  error   Error number
*
* Returns         true if string is satisfied that condition, false otherwise
*
*/
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidFullFileName(const std::string &str, Integer &error)
{
   error = 0;

   // full file name has to be not empty name
   if (str == "")
   {
      error = 1;
      return false;
   }

   // Get driver and verify driver
   std::string str1 = str;
   std::string driver = "";
   std::string::size_type pos = str1.find_first_of(':');
   if (pos != str1.npos)
   {
      driver = str1.substr(0, pos);
      if (driver.size() > 1)
      {
         error = 2;
         return false;
      }

      if ((driver.size() == 1) && (!isalpha(driver[0])))
      {
         error = 3;
         return false;
      }
   }

   // Get path and filename
   std::string path, filename;
   str1 = str1.substr(pos + 1);
   std::string::size_type pos1 = str1.find_last_of('/');
   pos = str1.find_last_of('\\');
   if (pos != str1.npos)
   {
      if (pos1 != str1.npos)
      {
         // Fix warning: warning C4244: '=' : conversion from 'Real' to 'unsigned int', possible loss of data
         //pos = GmatMathUtil::Min(pos, pos1);
         pos = pos < pos1 ? pos : pos1;
      }
   }
   else
   {
      if (pos1 != str1.npos)
         pos = pos1;
   }

   if (pos != str1.npos)
   {
      path = str1.substr(0, pos);
      filename = str1.substr(pos + 1);
   }
   else
   {
      path = "";
      filename = str1;
   }
   
   // Verify filename
   if (!IsValidFileName(filename))
   {
      error = 4;
      return false;
   }

   // Verify path. It contains any character with ASCII fro 32 to 127 except |*":<>?
   for (UnsignedInt i = 0; i < path.size(); i++)
   {
      if ((path[i] < 32) || (path[i] > 126) ||
         (path[i] == '|') || (path[i] == '*') ||
         (path[i] == '"') || (path[i] == ':') || (path[i] == '<') || (path[i] == '>') ||
         (path[i] == '?'))
      {
         error = 5;
         return false;
      }
   }
   //MessageInterface::ShowMessage("driver = <%s>   path = <%s>  filename = <%s>\n", driver.c_str(), path.c_str(), filename.c_str());
   return true;
}


StringArray GmatStringUtil::ParseName(const std::string& extendedName)
{
   static StringArray names;
   names.clear();

   std::string name = extendedName;
   std::string::size_type pos = name.find_last_of('.');
   if (pos != name.npos)
   {
      names.push_back(name.substr(0, pos));
      name = name.substr(pos+1);
      names.push_back(name);
   }
   else
   {
      names.push_back(name);
      names.push_back("");
   }
   
   return names;
}
