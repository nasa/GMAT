//$Id$
//------------------------------------------------------------------------------
//                                PlotCommand
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Created: 2011/09/08
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class declation for the PlotCommand.
 */
//------------------------------------------------------------------------------


#ifndef PlotCommand_hpp
#define PlotCommand_hpp

#include "GmatCommand.hpp"


/**
 * Command used to operate on plots
 */
class GMAT_API PlotCommand : public GmatCommand
{
public:
   PlotCommand(const std::string &plotTypeName);
   virtual          ~PlotCommand();
   PlotCommand(const PlotCommand &c);
   PlotCommand&      operator=(const PlotCommand &c);
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index);
   
   virtual const std::string&  
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   virtual bool         InterpretAction();
   
   virtual bool         Initialize();
   
   DEFAULT_TO_NO_CLONES    // Check this when the command is implemented

   enum
   {
      SUBSCRIBER = GmatCommandParamCount,
      PlotCommandParamCount  ///< Count of the parameters for this class
   };
   
protected:
   StringArray              plotNameList;   
   std::vector<Subscriber*> thePlotList;
   
   static const std::string
      PARAMETER_TEXT[PlotCommandParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[PlotCommandParamCount - GmatCommandParamCount];
};

#endif /* PlotCommand_hpp */
