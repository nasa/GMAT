//$Id: CallMatlabFunction.cpp 9460 2011-04-21 22:03:28Z lindajun $
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
#include "CallMatlabFunction.hpp"
#include "BeginFunction.hpp"
#include "StringTokenizer.hpp"
#include "StringUtil.hpp"          // for Replace()
#include "FileManager.hpp"         // for GetAllMatlabFunctionPaths()
#include "MatlabInterface.hpp"     // for Matlab Engine functions
#include "InterfaceException.hpp"
#include "MessageInterface.hpp"
#include <sstream>                 // for ostringstream

// If using MatlabInterface::EvalMatlabString() to send data to Matlab workspace
//#define __USE_EVAL_STRING__

//#define DEBUG_CALL_FUNCTION_PARAM
//#define DEBUG_CALL_FUNCTION_INIT
//#define DEBUG_CALL_FUNCTION_EXEC
//#define DEBUG_SEND_PARAM
//#define DEBUG_UPDATE_VAR
//#define DEBUG_UPDATE_OBJECT
//#define DEBUG_SHOW_ARRAY
//#define DEBUG_GET_OUTPUT
//#define DEBUG_GMAT_FUNCTION_INIT
//#define DEBUG_OBJECT_MAP
//#define DEBUG_GLOBAL_OBJECT_MAP
//#define DEBUG_RUN_COMPLETE
//#define DEBUG_MATLAB_EXEC
//#define DEBUG_MATLAB_EVAL
//#define DEBUG_CALLING_OBJECT

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif
#ifdef DEBUG_TRACE
#include <ctime>                 // for clock()
#endif

//---------------------------------
// static data
//---------------------------------

//------------------------------------------------------------------------------
// CallMatlabFunction::CallMatlabFunction()
//------------------------------------------------------------------------------
CallMatlabFunction::CallMatlabFunction() :
   CallFunction("CallMatlabFunction")
{
   objectTypeNames.push_back("CallMatlabFunction");
}


//------------------------------------------------------------------------------
// ~CallMatlabFunction()
//------------------------------------------------------------------------------
CallMatlabFunction::~CallMatlabFunction()
{
}


//------------------------------------------------------------------------------
// CallMatlabFunction(const CallMatlabFunction& cf) :
//------------------------------------------------------------------------------
CallMatlabFunction::CallMatlabFunction(const CallMatlabFunction& cf) :
   CallFunction (cf)
{
}


//------------------------------------------------------------------------------
// CallMatlabFunction& operator=(const CallMatlabFunction& cf)
//------------------------------------------------------------------------------
CallMatlabFunction& CallMatlabFunction::operator=(const CallMatlabFunction& cf)
{
   if (this == &cf)
      return *this;
   
   CallFunction::operator=(cf);
   
   return *this;
}


