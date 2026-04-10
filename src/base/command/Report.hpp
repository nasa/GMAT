//$Id$
//------------------------------------------------------------------------------
//                            Report
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Purchase
// order MOMS418823
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/07/06
//
/**
 *  Class definition for the Report command.
 */
//------------------------------------------------------------------------------
 
#ifndef Report_hpp
#define Report_hpp

#include "GmatCommand.hpp"
#include "ReportFile.hpp"
#include "Parameter.hpp"
#include "ElementWrapper.hpp"

/**
 * The Report command is used to write data to a ReportFile at specific times.
 * 
 * The ReportFile object is a subscriber used to generate data in an ASCII file.
 * Parameters added directly to the ReportFile are written whenever the 
 * publisher is fed data.  Parmeters that need to be seen only at specific times 
 * in a script are published using this command.
 * 
 * @note The Publisher does not currently pass text strings correctly.  The
 *       Report command is set to directly write data to the ReportFile until
 *       this defect is corrected.
 */
class GMAT_API Report : public GmatCommand
{
public:
   Report();
   virtual ~Report();
   Report(const Report &rep);
   Report& operator=(const Report &rep);
   
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
   
   // ElementWrapper accessor methods
   virtual const StringArray& 
                        GetWrapperObjectNameArray(bool completeSet = false);
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                                          const std::string &withName);
   virtual void         ClearWrappers();
   
   // Object accessor methods
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   
   // Command methods
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   bool                 Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // Generating string method
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   DEFAULT_TO_NO_CLONES

protected:
   /// Name of the subscriber
   std::string                rfName;
   /// The ReportFile subscriber that receives the data
   ReportFile                 *reporter;
   /// The ID for the subscriber
   Integer                    reportID;
   /// Array of parameter names
   StringArray                parmNames;
   /// Array of actual parameter names including index
   StringArray                actualParmNames;
   /// Number of parameters
   Integer                    numParams;
   /// Array of parameters that get written to the report
   std::vector<Parameter*>    parms;
   /// Flag indicating whether the header data has been written
   bool                       needsHeaders;
   /// Flag to detect if the command executed once
   bool                       hasExecuted;
   /// ElementWraper pointers of parameters
   std::vector<ElementWrapper*> parmWrappers;
   
   void WriteHeaders(std::stringstream &datastream, Integer colWidth);
   bool AddParameter(const std::string &paramName, Integer index,
                     Parameter *param = NULL);
   void DeleteParameters();
   
   enum
   {
      REPORTFILE = GmatCommandParamCount,
      ADD,
      ReportParamCount  ///< Count of the parameters for this class
   };
   
private:

   static const std::string
      PARAMETER_TEXT[ReportParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[ReportParamCount - GmatCommandParamCount];
};

#endif      // Report_hpp
