//$Id: MatlabInterface.hpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                             MatlabInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Linda Jun/GSFC
// Created: 2002/11/04
//
// Modifications:
//    2008.10.15  L. Jun - Made a singleton class
/*
 * Declares MatlabInterface functions. It is a singleton class -
 * only one instance of this class can be created.
 */
//------------------------------------------------------------------------------
#ifndef MatlabInterface_hpp
#define MatlabInterface_hpp

#include "matlabinterface_defs.hpp"
#include "Interface.hpp"

// Clear a build error with Visual Studio 2010
#ifdef _CHAR16T
#define CHAR16_T
#endif

#include "engine.h"           // for Matlab Engine

#include <string>
#include <map>

class MATLAB_API MatlabInterface : public Interface
{

public:

   ///@note GmatGlobal uses the same enum
   enum MatlabMode
   {
      SINGLE_USE = 30,
      SHARED,
      NO_MATLAB,  // MATLAB is not installed
   };
   
   static MatlabInterface* Instance();
   
   Integer       Open(const std::string &name = "");
   Integer       Close(const std::string &name = "");

   void          SetCallingObjectName(const std::string &calledFrom);
   std::string   GetCallingObjectName();
   
   Integer       PutRealArray(const std::string &matlabVarName, Integer numRows,
                              Integer numCols, const double *inArray);
   Integer       GetRealArray(const std::string &matlabVarName, Integer numElements,
                              double outArray[], Integer &numRowsReceived,
                              Integer &numColsReceived);
   Integer       GetString(const std::string &matlabVarName, std::string &outStr);
   Integer       EvalString(const std::string &evalString);
   Integer       SetOutputBuffer(Integer size);
   char*         GetOutputBuffer();
   bool          IsOpen(const std::string &engineName = "");
   void          RunMatlabString(const std::string &evalString); 
   void          SetMatlabMode(Integer mode);
   Integer       GetMatlabMode();
   
   // Access methods overriden from GmatBase
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

   // Parameter IDs
   enum
   {
      MATLAB_MODE = InterfaceParamCount,
      MatlabInterfaceParamCount
   };
   
   static const std::string
      PARAMETER_TEXT[MatlabInterfaceParamCount - InterfaceParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[MatlabInterfaceParamCount - InterfaceParamCount];
   
private:
   
   static MatlabInterface *instance;
   static const Integer MAX_OUT_SIZE;
   Engine *enginePtr;   
   std::map<std::string, Engine*> matlabEngineMap;
   std::string lastEngineName;
   std::string callingObjectName;
   Integer accessCount;
   Integer matlabMode;
   char *outBuffer;
   bool debugMatlabEngine;
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   MatlabInterface(const std::string &name);
   virtual ~MatlabInterface();
   MatlabInterface(const MatlabInterface &mi);
   MatlabInterface& operator=(const MatlabInterface& mi);
   
   Integer OpenEngineOnMac();
   Integer CloseEngineOnMac();
   Integer OpenSharedEngine();
   Integer CloseSharedEngine();
   Integer OpenSingleEngine(const std::string &engineName);
   Integer CloseSingleEngine(const std::string &engineName);
};

#endif // MatlabInterface_hpp
