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

#include "CsaltTestDriver.hpp"
#include "BaseException.hpp"

//#define DEBUG_SHOWRESULTS
const Real CsaltTestDriver::INF = std::numeric_limits<Real>::infinity();


//------------------------------------------------------------------------------
// CsaltTestDriver()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
CsaltTestDriver::CsaltTestDriver(const std::string &name) :
   testName                   (name),
   verbosity                  (VERBOSE),
   traj                       (NULL),
   pathObject                 (NULL),
   pointObject                (NULL),
   costLowerBound             (-INF),
   costUpperBound             (INF),
   maxMeshRefinementCount     (0),
   generateOptimizationOutput (true),
   writeControlHistory        (true),
   snoptConsoleOutputLevel (1)
{
   optimizationOutputFile = testName + "Data.txt";
   controlHistoryFile = testName + ".och";
   majorOptimalityTolerances = Rvector(1, 1.0E-4);
   majorIterationsLimits = IntegerArray(1,3000);
   totalIterationsLimits = IntegerArray(1,300000);
   feasibilityTolerances = Rvector(1, 1.0E-6);
   optimizationMode = StringArray(1, "Minimize");
}

//------------------------------------------------------------------------------
// CsaltTestDriver()
//------------------------------------------------------------------------------
/**
 * Constructor that sets value of snoptConsoleOutputLevel via user input
 */
 //------------------------------------------------------------------------------
CsaltTestDriver::CsaltTestDriver(const std::string &name, const Integer snoptConsoleOutputLevel) :
	testName(name),
	verbosity(VERBOSE),
	traj(NULL),
	pathObject(NULL),
	pointObject(NULL),
	costLowerBound(-INF),
	costUpperBound(INF),
	maxMeshRefinementCount(0),
	generateOptimizationOutput(true),
	writeControlHistory(true),
	snoptConsoleOutputLevel(snoptConsoleOutputLevel)
{
	optimizationOutputFile = testName + "Data.txt";
	controlHistoryFile = testName + ".och";
	majorOptimalityTolerances = Rvector(1, 1.0E-4);
	majorIterationsLimits = IntegerArray(1, 3000);
	totalIterationsLimits = IntegerArray(1, 300000);
	feasibilityTolerances = Rvector(1, 1.0E-6);
	optimizationMode = StringArray(1, "Minimize");
}

//------------------------------------------------------------------------------
// ~CsaltTestDriver()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
CsaltTestDriver::~CsaltTestDriver()
{
   // Path and point objects are no longer deleted in trajectory to avoid 
   // double delete issues, delete them here
   if (traj)
      delete traj;
   if (pathObject)
      delete pathObject;
   if (pointObject)
      delete pointObject;
   for (Integer ii = 0; ii < phaseList.size(); ++ii)
      if (phaseList.at(ii))
         delete phaseList.at(ii);
}

//------------------------------------------------------------------------------
// CsaltTestDriver::CsaltTestDriver(const CsaltTestDriver& driver)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
CsaltTestDriver::CsaltTestDriver(const CsaltTestDriver& driver)
{
}

//------------------------------------------------------------------------------
// CsaltTestDriver& CsaltTestDriver::operator =(const CsaltTestDriver& driver)
//------------------------------------------------------------------------------
/**
 * Assignment operator (Likely unused)
 *
 * @param driver The object providing data for this one
 *
 * @return This object, set to match driver
 */
//------------------------------------------------------------------------------
CsaltTestDriver& CsaltTestDriver::operator=(const CsaltTestDriver& driver)
{
   // Currently nothing to do here
   return *this;
}

//------------------------------------------------------------------------------
// Integer CsaltTestDriver::Run()
//------------------------------------------------------------------------------
/**
 * The entry point for the test case
 *
 * @return A code indicating teh state of the run
 */
