//$Id: MatlabInterface.cpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                               MatlabInterface
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
// Author: Linda Jun (NASA/GSFC)
// Created: 2010/03/31
//
/**
 * Implements MatlabInterface functions. It is a singleton class -
 * only one instance of this class can be created.
 */
//------------------------------------------------------------------------------
#include <stdlib.h>         // for NULL
#include <string.h>         // for memcpy()
#include <sstream>          // for std::stringstream

// Clear a build error with Visual Studio 2010
#ifdef _CHAR16T
#define CHAR16_T
#endif

#include "engine.h"         // for Matlab Engine
#include "matrix.h"         // for Matlab mxArray

#ifdef __WXMAC__
#include <stdlib.h>         // for system() to launch X11 application
#include <unistd.h>         // for gethostname()
#endif

#include "MatlabInterface.hpp"
#include "InterfaceException.hpp"
#include "GmatGlobal.hpp"          // for IsMatlabDebugOn()
#include "StringUtil.hpp"          // for DecomposeBy()
#include "MessageInterface.hpp"

//#define DEBUG_MATLAB_OPEN_CLOSE
//#define DEBUG_MATLAB_PUT_REAL
//#define DEBUG_MATLAB_GET_REAL
//#define DEBUG_MATLAB_GET_STRING
//#define DEBUG_MATLAB_EVAL

//--------------------------------------
//  initialize static variables
//--------------------------------------
MatlabInterface* MatlabInterface::instance = NULL;
const int MatlabInterface::MAX_OUT_SIZE = 8192;

const std::string
MatlabInterface::PARAMETER_TEXT[MatlabInterfaceParamCount - InterfaceParamCount] =
{
   "MatlabMode",
};

const Gmat::ParameterType
MatlabInterface::PARAMETER_TYPE[MatlabInterfaceParamCount - InterfaceParamCount] =
{
   Gmat::INTEGER_TYPE,       //"MatlabMode",
};


//--------------------------------------
//  public functions
//--------------------------------------

//------------------------------------------------------------------------------
// MatlabInterface* Instance()
//------------------------------------------------------------------------------
/*
 * Returns this instance
 */
//------------------------------------------------------------------------------
MatlabInterface* MatlabInterface::Instance()
{
   if (instance == NULL)
      instance = new MatlabInterface("OneInstance");
   
   return instance;
}


//------------------------------------------------------------------------------
// int Open(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Opens Matlab engine
 *
 * @param  name  Name used in identifying matlab engine
 * @return  1 = no error, 0 = error
 */
//------------------------------------------------------------------------------
int MatlabInterface::Open(const std::string &name)
{
   //=======================================================
   #ifdef __WXMAC__
   //=======================================================
   
   return OpenEngineOnMac();
   
   //=======================================================
   #else
   //=======================================================
   
   // If opening MATLAB engine for single use
   if (matlabMode == SINGLE_USE)
      return OpenSingleEngine(name);
   else if (matlabMode == SHARED)
      return OpenSharedEngine();
   else
   {
      #ifdef DEBUG_OPEN_CLOSE
      MessageInterface::ShowMessage
         ("MatlabInterface::Open() name='%s', returning 0, matlab mode is invalid, "
          "expecting 20 or 21\n");
      #endif
      return 0;
   }
   
   //=======================================================
   #endif  // End-ifdef __WXMAC__
   //=======================================================
} // end Open()


//------------------------------------------------------------------------------
//  int Close(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Closes Matlab engine
 *
 * @param  name  Name used in identifying matlab engine
 * @return  1 = no error, 0 = error
 */
//------------------------------------------------------------------------------
int MatlabInterface::Close(const std::string &name)
{
   //=======================================================
   #ifdef __WXMAC__
   //=======================================================
   
   return CloseEngineOnMac();
   
   //=======================================================
   #else
   //=======================================================
   
   if (matlabMode == SINGLE_USE)
      return CloseSingleEngine(name);
   else if (matlabMode == SHARED)
      return CloseSharedEngine();
   else
   {
      #ifdef DEBUG_OPEN_CLOSE
      MessageInterface::ShowMessage
         ("MatlabInterface::Close() name='%s', returning 0, matlab mode is invalid, "
          "expecting 20 or 21\n");
      #endif
      return 0;
   }
   
   //=======================================================
   #endif
   //=======================================================
} // end Close()


//------------------------------------------------------------------------------
// void SetCallingObjectName(const std::string &calledFrom)
//------------------------------------------------------------------------------
void MatlabInterface::SetCallingObjectName(const std::string &calledFrom)
{
   callingObjectName = calledFrom;
   #ifdef DEBUG_CALLED_FROM
   MessageInterface::ShowMessage
      ("MatlabInterface::SetCallingObjectName() calledFrom '%s'\n",
       calledFrom.c_str());
   #endif
}


