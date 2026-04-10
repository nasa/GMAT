//------------------------------------------------------------------------------
//                                  Save
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
 * Class implementation for the save command
 */
//------------------------------------------------------------------------------


#ifndef Save_hpp
#define Save_hpp

#include "SaveCommandDefs.hpp"
#include "GmatCommand.hpp"
#include <fstream>

/**
 * Command used to write objects to ASCII files.
 */
class SAVECOMMAND_API Save : public GmatCommand
{
public:
   Save();
   virtual ~Save();
   Save(const Save& sv);
   Save&                operator=(const Save&);
   
   // inherited from GmatCommand
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
   
   // Parameter accessors
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual const StringArray& 
                        GetStringArrayParameter(const Integer id) const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName);
      
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   DEFAULT_TO_NO_CLONES

protected:
   // Parameter IDs
   enum  
   {
      OBJECT_NAMES = GmatCommandParamCount,
      SaveParamCount
   };
   
   /// Name of the save file -- for now, it is objectName.objectType
   StringArray          fileNameArray;
   /// Toggle to allow multiple writes
   bool                 appendData;
   /// Toggle to tell if file was written this run
   bool                 wasWritten;
   /// Name of the objects that are written
   StringArray          objNameArray;
   /// Pointer to the objects
   ObjectArray          objArray;
   /// Toggle to show or hide emply fields
   bool                 writeVerbose;
   /// File streams used for the output
   std::ofstream        *fileArray;
   
   void  UpdateOutputFileNames(Integer index, const std::string objName);
   void  WriteObject(UnsignedInt i, GmatBase *o);
   
   static const std::string
      PARAMETER_TEXT[SaveParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[SaveParamCount - GmatCommandParamCount];
};

#endif // Save_hpp
