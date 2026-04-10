//$Id$
//------------------------------------------------------------------------------
//                              UserInputValidator
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
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Implements UserInputValidator class.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "UserInputValidator.hpp"
#include "GmatPanel.hpp"
#include "GmatDialog.hpp"
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "FileUtil.hpp"             // for GmatFileUtil::
#include "RealUtilities.hpp"        // for GmatMathUtil::
#include "TimeSystemConverter.hpp"
#include "DateUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CHECK_REAL
//#define DEBUG_CHECK_VARIABLE

//---------------------------------
// static variables
//---------------------------------
std::string UserInputValidator::lessOrEq = " <= ";
std::string UserInputValidator::lessThan = " < ";
std::string UserInputValidator::moreOrEq = " >= ";
std::string UserInputValidator::moreThan = " > ";

//------------------------------------------------------------------------------
// UserInputValidator()
//------------------------------------------------------------------------------
UserInputValidator::UserInputValidator()
{
   mObject = NULL;
   mGuiManager = NULL;
   mWindow = NULL;
   mIsInputValid = true;
   
   mMsgFormat =
      "The value of \"%s\" for field \"%s\" is not an allowed value%s. \n"
      "The allowed values are: [%s].";
}


//------------------------------------------------------------------------------
// ~UserInputValidator()
//------------------------------------------------------------------------------
UserInputValidator::~UserInputValidator()
{
}


//------------------------------------------------------------------------------
// void SetObject(GmatBase *obj)
//------------------------------------------------------------------------------
void UserInputValidator::SetObject(GmatBase *obj)
{
   if (obj != NULL)
   {
      mObject = obj;
      if (mObject->IsOfType(Gmat::COMMAND))
      {
         mMsgFormat =
            "The value of \"%s\" for field \"%s\" on command \""
            + mObject->GetTypeName() +  "\" is not an allowed value%s. \n"
            "The allowed values are: [%s].";
      }
      else
      {
         mMsgFormat =
            "The value of \"%s\" for field \"%s\" on object \""
            + mObject->GetName() +  "\" is not an allowed value%s. \n"
            "The allowed values are: [%s].";
      }
   }
}


//------------------------------------------------------------------------------
// void SetGuiManager(GuiItemManager *manager)
//------------------------------------------------------------------------------
void UserInputValidator::SetGuiManager(GuiItemManager *manager)
{
   mGuiManager = manager;
}


//------------------------------------------------------------------------------
// void SetWindow(wxWindow *window)
//------------------------------------------------------------------------------
void UserInputValidator::SetWindow(wxWindow *window)
{
   mWindow = window;
}


//------------------------------------------------------------------------------
// bool IsInputValid()
//------------------------------------------------------------------------------
bool UserInputValidator::IsInputValid()
{
   return mIsInputValid;
}


//------------------------------------------------------------------------------
// bool IsValidName(const wxString &name)
//------------------------------------------------------------------------------
/*
 * Checks for a valid name.
 *
 * @param  name  Input name to be valdated
 *
 * @return true if input name is valid, false otherwise
 */