//------------------------------------------------------------------------------
// std::string GetCallingObjectName()
//------------------------------------------------------------------------------
std::string MatlabInterface::GetCallingObjectName()
{
   return callingObjectName;
}


//------------------------------------------------------------------------------
// int PutRealArray(const std::string &matlabVarName, int numRow, int numCol,
//                  const double *inArray)
//------------------------------------------------------------------------------
/**
 * Put arrays to Matlab workspace.
 *
 * @param <matlabVarName> variable name in the MATLAB workspace
 * @param <numRows> number of rows in input array
 * @param <numCols> number of columns in input array
 * @return 1 = no error, 0 = error
 */
//------------------------------------------------------------------------------
int MatlabInterface::PutRealArray(const std::string &matlabVarName,
                                  int numRows, int numCols, const double *inArray)
{
   #ifdef DEBUG_MATLAB_PUT_REAL
   MessageInterface::ShowMessage
      ("MatlabInterface::PutRealArray() entered, matlabVarName='%s', numRows=%d, "
       "numCols=%d, inArray=%p\n", matlabVarName.c_str(), numRows, numCols, inArray);
   MessageInterface::ShowMessage("inArray = \n");
   for (int i = 0; i < numRows; i++)
      for (int j = 0; j < numCols; j++)
      {
         MessageInterface::ShowMessage("%f ", inArray[i*numCols+j]);
         if (j == numCols-1) MessageInterface::ShowMessage("\n");
      }
   #endif
   
   // Since MATLAB stores array in column major order, it needs to take transpose
   // before putting to MATLAB workspace
   Rmatrix rowMajorMat(numRows, numCols);
   for (int i = 0; i < numRows; i++)
      for (int j = 0; j < numCols; j++)
         rowMajorMat(i,j) = inArray[i*numCols+j];
   
   #ifdef DEBUG_MATLAB_PUT_REAL
   MessageInterface::ShowMessage("rowMajorMat=\n%s\n", rowMajorMat.ToString(16));
   #endif
   
   // Create column major matrix
   Rmatrix colMajorMat(numCols, numRows);
   colMajorMat = rowMajorMat.Transpose();

   #ifdef DEBUG_MATLAB_PUT_REAL
   MessageInterface::ShowMessage("colMajorMat=\n%s\n", colMajorMat.ToString(16));
   #endif
   
   const double *inColMajorArr = colMajorMat.GetDataVector();
   
   // create a matlab variable
   mxArray *mxArrayPtr = NULL;
   mxArrayPtr = mxCreateDoubleMatrix(numRows, numCols, mxREAL);
   
   #ifdef DEBUG_MATLAB_PUT_REAL
   MessageInterface::ShowMessage
      ("   mxArrayPtr <%p> created, now copying from inArray <%p>\n", mxArrayPtr, inColMajorArr);
   #endif
   
   // Call mxGetPr to access the real data in the mxArray that pm points to.
   // Once you have the starting address, you can access any other element in the mxArray.
   double *putArray = mxGetPr(mxArrayPtr);
   memcpy((void*)putArray, (void*)inColMajorArr, numRows*numCols*sizeof(double));
   
   #ifdef DEBUG_MATLAB_PUT_REAL
   MessageInterface::ShowMessage("   Now putting mxArrayPtr <%p> to matlab workspace\n");
   #endif
   
   // place the variable mxArrayPtr into the MATLAB workspace
   engPutVariable(enginePtr, matlabVarName.c_str(), mxArrayPtr);
   
   // Should I destroy after put to matlab?
   #ifdef DEBUG_MATLAB_PUT_REAL
   MessageInterface::ShowMessage("   Now destroying mxArrayPtr <%p>\n", mxArrayPtr);
   #endif
   mxDestroyArray(mxArrayPtr);
   
   return 1;
} // end PutRealArray()


//------------------------------------------------------------------------------
// int GetRealArray(const std::string &matlabVarName, int numElements,
//                  double outArray[], Integer &numRowsReceived, Integer &numColsReceived)
//------------------------------------------------------------------------------
/**
 * Get arrays from Matlab workspace.
 *
 * @param <matlabVarName> variable name in the MATLAB workspace
 * @param <numElements> number of elements to receive from MATLAB
 * @param <outArray> array to receive double array from MATLAB
 * @param <numRowsReceived> row dimension of received array to return
 * @param <numColsReceived> column dimension of received array to return
 * @Return 0 on error; else number of elemnets received from MATLAB
 * @exception <InterfaceException> thrown if empty output was received
 */
