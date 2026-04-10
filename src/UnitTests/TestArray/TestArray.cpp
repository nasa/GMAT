//------------------------------------------------------------------------------
//                                  TestArray
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
// Author: Linda Jun
// Created: 2007/09/26
//
/**
 * Test driver for testing array.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"

using namespace std;

int GetCharArray(TestOutput &out, int numChar, char outArray[], std::string &outStr)
{
   char charArray[20];
   charArray[0] = 'H';
   charArray[1] = 'a';
   charArray[2] = 'p';
   charArray[3] = 'p';
   charArray[4] = 'y';
   std::string str;
   str.assign(charArray, numChar);
   out.Put("str = ", str);
   
   outStr = str;
}


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   char chArray[20];
   std::string outStr;
   GetCharArray(out, 5, chArray, outStr);
   out.Put("outStr = ", outStr);
   
   GetCharArray(out, 3, chArray, outStr);
   out.Put("outStr = ", outStr);
   
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   MessageInterface::SetLogFile("../../../test/TestUtil/GmatLog.txt");
   TestOutput out("../../../test/TestUtil/TestArrayOut.txt");
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of Array!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