//------------------------------------------------------------------------------
bool UserInputValidator::IsValidName(const wxString &name)
{
   if (name == "")
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "The name is blank, please enter a valid name");
      
      SetErrorFlag();
      return false;
   }
   
   if (!GmatStringUtil::IsValidName(name.c_str()))
   {
      std::string format = GmatStringUtil::GetInvalidNameMessageFormat();
      MessageInterface::PopupMessage
         (Gmat::ERROR_, format.c_str(), name.WX_TO_C_STRING);
      
      SetErrorFlag();
      return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool CheckFileName(const wxString &str, const std::string &str,
//                    const std::string &field, bool onlyMsg = false)
//------------------------------------------------------------------------------
/*
 * Checks for a valid file name.
 *
 * @param  str        Input file name to be validated
 * @param  field      Field name should be used in the error message
 * @param  onlyMsg    if true, it only shows error message (false)
 *
 * @return true if input name is valid, false otherwise
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckFileName(const std::string &str,
                                       const std::string &field, bool onlyMsg)
{
   if (onlyMsg)
   {
      std::string msg = GmatFileUtil::GetInvalidFileNameMessage(1);
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(), "",
          msg.c_str());
      
      SetErrorFlag();
      return false;
   }

   // We don't want allow blank file name so pass false
   if (!GmatFileUtil::IsValidFileName(str, false))
   {
      std::string msg = GmatFileUtil::GetInvalidFileNameMessage(1);
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(), "",
          msg.c_str());
      
      SetErrorFlag();
      return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool CheckLength(const wxString &str, const std::string &field, ...)
//------------------------------------------------------------------------------
bool UserInputValidator::CheckLength(const std::string &str, const std::string &field,
                                     const std::string &expLength,
                                     const Integer min, const Integer max)
{
   Integer len = str.length();
   // We don't want allow blank file name so pass false
   if ((len < min) || (len > max))
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(), "",
          expLength.c_str());
      
      return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool CheckReal(Real &rvalue, const char *str,
//                const std::string &field, const std::string &expRange,
//                bool onlyMsg, bool checkRange, bool positive bool zeroOk)
//------------------------------------------------------------------------------
/*
 * @see CheckReal(Real &rvalue, const std::string &str, ...)
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckReal(Real &rvalue, const char *str,
                          const std::string &field, const std::string &expRange,
                          bool onlyMsg, bool checkRange, bool positive, bool zeroOk)
{
   return CheckReal(rvalue, std::string(str), field, expRange,
                    onlyMsg, checkRange, positive, zeroOk);
}


//------------------------------------------------------------------------------
// bool CheckReal(Real &rvalue, const std::string &str,
//                const std::string &field, const std::string &expRange,
//                bool onlyMsg, bool checkRange, bool positive bool zeroOk)
//------------------------------------------------------------------------------
/*
 * This method checks if input string is valid real number. It uses
 * GmatStringUtil::ToReal() to convert string to Real value. This method
 * pops up the error message and return false if input string is
 * not a real number.
 *
 * @param  rvalue     Real value to be set if input string is valid real number
 * @param  str        Input string value
 * @param  field      Field name should be used in the error message
 * @param  expRange   Expected value range to be used in the error message
 * @param  onlyMsg    if true, it only shows error message (false)
 * @param  checkRange if true, it will check for positive and zero (false)
 * @param  positive   if true, the value must be positive (false)
 * @param  zeroOk     if true, zero is allowed (false)
 *
 * @return true if value is valid, false otherwise
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckReal(Real &rvalue, const std::string &str,
                          const std::string &field, const std::string &expRange,
                          bool onlyMsg, bool checkRange, bool positive, bool zeroOk)
{
   #ifdef DEBUG_CHECK_REAL
   MessageInterface::ShowMessage
      ("UserInputValidator::CheckReal() str='%s', field='%s', expRange='%s'\n", str.c_str(),
       field.c_str(), expRange.c_str());
   #endif
   
   if (onlyMsg)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(), "",
          expRange.c_str());

      SetErrorFlag();
      return false;
   }
   
   // check for real value
   Real rval;
   if (GmatStringUtil::ToReal(str, &rval, false, false))
   {
      rvalue = rval;
      #ifdef DEBUG_CHECK_REAL
      MessageInterface::ShowMessage
         ("UserInputValidator::CheckReal() rval = %12.10f\n", rval);
      #endif
      
      if (checkRange)
      {
         if (!positive || (positive && rval > 0) || (zeroOk && rval >= 0))
            return true;
      }
      else
         return true;
   }
   
   #ifdef DEBUG_CHECK_REAL
   MessageInterface::ShowMessage
      ("UserInputValidator::CheckReal() NOT a valid real number\n");
   #endif
   MessageInterface::PopupMessage
      (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(), "",
       expRange.c_str());
   
   SetErrorFlag();
   return false;
}

//------------------------------------------------------------------------------
// bool CheckInteger(Integer &ivalue, const std::string &str,
//                   const std::string &field, const std::string &expRange,
//                   bool onlyMsg = false, bool positive, bool zeroOk)
//------------------------------------------------------------------------------
/*
 * @see CheckInteger(Integer &ivalue, const std::string &str, ...)
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckInteger(Integer &ivalue, const char *str,
                                      const std::string &field,
                                      const std::string &expRange, bool onlyMsg,
                                      bool checkRange, bool positive, bool zeroOk)
{
   return CheckInteger(ivalue, std::string(str), field, expRange, onlyMsg,
                       checkRange, positive, zeroOk);
}

//------------------------------------------------------------------------------
// bool CheckInteger(Integer &ivalue, const std::string &str,
//                   const std::string &field, const std::string &expRange,
//                   bool onlyMsg = false, bool positive, bool zeroOk)
//------------------------------------------------------------------------------
/*
 * This method checks if input string is valid integer number. It uses
 * GmatStringUtil::ToInteger() to convert string to Integer value. This method
 * pops up the error message and return false if input string is
 * not an integer number.
 *
 * @param  ivalue     Integer value to be set if input string is valid integer
 * @param  str        Input string value
 * @param  field      Field name should be used in the error message
 * @param  expRange   Expected value range to be used in the error message
 * @param  onlyMsg    if true, it only shows error message (false)
 * @param  checkRange if true, it will check for positive and zero (false)
 * @param  positive   if true, the value must be positive (false)
 * @param  zeroOk     if true, zero is allowed (false)
 *
 * @return true if value is valid, false otherwise
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckInteger(Integer &ivalue, const std::string &str,
                                      const std::string &field,
                                      const std::string &expRange, bool onlyMsg,
                                      bool checkRange, bool positive, bool zeroOk)
{
   if (onlyMsg)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(), "",
          expRange.c_str());
      
      SetErrorFlag();
      return false;
   }
   
   // check for integer value
   Integer ival;
   if (GmatStringUtil::ToInteger(str, &ival, false, false))
   {
      ivalue = ival;

      if (checkRange)
      {
         if (!positive || (positive && ival > 0) || (zeroOk && ival >= 0))
            return true;
      }
      else
         return true;
   }
   
   MessageInterface::PopupMessage
      (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(), "",
       expRange.c_str());
   
   SetErrorFlag();
   return false;
}


//------------------------------------------------------------------------------
// bool CheckIntegerRange(Integer &ivalue, const std::string &str, 
//         const std::string &field, Integer lower, Integer upper,
//         bool checkLower = true, bool checkUpper = true,
//         bool includeLower = false, bool includeUpper = false)
//------------------------------------------------------------------------------
/*
 * Checks an integer number against a lower and upper bound.
 *
 * @param  ivalue       Integer value to be set if input string is valid integer
 * @param  str          Input string value
 * @param  lower        Lower bound against which to check the value
 * @param  upper        Upper bound against which to check the value
 * @param  checkLower   Flag indicating whether or not to check the lower bound (true)
 * @param  checkUpper   Flag indicating whether or not to check the upper bound (true)
 * @param  includeLower Flag indicating whether or not the range check is inclusive of the
 *                      lower bound (false)
 * @param  includeUpper Flag indicating whether or not the range check is inclusive of the
 *                      upper bound (false)
 *
 * @return true if input value is valid, false otherwise
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckIntegerRange(Integer &ivalue, const std::string &str, 
                                           const std::string &field, Integer lower,
                                           Integer upper, bool checkLower, bool checkUpper,
                                           bool includeLower, bool includeUpper)
{
   Integer ival;
   if (GmatStringUtil::ToInteger(str, &ival, false, false))
   {
      Real rval = (Real)ival;
      
      bool retval = CheckRealRange(str, rval, field, lower, upper, checkLower,
                                   checkUpper, includeLower, includeUpper, true);
      if (retval)
         ivalue = ival;
      
      return retval;
   }
   
   std::string expRange = "Integer";
   MessageInterface::PopupMessage
      (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(), "",
       expRange.c_str());
   
   SetErrorFlag();
   return false;
}


//------------------------------------------------------------------------------
// bool CheckVariable(const char *varName, ObjectTypeArray ownerTypes,
//                    const std::string &field, const std::string &expRange,
//                    bool allowNumber  = true, bool allowNonPlottable = false,
//                    bool allowWholeArray = false)
//------------------------------------------------------------------------------
/*
 * Checks if input variable is a Number, Variable, Array element, or parameter of
 * input owner type.
 *
 * @param  varName     Input variable name to be checked
 * @param  ownerTypes  Input owner array of types if Parameter (such as Gmat::SPACECRAFT),
 *                     if type is UNKNOWN_OBJECT, it doesn't check for type
 * @param  field       Field name should be used in the error message
 * @param  expRange    Expected value range to be used in the error message
 * @param  allowNumber true if varName can be a Real number [true]
 * @param  allowNonPlottable  true if varName can be a non-plottable [false]
 * @param  allowWholeArray    true if varName can be an whole array [false]
 *
 * @return true if varName is valid
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckVariable(const char *varName, ObjectTypeArray ownerTypes,
                                       const std::string &field, const std::string &expRange,
                                       bool allowNumber, bool allowNonPlottable,
                                       bool allowObjectProperty, bool allowWholeArray)
{
   return CheckVariable(std::string(varName), ownerTypes, field, expRange, allowNumber,
                        allowNonPlottable, allowObjectProperty, allowWholeArray);
}


//------------------------------------------------------------------------------
// bool CheckVariable(const std::string &varName, ObjectTypeArray ownerTypes,
//                    const std::string &field, const std::string &expRange,
//                    bool allowNumber  = true, bool allowNonPlottable = false,
//                    bool allowWholeArray = false)
//------------------------------------------------------------------------------
/*
 * Checks if input variable is a Number, Variable, Array element, or parameter of
 * input owner type.
 *
 * @param  varName     Input variable name to be checked
 * @param  ownerTypes  Input owner array of types if Parameter (such as Gmat::SPACECRAFT),
 *                     if type is UNKNOWN_OBJECT, it doesn't check for type
 * @param  field       Field name should be used in the error message
 * @param  expRange    Expected value range to be used in the error message
 * @param  allowNumber true if varName can be a Real number [true]
 * @param  allowNonPlottable  true if varName can be a non-plottable [false]
 * @param  allowWholeArray    true if varName can be an whole array [false]
 *
 * @return true if varName is valid
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckVariable(const std::string &varName, ObjectTypeArray ownerTypes,
                                       const std::string &field, const std::string &expRange,
                                       bool allowNumber, bool allowNonPlottable,
                                       bool allowObjectProperty, bool allowWholeArray)
{
   #ifdef DEBUG_CHECK_VARIABLE
   MessageInterface::ShowMessage
      ("UserInputValidator::CheckVariable() entered, varName='%s', ownerTypes[0]=%d, field='%s'\n"
       "   expRange='%s'\n   allowNumber=%d, allowNonPlottable=%d, allowObjectProperty=%d, "
       "allowWholeArray=%d\n", varName.c_str(), ownerTypes.at(0), field.c_str(), expRange.c_str(),
       allowNumber, allowNonPlottable, allowObjectProperty, allowWholeArray);
   #endif
   
   if (mGuiManager == NULL)
   {
      MessageInterface::ShowMessage
         ("UserInputValidator::CheckVariable() mGuiManager is NULL\n");
      return false;
   }
   
   int retval = -1;
   try
   {
      for (unsigned int ii = 0; ii < ownerTypes.size(); ii++)
      {
         retval = mGuiManager->
            IsValidVariable(varName.c_str(), ownerTypes.at(ii), allowNumber, allowNonPlottable,
                            allowObjectProperty, allowWholeArray);
         if (retval != 0) break;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      SetErrorFlag();
      return false;
   }
   
   #ifdef DEBUG_CHECK_VARIABLE
   MessageInterface::ShowMessage("   retval = %d\n", retval);
   #endif
   
   if (retval == -1)
   {
      std::string lastMsg = mGuiManager->GetLastErrorMessage().WX_TO_STD_STRING;
      lastMsg = " - " + lastMsg;
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), varName.c_str(), field.c_str(),
          lastMsg.c_str(), expRange.c_str());
      
      mGuiManager->SetLastErrorMessage("");
      SetErrorFlag();
      return false;
   }
   else if (retval == 3)
   {
      std::string type, ownerName, depObj;
      GmatStringUtil::ParseParameter(varName, type, ownerName, depObj);
      
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "The value of \"%s\" for field \"%s\" "
          "has undefined object \"%s\".\nPlease create proper object first "
          "from the Resource Tree.\n", varName.c_str(), field.c_str(), ownerName.c_str());
      
      SetErrorFlag();
      return false;
   }
   else if (retval == 4)
   {
      std::string type, ownerName, depObj;
      GmatStringUtil::ParseParameter(varName, type, ownerName, depObj);
      
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "The value \"%s\" for field \"%s\" "
          "has unknown Parameter type \"%s\".\n", varName.c_str(), field.c_str(),
          type.c_str());
      
      SetErrorFlag();
      return false;
   }
   else if (retval == 5)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), varName.c_str(), field.c_str(),
          " - invalid array index", expRange.c_str());
      
      SetErrorFlag();
      return false;
   }
   else if (retval == 6)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), varName.c_str(), field.c_str(),
          " - invalid object field", expRange.c_str());
      
      SetErrorFlag();
      return false;
   }
   else if (retval == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), varName.c_str(), field.c_str(), "",
          expRange.c_str());
      
      SetErrorFlag();
      return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool CheckRealRange(Real value, Real lower, Real upper,
//                     bool checkLower = true, bool checkUpper = true,
//                     bool includeLower, bool includeUpper)
//------------------------------------------------------------------------------
/*
 * Checks a real number against a lower and upper bound.
 *
 * @param  sValue      string representation of the real value
 * @param  value       real value to be checked
 * @param  lower       lower bound against which to check the value
 * @param  upper       upper bound against which to check the value
 * @param  checkLower  flag indicating whether or not to check the lower bound
 * @param  checkUpper  flag indicating whether or not to check the upper bound
 * @param includeLower flag indicating whether or not the range check is inclusive of the
 *                     lower bound
 * @param includeUpper flag indicating whether or not the range check is inclusive of the
 *                     upper bound
 * @param  isInteger   flag indicating input value is integer number (false)
 *
 * @return true if input value is valid, false otherwise
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckRealRange(const std::string &sValue,
                                        Real value,        const std::string &field,
                                        Real lower,        Real upper,
                                        bool checkLower,   bool checkUpper,
                                        bool includeLower, bool includeUpper,
                                        bool isInteger)
{
   if ((!checkLower) && (!checkUpper)) return true;
   
   if (checkLower && checkUpper)
   {
      if ((value > lower) && (value < upper))                  return true;
      if (includeLower && GmatMathUtil::IsEqual(value, lower)) return true;
      if (includeUpper && GmatMathUtil::IsEqual(value, upper)) return true;
   }
   else if (checkLower)
   {
      if (value > lower) return true;
      if (includeLower && GmatMathUtil::IsEqual(value, lower)) return true;
   }
   else // checkUpper only
   {
      if (value < upper) return true;
      if (includeUpper && GmatMathUtil::IsEqual(value, upper)) return true;
   }

   std::string inputType = "Real Number";
   if (isInteger)
      inputType = "Integer Number";
   
   // range check failed; generate error message
   std::stringstream ss("");
   if (checkLower && checkUpper)
   {
      ss << lower;
      if (includeLower) ss << lessOrEq;
      else              ss << lessThan;
      ss << inputType;
      if (includeUpper) ss << lessOrEq;
      else              ss << lessThan;
      ss << upper;
   }
   else if (checkLower)
   {
      ss << inputType;
      if (includeLower) ss << moreOrEq;
      else              ss << moreThan;
      ss << lower;
   }
   else if (checkUpper)
   {
      ss << inputType;
      if (includeUpper) ss << lessOrEq;
      else              ss << lessThan;
      ss << upper;
   }

   std::string expRange = ss.str();
   MessageInterface::PopupMessage
      (Gmat::ERROR_, mMsgFormat.c_str(), sValue.c_str(), field.c_str(), "",
       expRange.c_str());

   SetErrorFlag();
   return false;
}

//------------------------------------------------------------------------------
// bool CheckTimeFormatAndValue(const std::string &format, const std::string& value,
//                              const std::string &field,  bool checkRange)
//------------------------------------------------------------------------------
/*
 * Checks the input epoch string for correct format and, on option, allowed value.
 *
 * @param  format      epoch format
 * @param  value       epoch value to be checked
 * @param  field       object's field name
 * @param  checkRange  flag indicating whether or not to check value range
 *
 * @return true if input value is valid, false otherwise
 */
//------------------------------------------------------------------------------
bool UserInputValidator::CheckTimeFormatAndValue(const std::string &format, const std::string& value,
                                                 const std::string &field,  bool checkRange)
{
   std::string timeFormat = "ModJulian";
   if (format.find("Gregorian") != format.npos)
      timeFormat = "Gregorian";

   std::string expRange;
   try
   {
      TimeSystemConverter::Instance()->ValidateTimeFormat(format, value, false);
   }
   catch (BaseException &)
   {
      if (timeFormat == "Gregorian")
      {
         expRange = "DD Mon YYYY HH:MM:SS.sss";
      }
      else
      {
         expRange = "Real Number";
      }
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), value.c_str(), field.c_str(), "",
          expRange.c_str());
      SetErrorFlag();
      return false;
   }
   if (checkRange)
   {
      try
      {
         TimeSystemConverter::Instance()->ValidateTimeFormat(format, value, true);
      }
      catch (BaseException &)
      {
         std::string expRange = "";
         if (timeFormat == "Gregorian")
         {
            expRange = "\"" + DateUtil::EARLIEST_VALID_GREGORIAN;
            expRange += "\" to \"" + DateUtil::LATEST_VALID_GREGORIAN;
            expRange += "\"";
         }
         else
         {
            expRange = DateUtil::EARLIEST_VALID_MJD;
            expRange += " <= Real Number <= " + DateUtil::LATEST_VALID_MJD;
         }
         MessageInterface::PopupMessage
            (Gmat::ERROR_, mMsgFormat.c_str(), value.c_str(), field.c_str(), "",
             expRange.c_str());
         SetErrorFlag();
         return false;
       }
   }
   return true;
}