//------------------------------------------------------------------------------
int MatlabInterface::GetRealArray(const std::string &matlabVarName,
                                  int numElements, double outArray[],
                                  Integer &numRowsReceived, Integer &numColsReceived)
{
   #ifdef DEBUG_MATLAB_GET_REAL
   MessageInterface::ShowMessage
      ("MatlabInterface::GetRealArray() entered, matlabVarName=%s, numElements=%d\n",
       matlabVarName.c_str(), numElements);
   #endif
   
   // get the variable from the MATLAB workspace
   mxArray *mxArrayPtr = NULL;
   mxArrayPtr = engGetVariable(enginePtr, matlabVarName.c_str());
   
   #ifdef DEBUG_MATLAB_GET_REAL
   MessageInterface::ShowMessage("   mxArrayPtr is <%p>\n", mxArrayPtr);
   #endif
   
   if (mxArrayPtr == NULL)
   {
      #ifdef DEBUG_MATLAB_GET_REAL
      MessageInterface::ShowMessage
         ("MatlabInterface::GetRealArray(): mxArrayPtr is NULL\n");
      #endif
      return 0;
   }
   
   size_t numElementsReceived = mxGetNumberOfElements(mxArrayPtr);
   numRowsReceived = (Integer)mxGetM(mxArrayPtr);
   numColsReceived = (Integer)mxGetN(mxArrayPtr);
   #ifdef DEBUG_MATLAB_GET_REAL
   MessageInterface::ShowMessage
      ("   numElementsReceived=%d, numRowsReceived=%d, numColsReceived=%d\n",
       numElementsReceived, numRowsReceived, numColsReceived);
   #endif
   
   if (mxIsDouble(mxArrayPtr))
   {
      #ifdef DEBUG_MATLAB_GET_REAL
      MessageInterface::ShowMessage
         ("MatlabInterface::GetRealArray() matlab double array pointer found <%p>, "
          "so calling mxGetPr()\n", mxArrayPtr);
      #endif
      
      // If real numeric pointer is NULL, throw an exception
      double *realPtr = mxGetPr(mxArrayPtr);
      if (realPtr == NULL)
         throw InterfaceException("Received empty real output from MATLAB");
      
      memcpy((char*)outArray, (char*)mxGetPr(mxArrayPtr),
             numElements*sizeof(double));
      
      #ifdef DEBUG_MATLAB_GET_REAL
      MessageInterface::ShowMessage("      outArray  = \n");
      for (unsigned int ii=0; ii < numElementsReceived; ii++)
         MessageInterface::ShowMessage("         %.12f\n", outArray[ii]);
      MessageInterface::ShowMessage("   Now destroying mxArrayPtr <%p>\n", mxArrayPtr);
      #endif
      
      mxDestroyArray(mxArrayPtr);
      return numElementsReceived;
   }
   // Added to handle logical scalar data (Bug 2376 fix)
   else if (mxIsLogicalScalar(mxArrayPtr))
   {
      #ifdef DEBUG_MATLAB_GET_REAL
      MessageInterface::ShowMessage
         ("MatlabInterface::GetRealArray() matlab logical scalar pointer found <%p>, "
          "so calling mxGetPr()\n", mxArrayPtr);
      #endif
      if (mxIsLogicalScalarTrue(mxArrayPtr))
         outArray[0] = 1.0;
      else
         outArray[0] = 0.0;
      
      #ifdef DEBUG_MATLAB_GET_REAL
      for (Integer ii=0; ii < numElements; ii++)
         MessageInterface::ShowMessage("      outArray[%d] = %f\n", ii, outArray[ii]);
      MessageInterface::ShowMessage("   Now destroying mxArrayPtr <%p>\n", mxArrayPtr);
      #endif
      
      mxDestroyArray(mxArrayPtr);
      return 1;
   }
   else
   {
      #ifdef DEBUG_MATLAB_GET_REAL
      MessageInterface::ShowMessage
         ("MatlabInterface::GetRealArray() Matlab variable '%s' "
          "is not a double array or logical scalar.\n", matlabVarName.c_str());
      #endif
      
      return 0;
   }
} // end GetRealArray()


//------------------------------------------------------------------------------
// int GetString(const std::string &matlabVarName, std::string &outStr)
//------------------------------------------------------------------------------
/**
 * Get char array from Matlab workspace.
 *
 * @param <matlabVarName> variable name in the MATLAB workspace
 * @param <outStr> string to receive char array from MATLAB
 * @Return 1 if matlab string variable found, 0 otherwise
 * @exception <InterfaceException> thrown if empty output was received
 */
