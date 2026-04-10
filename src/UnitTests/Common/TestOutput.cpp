//------------------------------------------------------------------------------
//                             TestOutput
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
// Author: Linda Jun
// Created: 2005/02/11
//
/**
 * Implements TestOutput class which provides output methods to console and file.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <ctime>                   // for clock()
#include "TestOutput.hpp"
#include "FileUtil.hpp"
#include "GmatBaseException.hpp"

#if !defined __TEST_NON_REAL__
#include "Linear.hpp"              // for GmatRealUtil namespace
const Real TestOutput::TEST_TOL = 1.0e-5;
#endif

using namespace std;

//------------------------------------------------------------------------------
// TestOutput(const std::string &filename)
//------------------------------------------------------------------------------
TestOutput::TestOutput(const std::string &filename)
{
   cout << "output file to open: " << filename << endl;
   //cout << "current path: " << GmatFileUtil::GetCurrentPath() << endl;
   cout << "current path: " << GmatFileUtil::GetCurrentWorkingDirectory() << endl;
   
   mOutFileName = filename;
   mOutPathName = GmatFileUtil::ParsePathName(mOutFileName);
   mOutfile.open(filename.c_str());
   
   if (!mOutfile.is_open())
   {
      mOutFileName = GmatFileUtil::ParseFileName(mOutFileName);
      mOutPathName = "";
      cout << "*** WARNING *** Cannot open output file: " << filename
           << "\nSo setting file name to " << mOutFileName << endl;
      
      mOutfile.open(mOutFileName.c_str());
      
      if (!mOutfile.is_open())
      {
         throw GmatBaseException
            ("**** ERROR **** Cannot open output file: " + mOutFileName);
      }
   }
   
   cout << "Successfully opend: " << mOutFileName << endl;
   
   mAddNewLine = true;
   mWidth = 1;
   mPrecision = 10;
}


//------------------------------------------------------------------------------
// std::string GetOutPathName()
//------------------------------------------------------------------------------
std::string TestOutput::GetOutPathName()
{
   return mOutPathName;
}


//------------------------------------------------------------------------------
// TestOutput::GetOutFileName()
//------------------------------------------------------------------------------
std::string TestOutput::GetOutFileName()
{
   return mOutFileName;
}


//------------------------------------------------------------------------------
// void Close()
//------------------------------------------------------------------------------
void TestOutput::Close()
{
   mOutfile.flush();
   mOutfile.close();
}


//------------------------------------------------------------------------------
// void SetWidth(int w)
//------------------------------------------------------------------------------
void TestOutput::SetWidth(int w)
{
   mOutfile.width(w);
   cout.width(w);
   mWidth = w;
}


//------------------------------------------------------------------------------
// void SetAddNewLine(bool addNewLine)
//------------------------------------------------------------------------------
void TestOutput::SetAddNewLine(bool addNewLine)
{
   mAddNewLine = addNewLine;
}


//----------------------------------------------------------
//@note
// Put(bool bval) causes Put(const std::string &str) not work.
// Put(string) calls put(bool bval)
//----------------------------------------------------------

//------------------------------------------------------------------------------
// void PutBool(bool bval)
//------------------------------------------------------------------------------
void TestOutput::PutBool(bool bval)
{
   if (bval)
   {
      mOutfile << "true" << endl;
      cout << "true" << endl;
   }
   else
   {
      mOutfile << "false" << endl;
      cout << "false" << endl;
   }
}


//------------------------------------------------------------------------------
// void Put(int ival)
//------------------------------------------------------------------------------
void TestOutput::Put(int ival)
{
   mOutfile << ival << endl;
   cout << ival << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str)
{
   if (mAddNewLine)
   {
      mOutfile << str << endl;
      cout << str << endl;
   }
   else
   {
      mOutfile << str;
      cout << str;
   }
   
   //mAddNewLine = true;
}


//------------------------------------------------------------------------------
// void PutLine(const std::string &str)
//------------------------------------------------------------------------------
void TestOutput::PutLine(const std::string &str)
{
   mOutfile << str << endl;
   cout << str << endl;
   mOutfile.flush();
}


//------------------------------------------------------------------------------
//  void Put(const std::string &str1, int ival1,
//           const std::string &str2 = "", int ival2 = 0,
//           const std::string &str3 = "", int ival3 = 0)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str1, int ival1,
                     const std::string &str2, int ival2,
                     const std::string &str3, int ival3)
{
   mOutfile << str1 << ival1;
   cout << str1 << ival1;

   if (str2 != "")
   {
      if (ival2 == -99999)
      {
         mOutfile << " " << str2;
         cout << " " << str2;
      }
      else
      {
         mOutfile << " " << str2 << ival2;
         cout << " " << str2 << ival2;
      }
   }
   
   if (str3 != "")
   {
      mOutfile << " " << str3 << ival3;
      cout << " " << str3 << ival3;
   }
   
   mOutfile << endl;
   cout << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str, bool bval)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str, bool bval)
{
   std::string sval;
   bval ? sval = "true" : sval = "false";
   
   mOutfile << str << sval << endl;
   cout << str << sval << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str, const char *sval)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str, const char *sval)
{
   mOutfile << str << sval << endl;
   cout << str << sval << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &st1, const std::string &str2, const std::string &str3)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str1, const std::string &str2,
                     const std::string &str3)
{
   mOutfile << str1 << str2 << str3 << endl;
   cout << str1 << str2 << str3 << endl;
}


//------------------------------------------------------------------------------
// void Put(int ival1, const std::string &str1)
//------------------------------------------------------------------------------
void TestOutput::Put(int ival1, const std::string &str1)
{
   mOutfile << ival1 << str1 << endl;
   cout << ival1 << str1 << endl;
}


//------------------------------------------------------------------------------
// void Validate(bool actual, bool expect, bool validate = true)
//------------------------------------------------------------------------------
void TestOutput::Validate(bool actual, bool expect, bool validate)
{
   std::string actualStr, expectStr;
   
   if (actual)
      actualStr = "true";
   else
      actualStr = "false";

   if (expect)
      expectStr = "true";
   else
      expectStr = "false";

   mOutfile << actualStr << endl << endl;
   cout << actualStr << endl << endl;
   
   if (validate)
   {
      if (actual != expect)
         throw GmatBaseException
            (">>>>> The expected result is : " + expectStr + ", but got " +
             actualStr + "\n");
   }
}


//------------------------------------------------------------------------------
// void Validate(const std::string &actual, const std::string &expect,
//               bool validate = true)
//------------------------------------------------------------------------------
void TestOutput::Validate(const std::string &actual, const std::string &expect,
                          bool validate)
{
   mOutfile << actual << endl << endl;
   cout << actual << endl << endl;
   
   if (validate)
   {
      if (actual != expect)
         throw GmatBaseException
            (">>>>> The expected result is \"" + expect + "\", but got \"" +
             actual + "\"\n");
   }
}


//------------------------------------------------------------------------------
// void Validate(const std::string &actual1, const std::string &actual2,
//               const std::string &expect1, const std::string &expect2,
//               bool validate = true)
//------------------------------------------------------------------------------
void TestOutput::Validate(const std::string &actual1, const std::string &actual2,
                          const std::string &expect1, const std::string &expect2,
                          bool validate)
{
   mOutfile << actual1 << ", " << actual2 << endl << endl;
   cout << actual1 << ", " << actual2 << endl << endl;
   
   if (validate)
   {
      if (actual1 != expect1 || actual2 != expect2)
         throw GmatBaseException
            (">>>>> The expected result is : " + expect1 + ", " + expect2 +
             ", but got " + actual1 + ", " + actual2 + "\n");
   }
}


#if !defined __TEST_NON_REAL__
//------------------------------------------------------------------------------
// void Validate(int actual, int expect, bool validate = true)
//------------------------------------------------------------------------------
void TestOutput::Validate(int actual, int expect, bool validate)
{
   std::string actualStr, expectStr;
   
   mOutfile << actual << endl << endl;
   cout << actual << endl << endl;
   
   if (validate)
   {
      if (actual != expect)
         throw GmatBaseException
            (">>>>> The expected result is : " + GmatRealUtil::ToString(expect) +
             ", but got " + GmatRealUtil::ToString(actual) + "\n");
   }
}


//------------------------------------------------------------------------------
// void Validate(int actual1, int actual2, int expect1, int expect2,
//               bool validate = true)
//------------------------------------------------------------------------------
void TestOutput::Validate(int actual1, int actual2, int expect1, int expect2,
                          bool validate)
{
   std::string actualStr1, expectStr1;
   std::string actualStr2, expectStr2;
   
   mOutfile << actual1 << ", " << actual2 <<endl << endl;
   cout << actual1 << ", " << actual2 << endl << endl;
   
   if (validate)
   {
      if (actual1 != expect1 || actual2 != expect2)
         throw GmatBaseException
            (">>>>> The expected result is : " + GmatRealUtil::ToString(expect1) +
             ", " + GmatRealUtil::ToString(expect2) + ", but got " +
             GmatRealUtil::ToString(actual1) + GmatRealUtil::ToString(actual2) + "\n");
   }
}


//------------------------------------------------------------------------------
// void SetPrecision(int p)
//------------------------------------------------------------------------------
void TestOutput::SetPrecision(int p)
{
   mOutfile.precision(p);
   cout.precision(p);
   mPrecision = p;
   mOutfile.setf(ios::right | ios::showpoint);
}


//------------------------------------------------------------------------------
// void Put(Real rval)
//------------------------------------------------------------------------------
void TestOutput::Put(Real rval)
{
   mOutfile << rval << endl;
   cout << rval << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str, Real rval1, Real rval2, Real rval3)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str, Real rval1, Real rval2, Real rval3)
{
   mOutfile << str << rval1 << "  " << rval2 << "  " << rval3 << endl;
   cout << str << rval1 << "  " << rval2 << "  " << rval3 << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str1, Real rvalx, const std::string &str2,
//          Real rval1, Real rval2, Real rval3)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str1, Real rvalx, const std::string &str2,
                     Real rval1, Real rval2, Real rval3)
{
   mOutfile << str1 << rvalx << str2 << rval1 << "  " << rval2 << "  " << rval3 << endl;
   cout << str1 << rvalx << str2 << rval1 << "  " << rval2 << "  " << rval3 << endl;
}


//------------------------------------------------------------------------------
// void Put(Real rval1, const std::string &str, Real rval2)
//------------------------------------------------------------------------------
void TestOutput::Put(Real rval1, const std::string &str, Real rval2)
{
   mOutfile << rval1 << str << rval2 << endl;
   cout << rval1 << str << rval2 << endl;
}


//------------------------------------------------------------------------------
// void Put(Real rval1, const std::string &str1,
//          Real rval2, const std::string &str2)
//------------------------------------------------------------------------------
void TestOutput::Put(Real rval1, const std::string &str1,
                     Real rval2, const std::string &str2)
{
   mOutfile << rval1 << str1 << rval2 << str2 << endl;
   cout << rval1 << str1 << rval2 << str2 << endl;
}


//------------------------------------------------------------------------------
// void Put(Real rval1, const std::string &str1,
//          Real rval2, const std::string &str2, Real rval3)
//------------------------------------------------------------------------------
void TestOutput::Put(Real rval1, const std::string &str1,
                     Real rval2, const std::string &str2, Real rval3)
{
   mOutfile.width(mWidth);
   mOutfile.precision(mPrecision);
   cout.width(mWidth);
   cout.precision(mPrecision);
   
   mOutfile << rval1 << str1 << rval2 << str2 << rval3 << endl;
   cout << rval1 << str1 << rval2 << str2 << rval3 << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str, Real rval)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str, Real rval)
{
   mOutfile << str << rval << endl;
   cout << str << rval << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str1, Real rval1,
//          const std::string &str2, Real rval2)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str1, Real rval1,
                     const std::string &str2, Real rval2)
{
   mOutfile << str1 << rval1 << str2 << rval2 << endl;
   cout << str1 << rval1 << str2 << rval2 << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str1, Real rval1, const std::string &str2)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str1, Real rval1, const std::string &str2)
{
   mOutfile << str1 << rval1 << str2 << endl;
   cout << str1 << rval1 << str2 << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str, const Rmatrix &rmat)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str, const Rmatrix &rmat)
{
   mOutfile << str << rmat << endl;
   cout << str << rmat << endl;
}


//------------------------------------------------------------------------------
// void Put(const Rmatrix &rmat)
//------------------------------------------------------------------------------
void TestOutput::Put(const Rmatrix &rmat)
{
   mOutfile << rmat << endl;
   cout << rmat << endl;
}


//------------------------------------------------------------------------------
// void Put(const Rvector &rvec)
//------------------------------------------------------------------------------
void TestOutput::Put(const Rvector &rvec)
{
   mOutfile << rvec << endl;
   cout << rvec << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str, const Rvector &rvec)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str, const Rvector &rvec)
{
   mOutfile << str << rvec << endl;
   cout << str << rvec << endl;
}


//------------------------------------------------------------------------------
// void Put(const std::string &str, const std::string &sval)
//------------------------------------------------------------------------------
void TestOutput::Put(const std::string &str, const std::string &sval)
{
   if (mAddNewLine)
   {
      mOutfile << str << sval << endl;
      cout << str << sval << endl;
   }
   else
   {
      mOutfile << str << sval;
      cout << str << sval;
   }
}


//------------------------------------------------------------------------------
// void CheckValue(Real actual, Real expect, Real tol = TEST_TOL)
//------------------------------------------------------------------------------
void TestOutput::CheckValue(Real actual, Real expect, Real tol)
{
   if (fabs(actual - expect) > tol)
   {
      throw GmatBaseException
         (">>>>> The expected result is : " + GmatRealUtil::ToString(expect) +
          ", but got " + GmatRealUtil::ToString(actual) + "\n");
   }
   else
   {
      if (tol > 1.0e-5)
      {
         mOutfile << "===> test passed with tol = " << tol << endl;
         cout << "===> test passed with tol = " << tol << endl;
      }
   }
}


//------------------------------------------------------------------------------
// void Validate(Real actual, Real expect, real tol = 1.0e-5, bool validate = true)
//------------------------------------------------------------------------------
void TestOutput::Validate(Real actual, Real expect, Real tol, bool validate)
{
   mOutfile << actual << endl;
   cout << actual << endl;
   
   if (validate)
      CheckValue(actual, expect, tol);
   
   mOutfile << endl;
   cout << endl;

}


//------------------------------------------------------------------------------
// void Validate(Real actual1, Real actual2, Real actual3,
//               Real expect1, Real expect2, Real expect3, Real tol = TEST_TOL,
//               bool validate = true);
//------------------------------------------------------------------------------
void TestOutput::Validate(Real actual1, Real actual2, Real actual3,
                          Real expect1, Real expect2, Real expect3, Real tol,
                          bool validate)
{
   mOutfile << "Actual values are " << actual1 << "  " << actual2 << "  " << actual3 << endl;
   cout << "Actual values are " << actual1 << "  " << actual2 << "  " << actual3 << endl;
   mOutfile << "Expect values are " << expect1 << "  " << expect2 << "  " << expect3 << endl;
   cout << "Expect values are " << expect1 << "  " << expect2 << "  " << expect3 << endl;
   
   if (validate)
   {
      CheckValue(actual1, expect1, tol);
      CheckValue(actual2, expect2, tol);
      CheckValue(actual3, expect3, tol);
   }
   
   mOutfile << endl;
   cout << endl;
}


//------------------------------------------------------------------------------
// void Validate(Rmatrix actual, Rmatrix expect, Real tol = TEST_TOL,
//               bool validate = true)
//------------------------------------------------------------------------------
void TestOutput::Validate(Rmatrix actualMat, Rmatrix expectMat, Real tol,
                          bool validate)
{
   mOutfile << actualMat << endl;
   cout << actualMat << endl;
   
   if (validate)
   {
      if (actualMat != expectMat)
      {
         std::stringstream ss("");
         ss << ">>>>> \nThe expected result is\n";
         ss << setw(20) << setprecision(20) << expectMat;
         ss << ", but got\n";
         ss << actualMat << "\n";
         
         throw GmatBaseException(ss.str());
      }
   }
   
   mOutfile << endl;
   cout << endl;
}


//------------------------------------------------------------------------------
// void Validate(Rvector actual, Rvector expect, Real tol = TEST_TOL,
//               bool validate = true)
//------------------------------------------------------------------------------
void TestOutput::Validate(Rvector actual, Rvector expect, Real tol,
                          bool validate)
{
   mOutfile << actual << endl;
   cout << actual << endl;

   if (validate)
   {
      if (actual != expect)
      {
         std::stringstream ss("");
         ss << ">>>>> \nThe expected result is\n";
         ss << setw(20) << setprecision(20) << expect;
         ss << ", but got\n";
         ss << actual << "\n";

         throw GmatBaseException(ss.str());
      }
   }

   mOutfile << endl;
   cout << endl;
}


#endif


