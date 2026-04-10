//------------------------------------------------------------------------------
//                           BeginFileThrust
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
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * Toggle command used to tell GMAT to use a thrust data file
 */
//------------------------------------------------------------------------------


#ifndef BeginFileThrust_hpp
#define BeginFileThrust_hpp

#include "ThrustFileDefs.hpp"
#include "GmatCommand.hpp"
#include "FileThrust.hpp"
#include "ThrustHistoryFile.hpp"
#include "PhysicalModel.hpp"
#include "Spacecraft.hpp"


class BeginFileThrust: public GmatCommand
{
public:
   BeginFileThrust(const std::string &name = "");
   virtual ~BeginFileThrust();
   BeginFileThrust(const BeginFileThrust& bft);
   BeginFileThrust& operator=(const BeginFileThrust& bft);

   virtual GmatBase* Clone() const;

   virtual bool         TakeAction(const std::string &action,
                           const std::string &actionData = "");
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                        const std::string &name);

   virtual GmatBase*    GetGmatObject(const UnsignedInt type,
                                  const std::string objName = "");

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const std::string&
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");

   virtual void SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual bool Initialize();
   virtual bool Execute();

   DEFAULT_TO_NO_CLONES

protected:
   /// The FileThrust that is available for the force models
   FileThrust *burnForce;
   /// The vector of forces managed by the Sandbox
   std::vector<PhysicalModel*>   *transientForces;
   /// Name of the thrust history file object
   std::string thfName;
   /// The thrust history file object
   ThrustHistoryFile *thrustFile;
   /// The names of the spacecraft that get maneuvered
   StringArray                   satNames;
   /// The spacecraft that get maneuvered
   std::vector<Spacecraft *>     sats;
};

#endif /* BeginFileThrust_hpp */