//------------------------------------------------------------------------------
int MatlabInterface::GetString(const std::string &matlabVarName,
                               std::string &outStr)
{
   #ifdef DEBUG_MATLAB_GET_STRING
   MessageInterface::ShowMessage
      ("MatlabInterface::GetString() entered, matlabVarName='%s', outStr='%s'\n",
       matlabVarName.c_str(), outStr.c_str());
   #endif

   // get the variable from the MATLAB workspace
   mxArray *mxArrayPtr = NULL;
   mxArrayPtr = engGetVariable(enginePtr, matlabVarName.c_str());

   if (mxArrayPtr == NULL)
   {
      #ifdef DEBUG_MATLAB_GET_STRING
      MessageInterface::ShowMessage
         ("MatlabInterface::GetString() mxArrayPtr is NULL\n");
      #endif
      return 0;
   }

   // check if it is string
   if (mxIsChar(mxArrayPtr))
   {
      #ifdef DEBUG_MATLAB_GET_STRING
      MessageInterface::ShowMessage
         ("MatlabInterface::GetString() matlab string pointer found <%p>, "
          "so calling mxGetString()\n", mxArrayPtr);
      #endif

      if (outBuffer == NULL)
         throw InterfaceException
            ("**** ERROR **** Failed to get string from MATLAB, output buffer is NULL\n");

      outBuffer[0] = '\0';
      mxGetString(mxArrayPtr, outBuffer, MAX_OUT_SIZE);
      outBuffer[MAX_OUT_SIZE] = '\0';
      outStr.assign(outBuffer);

      #ifdef DEBUG_MATLAB_GET_STRING
      MessageInterface::ShowMessage
         ("MatlabInterface::GetString() outStr =\n%s\n", outStr.c_str());
      MessageInterface::ShowMessage("   Now destroying mxArrayPtr <%p>\n", mxArrayPtr);
      #endif

      mxDestroyArray(mxArrayPtr);
      return 1;
   }
   else
   {
      #ifdef DEBUG_MATLAB_GET_STRING
      MessageInterface::ShowMessage
         ("MatlabInterface::GetString() Matlab variable is not a char array\n");
      #endif

      return 0;
   }
} // end GetString()


