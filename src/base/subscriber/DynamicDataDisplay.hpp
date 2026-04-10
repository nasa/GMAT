//$Id$
//------------------------------------------------------------------------------
//                                DynamicDataDisplay
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Created: 2018/01/24, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Declares DynamicDataDisplay methods
*/
//------------------------------------------------------------------------------
#ifndef DynamicDataDisplay_hpp
#define DynamicDataDisplay_hpp

#include "Subscriber.hpp"
#include "SubscriberException.hpp"
#include "Parameter.hpp"
#include "PlotInterface.hpp"
#include "StringUtil.hpp"
#include "RgbColor.hpp"
#include "ColorDatabase.hpp"
#include "DynamicDataStruct.hpp"
#include "TextParser.hpp"

class GMAT_API DynamicDataDisplay : public Subscriber
{
public:
   DynamicDataDisplay(const std::string &name);
   DynamicDataDisplay(const DynamicDataDisplay &orig);
   DynamicDataDisplay& operator=(const DynamicDataDisplay& orig);
   virtual ~DynamicDataDisplay(void);

   // Methods for this class
   bool UpdateData(StringArray paramsToUpdate);
   std::vector<std::vector<DDD>> GetDynamicDataStruct();
   void SetParamSettings(std::vector<std::vector<DDD>> newSettings);

   // Methods inherited from GmatBase
   virtual bool         Initialize();
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);

   virtual bool         SetName(const std::string &who,
                                const std::string &inputOldName);
   virtual bool         RenameRefObject(const UnsignedInt type,
      const std::string &oldName,
      const std::string &newName);

   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   virtual bool         IsSquareBracketAllowedInSetting(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);

   // methods for setting up the items to subscribe
   virtual const StringArray&
                        GetWrapperObjectNameArray(bool completeSet = false);

protected:
   virtual void         WriteStringArrayValue(Gmat::WriteMode mode,
                                              std::string &prefix,
                                              Integer id, bool writeQuotes,
                                              std::stringstream &stream);

private:
   bool AddParameter(const std::string &paramName, Integer index);
   bool SetParameterTextColor(const std::string &scriptString, Integer index);
   bool SetRowTextColors(const std::string &scriptString, Integer index);
   bool SetParameterBackgroundColor(const std::string &scriptString,
                                    Integer index);
   bool SetParameterWarnBounds(const std::string &scriptString, Integer index);
   bool SetParameterCritBounds(const std::string &scriptString, Integer index);
   //bool RemoveParameter(const std::string &paramName);

   /// Matrix of structs holding the parameter data for the display
   std::vector<std::vector<DDD>> displayData;
   /// The previous name of the current display
   std::string oldName;
   /// The total number of parameters being displayed
   Integer numParams;
   /// String array used to send reference object names when requested
   StringArray refObjectNames;
   /// Integer used to store which row in the display is currently being
   /// modified
   Integer currentRowIndex;
   /// Integer used to store which column in the display is currently being
   /// modified
   Integer currentColIndex;
   /// The total number of rows in the display
   Integer maxRowCount;
   /// The total number of columns in the display
   Integer maxColCount;
   /// The current parameter having its details updated
   std::string paramToUpdate;
   /// The color used when a parameter crosses a warning bound
   UnsignedInt warnTextColor;
   /// The color used when a parameter crosses a critical bound
   UnsignedInt critTextColor;
   /// A real value storing the Gmat representation of infinity
   Real inf;
   /// Flag indicating that an error message has already posted
   bool errorAlreadyShown;

public:
   enum
   {
      ADD_PARAMETERS = SubscriberParamCount,
      PARAM_TEXT_COLOR,
      ROW_TEXT_COLORS,
      PARAM_BACKGROUND_COLOR,
      WARN_BOUNDS,
      CRIT_BOUNDS,
      WARN_COLOR,
      CRIT_COLOR,
      DynamicDataDisplayParamCount
   };

   static const Gmat::ParameterType
      PARAMETER_TYPE[DynamicDataDisplayParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[DynamicDataDisplayParamCount - SubscriberParamCount];
};
#endif
