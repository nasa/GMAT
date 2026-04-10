//$Id$
//------------------------------------------------------------------------------
//                                  Set
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class implementation for the Set command
 */
//------------------------------------------------------------------------------


#ifndef Set_hpp
#define Set_hpp

#include "DataInterfaceDefs.hpp"
#include "DataInterface.hpp"
#include "GmatCommand.hpp"
#include <fstream>

/**
 * Retrieves data from a DataInterface and set it on a target object.
 */
class DATAINTERFACE_API Set : public GmatCommand
{
public:
   Set();
   virtual ~Set();
   Set(const Set& sv);
   Set&                operator=(const Set&);
   
   // inherited from GmatCommand
   virtual bool         InterpretAction();
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // inherited from GmatBase
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);

   virtual bool         Initialize();
   
   virtual GmatBase*    Clone() const;
   
//   // Parameter accessors
//   virtual std::string  GetParameterText(const Integer id) const;
//   virtual Integer      GetParameterID(const std::string &str) const;
//   virtual Gmat::ParameterType
//                        GetParameterType(const Integer id) const;
//   virtual std::string  GetParameterTypeString(const Integer id) const;
//
//   virtual bool         SetStringParameter(const Integer id,
//                                           const std::string &value);
//   virtual bool         SetStringParameter(const std::string &label,
//                                           const std::string &value);
//   virtual std::string  GetStringParameter(const Integer id,
//                                           const Integer index) const;
//   virtual std::string  GetStringParameter(const std::string &label,
//                                           const Integer index) const;
//   virtual const StringArray&
//                        GetStringArrayParameter(const Integer id) const;

   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
      
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   DEFAULT_TO_NO_CLONES

protected:
   /// Name of the object that receives the data
   std::string          targetName;
   /// The target object
   GmatBase*            target;
   /// Name of the interface object that accesses the data
   std::string          interfaceName;
   /// The DataInterface that grabs the data for the target
   DataInterface*       theInterface;
   /// Flag indicating if all (applicable) data should be read
   bool                 loadAll;
   /// The list of data elements requested, used if loadAll is false
   StringArray          selections;
   
   bool                 SetTargetParameterData(
                                             DataReader::readerDataType theType,
                                             const std::string &forField);
   Rvector6             ConvertToTargetCoordinateSystem(const std::string& from,
                                             Rvector6& fromState);
   GmatEpoch            ConvertToSystemTime(const std::string& from,
                                             GmatEpoch fromTime);
   void                 CheckForOptions(const std::string options);

//   // Parameter IDs
//   enum
//   {
//      OBJECT_NAMES = GmatCommandParamCount,
//      SetParamCount
//   };
//
//   static const std::string
//      PARAMETER_TEXT[SetParamCount - GmatCommandParamCount];
//   static const Gmat::ParameterType
//      PARAMETER_TYPE[SetParamCount - GmatCommandParamCount];
};

#endif // Set_hpp
