//------------------------------------------------------------------------------
//                           CsaltTestDriver
//------------------------------------------------------------------------------
// GMAT:  General Mission Analysis Tool
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jun 7, 2017
/**
 * Base class for test case drivers for CSALT
 */
//------------------------------------------------------------------------------

#ifndef CsaltTestDriver_hpp
#define CsaltTestDriver_hpp

#include "csalt.hpp"

/**
 * CSALT test cases are derived from this class, which provides core functionality.
 */
class CsaltTestDriver
{
public:
   CsaltTestDriver(const std::string &testName);
   CsaltTestDriver(const std::string &testName, const Integer snoptConsoleOutputLevel);
   virtual ~CsaltTestDriver();
   CsaltTestDriver(const CsaltTestDriver& driver);
   CsaltTestDriver& operator=(const CsaltTestDriver& driver);

   enum Verbosity
   {
      SILENT,
      BASIC,
      VERBOSE,
      VERBOSE_DEBUG
   };

   virtual Integer Run();

protected:
   /// Name for the test case, used to set file names and output identity
   std::string testName;
   /// Output verbosity setting: silent = 0, basic = 1, verbose = 2
   Verbosity   verbosity;

   Trajectory *traj;
   UserPathFunction *pathObject;
   UserPointFunction *pointObject;
   Real costLowerBound;
   Real costUpperBound;
   Rvector majorOptimalityTolerances;
   IntegerArray majorIterationsLimits;
   IntegerArray totalIterationsLimits;
   Rvector feasibilityTolerances;
   Integer maxMeshRefinementCount;
   StringArray optimizationMode;
   std::vector<Phase*> phaseList;
   bool generateOptimizationOutput;
   std::string optimizationOutputFile;
   bool writeControlHistory;
   std::string controlHistoryFile;
   Integer snoptConsoleOutputLevel;

   // A useful value
   static const Real INF;

   // Required method that sets the path & point objects, & other run properties
   virtual void SetPointPathAndProperties() = 0;
   // Required method to set up the phase(s)
   virtual void SetupPhases() = 0;

   virtual Real GetMaxError(const Rvector &vec);
   virtual Real GetMaxError(const Rmatrix &mat);
};

#endif /* CsaltTestDriver_hpp */