//------------------------------------------------------------------------------
// std::string FormEvalString()
//  String format
//    [Out1, Out2] = FunctionName(In1, In2, In3) or
//    [arrA(3,3)] = FunctionName(arrB(2,2)) - Implemented on 2013.01.04 by LOJ
//------------------------------------------------------------------------------
std::string CallMatlabFunction::FormEvalString()
{
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage
      ("CallMatlabFunction::FormEvalString() entered, mFunction=<%p>'%s'\n",
       mFunction, mFunction ? mFunction->GetName().c_str() : "NULL");
   #endif
   std::string evalString = "";
   
   // left hand side of evaluation string and equals (if necessary)
   if (mOutputList.size() > 1)
   {
      evalString = evalString + "[";
      //Parameter *param = (Parameter *)mOutputList[0];
      //evalString = evalString + param->GetName();
      std::string outName = mOutputNames[0];
      evalString = evalString + outName;
      //for (unsigned int i=1; i<mOutputList.size(); i++)
      for (unsigned int i=1; i<mOutputNames.size(); i++)
      {
         //param = (Parameter *)mOutputList[i];
         //evalString = evalString + ", " + param->GetName();
         outName = mOutputNames[i];
         evalString = evalString + ", " + outName;
      }
      
      evalString = evalString + "] = ";
   }
   else if (mOutputList.size() == 1)
   {
      //Parameter *param = (Parameter *)mOutputList[0];
      //evalString = "[" + evalString + param->GetName() + "]";
      std::string outName = mOutputNames[0];
      evalString = "[" + evalString + outName + "]";
      evalString = evalString + " = ";
   }
   else if (mOutputList.size() == 0)
   {
      // no left hand side
   }
   else
   {
      // need to throw an exception here
   }
   
   
   // right hand side of evaluation string
   // function name and left parenthesis
   evalString = evalString + mFunction->GetName().c_str() + "(";
   
   // input parameters
   if (mInputList.size() > 0)
   {
      //Parameter *param = (Parameter *)mInputList[0];
      //evalString = evalString + param->GetName();
      std::string inName = mInputNames[0];
      evalString = evalString + inName;
      //for (unsigned int i=1; i<mInputList.size(); i++)
      for (unsigned int i=1; i<mInputNames.size(); i++)
      {
         //param = (Parameter *)mInputList[i];
         //evalString = evalString + ", " + param->GetName();
         inName = mInputNames[i];
         evalString = evalString + ", " + inName;
      }
   }
   
   // right parenthesis and semi-colon
   evalString = evalString + ");";
   
   return evalString;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the CallMatlabFunction.
 *
 * @return clone of the CallMatlabFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CallMatlabFunction::Clone() const
{
   return (new CallMatlabFunction(*this));
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool CallMatlabFunction::Initialize()
{
   #ifdef DEBUG_CALL_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("CallMatlabFunction::Initialize() this=<%p> entered, command = '%s'\n   "
       "function type is '%s', callingFunction is '%s'\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
       callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
   #endif
   
   if (!GmatGlobal::Instance()->IsMatlabAvailable())
      throw CommandException
         ("MATLAB is not installed.  GMAT will not run if any CallMatlabFunction"
          " uses MATLAB function");
   
   bool rv = true;  // Initialization return value
   
   rv = CallFunction::Initialize();
   
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps("In CallMatlabFunction::Initialize()");
   #endif
   
   // We need to add all Matlab paths to the bottom of the path using path(path, 'newpath')
   // since FileManager::GetAllMatlabFunctionPaths() returns in top to bottom order
   if (isMatlabFunction)
   {
      // Get Matlab and FileManager instance
      matlabIf = MatlabInterface::Instance();
      FileManager *fm = FileManager::Instance();
      
      // Open Matlab engine
      if (!matlabIf->IsOpen())
         matlabIf->Open("GmatMatlab");
      
      // Change directory to working directory where GMAT application is so that
      // relative path specified in the startup file works. (LOJ: 2012.09.24)
      std::string workingDir = fm->GetCurrentWorkingDirectory();
      #ifdef DEBUG_CALL_FUNCTION_INIT
      MessageInterface::ShowMessage("   Changing working dir to '%s'\n", workingDir.c_str());
      #endif
      workingDir = "cd " + workingDir;
      matlabIf->EvalString(workingDir);
      
      #ifdef DEBUG_CALLING_OBJECT
      MessageInterface::ShowMessage
         ("   Setting calling object '%s'\n", mFunctionPathAndName.c_str());
      #endif
      
      // Set calling function name
      matlabIf->SetCallingObjectName(mFunctionPathAndName);
      
      // Get all matlab function paths from the startup file
      StringArray paths = fm->GetAllMatlabFunctionPaths();
      
      #ifdef DEBUG_CALL_FUNCTION_INIT
      MessageInterface::ShowMessage("   Found %d matlab path\n", paths.size());
      #endif
      
      // Add path to the top of the Matlab path in reverse order(loj: 2008.10.16)
      std::string pathName;
      StringArray::reverse_iterator rpos = paths.rbegin();
      std::string addPath;
      while (rpos != paths.rend())
      {
         pathName = *rpos;
         if (pathName != "")
         {
            #ifdef DEBUG_CALL_FUNCTION_INIT
            MessageInterface::ShowMessage
               ("   Adding matlab path '%s' to the top\n", pathName.c_str());
            #endif
            addPath = "path('" + pathName + "', path)";
            matlabIf->EvalString(addPath);
         }
         rpos++;
      }
   
      // Initialize input parameters only for MatlabFunction
      // Initialization of GmatFunctions are handled by FunctionManager during
      // execution
      
      // add input/output parameters
      mInputList.clear();
      GmatBase *mapObj;
      // need to initialize input parameters
      for (StringArray::iterator i = mInputNames.begin();
           i != mInputNames.end(); ++i)
      {
         if ((mapObj = FindObject(*i))  == NULL)
            throw CommandException("CallMatlabFunction command cannot find Parameter " +
                                   *i + " in script line\n   \"" +
                                   GetGeneratingString(Gmat::SCRIPTING) + "\"");
         
         #ifdef DEBUG_CALL_FUNCTION_INIT
         MessageInterface::ShowMessage("   Adding input parameter '%s'\n", i->c_str());
         #endif
         
         mInputList.push_back((Parameter *)mapObj);
      }
      
      // need to initialize output parameters
      mOutputList.clear();
      
      for (StringArray::iterator i = mOutputNames.begin();
           i != mOutputNames.end();++i)
      {
         if ((mapObj = FindObject(*i))  == NULL)
            throw CommandException("CallMatlabFunction command cannot find Parameter " + (*i));
         
         #ifdef DEBUG_CALL_FUNCTION_INIT
         MessageInterface::ShowMessage("   Adding output parameter '%s'\n", i->c_str());
         #endif
         
         mOutputList.push_back((Parameter *)mapObj);
      }
      
      if (mInputList.size() > 0)
         if (mInputList[0] == NULL)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_,
                "CallMatlabFunction::Initialize() CallMatlabFunction will not be created.\n"
                "The first parameter selected as input for the CallMatlabFunction is NULL\n");
            return false;
         }
      
      if (mOutputList.size() > 0)
         if (mOutputList[0] == NULL)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_,
                "CallMatlabFunction::Initialize() CallMatlabFunction will not be created.\n"
                "The first parameter selected as output for the CallMatlabFunction is NULL\n");
            return false;
         }
   }
   
   #ifdef DEBUG_CALL_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("CallMatlabFunction::Initialize() this=<%p> returning %d\n", this, rv);
   #endif
   return rv;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
