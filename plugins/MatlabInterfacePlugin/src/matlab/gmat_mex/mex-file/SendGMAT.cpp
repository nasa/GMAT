//------------------------------------------------------------------------------
//                              SendGMAT
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
// Author:  Joey Gurganus
// Created: 2005/12/08
//
/**
 * Implements the SendGMAT that is used to send data to the GMAT server from 
 * MATLAB client.
 */
//------------------------------------------------------------------------------


#include <iostream>
#include <exception>
#include "mex.h"
#include "MatlabClient.hpp"

// #define CHECK_BUG 1

// extern void _main();

static MatlabClient *matlabClient;

std::string SendGMAT(const std::string inputStr1, 
                            const std::string inputStr2)
{
   std::string output("");
#if CHECK_BUG 
   mexPrintf("\n######### Test Driver #########\n"); 
   mexPrintf("\ninputStr1 = %s and inputStr2 = %s", 
             inputStr1.c_str(), inputStr2.c_str()); 
#endif
   
   try { 

   if (inputStr1 == "Poke" && inputStr2 == "Open;")
   {
      if (!mexIsLocked())
      {
         #if CHECK_BUG 
             mexPrintf("\nCreating new MatlabClient and starting lock...\n");
         #endif
         matlabClient = new MatlabClient();
         if (!matlabClient->Connect())
         {
            mexErrMsgTxt("\nNot able to connnect GMAT server\n");
         }
         mexLock();
      }
      else
      {
         #if CHECK_BUG 
             mexPrintf("\nLock but still \"Open;\"???\n");
         #endif
      }
   }

#if CHECK_BUG 
   if (mexIsLocked() && !matlabClient->IsConnected())
      mexPrintf("\nIt hasn't connected\n"); 
#endif

   // if (matlabClient->Connect())
   if (mexIsLocked() && matlabClient->IsConnected())
   {
#if CHECK_BUG 
      mexPrintf("\nConnected successful and start to request...\n"); 
#endif

      if (inputStr1 == "Advise")
      {
         matlabClient->GetConnection()->StartAdvise(inputStr2.c_str()); 
      }
      else if (inputStr1 == "Poke")
      {
         matlabClient->GetConnection()->Poke(_T("script"), 
                                           (wxChar *)inputStr2.c_str()); 
      }
      else if (inputStr1 == "Request")
      {
         // Receive data from GMAT server
#if CHECK_BUG 
      mexPrintf("\nRequesting data from server : %s", inputStr2.c_str()); 
#endif
         output = (std::string) matlabClient->GetConnection()->Request(
                                        (wxChar *)inputStr2.c_str());
#if CHECK_BUG 
      mexPrintf("\noutput =  %s", inputStr2.c_str()); 
#endif
      }
      else if (inputStr1 == "Execute")
      {
         matlabClient->GetConnection()->Execute((wxChar *)inputStr2.c_str());
      }
      else
      {
         mexErrMsgTxt("\nCan't find the type of message\n");
      }

#if CHECK_BUG 
      if (matlabClient->IsConnected())
         mexPrintf("\nIt is still connected\n"); 
#endif

      if (inputStr1 == "Poke" && inputStr2 == "Close;")
      {
         #if CHECK_BUG 
              mexPrintf("\nCheck if it is locked..."); 
         #endif
         if (mexIsLocked())
         { 
            #if CHECK_BUG 
                mexPrintf("\nStart to unlock and destroy matlabClient.\n"); 
            #endif
            mexUnlock();
            if (!matlabClient->Disconnect())
                mexErrMsgTxt("\nNot able to Disconnect\n"); 
            delete matlabClient;
         }
      }

   }
   else 
   {
      if (inputStr2 == "Close;")
         mexErrMsgTxt("\nMatlab Client hasn't connected to GMAT Server.\n"); 
      else
         mexErrMsgTxt("\nNot able to connect GMAT server.\n"); 
   } // if-else


   }  // try
   catch(std::exception& e)
   {
     mexUnlock();
     if (matlabClient != NULL)
        delete matlabClient;
     mexPrintf("\nException: %s\n", e.what()); 
   }  // catch

   return output;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

   char        *input1, *input2;

   /* Check for proper number of arguments */
   if (nrhs != 2) 
   {
      mexErrMsgTxt("MEXCPP requires two input arguments.");
   } 
   else if (nlhs > 1) 
   {
      mexErrMsgTxt("Too many output arguments.   "
                   "Need zero or one output argument.");
   }

   // Check to be sure input is of type char
   if (!(mxIsChar(prhs[0])) || !(mxIsChar(prhs[1])) )
       mexErrMsgTxt("Input must be of type string.\n.");

   // Input must be a row vector. 
   if (mxGetM(prhs[0]) != 1 || mxGetM(prhs[1]) != 1)
       mexErrMsgTxt("Input(s) must be a row vector.\n.");


#if 0    // ************* Below this code came from mathwork online ************ 
   int bufferLength1 = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;
   int bufferLength2 = (mxGetM(prhs[1]) * mxGetN(prhs[1])) + 1;

   // Allocate memory for input and output strings.
   input1 = (char *) mxCalloc(bufferLength1, sizeof(char)); 
   input2 = (char *) mxCalloc(bufferLength2, sizeof(char)); 
  
   // Copy string data from prhs into C string
   int status = mxGetString(prhs[0], input1, bufferLength1);
   if (status != 0)
      mexWarnMsgTxt("Not enough space on first input.  String is truncated.");

   status = mxGetString(prhs[1], input2, bufferLength2);
   if (status != 0)
      mexWarnMsgTxt("Not enough space on second input.  String  is truncated.");

   // *************** Above this code came from mathwork online ************** 

#else
   input1 = mxArrayToString(mxDuplicateArray(prhs[0]));
   input2 = mxArrayToString(mxDuplicateArray(prhs[1]));
#endif

   std::string inputStr1(input1);
   std::string inputStr2(input2);

   mxFree(input1);
   mxFree(input2);

   std::string result = SendGMAT(inputStr1, inputStr2);
  
   plhs[0] = mxCreateString(result.c_str()); 

   return;
}
