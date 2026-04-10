//------------------------------------------------------------------------------
//                              SNOPTFunctionWrapper
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
// Author: Jeremy Knittel / NASA
// Created: 2016.08.09
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "SNOPTFunctionWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SNOPT_FUNCTION

//------------------------------------------------------------------------------
// void SNOPTFunctionWrapper(SNOPT_INTEGER  *Status,
//                           SNOPT_INTEGER  *n,
//                           SNOPT_DOUBLE   x[],
//                           SNOPT_INTEGER  *needF,
//                           SNOPT_INTEGER  *nF,
//                           SNOPT_DOUBLE   F[],
//                           SNOPT_INTEGER  *needG,
//                           SNOPT_INTEGER  *neG,
//                           SNOPT_DOUBLE   G[],
//                           char           *cu,
//                           SNOPT_INTEGER  *lencu,
//                           SNOPT_INTEGER  iu[],
//                           SNOPT_INTEGER  *leniu,
//                           SNOPT_DOUBLE   ru[],
//                           SNOPT_INTEGER  *lenru)
//------------------------------------------------------------------------------
/**
 * This method is a wrapper for the SNOPT function
 *
 * @param <> various inputs/outputs to/from the SNOPT function
 */
//------------------------------------------------------------------------------
void SNOPTFunctionWrapper(SNOPT_INTEGER  *Status,
						  SNOPT_INTEGER  *n,
						  SNOPT_DOUBLE   x[],
						  SNOPT_INTEGER  *needF,
						  SNOPT_INTEGER  *nF,
						  SNOPT_DOUBLE   F[],
						  SNOPT_INTEGER  *needG,
						  SNOPT_INTEGER  *neG,
						  SNOPT_DOUBLE   G[],
						  char           *cu,
						  SNOPT_INTEGER  *lencu,
						  SNOPT_INTEGER  iu[],
						  SNOPT_INTEGER  *leniu,
						  SNOPT_DOUBLE   ru[],
						  SNOPT_INTEGER  *lenru)
{
	// Get a pointer to the optimization object so that 
	// we can access the sparsity pattern and get a pointer
	// to the trajectory object so we can evaluate the 
	// function values
	SnoptOptimizer * Opt = (SnoptOptimizer*) ru;
	
   //SNOPT_INTEGER statusSNOpt = -3;
   //*Status = -2;

	// Create GMAT vectors and matrices to hold data
	Rvector DecVec(*n);
	Rvector CostConstraint(*nF);
	RSMatrix Jacobian(*n,*nF);
		
	// Grab the new decision vector and put it into the Rvector
	for (int k = 0; k < *n; k++)
	{
		DecVec[k] = x[k];		
	}
	
	if (Opt->traj->GetIfScaling())
	{
		Opt->traj->GetScaleHelper()->UnScaleDecisionVector(DecVec);
	}
		
   #ifdef DEBUG_SNOPT_FUNCTION
      static bool firstTime = true;
      if (firstTime)
         MessageInterface::ShowMessage(
                        "SNOPTFunctionWrapper --- DecVec = %s\n",
                        DecVec.ToString(16).c_str());
   #endif
	// Use the Optimization pointer to update the decision vector
	// which automatically recalculates Jacobian and Function values
	Opt->traj->SetDecisionVector(DecVec);
	
	// Get the new cost and constraint function values
	CostConstraint = Opt->traj->GetCostConstraintFunctions();

   #ifdef DEBUG_SNOPT_FUNCTION
      if (firstTime)
         MessageInterface::ShowMessage(
         "SNOPTFunctionWrapper --- CostConstraint = %s\n",
         CostConstraint.ToString(16).c_str());
   #endif
	
	if (Opt->traj->GetIfScaling())
	{
		Opt->traj->GetScaleHelper()->ScaleCostConstraintVector(CostConstraint);	
	}
	
   #ifdef DEBUG_SNOPT_FUNCTION
      if (firstTime)
         MessageInterface::ShowMessage("SNOPTFunctionWrapper --- F:\n");
   #endif
	// Put the cost and constraint values into SNOPT's c array
	for (int k = 0; k < *nF; k++)
	{
		F[k] = CostConstraint[k];
	}
   #ifdef DEBUG_SNOPT_FUNCTION
      if (firstTime)
      {
         for (int k = 0; k < *nF; k++)
         {
            MessageInterface::ShowMessage("------ (%d)  = %12.10f\n", k, F[k]);
         }
      }
   #endif
	
	// Get the Jacobian Values
	Jacobian = Opt->traj->GetJacobian();
	
	if (Opt->traj->GetIfScaling())
	{
      #ifdef DEBUG_SNOPT_FUNCTION
         if (firstTime)
            MessageInterface::ShowMessage(
                              "SNOPTFunctionWrapper thinks it's scaling?!?!\n");
      #endif
		Opt->traj->GetScaleHelper()->ScaleJacobian(Jacobian);
	}
	
   #ifdef DEBUG_SNOPT_FUNCTION
      if (firstTime)
      {
         MessageInterface::ShowMessage(
                                 "SNOPTFunctionWrapper --- Opt->iGfun:\n");
         for (Integer ii = 0; ii < Opt->iGfun.size(); ii++)
            MessageInterface::ShowMessage("iGfun(%d)  = %d\n",
                                          ii+1, Opt->iGfun.at(ii));
         MessageInterface::ShowMessage(
                                 "SNOPTFunctionWrapper --- Opt->jGvar:\n");
         for (Integer ii = 0; ii < Opt->jGvar.size(); ii++)
            MessageInterface::ShowMessage("jGvar(%d)  = %d\n",
                                          ii+1, Opt->jGvar.at(ii));

         MessageInterface::ShowMessage("SNOPTFunctionWrapper G:\n");
      }
   #endif
	// Put the Jacobian Values in the SNOPT's c array
	for (UnsignedInt k = 0; k < Opt->iGfun.size(); k++)
	{
      // -1 here to 'fix' indexed
		G[k] = Jacobian(Opt->iGfun[k]-1,Opt->jGvar[k]-1);
	}
   #ifdef DEBUG_SNOPT_FUNCTION
      if (firstTime)
      {
         for (UnsignedInt k = 0; k < Opt->iGfun.size(); k++)
         {
            MessageInterface::ShowMessage("Jacobian(%d)  = %12.10f\n", k, G[k]);
         }
      }
      firstTime = false;
   #endif
	
   // Check if the optimization should be stopped
   bool stopOpt = Opt->traj->GetIfStopping();
   if (stopOpt)
      *Status = -2;
}