bool CallMatlabFunction::Execute()
{
   bool status = false;
   
   if (mFunction == NULL)
      throw CommandException("Function is not defined for CallMatlabFunction");
   
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      ("=== CallMatlabFunction::Execute() entered, '%s' Count = %d\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount);
   #endif
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("CallMatlabFunction::Execute() this=<%p> entered, command = '%s'\n   "
       "function type is '%s', callingFunction is '%s'\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
       callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMaps("object maps at the start");
      #endif
   #endif
      
   if (isMatlabFunction)
   {
      #ifdef DEBUG_CALL_FUNCTION_EXEC
      MessageInterface::ShowMessage("   calling ExecuteMatlabFunction()\n");
      #endif
      
      status = ExecuteMatlabFunction();
      BuildCommandSummary(true);
      
      #ifdef DEBUG_CALL_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("CallMatlabFunction::Execute() MatlabFunction exiting with %d\n", status);
         #ifdef DEBUG_OBJECT_MAP
         ShowObjectMaps("object maps at the end");
         #endif
      #endif
   }
   
   
   #ifdef DEBUG_TRACE
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      ("=== CallMatlabFunction::Execute() exiting, '%s' Count = %d, Run Time: %f seconds\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount, (Real)(t2-t1)/CLOCKS_PER_SEC);
   #endif
   
   return status;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void CallMatlabFunction::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("CallMatlabFunction::RunComplete() entered for this=<%p> '%s',\n   "
       "FCS %sfinalized\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       fm.IsFinalized() ? "already " : "NOT ");
   #endif
   
   if (!fm.IsFinalized())
   {
      #ifdef DEBUG_RUN_COMPLETE
      MessageInterface::ShowMessage("   calling FunctionManager::Finalize()\n");
      #endif
      fm.Finalize();
   }
   
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
// bool ExecuteMatlabFunction()
//------------------------------------------------------------------------------
bool CallMatlabFunction::ExecuteMatlabFunction()
{
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage
      ("CallMatlabFunction::ExecuteMatlabFunction() entered, mFunctionName='%s', "
       "function=<%p>\n", mFunctionName.c_str(), mFunction);
   if (mFunction)
      MessageInterface::ShowMessage
         ("   functionPath='%s', functionName='%s'\n",
          mFunction->GetStringParameter("FunctionPath").c_str(),
          mFunction->GetStringParameter("FunctionName").c_str());
   #endif
   
   #ifdef DEBUG_CALLING_OBJECT
   MessageInterface::ShowMessage
      ("CallMatlabFunction::ExecuteMatlabFunction() setting calling object '%s'\n",
       mFunctionPathAndName.c_str());
   #endif
   
   matlabIf->SetCallingObjectName(mFunctionPathAndName);
   
   // open Matlab engine
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Opening Matlab Engine\n");
   #endif
   matlabIf->Open("GmatMatlab");
   
   // set format long so that we don't loose precision between string transmission
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Sending format long\n");
   #endif
   matlabIf->EvalString("format long");
   
   // Clear last errormsg
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Sending clear errormsg\n");
   #endif
   matlabIf->EvalString("clear errormsg");
   
   // add the path to the top of the path list using path('newpath', path)
   Integer pathId = mFunction->GetParameterID("FunctionPath");
   std::string thePath = mFunction->GetStringParameter(pathId);      
   
   if (thePath != "")
   {
      #ifdef DEBUG_MATLAB_EXEC
      MessageInterface::ShowMessage(".....Sending path to use\n");
      #endif
      std::string setPath = "path('" + thePath + "', path)";
      matlabIf->EvalString(setPath);
   }
   
   // send the in parameters
   for (unsigned int i=0; i<mInputList.size(); i++)
   {
      Parameter *param = (Parameter *)mInputList[i];
      #ifdef DEBUG_MATLAB_EXEC
      MessageInterface::ShowMessage
         (".....Sending input parameter <%p> '%s', %d out of %d\n", param,
          mInputNames[i].c_str(), i+1, mNumInputParams);
      #endif
      SendInParam(param, mInputNames[i]);
   }
   
   //  Eval String
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Forming evaluate string\n");
   #endif
   std::string evalString = FormEvalString();
   EvalMatlabString(evalString);
   
   // get the value for the out parameters
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Getting output parameters\n");
   #endif
   GetOutParams();
   
   return true;
   
}


//------------------------------------------------------------------------------
// void SendInParam(Parameter *param, const std::string &inStr)
//------------------------------------------------------------------------------
void CallMatlabFunction::SendInParam(Parameter *param, const std::string &inStr)
{
   #ifdef DEBUG_SEND_PARAM
   MessageInterface::ShowMessage
      ("CallMatlabFunction::SendInParam() entered, param=<%p>'%s', inStr='%s'\n",
       param, param ? param->GetName().c_str() : "NULL", inStr.c_str());
   #endif
   
   if (param == NULL)
   {
      MessageInterface::ShowMessage("Parameter is null");
      return;
   }
   
   std::string paramName = param->GetName();
   
   #ifdef DEBUG_SEND_PARAM
   MessageInterface::ShowMessage
      ("Parameter name=%s, type=%s\n", paramName.c_str(),
       param->GetTypeName().c_str());
   #endif
   
   if (param->GetTypeName() == "Array")
   {
      Array *array = (Array *)param;
      int numRows = array->GetIntegerParameter("NumRows");
      int numCols = array->GetIntegerParameter("NumCols");
      Rmatrix rmatrix = array->GetRmatrix();
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage
         ("numRows=%d, numCols=%d\nrmatrix=\n%s\n", numRows, numCols,
          rmatrix.ToString(16).c_str());
      #endif
      
      // If sending array element, use EvalString (GMT-3324 fix, LOJ:2013.01.04)
      if (GmatStringUtil::IsParenPartOfArray(inStr))
      {
         std::string name, rowStr, colStr;
         Integer row, col;
         GmatStringUtil::GetArrayIndex(inStr, rowStr, colStr, row, col, name);
         
         #ifdef DEBUG_SEND_PARAM
         MessageInterface::ShowMessage
            ("'%s' is an array element, rows=%d, cols=%d\n", inStr.c_str(), row, col);
         #endif
         
         if (row == -1 || col == -1)
         {
            std::string msg =
               "Input array element index is invalid, it must be greater than 0 in " + inStr;
            #ifdef DEBUG_SEND_PARAM
            MessageInterface::ShowMessage("CallMatlabFunction::SendInParam() " + msg + "\n");
            #endif
            throw CommandException(msg);
         }
      }
      
      
      //----------------------------------------------------
      #ifdef __USE_EVAL_STRING__
      //----------------------------------------------------
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage("===> Using EvalString\n");
      #endif
      std::ostringstream os;
      os.precision(18);
      
      for (int j=0; j<numRows; j++)
      {
         os << "[";
         for (int k=0; k<numCols; k++)
            os << rmatrix(j, k) << ",";
         os << "], \n";
      }
      
      std::string inParamString = paramName + " = [" + os.str() + "];";
      EvalMatlabString(inParamString);
      
      //----------------------------------------------------
      #else
      //----------------------------------------------------
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage("===> Not using EvalString\n");
      #endif
      
      const Real *realArray = rmatrix.GetDataVector();
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage("realArray = \n");
      for (int i = 0; i < numRows; i++)
         for (int j = 0; j < numCols; j++)
         {
            MessageInterface::ShowMessage("%f ", realArray[i*numCols+j]);
            if (j == numCols-1) MessageInterface::ShowMessage("\n");
         }
      MessageInterface::ShowMessage
         (".....Putting RealArray data <%p> to Matlab workspace\n", realArray);
      #endif
      matlabIf->PutRealArray(paramName, numRows, numCols, realArray);
      
      //----------------------------------------------------
      #endif
      //----------------------------------------------------
   }
   else if (param->GetTypeName() == "Variable")
   {
      //----------------------------------------------------
      #ifndef __USE_EVAL_STRING__
      //----------------------------------------------------
      std::ostringstream os;
      os.precision(18);
      os << param->EvaluateReal();
      
      std::string inParamString = paramName + " = " + os.str() + ";";
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage
         ("Sent string %s to matlab\n", inParamString.c_str());
      #endif
      
      EvalMatlabString(inParamString);
      
      //----------------------------------------------------
      #else
      //----------------------------------------------------
      static Real *realVal = new Real[1];
      realVal[0] = param->EvaluateReal();
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage
         (".....Putting Real data %p to Matlab workspace\n", realVal);
      #endif
      
      matlabIf->PutRealArray(paramName, 1, 1, (const Real*)realVal);
      
      //----------------------------------------------------
      #endif
      //----------------------------------------------------
   }
   else if (param->GetTypeName() == "String")
   {
      StringVar *stringVar = (StringVar *)param;
      std::string inParamString = paramName + " = '" +
         stringVar->GetString() +"';";
      
      EvalMatlabString(inParamString);
   }
   else // Other Parameters or whole object
   {
      std::string inParamString;
      bool paramAllowed = true;
      if (param->IsOfType(Gmat::PARAMETER))
      {
         Gmat::ParameterType returnType = param->GetReturnType();
         #ifdef DEBUG_SEND_PARAM
         MessageInterface::ShowMessage("Parameter return type :%d\n", returnType);
         #endif
         if (returnType == Gmat::REAL_TYPE)
         {
            // Evaluate Parameter first then get String value
            param->EvaluateReal();
            inParamString = paramName + " = " + param->ToString();
         }
         else if (returnType == Gmat::STRING_TYPE)
         {
            // Evaluate Parameter first then get String value
            param->EvaluateString();
            std::string sval = param->ToString();
            #ifdef DEBUG_SEND_PARAM
            MessageInterface::ShowMessage("   sval=<%s>\n", sval.c_str());
            #endif
            // If it is a form of a vector such as [1 0 0 0], do not put single quotes
            // For now throw an exception (GMT-3526)
            if (GmatStringUtil::IsEnclosedWithBrackets(sval))
            {
               //inParamString = paramName + " = " + sval;
               paramAllowed = false;
            }
            else
               inParamString = paramName + " = " + "'" + sval + "'";
         }
         else
         {
            paramAllowed = false;
         }
         
         if (!paramAllowed)
         {
            std::string msg =
               "Currently sending non-REAL or non-STRING type of Parameter is not allowed in " +
               GetGeneratingString(Gmat::NO_COMMENTS);
            #ifdef DEBUG_SEND_PARAM
            MessageInterface::ShowMessage("CallMatlabFunction::SendInParam() " + msg + "\n");
            #endif
            throw CommandException(msg);
         }
      }
      else
      {
         #ifdef DEBUG_SEND_PARAM
         MessageInterface::ShowMessage
            ("It is not a Parameter, so calling GetGeneratingString()\n");
         #endif
         
         if (param->GetTypeName() == "Spacecraft")
            param->TakeAction("UpdateEpoch");
         
         inParamString = param->GetGeneratingString(Gmat::MATLAB_STRUCT);
      }
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage
         ("Generated param string :\n%s\n", inParamString.c_str());
      #endif
      
      EvalMatlabString(inParamString);
   }
}


//------------------------------------------------------------------------------
// void GetOutParams()
//------------------------------------------------------------------------------
void CallMatlabFunction::GetOutParams()
{
   #ifdef DEBUG_GET_OUTPUT
   MessageInterface::ShowMessage("CallMatlabFunction::GetOutParams() entered\n");
   #endif
   
   try
   {
      for (unsigned int i=0; i<mOutputList.size(); i++)
      {
         Parameter *outParam = (Parameter *)mOutputList[i];
         std::string varName = outParam->GetName();
         std::string outStr = mOutputNames[i];
         
         #ifdef DEBUG_GET_OUTPUT
         MessageInterface::ShowMessage
            ("CallMatlabFunction::GetOutParams() OutParamType=%s, name=%s, outStr=%s\n",
             outParam->GetTypeName().c_str(), varName.c_str(), outStr.c_str());
         #endif
         
         if (outParam->GetTypeName() == "Array")
         {
            Array *array = (Array *)outParam;
            int numRows = array->GetIntegerParameter("NumRows");
            int numCols = array->GetIntegerParameter("NumCols");
            int totalCells = numRows * numCols;            
            double *outArray = new double[totalCells];
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (outArray, "outArray", "CallMatlabFunction::GetOutParams()",
                "*outArray = new double[totalCells]");
            #endif
            
            #ifdef DEBUG_GET_OUTPUT
            MessageInterface::ShowMessage
               ("numRows=%d, numCols=%d, totalCells=%d\n", numRows, numCols, totalCells);
            MessageInterface::ShowMessage
               ("CallMatlabFunction::GetOutParams() calling MI::GetRealArray()\n");
            #endif
            
            Integer numRowsReceived, numColsReceived;
            Integer numReceived = matlabIf->GetRealArray(varName, totalCells, outArray,
                                                         numRowsReceived, numColsReceived);
            
            #ifdef DEBUG_GET_OUTPUT
            MessageInterface::ShowMessage
               ("numRowsReceived=%d, numColsReceived=%d\n", numRowsReceived, numColsReceived);
            #endif
            
            // Check if correct number of elements received
            if (numReceived == 0)
            {
               std::string errMsg = "CallMatlabFunction cannot assign result to ";
               errMsg = errMsg + varName + ": dimension mismatch";
               HandleError(&outArray, errMsg, numRows, numCols, numRowsReceived, numColsReceived);
            }
            
            // Check if correct number of rows and columns received
            // number of elements received cannot be more than original size
            if (numRowsReceived > numRows || numColsReceived > numCols)
            {
               std::string errMsg = "CallMatlabFunction cannot assign result to ";
               errMsg = errMsg + varName + ": dimension mismatch";
               HandleError(&outArray, errMsg, numRows, numCols, numRowsReceived, numColsReceived);
            }
            
            // Check if whole array is assigned to a variable
            if (numReceived == 1 && (numRows != 1 && numCols != 1))
            {
               // If output is not an array element, throw an exception
               if (!GmatStringUtil::IsSimpleArrayElement(outStr))
               {
                  std::string errMsg = "CallMatlabFunction cannot assign result to ";
                  errMsg = errMsg + varName + ": dimension mismatch";
                  HandleError(&outArray, errMsg, numRows, numCols, numRowsReceived, numColsReceived);
               }
            }
            
            // Check if assigning whole array to array of different dimension
            if (numRowsReceived != numRows || numColsReceived != numCols)
            {
               // If output is an array element, throw an exception
               if (!GmatStringUtil::IsSimpleArrayElement(outStr))
               {
                  std::string errMsg = "CallMatlabFunction cannot assign result to ";
                  errMsg = errMsg + varName + ": dimension mismatch";
                  HandleError(&outArray, errMsg, numRows, numCols, numRowsReceived, numColsReceived);
               }
            }
            
            // Assign results
            #ifdef DEBUG_GET_OUTPUT
            MessageInterface::ShowMessage("outArray  = \n");
            for (Integer i=0; i < totalCells; i++)
               MessageInterface::ShowMessage("   %.12f\n", outArray[i]);
            #endif
            
            // Since MATLAB stores array in column major order, it needs to take transpose
            // before putting array to matrix.
            Rmatrix colMajorMat(numCols, numRows);
            for (int i = 0; i < numCols; i++)
               for (int j = 0; j < numRows; j++)
                  colMajorMat(i,j) = outArray[i*numRows+j];
            
            #ifdef DEBUG_SHOW_ARRAY
            MessageInterface::ShowMessage("colMajorMat=\n%s\n", colMajorMat.ToString(16).c_str());
            #endif
            
            // Create row major matrix
            Rmatrix rowMajorMat(numRows, numCols);
            rowMajorMat = colMajorMat.Transpose();
            
            #ifdef DEBUG_SHOW_ARRAY
            MessageInterface::ShowMessage("rowMajorMat=\n%s\n", rowMajorMat.ToString(16).c_str());
            #endif
            
            // Handle array element here (GMT-3324 fix, LOJ:2013.01.04)
            if (GmatStringUtil::IsParenPartOfArray(outStr))
            {
               std::string name, rowStr, colStr;
               Integer row, col;
               GmatStringUtil::GetArrayIndex(outStr, rowStr, colStr, row, col, name);
               
               #ifdef DEBUG_GET_OUTPUT
               MessageInterface::ShowMessage
                  ("'%s' is an array element, rows=%d, cols=%d\n", outStr.c_str(), row, col);
               #endif
               
               if (row == -1 || col == -1)
               {
                  std::string errMsg =
                     "Output array element index is invalid, it must be greater than 0 in " + outStr;
                  HandleError(&outArray, errMsg, numRows, numCols, numRowsReceived, numColsReceived);
               }
               else
               {
                  // Set array element
                  array->SetRealParameter("SingleValue", rowMajorMat(row, col), row, col);
               }
            }
            else
            {
               // Assign rowMajorMat to array
               array->SetRmatrixParameter("RmatValue", rowMajorMat);
            }
            
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (outArray, "outArray", "CallMatlabFunction::GetOutParams()",
                "deletinig outArray");
            #endif
            delete [] outArray;
            
         }
         else if (outParam->GetTypeName() == "Variable")
         {
            double *outArray = new double[1];
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (outArray, "outArray", "CallMatlabFunction::GetOutParams()",
                "*outArray = new double[1]");
            #endif
            
            #ifdef DEBUG_UPDATE_VAR
            MessageInterface::ShowMessage
               (".....Calling matlabIf->GetRealArray()\n");
            #endif
            Integer numRowsReceived, numColsReceived;
            Integer numReceived = matlabIf->GetRealArray(varName, 1, outArray,
                                                         numRowsReceived, numColsReceived);
            if (numReceived == 1)
            {
               outParam->SetReal(outArray[0]);
               std::ostringstream ss;
               ss.precision(18);
               ss << outArray[0];
               outParam->SetStringParameter("Expression", ss.str());
               
               #ifdef DEBUG_UPDATE_VAR
               MessageInterface::ShowMessage
                  ("The EvaluateReal is %f\n",  outParam->EvaluateReal());
               MessageInterface::ShowMessage
                  ("The GetReal is %f\n", outParam->GetReal());
               #endif
            }
            
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (outArray, "outArray", "CallMatlabFunction::GetOutParams()",
                "deletinig outArray");
            #endif
            delete [] outArray;
            
            if (numReceived != 1)
            {
               CommandException ce;
               ce.SetDetails("CallMatlabFunction cannot assign result to \"%s\": type mismatch",
                             varName.c_str());
               throw ce;
            }
         }
         else if (outParam->GetTypeName() == "String")
         {
            //----------------------------------------------
            #ifdef __USE_EVAL_STRING__
            //----------------------------------------------            
            #ifdef DEBUG_UPDATE_VAR
            MessageInterface::ShowMessage
               (".....Calling matlabIf->SetOutputBuffer()\n");
            #endif
            
            // need to output string value to buffer
            int bufSize = matlabIf->SetOutputBuffer(512);
            EvalMatlabString(varName);
            
            char *outBuffer = matlabIf->GetOutputBuffer();
            
            // get rid of "var ="
            char *ptr = strtok((char *)outBuffer, "=");
            ptr = strtok(NULL, "\n");
            
            //----------------------------------------------
            #else
            //----------------------------------------------
            #ifdef DEBUG_UPDATE_VAR
            MessageInterface::ShowMessage
               (".....Calling matlabIf->GetString('%s','%s')\n",
                varName.c_str(), outStr.c_str());
            #endif
            std::string outStr;
            if (matlabIf->GetString(varName, outStr) == 1)
               outParam->SetStringParameter("Expression", outStr);
            else
            {
               CommandException ce;
               ce.SetDetails("CallMatlabFunction cannot assign result to \"%s\": type mismatch",
                             varName.c_str());
               throw ce;
            }
            //----------------------------------------------
            #endif
            //----------------------------------------------
         }
         else // Other Parameters or whole object
         {
            #ifdef DEBUG_UPDATE_OBJECT
            MessageInterface::ShowMessage
               (".....Calling matlabIf->SetOutputBuffer() for object\n");
            #endif
            
            char buffer[8192];
            int bufSize = matlabIf->SetOutputBuffer(8192);
            EvalMatlabString(varName);
            char *outBuffer = matlabIf->GetOutputBuffer();
            
            // copy output to local buffer
            strncpy(buffer, outBuffer, bufSize);
            
            #ifdef DEBUG_UPDATE_OBJECT
            MessageInterface::ShowMessage("buffer=\n%s\n", buffer);
            #endif
            
            // assign new value to object
            UpdateObject(outParam, buffer);
         }
      }
   }
   catch (BaseException &e)
   {
      std::string moreMsg = e.GetFullMessage() + " in \n" +
         GetGeneratingString(Gmat::NO_COMMENTS);
      e.SetMessage("");
      e.SetDetails(moreMsg);
      throw;
   }
}


//------------------------------------------------------------------------------
// void EvalMatlabString(const std::string &evalString)
//------------------------------------------------------------------------------
void CallMatlabFunction::EvalMatlabString(const std::string &evalString)
{
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage
      ("CallMatlabFunction::EvalMatlabString() calling MI::RunMatlabString() with\n"
       "======================================================================\n"
       "%s\n\n", evalString.c_str());
   #endif
   
   try
   {
      matlabIf->RunMatlabString(evalString);
   }
   catch (InterfaceException &ie)
   {
      bool rethrowException = true;
      std::string errMsg = ie.GetFullMessage();
      #ifdef DEBUG_MATLAB_EVAL
      MessageInterface::ShowMessage(errMsg + "\n");
      #endif
      
      // If error from evaluating string but not from opening matlab engine or
      // not from undefined function, or error from MATLAB, close and re-initialize
      // and re-execute as user may have manually closed it.
      if (errMsg.find("engEvalString") != errMsg.npos)
      {
         rethrowException = false;
         MessageInterface::ShowMessage("Trying to close and reopen MATLAB engine...\n");
         matlabIf->Close();
         Initialize();
         Execute();
      }
      
      if (rethrowException)
         throw;
   }
}


//------------------------------------------------------------------------------
// void ClearInputParameters()
//------------------------------------------------------------------------------
void CallMatlabFunction::ClearInputParameters()
{
   mInputList.clear();
   mInputNames.clear();
   mNumInputParams = 0;
}


//------------------------------------------------------------------------------
// void ClearOutputParameters()
//------------------------------------------------------------------------------
void CallMatlabFunction::ClearOutputParameters()
{
   mOutputList.clear();
   mOutputNames.clear();
   mNumOutputParams = 0;
}


//------------------------------------------------------------------------------
// void UpdateObject(GmatBase *obj, char *buffer)
//------------------------------------------------------------------------------
void CallMatlabFunction::UpdateObject(GmatBase *obj, char *buffer)
{
   if (obj == NULL)
   {
      #ifdef DEBUG_UPDATE_OBJECT
      MessageInterface::ShowMessage
         ("CallMatlabFunction::UpdateObject() entered, obj is NULL, so just leaving\n");
      #endif
      return;
   }
   
   StringTokenizer st(buffer, "=\n");
   StringArray tokens = st.GetAllTokens();

   // Mac/Linux sometimes return wuth the MATLAB prompt at head of the string
   if (tokens[0] == ">> ")
      tokens.erase(tokens.begin());

   int numTokens = tokens.size();
   
   #ifdef DEBUG_UPDATE_OBJECT
   MessageInterface::ShowMessage
      ("CallMatlabFunction::UpdateObject() entered, obj=<%p><%s>'%s', numTokens=%d\n",
       obj, obj->GetTypeName().c_str(), obj->GetName().c_str(), numTokens);
   for (int i=0; i<numTokens; i++)
      MessageInterface::ShowMessage("tokens[%2d]=<%s>\n", i, tokens[i].c_str());
   #endif
   
   int id;
   Gmat::ParameterType type;
   std::string objName = obj->GetName();
   std::string name, value, newValue;
   
   // If answer is returned for Parameter
   if (tokens[0] == "ans ")
   {
      value = tokens[1];
      value = GmatStringUtil::Strip(value);
      value = GmatStringUtil::RemoveEnclosingString(value,"'");
      #ifdef DEBUG_UPDATE_OBJECT
      MessageInterface::ShowMessage("   Received ans = %s\n", value.c_str());
      #endif
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         Parameter *param = (Parameter*)obj;
         if (param->IsSettable())
         {
            if (param->GetReturnType() == Gmat::REAL_TYPE)
            {
               Real rval;
               if (GmatStringUtil::ToReal(value, rval))
               {
                  #ifdef DEBUG_UPDATE_OBJECT
                  MessageInterface::ShowMessage
                     ("   Setting rval: %f to object named '%s'\n", rval, objName.c_str());
                  #endif
                  param->SetReal(rval);
               }
               else
                  MessageInterface::ShowMessage
                     ("**** ERROR **** Cannot set \"" + value + "\" to Parameter '" +
                      objName + "'; The value cannot be converted to Real number\n");
            }
            else if (param->GetReturnType() == Gmat::STRING_TYPE)
            {
               
               #ifdef DEBUG_UPDATE_OBJECT
               MessageInterface::ShowMessage
                  ("   Setting string: '%s' to object named '%s'\n", value.c_str(), objName.c_str());
               #endif
               param->SetString(value);
            }
            else
            {
               MessageInterface::ShowMessage
                  ("**** ERROR **** Cannot set \"" + value + "\" to Parameter \"" +
                   objName + "\"; Setting value of non-REAL or non-STRING type has "
                   "not been implemented yet\n");
            }
         }
         else
            MessageInterface::ShowMessage
               ("**** ERROR **** Cannot set \"" + value + "\" to Parameter \"" +
                objName + "\"; The Parameter is not settable\n");
      }
      #ifdef DEBUG_UPDATE_OBJECT
      MessageInterface::ShowMessage("CallMatlabFunction::UpdateObject() leaving\n");
      #endif
      return;
   }
   
   //======================================================================
   // The following is the sample spacecraft output received from the MATLAB
   //sat2 = 
   //
   //                    X: 7123
   //         UTCGregorian: '01 Jan 2012 11:59:28.000'
   //                   Q1: 1
   //                   Q2: 0
   //                   Q3: 0
   //                   Q4: 0
   //           Quaternion: '[ 1 0 0 0]'
   //======================================================================
   
   // Handle whole object here
   StringArray names;
   StringArray values;
   std::string line, sval;
   
   // Parse object field names and values
   #ifdef DEBUG_UPDATE_OBJECT
   MessageInterface::ShowMessage("===> Parsing object fields and values\n");
   #endif
   for (int i = 2; i < numTokens; i++)
   {
      line = tokens[i];
      std::string::size_type index = line.find_first_of(":");
      name = line.substr(0, index);
      sval = line.substr(index+1);
      name = GmatStringUtil::Strip(name);
      sval = GmatStringUtil::Strip(sval);
      names.push_back(name);
      values.push_back(sval);
      #ifdef DEBUG_UPDATE_OBJECT
      MessageInterface::ShowMessage
         ("   name = %30s, value = %s\n", name.c_str(), sval.c_str());
      #endif
   }
   
   // Now set object field and its value
   #ifdef DEBUG_UPDATE_OBJECT
   MessageInterface::ShowMessage("===> Setting values to object fields\n");
   #endif
   
   std::string fieldType;
   for (unsigned int i = 0; i < names.size(); i++)
   {
      name = names[i];
      value = values[i];
      
      try
      {
         id = obj->GetParameterID(name);
         type = obj->GetParameterType(id);
         
         #ifdef DEBUG_UPDATE_OBJECT
         MessageInterface::ShowMessage
            ("   name=%s, value=%s, id=%d, type=%d\n", name.c_str(), value.c_str(), id, type);
         #endif
         
         switch (type)
         {
         case Gmat::REAL_TYPE:
         {
            Real rval;
            if (GmatStringUtil::ToReal(value, rval))
            {
               #ifdef DEBUG_UPDATE_OBJECT
               MessageInterface::ShowMessage
                  ("      REAL_TYPE: value=%s, rval=%f\n", value.c_str(), rval);
               #endif
               obj->SetRealParameter(id, rval);
            }
            else
               MessageInterface::ShowMessage
                  ("**** ERROR **** Cannot convert \"" + value + "\" to Real number\n");
            break;
         }
         case Gmat::INTEGER_TYPE:
            Integer ival;
            if (GmatStringUtil::ToInteger(value, ival))
            {
               #ifdef DEBUG_UPDATE_OBJECT
               MessageInterface::ShowMessage
                  ("      INTEGER_TYPE: value=%s, ival=%d\n", value.c_str(), ival);
               #endif
               obj->SetIntegerParameter(id, ival);
            }
            else
               MessageInterface::ShowMessage
                  ("**** ERROR **** Cannot convert \"" + value + "\" to Integer number\n");
            break;
         case Gmat::STRING_TYPE:
            fieldType = "STRING_TYPE";
         case Gmat::RVECTOR_TYPE:
            if (type != Gmat::STRING_TYPE)
               fieldType = "RVECTOR_TYPE";
         case Gmat::ENUMERATION_TYPE:
            if (type != Gmat::STRING_TYPE && type != Gmat::RVECTOR_TYPE)
               fieldType = "ENUMERATION_TYPE";
         case Gmat::OBJECT_TYPE:
            if (type != Gmat::STRING_TYPE && type != Gmat::RVECTOR_TYPE &&
                type != Gmat::ENUMERATION_TYPE)
               fieldType = "OBJECT_TYPE";
         case Gmat::FILENAME_TYPE:
            if (type != Gmat::STRING_TYPE && type != Gmat::RVECTOR_TYPE &&
                type != Gmat::ENUMERATION_TYPE && type != Gmat::OBJECT_TYPE)
               fieldType = "FILENAME_TYPE";
            
            newValue = GmatStringUtil::RemoveEnclosingString(value, "'");
            #ifdef DEBUG_UPDATE_OBJECT
            MessageInterface::ShowMessage
               ("      %s: value=<%s>, newValue=<%s>\n", fieldType.c_str(),
                value.c_str(), newValue.c_str());
            #endif
            obj->SetStringParameter(id, newValue);
            break;
         case Gmat::BOOLEAN_TYPE:
         {
            bool bval = false;
            if (value == "1")
               bval = true;
            #ifdef DEBUG_UPDATE_OBJECT
            MessageInterface::ShowMessage
               ("      BOOLEAN_TYPE: value=<%s>, bval=%d\n", value.c_str(), bval);
            #endif
            obj->SetBooleanParameter(id, bval);
            break;
         }
         default:
            throw CommandException
               ("Currently CallMatlabFunction cannot update output object for " + name + 
                " of parameter type: " + GmatBase::PARAM_TYPE_STRING[type]);
         }
      }
      catch (BaseException &e)
      {
         // Should we ignore unknown fields?
         MessageInterface::ShowMessage
            ("*** WARNING *** The field \"%s\" is not defined in the object named \"%s\", so ignoring\n",
             name.c_str(), objName.c_str());
      }
   }
   
   #ifdef DEBUG_UPDATE_OBJECT
   MessageInterface::ShowMessage
      ("CallMatlabFunction::UpdateObject() leaving, new %s =\n%s\n", obj->GetName().c_str(),
       obj->GetGeneratingString(Gmat::MATLAB_STRUCT).c_str());
   #endif
}


//------------------------------------------------------------------------------
// void HandleError(double **outArray, std::string &errMsg, ...)
//------------------------------------------------------------------------------
void CallMatlabFunction::HandleError(double **outArray, std::string &errMsg, Integer numRows,
                                     Integer numCols, Integer numRowsReceived,
                                     Integer numColsReceived)
{
   MessageInterface::ShowMessage("CallMatlabFunction::HandleError() entered\n");
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (*outArray, "outArray", "CallMatlabFunction::GetOutParams()",
       "deletinig outArray");
   #endif
   delete [] *outArray;
   #ifdef DEBUG_GET_OUTPUT
   MessageInterface::ShowMessage
      ("CallMatlabFunction::GetOutParams() error occurred in "
       "matlabIf->GetRealArray(), dimension mismatch\n   numRows=%d, "
       "numCols=%d, numRowsReceived=%d, numColsReceived=%d\n", numRows, numCols,
       numRowsReceived, numColsReceived);
   #endif
   CommandException ce(errMsg);
   throw ce;
}

