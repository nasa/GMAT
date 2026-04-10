//------------------------------------------------------------------------------
//                                  TestCBUpdates
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
// Author: Wendy Shoan
// Created: 2004/08/31
//
/**
 * Test driver for Solar System stuff (testing updates for AtmosphereFactory)
 */
//------------------------------------------------------------------------------
#include <map>
#include <iostream>
#include <string>
#include <list>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "EopFile.hpp"
#include "Rmatrix.hpp"
#include "TimeTypes.hpp"

using namespace std;
using namespace GmatTimeUtil;


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   
   cout.setf(ios::fixed);
   cout.precision(16);

   cout << "============================== Test EopFile =========================" << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

   std::string eopFileName = "/GMAT/dev/datafiles/EOPFiles/eopc04.62-now";
   //std::string eopFileName = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/eop/eopc04.62-now";
   EopFile*    eop = new EopFile(eopFileName);

   cout << "EOP File has been created to read EOpC04 file! .............." << endl;

   cout << "NOW trying to initialize ..........." << endl;

   try
   {
      eop->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "ERROR - " << be.GetMessage() << endl;
   }

   Real useMjd;

   bool OK  = true;
   Real off = -999.9;
   Real ir;
   Real x, y, lod;
   Real utcMjd = 37665.0;  // jan 1, 1962
   for (ir = utcMjd; ir < 53402.0; ir+=700.0)
   {
      off = eop->GetUt1UtcOffset(ir);
      cout << "Offset for time " << ir << " is " << off << endl;
   }
   cout << "Now try to get results in between days" << endl;
   ir = 37000.0;
   off = eop->GetUt1UtcOffset(ir);
   OK  = eop->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   ir = 54000.0;
   off = eop->GetUt1UtcOffset(ir);
   OK  = eop->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   ir = 40000.5;
   off = eop->GetUt1UtcOffset(ir);
   OK  = eop->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   /*
   Rmatrix pm = eop->GetPolarMotionData();
   Integer r, c;
   pm.GetSize(r,c);
   Integer i;
   for (i = 0; i < r; i+=100)
   {
      if (pm.GetElement(i,0) == 0.0) useMjd = 0.0;
      else                          useMjd = pm.GetElement(i,0) - JD_MJD_OFFSET;
      cout << "row " << i << "::  mjd = " << useMjd << " ; x = "
      << pm.GetElement(i,1) << " ; y = " << pm.GetElement(i,2) << endl;
      cout << "......... and for mjd " << useMjd << " lod = " << pm.GetElement(i,3) << endl;
   }
    */

   cout << "....................................................." << endl;
   cout << "....................................................." << endl;
   
   std::string eopFinals = "/GMAT/dev/datafiles/EOPFiles/finals.data";
   //std::string eopFinals = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/eop/finals.data";
   EopFile*    eop2 = new EopFile(eopFinals, GmatEop::FINALS);

   cout << "EOP File has been created to read finals.data file! .............." << endl;

   cout << "NOW trying to initialize ..........." << endl;

   try
   {
      eop2->Initialize();
   }
   catch (BaseException &be2)
   {
      cout << "ERROR - " << be2.GetMessage() << endl;
   }

   off = -999.9;
   utcMjd = 37665.0;  // jan 1, 1962
   for (ir = utcMjd; ir < 53402.0; ir+=700.0)
   {
      off = eop2->GetUt1UtcOffset(ir);
      cout << "Offset for time " << ir << " is " << off << endl;
   }
   cout << "Now try to get results in between days" << endl;
   ir = 37000.0;
   off = eop2->GetUt1UtcOffset(ir);
   OK  = eop2->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   ir = 54000.0;
   off = eop2->GetUt1UtcOffset(ir);
   OK  = eop2->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   ir = 48628.5;
   off = eop2->GetUt1UtcOffset(ir);
   OK  = eop2->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   ir = 53396.0;
   off = eop2->GetUt1UtcOffset(ir);
   OK  = eop2->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   ir = 53397.0;
   off = eop2->GetUt1UtcOffset(ir);
   OK  = eop2->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   ir = 53767.0;
   off = eop2->GetUt1UtcOffset(ir);
   OK  = eop2->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   ir = 53768.0;
   off = eop2->GetUt1UtcOffset(ir);
   OK  = eop2->GetPolarMotionAndLod(ir, x, y, lod);
   cout << "Offset for time " << ir << " is " << off << endl;
   cout << "........ and x = " << x << "; y = " << y << "; lod = " << lod << endl;
   /*
   pm = eop2->GetPolarMotionData();
   pm.GetSize(r,c);
   for (i = 0; i < r; i+=100)
   {
      if (pm.GetElement(i,0) == 0.0) useMjd = 0.0;
      else                           useMjd = pm.GetElement(i,0) - JD_MJD_OFFSET;
      cout << "row " << i << "::  mjd = " << useMjd << " ; x = "
      << pm.GetElement(i,1) << " ; y = " << pm.GetElement(i,2) << endl;
      cout << "......... and for mjd " << useMjd << " lod = " << pm.GetElement(i,3) << endl;
   }
    */

   delete eop;
   delete eop2;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   cout << "========================== End Test EopFile =========================" << endl;
   
}