//------------------------------------------------------------------------------
//void StopOptimizer(SNOPT_INTEGER *iAbort, SNOPT_INTEGER KTcond[],
//           SNOPT_INTEGER *MjrPrt, SNOPT_INTEGER *minimz, SNOPT_INTEGER *m,
//           SNOPT_INTEGER *maxS, SNOPT_INTEGER *n, SNOPT_INTEGER *nb,
//           SNOPT_INTEGER *nnCon0, SNOPT_INTEGER *nnCon, SNOPT_INTEGER *nnObj0,
//           SNOPT_INTEGER *nnObj, SNOPT_INTEGER *nS, SNOPT_INTEGER *itn,
//           SNOPT_INTEGER *nMajor, SNOPT_INTEGER *nMinor, SNOPT_INTEGER *nSwap,
//           SNOPT_DOUBLE *condHz, SNOPT_INTEGER *iObj, SNOPT_DOUBLE *sclObj,
//           SNOPT_DOUBLE *ObjAdd, SNOPT_DOUBLE *fMrt, SNOPT_DOUBLE *PenNrm,
//           SNOPT_DOUBLE *step, SNOPT_DOUBLE *prInf, SNOPT_DOUBLE *duInf,
//           SNOPT_DOUBLE *vimax, SNOPT_DOUBLE *virel, SNOPT_INTEGER hs[],
//           SNOPT_INTEGER *ne, SNOPT_INTEGER *nlocJ, SNOPT_INTEGER locJ[],
//           SNOPT_INTEGER indJ[], SNOPT_DOUBLE Jcol[], SNOPT_INTEGER *negCon,
//           SNOPT_DOUBLE Ascale[], SNOPT_DOUBLE bl[], SNOPT_DOUBLE bu[],
//           SNOPT_DOUBLE fCon[], SNOPT_DOUBLE gCon[], SNOPT_DOUBLE gObj[],
//           SNOPT_DOUBLE yCon[], SNOPT_DOUBLE pi[], SNOPT_DOUBLE rc[],
//           SNOPT_DOUBLE rg[], SNOPT_DOUBLE x[], char cu[],
//           SNOPT_INTEGER *lencu, SNOPT_INTEGER iu[], SNOPT_INTEGER *leniu,
//           SNOPT_DOUBLE ru[], SNOPT_INTEGER *lenru, char cw[],
//           SNOPT_INTEGER *lencw, SNOPT_INTEGER iw[], SNOPT_INTEGER *leniw,
//           SNOPT_DOUBLE rw[], SNOPT_INTEGER *lenrw)
//------------------------------------------------------------------------------
/**
 * This method is an snSTOP method called after every iteration of the
 * optimizer
 *
 * @param <> various inputs/outputs to/from the SNOPT function
 */
 //-----------------------------------------------------------------------------
