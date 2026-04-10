//$Id$
//------------------------------------------------------------------------------
//                                  ReportFile
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
// Author: LaMont Ruley
// Created: 2003/10/23
// Modified:  2004/7/16 by Allison Greene to include copy constructor
//                         and operator=
//
/**
 * Definition for the ReportFile class.
 */
//------------------------------------------------------------------------------


#ifndef ReportFile_hpp
#define ReportFile_hpp


#include "Subscriber.hpp"
#include <fstream>

#include "Parameter.hpp"
#include <map>
#include <iostream>
#include <iomanip>

class GMAT_API ReportFile : public Subscriber
{
public:
   ReportFile(const std::string &typeName, const std::string &name,
              const std::string &fname = "", 
              Parameter *firstParam = NULL);
   
   virtual ~ReportFile(void);
   
   ReportFile(const ReportFile &);
   ReportFile& operator=(const ReportFile&);
   
   // methods for this class
   std::string          GetDefaultFileName();
   std::string          GetFullPathFileName();
   Integer              GetNumParameters();
   bool                 AddParameter(const std::string &paramName, Integer index);
   bool                 WriteData(WrapperArray dataArray, bool parsable = false);
   
   // methods inherited from GmatBase
   virtual bool         Initialize();
   
   virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
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
   
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);

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
   /// Name of the output path
   std::string          outputPath;
   /// Name of the report file
   std::string          fileName;
   /// Default file name of the report file when it is not set
   std::string          defFileName;
   /// Full file name with path
   std::string          fullPathFileName;
   /// Precision for output of real data
   Integer              precision;  
   /// Width of column
   Integer              columnWidth;   
   /// Wether to append the existing file or first clear it
   bool                 appendToExistingFile;
   /// Write the headers on the top of the column
   bool                 writeHeaders;
   /// Flag used to turn headers back on
   bool                 headerReset;
   /// Left justify
   bool                 leftJustify;
   /// Fill right field with 0
   bool                 zeroFill;
   /// Using default file name indicator
   bool                 usingDefaultFileName;
   /// fixed width columns
   bool                 fixedWidth;
   /// Flag for writing final solver data
   bool                 writeFinalSolverData;
   std::ofstream::pos_type finalSolverDataPosition;
   /// delimiter
   char                 delimiter;
   
   /// output data stream
   std::ofstream        dstream;
   std::vector<Parameter*> mParams;
   
   Integer              mNumParams;
   StringArray          mParamNames;
   StringArray          mAllRefObjectNames;
   Integer              lastUsedProvider;
   Real                 mLastReportTime;
   bool                 usedByReport;
   bool                 calledByReport;
   bool                 initial;
   bool                 initialFromReport;
   
   virtual bool         OpenReportFile();
   void                 ClearParameters();
   void                 WriteHeaders();
   Integer              WriteMatrix(StringArray *output, Integer param,
                                    const Rmatrix &rmat, UnsignedInt &maxRow,
                                    Integer defWidth);
   
   // methods inherited from Subscriber
   virtual bool         Distribute(Integer len);
   virtual bool         Distribute(const Real * dat, Integer len);
   
   virtual bool         IsNotANumber(Real rval);

   enum
   {
      FILENAME = SubscriberParamCount,
      FULLPATH_FILENAME,
      PRECISION,
      ADD,
      WRITE_HEADERS,
      LEFT_JUSTIFY,
      ZERO_FILL,
      FIXED_WIDTH,
      DELIMITER,
      COL_WIDTH,
      WRITE_REPORT,
      APPEND_TO_EXISTING_FILE,
      ReportFileParamCount  ///< Count of the parameters for this class
   };

private:
      
   static const std::string
      PARAMETER_TEXT[ReportFileParamCount - SubscriberParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[ReportFileParamCount - SubscriberParamCount];
   
};


#endif