//------------------------------------------------------------------------------
Integer CsaltTestDriver::Run()
{
   Integer retval = 0;

   std::string outFormat = "%16.9f ";

   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "CsaltLog.txt");

   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);

   if (verbosity != SILENT)
   {
      MessageInterface::ShowMessage("%s\n",
            GmatTimeUtil::FormatCurrentTime().c_str());
      MessageInterface::ShowMessage("\n*** Running the %s CSALT problem ***\n",
            testName.c_str());
   }

   try
   {
      // Create the trajectory and set core settings
      traj = new Trajectory();
      SetPointPathAndProperties();
      if (pathObject)
         traj->SetUserPathFunction(pathObject);
      if (pointObject)
         traj->SetUserPointFunction(pointObject);
      traj->SetMajorIterationsLimit(majorIterationsLimits);
      traj->SetOptimalityTolerances(majorOptimalityTolerances);
      traj->SetFeasibilityTolerances(feasibilityTolerances);
      traj->SetTotalIterationsLimit(totalIterationsLimits);
      traj->SetOptimizationMode(optimizationMode);
      traj->SetCostLowerBound(costLowerBound);
      traj->SetCostUpperBound(costUpperBound);
      traj->SetMaxMeshRefinementCount(maxMeshRefinementCount);
      traj->SetFailedMeshOptimizationAllowance(false);
      traj->SetMeshRefinementGuessMode("LastSolutionMostRecentMesh");
	  traj->SetSnoptConsoleOutputLevel(this->snoptConsoleOutputLevel);
   
      SetupPhases();

      if (phaseList.size() == 0)
         retval = -1;
      else
      {
         traj->SetPhaseList(phaseList);

         if (verbosity == VERBOSE)
            MessageInterface::ShowMessage("*** TEST *** initializing the Trajectory\n");
         traj->Initialize();

         if (verbosity == VERBOSE)
            MessageInterface::ShowMessage("*** TEST *** setting up the call to Optimize!!\n");

         Rvector  dv2      = traj->GetDecisionVector();
         Rvector  C        = traj->GetCostConstraintFunctions(); // nonDim);

         if (verbosity == VERBOSE_DEBUG)
         {
            RSMatrix conSp    = phaseList[0]->GetConSparsityPattern();

            MessageInterface::ShowMessage("*** TEST *** Con sparsity pattern from Phase1:\n");
            Integer rJac = conSp.size1();
            Integer cJac = conSp.size2();
            for (Integer cc = 0; cc < cJac; cc++)
            {
               for (Integer rr = 0; rr < rJac; rr++)
               {
                  Real jacTmp = conSp(rr,cc);
                  if (jacTmp != 0.0)
                  MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n", rr+1, cc+1, jacTmp);
               }
            }
         }

         // ------------------ Optimizing --------------------------------------------
         Rvector z = dv2;
         Rvector F(C.GetSize());
         Rvector xmul(dv2.GetSize());
         Rvector Fmul(C.GetSize());
         Integer exitFlag;

         if (verbosity == VERBOSE)
            MessageInterface::ShowMessage("*** TEST *** Optimizing!!\n");

         if (generateOptimizationOutput)
            traj->Optimize(z, F, xmul, Fmul, exitFlag, optimizationOutputFile);
         else
            traj->Optimize(z, F, xmul, Fmul, exitFlag);

         if (writeControlHistory)
            traj->WriteToFile(controlHistoryFile);


            #ifdef DEBUG_SHOWRESULTS
               MessageInterface::ShowMessage("*** TEST *** z:\n%s\n",
                                             z.ToString(12).c_str());
               MessageInterface::ShowMessage("*** TEST *** F:\n%s\n",
                                             F.ToString(12).c_str());
               MessageInterface::ShowMessage("*** TEST *** xmul:\n%s\n",
                                             xmul.ToString(12).c_str());
               MessageInterface::ShowMessage("*** TEST *** Fmul:\n%s\n",
                                             Fmul.ToString(12).c_str());
               MessageInterface::ShowMessage("*** TEST *** Optimization complete!!\n");

               Phase *phase1 = traj->GetPhaseList()[0];
               Rvector dvP1 = phase1->GetDecVector();
               MessageInterface::ShowMessage("*** TEST *** dvP1:\n%s\n",
                                             dvP1.ToString(12).c_str());

               // Interpolate solution
               Rvector timeVector = phase1->GetTimeVector();
               DecisionVector *dv = phase1->GetDecisionVector();
               Rmatrix stateSol   = dv->GetStateArray();
               Rmatrix controlSol = dv->GetControlArray();
               Rvector staticSol = dv->GetStaticVector();
            #endif

         if (verbosity != SILENT)
            MessageInterface::ShowMessage("*** END %s TEST ***\n", testName.c_str());
      } // Phase[0] exists

   }
   catch (BaseException &ex)
   {
      MessageInterface::ShowMessage("Caught a CSALT Exception:\n\n%s\n\n",
            ex.GetFullMessage().c_str());
   }

   return retval;
}


//------------------------------------------------------------------------------
// Protected Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Real GetMaxValue(const Rvector &vec)
//------------------------------------------------------------------------------
/**
 * Finds the largest value in an Rvector
 *
 * @param vec The Rvector
 *
 * @return The largest value
 */
//------------------------------------------------------------------------------
Real CsaltTestDriver::GetMaxError(const Rvector &vec)
{
   Real max = -999.99;

   for (Integer ii = 0; ii < vec.GetSize(); ++ii)
      if (vec(ii) > max)
         max = vec(ii);

   return max;
}

//------------------------------------------------------------------------------
// Real GetMaxValue(const Rmatrix &mat)
//------------------------------------------------------------------------------
/**
 * Finds the largest value in an Rmatrix
 *
 * @param mat The Rmatrix
 *
 * @return The largest value
 */
//------------------------------------------------------------------------------
Real CsaltTestDriver::GetMaxError(const Rmatrix &mat)
{
   Real max = -999.99;
   Integer r, c;
   mat.GetSize(r,c);

   for (Integer ii = 0; ii < r; ++ii)
      for (Integer jj = 0; jj < c; ++jj)
         if (mat(ii,jj) > max)
            max = mat(ii,jj);

   return max;
}


