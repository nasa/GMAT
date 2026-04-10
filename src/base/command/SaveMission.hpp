//$Id$
//------------------------------------------------------------------------------
//                                  SaveMission
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
// Author: Linda Jun (NASA/GSFC)
// Created: 2010/08/03
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class declaration for the SaveMission command
 */
//------------------------------------------------------------------------------
#ifndef SaveMission_hpp
#define SaveMission_hpp

#include "GmatCommand.hpp"

/**
 * Command used to write whole mission to ASCII files.
 */
class GMAT_API SaveMission : public GmatCommand
{
public:
   SaveMission();
   virtual ~SaveMission();
   SaveMission(const SaveMission& sv);
   SaveMission&         operator=(const SaveMission&);
   
   // inherited from GmatCommand
   virtual bool         Execute();
   
   // inherited from GmatBase
   virtual bool         InterpretAction();
   virtual GmatBase*    Clone(void) const;
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   // Parameter accessors
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   
   // for generating string
   virtual const std::string&
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   
   DEFAULT_TO_NO_CLONES

protected:
   // Parameter IDs
   enum  
   {
      FILE_NAME = GmatCommandParamCount,
      SaveMissionParamCount
   };
   
   std::string fileName;
   
   static const std::string
      PARAMETER_TEXT[SaveMissionParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[SaveMissionParamCount - GmatCommandParamCount];
};

#endif // SaveMission_hpp
