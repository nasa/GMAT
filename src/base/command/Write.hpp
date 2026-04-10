//$Id$
//------------------------------------------------------------------------------
//                            Write
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
// Author: Linda Jun, NASA/GSFC
// Created: 2016.01.22
//
/**
 *  Class definition for the Write command.
 */
//------------------------------------------------------------------------------
 
#ifndef Write_hpp
#define Write_hpp

#include "GmatCommand.hpp"
#include "Parameter.hpp"
#include "ReportFile.hpp"
#include "ElementWrapper.hpp"

/**
 * The Write command is used to write data to a GUI Message Window.
 */
class GMAT_API Write : public GmatCommand
{
public:
   Write();
   virtual ~Write();
   Write(const Write &dispCmd);
   Write& operator=(const Write &dispCmd);
   
   // Inherited from GmatCommand
   virtual bool         InterpretAction();
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                                          const std::string &withName);
   virtual const StringArray& 
                        GetWrapperObjectNameArray(bool completeSet = false);
   virtual void         ClearWrappers();
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   bool                 Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // Inherited from GmatBase
   virtual GmatBase*    Clone() const;
   
   // Parameter accessor methods
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
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
     
   // Object accessor methods
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   // Generating string method
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   DEFAULT_TO_NO_CLONES

protected:
   /// Array of parameter names
   StringArray                elementNames;
   /// Array of actual parameter names including index
   StringArray                actualElementNames;
   /// Number of parameters
   Integer                    numElements;
   /// Array of writable objects
   ObjectArray                writableObjects;
   /// Map of original writable object flags
   std::map<GmatBase*, bool>  origWritableFlagMap;
   /// ElementWraper pointers of parameters
   std::vector<ElementWrapper*> elementWrappers;
   /// write to message window is optional, defaults to true
   bool                       messageWindowOn;
   /// write to log file is optional, defaults to false
   bool                       logFileOn;
   /// ReportFile is optional and does not have a default
   std::string                reportFile;
   /// The ReportFile subscriber that receives the data
   ReportFile                 *reporter;
   bool                       hasExecuted;
   bool                       needsHeaders;
   
   bool                       writeObjectPropertyWarning;

   /// Parsing function for options
   void CheckForOptions(std::string &opts);
   bool AddElements(const std::string &paramName, Integer index);
   void DeleteElements();
   void ExecuteReport();
   void WriteHeaders(std::stringstream &datastream, Integer colWidth);
   
   bool VerifyObjects();

   enum OutputStyle
   {
      CONCISE,
      VERBOSE,
      SCRIPTABLE
   };
   /// write output style as concise, verbose, or scriptable
   OutputStyle                outputStyle;

   enum
   {
      ADD = GmatCommandParamCount,
      WriteParamCount  ///< Count of the parameters for this class
   };
   
private:

   static const std::string
      PARAMETER_TEXT[WriteParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[WriteParamCount - GmatCommandParamCount];
};

#endif      // Write_hpp
