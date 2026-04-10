//$Id$
//------------------------------------------------------------------------------
//                                UpdateDynamicData
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
// Created: 2017/01/24, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Declares UpdateDynamicData methods
*/
//------------------------------------------------------------------------------
#ifndef UpdateDynamicData_hpp
#define UpdateDynamicData_hpp

#include "GmatCommand.hpp"
#include "DynamicDataDisplay.hpp"
#include "DynamicDataStruct.hpp"
#include "Parameter.hpp"
#include "ElementWrapper.hpp"
#include "TextParser.hpp"

class GMAT_API UpdateDynamicData : public GmatCommand
{
public:
   UpdateDynamicData();
   UpdateDynamicData(const UpdateDynamicData &updateData);
   UpdateDynamicData& operator=(const UpdateDynamicData &updateData);
   virtual ~UpdateDynamicData();

   virtual GmatBase*    Clone() const;

   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name, const Integer index);
   virtual bool RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName);
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);

   // Parameter accessor methods
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual const StringArray&  GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&  GetStringArrayParameter(const std::string &label) const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");

   virtual bool InterpretAction();
   virtual bool Initialize();
   virtual bool Execute();

   DEFAULT_TO_NO_CLONES

protected:
   /// Pointer to the dynamic data display this command is updating
   DynamicDataDisplay *dynamicData;
   /// Struct containing data used in dynamic data display
   std::vector<std::vector<DDD>> dynamicDataStruct;
   /// The name of the display the command is updating
   std::string dynamicTableName;
   /// Array of data names if only specific parameters are to be updated
   StringArray dataToUpdate;
   StringArray availableParams;

   enum
   {
      DYNAMIC_DATA_DISPLAY = GmatCommandParamCount,
      ADD_UPDATE_DATA,
      AVAILABLE_PARAMS,
      UpdateDynamicDataParamCount
   };

private:

   static const std::string
      PARAMETER_TEXT[UpdateDynamicDataParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[UpdateDynamicDataParamCount - GmatCommandParamCount];
};
#endif