//------------------------------------------------------------------------------
// wxArrayString ToWxArrayString(const StringArray &array)
//------------------------------------------------------------------------------
/**
 * Converts std::string array to wxString array.
 */
//------------------------------------------------------------------------------
wxArrayString UserInputValidator::ToWxArrayString(const StringArray &array)
{
   wxArrayString newArray;
   for (UnsignedInt i=0; i<array.size(); i++)
      newArray.Add(array[i].c_str());
   
   return newArray;
}


//------------------------------------------------------------------------------
// wxString ToWxString(const wxArrayString &names)
//------------------------------------------------------------------------------
/**
 * Converts wxString array to wxString separated by comma.
 */
//------------------------------------------------------------------------------
wxString UserInputValidator::ToWxString(const wxArrayString &names)
{
   wxString str = "";
   wxString delimiter = ", ";
   if (names.Count() > 0)
   {
      str = names[0];
      
      for (unsigned int i=1; i<names.Count(); i++)
         str = str + delimiter + names[i];
   }
   
   return str;
}


//------------------------------------------------------------------------------
/// wxString ToWxString(const std::string &stdStr)
//------------------------------------------------------------------------------
wxString UserInputValidator::ToWxString(const std::string &stdStr)
{
   wxString wxStr = stdStr.c_str();
   return wxStr;
}