//------------------------------------------------------------------------------
//  int EvalString(const std::string &evalString)
//------------------------------------------------------------------------------
//  Purpose:
//     Evaluates matlab string.
//
//  Returns:
//     0, if string evaluated was successful
//     nonzero, if unsuccessful
//------------------------------------------------------------------------------
int MatlabInterface::EvalString(const std::string &evalString)
{
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage
      ("MatlabInterface::EvalString() calling engEvalString with\n"
       "======================================================================\n"
       "%s\n\n", evalString.c_str());
   #endif
   
   // return value is 0 if the command was evaluated by the MATLAB engine,
   // and a nonzero value if unsuccessful. Possible reasons for failure include
   // the engine session is no longer running or the engine pointer is invalid or NULL.
   int retval = engEvalString(enginePtr, evalString.c_str());
   
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage("MatlabInterface::EvalString() exiting with %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
//  int SetOutputBuffer(int size)
//------------------------------------------------------------------------------
/**
 * Sets outBuffer to input size. All result using EvalString() will use this
 * buffer.
 *
 * @param size   size of the buffer, it will use size-1.
 *
 * @return size of the buffer used, 0 = error
 */
//------------------------------------------------------------------------------
int MatlabInterface::SetOutputBuffer(int size)
{
   outBuffer[0] = '\0';
   int sizeToUse = size-1;

   if (sizeToUse > MAX_OUT_SIZE)
      sizeToUse = MAX_OUT_SIZE;

   engOutputBuffer(enginePtr, outBuffer, sizeToUse);
   return sizeToUse;
}


//------------------------------------------------------------------------------
// char* GetOutputBuffer()
//------------------------------------------------------------------------------
/*
 * Returns Matlab output buffer pointer which has a pointer to Matlab result
 * using EvalStaring()
 */
//------------------------------------------------------------------------------
char* MatlabInterface::GetOutputBuffer()
{
   return outBuffer;
}


//------------------------------------------------------------------------------
// bool IsOpen(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Checks if engine is open. If name is blank for single use, it will
 * return true if there are more than one engine is opened.
 */
//------------------------------------------------------------------------------
bool MatlabInterface::IsOpen(const std::string &name)
{
   #ifdef DEBUG_MATLAB_OPEN_CLOSE
   MessageInterface::ShowMessage
      ("MatlabInterface::IsOpen() entered, name='%s', matlabMode=%d, enginePtr=<%p>\n",
       name.c_str(), matlabMode, enginePtr);
   #endif
   
   bool isOpen = false;
   if (matlabMode == SINGLE_USE)
   {
      if (name != "")
      {
         std::map<std::string, Engine*>::iterator pos = matlabEngineMap.find(name);
         if (pos == matlabEngineMap.end())
            //return false;
            isOpen = false;
         else
            //return true;
            isOpen = true;
      }
      else
      {
         if (matlabEngineMap.empty())
            //return false;
            isOpen = false;
         else
            //return true;
            isOpen = true;
      }
   }
   else
   {
      //return (MatlabInterface::enginePtr != NULL);
      isOpen = (MatlabInterface::enginePtr != NULL);
   }
   
   #ifdef DEBUG_MATLAB_OPEN_CLOSE
   MessageInterface::ShowMessage
      ("MatlabInterface::IsOpen() returning %d\n", isOpen);
   #endif
   
   return isOpen;
}


//------------------------------------------------------------------------------
// void RunMatlabString(const std::string &evalString)
//------------------------------------------------------------------------------
void MatlabInterface::RunMatlabString(const std::string &evalString)
{
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage
      ("MatlabInterface::RunMatlabString() entered\n"
       "======================================================================\n"
       "%s\n\n", evalString.c_str());
   #endif
   
   if (!IsOpen())
   {
      // Let's try to open it first (loj: 2008.03.06)
      if (Open() == 0)
         throw InterfaceException("**** ERROR **** Failed to open MATLAB engine\n");
   }
   
   std::string newEvalStr = evalString;
   
   // To handle special case of 'cd' with directory with blank spaces, cd c:\my test directory,
   // used the functional form of cd, such as cd('directory-spec'),
   // since Matlab cannot handle cd to directory name with blanks (LOJ: 2010.10.07)
   // This will fix bug 2032
   StringArray parts = GmatStringUtil::DecomposeBy(newEvalStr, " ");
   if (parts.size() > 1)
   {      
      std::string firstToken = GmatStringUtil::ToLower(parts[0]);
      
      #ifdef DEBUG_MATLAB_EVAL
      MessageInterface::ShowMessage("   firstToken='%s'\n", firstToken.c_str());
      #endif
      
      if (firstToken == "cd")
         newEvalStr = "cd(" + GmatStringUtil::AddEnclosingString(parts[1], "'") + ")";
      
      #ifdef DEBUG_MATLAB_EVAL
      MessageInterface::ShowMessage
         ("   String starts with 'cd' so changed to function form\n"
          "   old name = '%s'\n   new name = '%s'\n", evalString.c_str(), newEvalStr.c_str());
      #endif
   }
   
   // add try/catch to string to evaluate
   newEvalStr = "try,\n  beep off;\n  " + newEvalStr + "\ncatch\n  errormsg = lasterr;\nend";
   
   bool errorReturned = false;
   std::string errorStr;
   // call to evaluate string
   if (EvalString(newEvalStr) == 0)
   {
      // if there was an error throw an exception
      #ifdef DEBUG_MATLAB_EVAL
      MessageInterface::ShowMessage("   Now checking if there was an error\n");
      #endif
      if (GetString("errormsg", errorStr) == 1)
      {
         #ifdef DEBUG_MATLAB_EVAL
         MessageInterface::ShowMessage
            ("Error occurred in Matlab function '%s'\n'%s'\n",
             callingObjectName.c_str(), errorStr.c_str());
         #endif
         errorReturned = true;
      }
      #ifdef DEBUG_MATLAB_EVAL
      MessageInterface::ShowMessage("   No error found\n");
      #endif
   }
   else
   {
      errorReturned = true;
      errorStr = "Error encountered during engEvalString of \"" + evalString + "\"";
   }
   
   if (errorReturned)
   {
      throw InterfaceException("\"" + callingObjectName + ".m\", " + errorStr);
   }
   
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage("MatlabInterface::RunMatlabString() exiting\n\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetMatlabMode(int mode)
//------------------------------------------------------------------------------
/**
 * Sets matlab run mode.
 *
 * @param  mode  The matlab run mode
 *               (0 = shared, 1 = single use, -1 = no MATLAB installed)
 */
//------------------------------------------------------------------------------
void MatlabInterface::SetMatlabMode(int mode)
{
   matlabMode = mode;
}

//------------------------------------------------------------------------------
// int GetMatlabMode()
//------------------------------------------------------------------------------
/**
 * Return matlab run mode.
 *
 * @return  The matlab run mode (0 = shared, 1 = single use)
 */
//------------------------------------------------------------------------------
int MatlabInterface::GetMatlabMode()
{
   return matlabMode;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MatlabInterface.
 *
 * @return clone of the MatlabInterface.
 */
//------------------------------------------------------------------------------
GmatBase* MatlabInterface::Clone() const
{
   return (new MatlabInterface(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void MatlabInterface::Copy(const GmatBase* orig)
{
   operator=(*((MatlabInterface *)(orig)));
}


//--------------------------------------
//  private functions
//--------------------------------------

//------------------------------------------------------------------------------
//  <constructor>
//  MatlabInterface()
//------------------------------------------------------------------------------
MatlabInterface::MatlabInterface(const std::string &name) :
   Interface ("MatlabInterface", name)
{
   enginePtr = 0;
   accessCount = 0;
   matlabMode = SHARED;
   outBuffer = new char[MAX_OUT_SIZE+1];
   for (int i=0; i<=MAX_OUT_SIZE; i++)
      outBuffer[i] = '\0';
   debugMatlabEngine = GmatGlobal::Instance()->IsMatlabDebugOn();
}


//------------------------------------------------------------------------------
//  <destructor>
//  MatlabInterface()
//------------------------------------------------------------------------------
MatlabInterface::~MatlabInterface()
{
   if (enginePtr != NULL)
      Close();
   delete [] outBuffer;
}


//------------------------------------------------------------------------------
// MatlabInterface(const MatlabInterface &mi)
//------------------------------------------------------------------------------
MatlabInterface::MatlabInterface(const MatlabInterface &mi) :
   Interface (mi)
{
}


//------------------------------------------------------------------------------
// MatlabInterface& operator=(const MatlabInterface& mi)
//------------------------------------------------------------------------------
MatlabInterface& MatlabInterface::operator=(const MatlabInterface& mi)
{
   if (&mi != this)
   {
      Interface::operator=(mi);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// int OpenEngineOnMac()
//------------------------------------------------------------------------------
/**
 * Opens Matlab engine on Mac.
 *
 * @return  1 = no error, 0 = error
 */
//------------------------------------------------------------------------------
int MatlabInterface::OpenEngineOnMac()
{
#ifdef __WXMAC__
   if (debugMatlabEngine)
   {
      MessageInterface::ShowMessage
         ("MatlabInterface::OpenEngineOnMac() enginePtr=%p\n", enginePtr);
   }
   
   if (enginePtr == NULL)
      MessageInterface::ShowMessage("Please wait while MATLAB engine opens...\n");
   
   /// Check if MATLAB engine is still running then doesn't need to re-launch
   if (enginePtr != NULL)
   {
      accessCount++;
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("Attempting to reopen MATLAB engine ... accessCount = %d\n",
             accessCount);
      }
      return 1;
   }
   
//   // open the X11 application before launching the matlab
//   system("open -a X11");
   // need to get IP address or hostname here
   char hName[128];
   int OK = gethostname(hName, 128);
   
   if (OK != 0)
   {
      //MessageInterface::ShowMessage("Error getting host name\n");
      return 0;
   }
   
   std::string hNameStr(hName);
//   std::string runString = "matlab -maci ";  // for 32-bit ONLY for now!!!
   // do we want -nodisplay here as well??
   std::string runString = "matlab -nosplash -nodesktop";  // for 64-bit ONLY for now!!!
   if ((enginePtr = engOpen(runString.c_str())))
   {
      MessageInterface::ShowMessage(
               "Successfully opened MATLAB engine using startcmd \"%s\"\n",
               runString.c_str());
      
      accessCount++;
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("Attempting to open MATLAB engine ... accessCount = %d\n",
             accessCount);
      }
      //engSetVisible(enginePtr,1);  // rats!
      return 1;
   }
   else
   {
      MessageInterface::ShowMessage(
               "Failed to open MATLAB engine using startcmd \"%s\"\n",
               runString.c_str());
      return 0;
   }
#else
   return 0;
#endif
}


//------------------------------------------------------------------------------
// int CloseEngineOnMac()
//------------------------------------------------------------------------------
/**
 * Closes shared MATLAB engine on Mac.
 *
 * @return 1 if successfully closed, 0 otherwise
 */
//------------------------------------------------------------------------------
int MatlabInterface::CloseEngineOnMac()
{
#ifdef __WXMAC__

   int retval = 1; // set to success
   
   // Check if MATLAB engine is still running then close it.
   if (enginePtr != NULL)
   {
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("MatlabInterface::Close() enginePtr=%p\n", enginePtr);
      }
      
      accessCount--;
      
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("Attempting to close MATLAB engine ... accessCount = %d\n",
             accessCount);
      }
      
//      // need to close X11 here ------------ **** TBD ****
//      MessageInterface::ShowMessage
//         ("Closing MATLAB engine ... please close X11 ...\n");
      
      //==============================================================
      // int engClose(Engine *ep);
      // 0 on success, and 1 otherwise. Possible failure includes
      // attempting to terminate a MATLAB engine that was
      // already terminated.
      //==============================================================
      retval = engClose(enginePtr);
      if (retval == 0)
      {
         retval = 1;
         MessageInterface::ShowMessage("MATLAB engine successfully closed\n");
      }
      else
      {
         retval = 0;;
         MessageInterface::ShowMessage("\nError closing MATLAB\n");
      }
      
      enginePtr = NULL;      // set to NULL, so it can be reopened
   }
   else
   {
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("\nUnable to close MATLAB engine because it is not currently running\n");
      }
   }
   return retval;
#else
   return 0;
#endif
}


//------------------------------------------------------------------------------
// int OpenSharedEngine()
//------------------------------------------------------------------------------
/**
 * Opens shared Matlab engine on Windows.
 *
 * @return  1 = no error, 0 = error
 */
//------------------------------------------------------------------------------
int MatlabInterface::OpenSharedEngine()
{
   if (debugMatlabEngine)
      MessageInterface::ShowMessage
         ("MatlabInterface::OpenSharedEngine() enginePtr=%p\n", enginePtr);
   
   if (enginePtr == NULL)
   {
      MessageInterface::ShowMessage("Please wait while MATLAB engine opens...\n");
   }
   else
   {
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage("Connecting to current MATLAB engine\n");
         MessageInterface::ShowMessage
            ("MatlabInterface::OpenSharedEngine() returning 1\n");;
      }
      
      return 1;
   }
   
   // open new MATLAB engine
   if ((enginePtr = engOpen(NULL)))
   {
      accessCount++;
      
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("Attempting to open MATLAB engine ... accessCount = %d, "
             "enginePtr=%p\n", accessCount, enginePtr);
      }
      
      // set precision to long
      EvalString("format long");
      MessageInterface::ShowMessage("MATLAB engine successfully opened\n");
      if (debugMatlabEngine)
         MessageInterface::ShowMessage
            ("MatlabInterface::OpenSharedEngine() returning 1\n");;
      return 1;
   }
   else
   {
      MessageInterface::ShowMessage("Failed to open MATLAB engine ...\n");
      if (debugMatlabEngine)
         MessageInterface::ShowMessage
            ("MatlabInterface::OpenSharedEngine() returning 0\n");;
      return 0;
   }
}


//------------------------------------------------------------------------------
// int CloseSharedEngine()
//------------------------------------------------------------------------------
/**
 * Closes shared MATLAB engine on Windows.
 *
 * @return 1 if successfully closed, 0 otherwise
 */
//------------------------------------------------------------------------------
int MatlabInterface::CloseSharedEngine()
{
   #ifdef DEBUG_MATLAB_OPEN_CLOSE
   MessageInterface::ShowMessage
      ("MatlabInterface::CloseSharedEngine() entered, enginePtr=<%p>\n", enginePtr);
   #endif
   
   int retval = 0; // set to failed
   
   // Check if MATLAB engine is still running then close it.
   if (enginePtr != NULL)
   {
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("MatlabInterface::CloseSharedEngine() enginePtr=%p\n", enginePtr);
      }
      
      accessCount--;
      
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("Attempting to close MATLAB engine ... accessCount = %d\n", accessCount);
      }
      
      //==============================================================
      // int engClose(Engine *ep);
      // 0 on success, and 1 otherwise. Possible failure includes
      // attempting to terminate a MATLAB engine that was
      // already terminated.
      //==============================================================
      retval = engClose(enginePtr);
      if (retval == 0)
      {
         retval = 1;
         MessageInterface::ShowMessage("MATLAB engine successfuly closed\n");
      }
      else
      {
         retval = 0;
         MessageInterface::ShowMessage("Error closing MATLAB\n");
      }
      
      enginePtr = NULL; // set to NULL, so it can be reopened
   }
   else
   {
      retval = 0;
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("\nUnable to close MATLAB engine because it is not currently running\n");
      }
   }
   
   #ifdef DEBUG_MATLAB_OPEN_CLOSE
   MessageInterface::ShowMessage
      ("MatlabInterface::CloseSharedEngine() returning retval=%d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// int OpenSingleEngine(const std::string &name);
//------------------------------------------------------------------------------
/**
 * Opens single Matlab engine on Windows.
 *
 * @return  1 = no error, 0 = error
 */
//------------------------------------------------------------------------------
int MatlabInterface::OpenSingleEngine(const std::string &name)
{
   //=================================================================
   //Engine* engOpenSingleUse(const char *startcmd, void *dcom, int *retstatus);
   //startcmd
   //   String to start MATLAB process. On Microsoft Windows systems,
   //   the startcmd string must be NULL.
   //dcom
   //   Reserved for future use; must be NULL.
   //retstatus
   //   Return status; possible cause of failure.
   //      0 = success
   //     -2 = error - second argument must be NULL
   //     -3 = error - engOpenSingleUse failed
   //=================================================================
      
   if (debugMatlabEngine)
      MessageInterface::ShowMessage
         ("MatlabInterface::OpenSingleEngine() name='%s'\n", name.c_str());
   
   // Check for the engine name first      
   if (name == "")
   {
      std::stringstream ss("");
      ss.width(1);
      ss << accessCount+1;
      lastEngineName = "matlabEngine_" + ss.str();
   }
   else
   {
      lastEngineName = name;
   }
   
   if (debugMatlabEngine)
   {
      MessageInterface::ShowMessage
         ("Attempting to open MATLAB engine '%s' for single use ... accessCount = %d\n",
          lastEngineName.c_str(), accessCount+1);
   }
   
   if (matlabEngineMap.find(lastEngineName) != matlabEngineMap.end())
   {
      enginePtr = matlabEngineMap[lastEngineName];
      
      if (debugMatlabEngine)
      {
         MessageInterface::ShowMessage
            ("'%s' already opened for single use, enginePtr=<%p>\n",
             lastEngineName.c_str(), enginePtr);
         MessageInterface::ShowMessage
            ("MatlabInterface::OpenSingleEngine() returning 1\n");
      }
      
      return 1;
   }
   
   // Set current engine pointer to enginePtr
   int retval = -99;
   enginePtr = engOpenSingleUse(NULL, NULL, &retval);
   
   if (retval != 0)
   {
      MessageInterface::ShowMessage
         ("Failed to open MATLAB engine for single use...\n");
      if (debugMatlabEngine)
         MessageInterface::ShowMessage
            ("MatlabInterface::OpenSingleEngine() returning 0\n");
      return 0;
   }
   
   accessCount++;
   matlabEngineMap.insert(std::make_pair(lastEngineName, enginePtr));
   
   if (debugMatlabEngine)
   {
      MessageInterface::ShowMessage
         ("Added <%p>'%s' to matlabEngineMap\n", enginePtr, lastEngineName.c_str());
   }
   
   // set precision to long
   EvalString("format long");
   MessageInterface::ShowMessage
      ("MATLAB engine '%s' successfully opened\n", lastEngineName.c_str());
   
   if (debugMatlabEngine)
      MessageInterface::ShowMessage
         ("MatlabInterface::OpenSingleEngine() returning 1\n");
   
   return 1;
}


//------------------------------------------------------------------------------
// int CloseSingleEngine(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Closes single MATLAB engine on Windows.
 *
 * @param  name  Name used in identifying matlab engine
 * @return 1 if successfully closed, 0 otherwise
 */
//------------------------------------------------------------------------------
int MatlabInterface::CloseSingleEngine(const std::string &name)
{
   int retval = 1; // set to success
   bool failedToClose = false;
   
   if (name != "")
   {
      std::map<std::string, Engine*>::iterator pos = matlabEngineMap.find(name);
      if (pos != matlabEngineMap.end())
      {
         //==============================================================
         // int engClose(Engine *ep);
         // 0 on success, and 1 otherwise. Possible failure includes
         // attempting to terminate a MATLAB engine that was
         // already terminated.
         //==============================================================
         retval = engClose(pos->second);
         if (retval == 0)
         {
            MessageInterface::ShowMessage
               ("MATLAB engine '%s' successfully closed\n", name.c_str());
            matlabEngineMap.erase(pos);
         }
         else
         {
            failedToClose = true;
            MessageInterface::ShowMessage
               ("Error closing MATLAB engine '%s'\n", name.c_str());
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("Error closing MATLAB engine '%s'\n", name.c_str());
      }
   }
   else
   {
      // Close all matlab engines
      for (std::map<std::string, Engine*>::iterator pos = matlabEngineMap.begin();
           pos != matlabEngineMap.end(); ++pos)
      {
         if (debugMatlabEngine)
         {
            MessageInterface::ShowMessage
               ("MatlabInterface::CloseSingleEngine() about to close engine <%p>'%s'\n",
                pos->second, (pos->first).c_str());
         }
         
         retval = engClose(pos->second);
         if (retval == 0)
         {
            MessageInterface::ShowMessage
               ("MATLAB engine '%s' successfully closed\n", (pos->first).c_str());
         }
         else
         {
            failedToClose = true;
            MessageInterface::ShowMessage
               ("Error closing MATLAB engine '%s'\n", (pos->first).c_str());
         }
      }
      
      if (!failedToClose)
         matlabEngineMap.clear();
   }
   
   if (failedToClose)
      retval = 0;
   
   return retval;
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer MatlabInterface::GetParameterID(const std::string &str) const
{
   for (Integer i = InterfaceParamCount; i < MatlabInterfaceParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - InterfaceParamCount])
         return i;
   }
   
   return Interface::GetParameterID(str);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool MatlabInterface::IsParameterReadOnly(const Integer id) const
{
   // All parameters are read only
   return true;
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer MatlabInterface::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case MATLAB_MODE:
      return matlabMode;
   default:
      return Interface::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer MatlabInterface::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case MATLAB_MODE:
      matlabMode = value;
      return matlabMode;
   default:
      return Interface::SetIntegerParameter(id, value);
   }
}


//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer MatlabInterface::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const std::string &label, const Integer value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer MatlabInterface::SetIntegerParameter(const std::string &label,
                                             const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

