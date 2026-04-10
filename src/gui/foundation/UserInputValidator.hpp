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
/** Validates user input entered via the GUI.
 * Declares UserInputValidator class.
 */
//------------------------------------------------------------------------------

#ifndef UserInputValidator_hpp
#define UserInputValidator_hpp

#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatBase.hpp"

class UserInputValidator
{
public:
   
   // constructors
   UserInputValidator();
   ~UserInputValidator();
   
   void SetObject(GmatBase *obj);
   void SetGuiManager(GuiItemManager *manager);
   void SetWindow(wxWindow *window);
   bool IsInputValid();
   
   bool IsValidName(const wxString &name);
   bool CheckFileName(const std::string &str, const std::string &field,
                      bool onlyMsg = false);
   
   bool CheckLength(const std::string &str, const std::string &field,
                    const std::string &expLength,
                    const Integer min, const Integer max);

   bool CheckReal(Real &rvalue, const char *str,
                  const std::string &field, const std::string &expRange,
                  bool onlyMsg = false, bool checkRange = false, 
                  bool positive = false, bool zeroOk = false);
   bool CheckReal(Real &rvalue, const std::string &str,
                  const std::string &field, const std::string &expRange,
                  bool onlyMsg = false, bool checkRange = false, 
                  bool positive = false, bool zeroOk = false);
   
   bool CheckInteger(Integer &ivalue, const char *str,
                     const std::string &field, const std::string &expRange,
                     bool onlyMsg = false, bool checkRange = false,
                     bool positive = false, bool zeroOk = false);
   
   bool CheckInteger(Integer &ivalue, const std::string &str,
                     const std::string &field, const std::string &expRange,
                     bool onlyMsg = false, bool checkRange = false,
                     bool positive = false, bool zeroOk = false);
   
   bool CheckIntegerRange(Integer &ivalue, const std::string &str, 
                          const std::string &field,
                          Integer lower, Integer upper,
                          bool checkLower = true, bool checkUpper = true,
                          bool includeLower = false,
                          bool includeUpper = false);
   
   bool CheckVariable(const char *varName, ObjectTypeArray ownerTypes,
                      const std::string &field, const std::string &expRange,
                      bool allowNumber = true, bool allowNonPlottable = false,
                      bool allowObjectProperty = false, bool allowWholeArray = false);
   
   bool CheckVariable(const std::string &varName, ObjectTypeArray ownerTypes,
                      const std::string &field, const std::string &expRange,
                      bool allowNumber = true, bool allowNonPlottable = false,
                      bool allowObjectProperty = false, bool allowWholeArray = false);
   
   bool CheckRealRange(const std::string &sValue, Real value, const std::string &field,
                       Real lower, Real upper,
                       bool checkLower = true, bool checkUpper = true,
                       bool includeLower = false,
                       bool includeUpper = false, bool isInteger = false);
   
   bool CheckTimeFormatAndValue(const std::string &format, const std::string& value,
                                const std::string &field,  bool checkRange = false);

   wxArrayString ToWxArrayString(const StringArray &array);
   wxString ToWxString(const wxArrayString &names);
   wxString ToWxString(const std::string &stdStr);
   wxString ToWxString(Real realVal);
   wxString ToWxString(Integer intVal);
   Real     ToReal(const wxString &wxStr);
   Integer  ToInteger(const wxString &wxStr);
   
protected:
   static std::string lessOrEq;
   static std::string lessThan;
   static std::string moreOrEq;
   static std::string moreThan;
   
   GmatBase       *mObject;
   GuiItemManager *mGuiManager;
   wxWindow       *mWindow;
   bool           mIsInputValid;;
   std::string    mObjectName;
   std::string    mMsgFormat;
   
   void SetErrorFlag();
   
};

#endif // UserInputValidator_hpp
