//$Id: CallMatlabFunction.hpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                                 CallMatlabFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the CallMatlabFunction command class
 */
//------------------------------------------------------------------------------
#ifndef CallMatlabFunction_hpp
#define CallMatlabFunction_hpp

#include "matlabinterface_defs.hpp"
#include "CallFunction.hpp"
#include "MatlabInterface.hpp"

class MATLAB_API CallMatlabFunction : public CallFunction
{
public:
   CallMatlabFunction();
   virtual ~CallMatlabFunction();
   
   CallMatlabFunction(const CallMatlabFunction& cf);
   CallMatlabFunction&  operator=(const CallMatlabFunction& cf);
   
   std::string          FormEvalString();
   
   // override GmatCommand methods
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // override GmatBase methods
   virtual GmatBase*    Clone() const;

protected:
   
   MatlabInterface *matlabIf;
   
   bool ExecuteMatlabFunction();
   void SendInParam(Parameter *param, const std::string &inStr);
   void GetOutParams();
   void EvalMatlabString(const std::string &evalString);
   void ClearInputParameters();
   void ClearOutputParameters();
   void UpdateObject(GmatBase *obj, char *buffer);
   void HandleError(double **outArray, std::string &errMsg, Integer numRows,
                    Integer numCols, Integer numRowsReceived,
                    Integer numColsReceived);
};

#endif // CallMatlabFunction_hpp