//------------------------------------------------------------------------------
// wxString ToWxString(Real realVal)
//------------------------------------------------------------------------------
/**
 * Formats real value using default DATA_PRECESION defined in GmatGlobal.
 */
//------------------------------------------------------------------------------
wxString UserInputValidator::ToWxString(Real realVal)
{
   std::string strVal =
      GmatStringUtil::ToString(realVal, GmatGlobal::DATA_PRECISION, false, 1);
   return ToWxString(strVal);
}


//------------------------------------------------------------------------------
// wxString ToWxString(Integer intVal)
//------------------------------------------------------------------------------
/**
 * Formats integer value.
 */
//------------------------------------------------------------------------------
wxString UserInputValidator::ToWxString(Integer intVal)
{
   std::string strVal =
      GmatStringUtil::ToString(intVal, 1);
   return ToWxString(strVal);
}


//------------------------------------------------------------------------------
// Real ToReal(const wxString &strVal)
//------------------------------------------------------------------------------
/**
 * Converts wxString value to Real value. If input string is not a number
 * it returns GmatRealConstants::REAL_UNDEFINED.
 */
//------------------------------------------------------------------------------
Real UserInputValidator::ToReal(const wxString &wxStr)
{
   Real realVal;
   if (GmatStringUtil::ToReal(wxStr.c_str(), realVal))
   {
      return realVal;
   }
   else
   {
      // I want to return NaN if string is not a valid real number.
      // Does this code only works on Windows? Commented out (LOJ: 2012.06.19)
      //unsigned long long rawNan = 0x7ff0000000000000;
      //Real nanVal = *( Real* )&rawNan;
      //return nanVal;
      return GmatRealConstants::REAL_UNDEFINED;
   }
}


//------------------------------------------------------------------------------
// Integer ToInteger(const wxString &strVal)
//------------------------------------------------------------------------------
/**
 * Converts wxString value to integer value. If input string is not a number
 * it returns GmatIntegerConstants::INTEGER_UNDEFINED (-987654321)
 */
//------------------------------------------------------------------------------
Integer UserInputValidator::ToInteger(const wxString &wxStr)
{
   Integer intVal;
   if (GmatStringUtil::ToInteger(wxStr.c_str(), intVal, false, false))
      return intVal;
   else
      return GmatIntegerConstants::INTEGER_UNDEFINED;
}


//------------------------------------------------------------------------------
// void SetErrorFlag()
//------------------------------------------------------------------------------
/*
 * Sets canClose flag to window passed to this class
 */
//------------------------------------------------------------------------------
void UserInputValidator::SetErrorFlag()
{
   mIsInputValid = false;
   
   if (mWindow == NULL)
      return;
   
   if (mWindow->IsKindOf(CLASSINFO(wxPanel)))
      ((GmatPanel*)mWindow)->SetCanClose(false);
   else if (mWindow->IsKindOf(CLASSINFO(wxDialog)))
      ((GmatDialog*)mWindow)->SetCanClose(false);
   
}