void StopOptimizer(SNOPT_INTEGER *iAbort, SNOPT_INTEGER KTcond[],
             SNOPT_INTEGER *MjrPrt, SNOPT_INTEGER *minimz, SNOPT_INTEGER *m,
             SNOPT_INTEGER *maxS, SNOPT_INTEGER *n, SNOPT_INTEGER *nb,
             SNOPT_INTEGER *nnCon0, SNOPT_INTEGER *nnCon, SNOPT_INTEGER *nnObj0,
             SNOPT_INTEGER *nnObj, SNOPT_INTEGER *nS, SNOPT_INTEGER *itn,
             SNOPT_INTEGER *nMajor, SNOPT_INTEGER *nMinor, SNOPT_INTEGER *nSwap,
             SNOPT_DOUBLE *condHz, SNOPT_INTEGER *iObj, SNOPT_DOUBLE *sclObj,
             SNOPT_DOUBLE *ObjAdd, SNOPT_DOUBLE *fMrt, SNOPT_DOUBLE *PenNrm,
             SNOPT_DOUBLE *step, SNOPT_DOUBLE *prInf, SNOPT_DOUBLE *duInf,
             SNOPT_DOUBLE *vimax, SNOPT_DOUBLE *virel, SNOPT_INTEGER hs[],
             SNOPT_INTEGER *ne, SNOPT_INTEGER *nlocJ, SNOPT_INTEGER locJ[],
             SNOPT_INTEGER indJ[], SNOPT_DOUBLE Jcol[], SNOPT_INTEGER *negCon,
             SNOPT_DOUBLE Ascale[], SNOPT_DOUBLE bl[], SNOPT_DOUBLE bu[],
             SNOPT_DOUBLE fCon[], SNOPT_DOUBLE gCon[], SNOPT_DOUBLE gObj[],
             SNOPT_DOUBLE yCon[], SNOPT_DOUBLE pi[], SNOPT_DOUBLE rc[],
             SNOPT_DOUBLE rg[], SNOPT_DOUBLE x[], char cu[],
             SNOPT_INTEGER *lencu, SNOPT_INTEGER iu[], SNOPT_INTEGER *leniu,
             SNOPT_DOUBLE ru[], SNOPT_INTEGER *lenru, char cw[],
             SNOPT_INTEGER *lencw, SNOPT_INTEGER iw[], SNOPT_INTEGER *leniw,
             SNOPT_DOUBLE rw[], SNOPT_INTEGER *lenrw)
{
   SnoptOptimizer * Opt = (SnoptOptimizer*) cw;
   std::string appType = Opt->traj->GetApplicationType();

   // Check if this was a feasible solution and if optimality has improved
   Opt->traj->CheckSolutionFeasibility(*duInf, *vimax);
   Opt->traj->UpdateGuessForMesh();

   if (appType != "Console")
   {
      // Print the data
      if (*nMajor % 10 == 0)
      {
         MessageInterface::ShowMessage("Major Minors     Step "
            "Feasible  Optimal    MeritFunction     nS\n");
      }

      MessageInterface::ShowMessage("%5d %6d %8.1e %8.1e %8.1e %16.8e %6d\n", *nMajor,
         *nMinor, *prInf, *duInf, *vimax, *PenNrm, *nS);
   }
   else
   {
      // Print the data in the log only
      MessageInterface::ToggleConsolePrinting(false);
      if (*nMajor % 10 == 0)
      {
         MessageInterface::LogMessage("Major Minors     Step "
            "Feasible  Optimal    MeritFunction     nS\n");
      }

      MessageInterface::LogMessage("%5d %6d %8.1e %8.1e %8.1e %16.8e %6d\n", *nMajor,
         *nMinor, *prInf, *duInf, *vimax, *PenNrm, *nS);
      MessageInterface::ToggleConsolePrinting(true);
   }
   Opt->SetCurrentIterationData(*itn, *nMajor, *PenNrm);
}
