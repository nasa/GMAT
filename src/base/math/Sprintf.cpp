//$Id$
//------------------------------------------------------------------------------
//                                  Sprintf
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
// Created: 2016.02.17
//
/**
 * Implements Sprintf class for string concatenation.
 */
//------------------------------------------------------------------------------

#include "Sprintf.hpp"
#include "ElementWrapper.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include <stdio.h>               // for sprintf()

//#define DEBUG_VALIDATE_INPUT
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Sprintf()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Sprintf::Sprintf(const std::string &name)
   : StringFunctionNode("Sprintf", name)
{
}


//------------------------------------------------------------------------------
// ~Sprintf()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Sprintf::~Sprintf()
{
}


//------------------------------------------------------------------------------
//  Sprintf(const Sprintf &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Sprintf object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Sprintf::Sprintf(const Sprintf &copy) :
   StringFunctionNode      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Sprintf operation.
 *
 * @return clone of the Sprintf operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Sprintf::Clone() const
{
   return (new Sprintf(*this));
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Sprintf::ValidateInputs()
{
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("Sprintf::ValidateInputs() <%p><%s> entered, desc='%s'\n", this,
       GetTypeName().c_str(), desc.c_str());
   #endif
   
   if (!StringFunctionNode::ValidateInputs())
      return false;
   
   // Add any other validation here
   
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("Sprintf::ValidateInputs() <%p><%s> returning true\n");
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// std;:string EvaluateString()
//------------------------------------------------------------------------------
/**
 * @return the Sprintf of left node
 *
 */
//------------------------------------------------------------------------------
std::string Sprintf::EvaluateString()
{
   static const int MAX_OUTPUT_LENGTH = 30000;
   char outBuffer[MAX_OUTPUT_LENGTH];
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Sprintf::EvaluateString() <%p>'%s' entered, inputArgWrappers.size()=%d\n",
       this, GetName().c_str(), inputArgWrappers.size());
   #endif

   ValidateWrappers();
   
   std::string result;
   int numArgs = inputArgWrappers.size() - 1;
   
   // Parse format
   // Assuming first arg is the format
   std::string format = inputArgWrappers[0]->EvaluateString();
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage("   format = '%s'\n", format.c_str());
   #endif
   
   // Check for number of args matches format code
   int numPercentSign = GmatStringUtil::NumberOfOccurrences(format, '%');
   int numAsterisks = GmatStringUtil::NumberOfOccurrences(format, '*');
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage("   numArgs = %d\n", numArgs);
   MessageInterface::ShowMessage("   numPercentSign = %d\n", numPercentSign);
   MessageInterface::ShowMessage("   numAsterisks = %d\n", numAsterisks);
   #endif
   
   if ((numPercentSign + numAsterisks) != numArgs)
   {
      throw MathException
         ("Error evaluating \"" + GetName() +
          "\"; Number of formatting specs and arguments doesn't match.");
   }
   
   StringArray specArray;
   StringArray resultArray;
   
   // Parse format specs
   // @note: Valid formats in GMAT are: %a, %A, %e, %E, %f, %F, %g, %G.
   //        %c, %d, %i, %o, %u, %x, and %X are not valid.
   //================================================================================
   // Format Specifier
   // A format specifier follows this prototype:
   // %[flags][width][.precision][length]specifier
   //
   // specifier
   // %a      Hexadecimal floating point, lowercase
   // %A      Hexadecimal floating point, uppercase
   // %c      Character (NOT SUPPORTED in GMAT)
   // %d/%i   Signed decimal integers (NOT SUPPORTED in GMAT)
   // %e      Scientific notation (lowercase e)
   // %E      Scientific notation (uppercase E)
   // %f      Decimal floating point (lowercase)
   // %F      Decimal floating point (uppercase)
   // %g      Uses %e or %f, whichever is shorter
   // %G      Uses %E or %F, whichever is shorter
   // %o      Unsigned octal (NOT SUPPORTED in GMAT)
   // %s      String of characters
   // %p      Pointer address (NOT SUPPORTED in GMAT)
   // %n      Nothing printed. The corresponding argument must be a pointer to a signed int.
   //         The number of characters written so far is stored in the pointed location. (NOT SUPPORTED in GMAT)
   // %u      Unsigned decimal integers (NOT SUPPORTED in GMAT)
   // %x      Unsigned hexadecimal (lowercase letters) (NOT SUPPORTED in GMAT)
   // %X      Unsigned hexadecimal (uppercase letters) (NOT SUPPORTED in GMAT)
   // %%      Prints a percent(%) sign)
   //
   // flags
   // -       Left-justify within the given field width; Right justification is the default
   // +       Forces to preceed the result with a plus or minus sign (+ or -) even for 
   //         positive numbers. By default, only negative numbers are preceded with a - sign.
   // #       Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X
   //         respectively for values different than zero.
   //         Used with a, A, e, E, f, F, g or G it forces the written output to
   //         contain a decimal point even if no more digits follow. By default,
   //         if no digits follow, no decimal point is written.
   // 0       Left-pads the number with zeroes (0) instead of spaces when padding
   //         is specified
   //
   // width
   // (number) Minimum number of characters to be printed. If the value to be
   //          printed is shorter than this number, the result is padded with blank spaces.
   //          The value is not truncated even if the result is larger.
   // *        The width is not specified in the format string, but as an additional
   //          integer value argument preceding the argument that has to be formatted.
   //
   // .precision
   // .number For a, A, e, E, f and F specifiers: this is the number of digits to be
   //         printed after the decimal point (by default, this is 6).
   // .*      The precision is not specified in the format string, but as an additional
   //         integer value argument preceding the argument that has to be formatted.
   //
   // length
   // (none)  double for fF, eE, gG
   // L       long double for fF, eE, gG
   //================================================================================
   // (Needs more working parsing format specs such as space is allowed after % sign)
   // Current code assumes no space between % sign and spec code such as %i, %e, %10.5f, %g, %s
   
   StringArray formatSpecs = GmatStringUtil::SeparateBy(format, "%");
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage("   formatSpecs.size() = %d\n", formatSpecs.size());
   #endif
   unsigned int start = 0;
   if ((int)formatSpecs.size() > numPercentSign)
      start = 1;
   
   for (unsigned int i = start; i < formatSpecs.size(); i++)
   {
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage("   formatSpecs[%d] = '%s'\n", i, formatSpecs[i].c_str());
      #endif
      
      // find specifier
      std::string code = formatSpecs[i];
      std::string::size_type specPos = code.find_first_of("aAcdieEfFgGnopsuxX");
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage("   specPos = %u\n", specPos);
      #endif
      
      if (specPos != code.npos)
      {
         std::string spec = code.substr(0, specPos + 1);
         if (formatSpecs.size() > 1)
            spec = "%" + spec;
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage("   Adding '%s' to specArray\n", spec.c_str());
         #endif
         specArray.push_back(spec);
      }
      else
      {
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage
            ("***** Invalid format specifier found in Sprintf::EvaluateString()\n");
         #endif
         throw MathException
            ("Error evaluating \"" + GetName() + "\"; Invalid format specifier found. "
             "Allowed specs for Variable and Array element are: \"%a, %A, %e, %E, %f, %F, %g, %G\"; "
             "and \"%s\" is only allowed spec for String");
      }
   }
   
   Real rval = -99999.999;
   Integer ival = -99999;
   bool bval = false;
   std::string sval = "";
   Rmatrix rmat;
   Rvector rvec;
   int retval = -1;
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage("   ========== Now going through each format spec and input\n");
   #endif
   
   int specIndex = -1;;
   for (unsigned int i = 1; i < inputArgWrappers.size(); i++)
   {
      specIndex++;
      //std::string formatSpec = specArray[i-1];
      std::string formatSpec = specArray[specIndex];
      ElementWrapper *wrapper = inputArgWrappers[i];
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("   ===== formatSpec[%d]: '%s', wrapperDesc: '%s', stringVal: '%s'\n",
          specIndex, formatSpec.c_str(),
          wrapper ? wrapper->GetDescription().c_str() : "NULL",
          wrapper ? wrapper->ToString().c_str() : "NULL");
      #endif
      
      if (wrapper == NULL)
      {
         throw MathException
            ("Error evaluating \"" + GetName() + "\"; Encountered with unsupported "
             "spec or unknown error in sprintf()");
      }
      
      Gmat::ParameterType dataType = wrapper->GetDataType();
      int firstVal  = -99999;
      int secondVal = -99999;
      // Check if variable width or precision is used, for example '%*.*f'
      int numStars = GmatStringUtil::NumberOfOccurrences(formatSpec, '*');
      // First value is width
      if (numStars > 1)
      {
         if (dataType != Gmat::REAL_TYPE)
         {
            std::string typeStr = GmatBase::PARAM_TYPE_STRING[dataType];
            throw MathException
               ("Error evaluating \"" + GetName() + "\"; The data type \"" +
                typeStr + "\" is not compatible with format spec in sprintf(). "
                "It requires integer value");
         }
         
         firstVal = (int)(wrapper->EvaluateReal());
         
         // Get next value for '*'
         i++;
         wrapper = inputArgWrappers[i];
         if (wrapper == NULL)
         {
            throw MathException
               ("Error evaluating \"" + GetName() + "\"; Encountered with unsupported "
                "spec or unknown error in sprintf()");
         }
         dataType = wrapper->GetDataType();
      }
      
      // Second value is width or precision
      if (numStars > 0)
      {
         if (dataType != Gmat::REAL_TYPE)
         {
            std::string typeStr = GmatBase::PARAM_TYPE_STRING[dataType];
            throw MathException
               ("Error evaluating \"" + GetName() + "\"; The data type \"" +
                typeStr + "\" is not compatible with format spec in sprintf(). "
                "It requires integer value");
         }

         if (numStars > 1)
            secondVal = (int)(wrapper->EvaluateReal());
         else
            firstVal = (int)(wrapper->EvaluateReal());
         
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage
            ("   firstVal = %d, secondVal = %d\n", firstVal, secondVal);
         #endif
         
         // Get next value for % format
         i++;
         wrapper = inputArgWrappers[i];
         if (wrapper == NULL)
         {
            throw MathException
               ("Error evaluating \"" + GetName() + "\"; Encountered with unsupported "
                "spec or unknown error in sprintf()");
         }
         dataType = wrapper->GetDataType();
      }
      
      // Now format value
      switch (dataType)
      {
      case Gmat::REAL_TYPE:
         rval = wrapper->EvaluateReal();
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage
            ("   It is a real, formatSpec = '%s', rval=%.15f\n", formatSpec.c_str(), rval);
         #endif
         
         // Check for invalid format spec for Real data
         if (formatSpec.find_last_of("cdinopsuxX") != std::string::npos)
         {
            std::string typeStr = GmatBase::PARAM_TYPE_STRING[dataType];
            throw MathException
               ("Error evaluating \"" + GetName() + "\"; The data type \"" +
                typeStr + "\" is not compatible with format spec in sprintf(). "
                "Allowed specs are: \"%a, %A, %e, %E, %f, %F, %g, %G\" ");
         }
         
         // Visual Studio implemented snprintf in VS 2015
         //snprintf(outBuffer, MAX_OUTPUT_LENGTH, formatSpec.c_str(), rval);
         if (numStars == 2)
            retval = sprintf(outBuffer, formatSpec.c_str(), firstVal, secondVal, rval);
         else if (numStars == 1)
            retval = sprintf(outBuffer, formatSpec.c_str(), firstVal, rval);
         else
            retval = sprintf(outBuffer, formatSpec.c_str(), rval);
         
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage("   retval=%d, outBuffer = '%s'\n", retval, outBuffer);
         #endif
         resultArray.push_back(outBuffer);
         break;
      case Gmat::STRING_TYPE:
         sval = wrapper->EvaluateString();
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage
            ("   It is a string, formatSpec='%s', sval='%s', numStars=%d\n",
             formatSpec.c_str(), sval.c_str());
         #endif
         
         // Check for format spec, it should be '%s' otherwise throw an exception
         if (formatSpec.find_last_of("s") == std::string::npos)
         {
            std::string typeStr = GmatBase::PARAM_TYPE_STRING[dataType];
            throw MathException
               ("Error evaluating \"" + GetName() + "\"; The data type \"" +
                typeStr + "\" is not compatible with format spec in sprintf(). "
                "Only allowed spec for String type is \"%s\"");
         }
         
         // Call sprintf()
         if (numStars == 2)
            retval = sprintf(outBuffer, formatSpec.c_str(), firstVal, secondVal, sval.c_str());
         else if (numStars == 1)
            retval = sprintf(outBuffer, formatSpec.c_str(), firstVal, sval.c_str());
         else
            sprintf(outBuffer, formatSpec.c_str(), sval.c_str());
         
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage("   outBuffer = '%s'\n", outBuffer);
         #endif
         resultArray.push_back(outBuffer);
         break;
      default:
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage
            ("***** Sprintf::EvaluateString() Using default wrapper->ToString() "
             "for the data type %s\n", GmatBase::PARAM_TYPE_STRING[dataType].c_str());
         #endif
         // Just add string value for now
         sval = wrapper->ToString();
         if (GmatStringUtil::EndsWith(formatSpec, "s"))
         {
            if (numStars == 2)
               retval = sprintf(outBuffer, formatSpec.c_str(), firstVal, secondVal, sval.c_str());
            else if (numStars == 1)
               retval = sprintf(outBuffer, formatSpec.c_str(), firstVal, sval.c_str());
            else
               sprintf(outBuffer, formatSpec.c_str(), sval.c_str());
            
            #ifdef DEBUG_EVALUATE
            MessageInterface::ShowMessage("   outBuffer = '%s'\n", outBuffer);
            #endif
            resultArray.push_back(outBuffer);
         }
         else
         {
            std::string typeStr = GmatBase::PARAM_TYPE_STRING[dataType];
            throw MathException
               ("Error evaluating \"" + GetName() + "\"; The data type \"" +
                typeStr + "\" is not compatible with format spec in sprintf()");
         }
      }
   }
   
   result = format;
   
   // Replace format spec with actual formatted output
   for (unsigned int i = 0; i < specArray.size(); i++)
   {
      result = GmatStringUtil::ReplaceFirst(result, specArray[i], resultArray[i]);
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage("   result = '%s'\n", result.c_str());
      #endif
   }
   
   //@note:
   // Can I use vsnprintf here?
   // int vsnprintf (char * s, size_t n, const char * format, va_list arg );   
   
   // Why \n not working in the format spec? 
   // sprintf(outBuffer, "%s\n\n", "===== Begin Testing.....");
   // MessageInterface::ShowMessage(outBuffer);
   // MessageInterface::ShowMessage("===== End Testing...\n");
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Sprintf::EvaluateString() <%p>'%s' returning:\n   '%s'\n", this,
       GetName().c_str(), result.c_str());
   #endif
   return result;
}

