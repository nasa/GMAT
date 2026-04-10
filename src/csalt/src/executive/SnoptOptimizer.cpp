//------------------------------------------------------------------------------
//                              SnoptOptimizer
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

#include "SnoptOptimizer.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SnoptOptimizer

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
SnoptOptimizer::SnoptOptimizer(Trajectory* trajectory_in) :
	traj    (trajectory_in),
	iGfun   (),
	jGvar   (),
   Problem (),
   stopOptimization(false),
   totalNumIter(0),
   totalNumMajorIter(0),
   objFinalVal(0.0)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
SnoptOptimizer::SnoptOptimizer(const SnoptOptimizer &copy) :
   traj    (copy.traj),
   iGfun   (copy.iGfun),
   jGvar   (copy.jGvar),
   Problem (copy.Problem),
   stopOptimization(copy.stopOptimization),
   totalNumIter(copy.totalNumIter),
   totalNumMajorIter(copy.totalNumMajorIter),
   objFinalVal(copy.objFinalVal)

{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
SnoptOptimizer& SnoptOptimizer::operator=(const SnoptOptimizer &copy)
{
   if (&copy == this)
      return *this;
   
   traj    = copy.traj;
   iGfun   = copy.iGfun;
   jGvar   = copy.jGvar;
   Problem = copy.Problem;
   stopOptimization = copy.stopOptimization;
   totalNumIter = copy.totalNumIter;
   totalNumMajorIter = copy.totalNumMajorIter;
   objFinalVal = copy.objFinalVal;

   return *this;   
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
SnoptOptimizer::~SnoptOptimizer()
{
   // nothing to do here 
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the SnoptOptimizer
 *
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::Initialize()
{
    // Configure the SnoptOptimizer and prepare for use
	// Create the SNOPT problem and initialize the inputs
	
	// Set the derivative option          
   Problem.setIntParameter((char*)"Derivative Option",2);
   // Set the derivative verification level
   Problem.setIntParameter((char*)"Verify Level",-1);
   // Print CPU times at bottom of SNOPT print file
   Problem.setIntParameter((char*)"Timing level",3);
   //  Set the SNOPT internal scaling option
   Problem.setIntParameter((char*)"Scale Option", 1);
   //  Set tolerance on optimality
   Problem.setRealParameter((char*)"Major optimality tolerance",1e-4);
   //  Set tolerance on feasibility
   Problem.setRealParameter((char*)"Major feasibility tolerance",1e-6);
   //  Set number of iterations
   Problem.setIntParameter((char*)"Major iterations limit",3000);
   //  Set number of iterations
   Problem.setIntParameter((char*)"Iterations limit",300000);
   //  Set objective row
   Problem.setIntParameter((char*)"Objective row",1);
   //  Set the user function
   Problem.setUserFun(SNOPTFunctionWrapper);
   //  Set the problem name
   Problem.setProbName((char*)"CSALT");
   //  Set the stop method
   //isnSTOP stopMethod = StopOptimizer;
   Problem.setSTOP(StopOptimizer);
   //Problem.setIntParameter((char*)"Verify level", 3);
}

//------------------------------------------------------------------------------
// void SetScaling(bool ifScaling)
//------------------------------------------------------------------------------
/**
 * Sets the flag indicating whether or not use scaling
 *
 * @param <ifScaling>   is scaling requested?
 *
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::SetScaling(bool ifScaling)
{
	// Whether to use SNOPT's internal scaling or whether the 
	// problem has already been scaled
	if (ifScaling)	
	    Problem.setIntParameter((char*)"Scale Option",0);	
	else
	    Problem.setIntParameter((char*)"Scale Option",1);	
}

//------------------------------------------------------------------------------
// void SetSnoptOptimizerOutputFile(const std::string &optFile)
//------------------------------------------------------------------------------
/**
 * Sets the output file for the SNOPT SnoptOptimizer
 *
 * @param optFile The name of the output file
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::SetOptimizerOutputFile(const std::string &optFile)
{
   Problem.setPrintFile(optFile.c_str());
}

//------------------------------------------------------------------------------
// void SetFeasibiltyTol(Real feasTol)
//------------------------------------------------------------------------------
/**
 * Sets the feasibility tolerance for the SNOPT optimizer
 *
 * @param feasTol the feasibility tolerance
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::SetFeasibilityTolerance(Real feasTol)
{
   Problem.setRealParameter((char*)"Major feasibility tolerance",
                            feasTol);
}

//------------------------------------------------------------------------------
// void SetMajorOptimalityTol(Real optTol)
//------------------------------------------------------------------------------
/**
 * Sets the major optimality tolerance for the SNOPT optimizer
 *
 * @param optTol the major optimality tolerance
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::SetMajorOptimalityTolerance(Real optTol)
{
   Problem.setRealParameter((char*)"Major optimality tolerance", optTol);
}

//------------------------------------------------------------------------------
// void SetMajorIterationsLimit(Integer majorIterLimit)
//------------------------------------------------------------------------------
/**
 * Sets the major iterations limit for the SNOPT optimizer
 *
 * @param majorIterLimit the major iterations limit
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::SetMajorIterationsLimit(Integer majorIterLimit)
{
   Problem.setIntParameter((char*)"Major iterations limit", majorIterLimit);
}

//------------------------------------------------------------------------------
// void SetTotalIterationsLimit(Integer totalIterLimit)
//------------------------------------------------------------------------------
/**
 * Sets the total iterations limit for the SNOPT optimizer
 *
 * @param totalIterLimit the total iterations limit
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::SetTotalIterationsLimit(Integer totalIterLimit)
{
   Problem.setIntParameter((char*)"Iterations limit", totalIterLimit);
}

//------------------------------------------------------------------------------
// void SetOptimizationMode(const char* optMode)
//------------------------------------------------------------------------------
/**
 * Sets the optimization mode for the SNOPT optimizer
 *
 * @param optMode the optimization mode
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::SetOptimizationMode(const std::string &optMode)
{
   Problem.setParameter(optMode.c_str());
}

//------------------------------------------------------------------------------
// void SetSnoptConsoleOutputLevel(const Integer level)
//------------------------------------------------------------------------------
/**
 * Sets the amount of output printed to screen by the SNOPT optimizer
 *
 * @param level the level of output to print to screen
 * Current valid options for level:
 * 0: no output (major print level = 0, minor print level = 0, print no = 0,
 * summary file = 0, suppress parameters)
 * 1: default output (default choice)
 * Any value other than 0 is interpreted as 1.
 */
 //------------------------------------------------------------------------------
void SnoptOptimizer::SetSnoptConsoleOutputLevel(const Integer level)
{
	switch (level)
	{
	case 0:
		this->Problem.setIntParameter((char*)"Major Print Level", 0);
		this->Problem.setIntParameter((char*)"Minor print level", 0);
		this->Problem.setIntParameter((char*)"Print No", 0);
		this->Problem.setIntParameter((char*)"Summary file", 0);
		this->Problem.setParameter((char*)"Suppress parameters");
		break;
	// do nothing if level is anything other than 0
	}
}

//------------------------------------------------------------------------------
// void SnoptOptimizer::SetCurrentIterationData(Integer iterCount, 
//    Integer majorIterCount, Real objValue)
//------------------------------------------------------------------------------
/**
 * Sets data about the current iteration, used to print final iteration counts
 * and data at the end of a run
 *
 * @param iterCount the current number of iterations taken
 * @param majorIterCount the current number of major iterations taken
 * @param objFinalVal the current value of the objective function
 */
 //------------------------------------------------------------------------------
void SnoptOptimizer::SetCurrentIterationData(Integer iterCount,
   Integer majorIterCount, Real objValue)
{
   totalNumIter = iterCount;
   totalNumMajorIter = majorIterCount;
   objFinalVal = objValue;
}

//------------------------------------------------------------------------------
// void Optimize(Rvector       &decVec,   const Rvector  &decVecLB,
//               const Rvector &decVecUB, const Rvector  &funLB,
//               const Rvector &funUB,    const RSMatrix &sPatternMat,
//               Rvector       &F,        Rvector        &xmul,
//               Rvector       &Fmul, Integer &exitFlag)
//------------------------------------------------------------------------------
/**
 * Performs the optimization
 *
 * @param <decVec>        the decision vector
 * @param <decVecLB>      the decision vector lower bounds
 * @param <decVecUB>      the decision vector upper bounds
 * @param <funLB>         the objective and constraint function lower bounds
 * @param <funUB>         the objective and constraint function upper bounds
 * @param <sPatternMat>   the dsparsity pattern
 * @param <F>             the objective and constraint function values
 * @param <xmul>          the lagrange multipliers for the KKT optimality 
 *                        conditions of the state bounds
 * @param <Fmul>          the lagrange multipliers for the KKT optimality 
 *                        conditions of the objective and constraint functions
 * @param <exitFlag>      the exit flag value received directly from SNOPT
 *
 */
//------------------------------------------------------------------------------
void SnoptOptimizer::Optimize(Rvector       &decVec, const Rvector  &decVecLB,
    const Rvector &decVecUB, const Rvector  &funLB,
    const Rvector &funUB, const RSMatrix &sPatternMat,
    Rvector       &F, Rvector        &xmul,
    Rvector       &Fmul, Integer &exitFlag)
{

#ifdef DEBUG_SnoptOptimizer
    MessageInterface::ShowMessage("--- decVec passed in to Optimize = %s\n",
        decVec.ToString(12).c_str());
    MessageInterface::ShowMessage("--- decVecLB passed in to Optimize = %s\n",
        decVecLB.ToString(12).c_str());
    MessageInterface::ShowMessage("--- decVecUB passed in to Optimize = %s\n",
        decVecUB.ToString(12).c_str());
    MessageInterface::ShowMessage("--- funLB passed in to Optimize = %s\n",
        funLB.ToString(12).c_str());
    MessageInterface::ShowMessage("--- funUB passed in to Optimize = %s\n",
        funUB.ToString(12).c_str());
    Integer rSparse = sPatternMat.size1();
    Integer cSparse = sPatternMat.size2();
    MessageInterface::ShowMessage(
        " ----- sparsity pattern passed in to Optimize() size (%d, %d): \n",
        rSparse, cSparse);
    for (Integer cc = 0; cc < cSparse; cc++)
    {
        for (Integer rr = 0; rr < rSparse; rr++)
        {
            Real jacTmp = sPatternMat(rr,cc);
            if (jacTmp != 0.0)
                MessageInterface::ShowMessage(
                "      sPatternMat(%d, %d) =  %12.10f\n",
                rr+1, cc+1, jacTmp);
        }
    }
#endif

    std::string snoptStartMsg = "\n========== SNOPT Data Start "
       "==========\n";
    if (traj->GetApplicationType() != "Console")
       MessageInterface::ShowMessage(snoptStartMsg);
    else
    {
       MessageInterface::ToggleConsolePrinting(false);
       MessageInterface::LogMessage(snoptStartMsg);
       MessageInterface::ToggleConsolePrinting(true);
    }

    // TODO: Better GMAT-ish way to do this without converting
    // the Rvectors to std::vectors?
    std::vector<SNOPT_DOUBLE>    x(decVec.GetSize());
    std::vector<SNOPT_DOUBLE>    xLB(decVec.GetSize());
    std::vector<SNOPT_DOUBLE>    xUB(decVec.GetSize());
    std::vector<SNOPT_DOUBLE>    xmul_local(decVec.GetSize(), 0.0);
    std::vector<SNOPT_INTEGER>   xstate(decVec.GetSize(), 0);
    std::vector<SNOPT_DOUBLE>    F_local(F.GetSize());
    std::vector<SNOPT_DOUBLE>    F_localLB(F.GetSize());
    std::vector<SNOPT_DOUBLE>    F_localUB(F.GetSize());
    std::vector<SNOPT_DOUBLE>    Fmul_local(F.GetSize(), 0.0);
    std::vector<SNOPT_INTEGER>   Fstate(F.GetSize(), 0);

    // Loop through the decision vector and put the RVector 
    // data into the std::vectors
    for (Integer ii = 0; ii < decVec.GetSize(); ii++)
    {
        x[ii] = decVec[ii];
        xLB[ii] = decVecLB[ii];
        xUB[ii] = decVecUB[ii];
    }

    // Loop through the objective function and constraints and put 
    // the RVector data into the std::vectors
    for (Integer ii = 0; ii < F.GetSize(); ii++)
    {
        F_local[ii] = F[ii];
        F_localLB[ii] = funLB[ii];
        F_localUB[ii] = funUB[ii];
    }

    // Pass pointers to SNOPT for the decision vector and its bounds
    Problem.setX(x.data(), xLB.data(), xUB.data(),
        xmul_local.data(), xstate.data());

    // Pass pointers to SNOPT for the objective function and constraints
    Problem.setF(F_local.data(), F_localLB.data(), F_localUB.data(),
        Fmul_local.data(), Fstate.data());

    // We don't know what constraints are linear, so assume they are
    // all nonlinear - Or maybe there is no need to do this??

    SNOPT_INTEGER lenA = 0, neA = 0;
    std::vector<Integer> iAfun, jAvar;
    std::vector<double> A;
    Problem.setA(lenA, neA, iAfun.data(), jAvar.data(), A.data());


    // Loop over the sparsity pattern and determine where non-zeros are and
    // constuct vectors that SNOPT requires for the sparsity pattern
    // (iGfun,jGvar)

    // Sparse Matrix Utility Style Coding by YK
    Integer numNonLinearNonZeros =
        SparseMatrixUtil::GetNumNonZeroElements(&sPatternMat);

    iGfun.clear();
    jGvar.clear();

    SparseMatrixUtil::GetSparsityPattern(&sPatternMat, iGfun, jGvar);

    // Probably should cast iGfun and jGVar to std::vector<SNOPT_INTEGER>
    // here -> N/N
    Integer numTotalNonZeros = numNonLinearNonZeros;  // No linear constraints
#ifdef DEBUG_SnoptOptimizer
    MessageInterface::ShowMessage("--- numTotalNonZeros     = %d\n",
        numTotalNonZeros);
    MessageInterface::ShowMessage("--- numNonLinearNonZeros = %d\n",
        numNonLinearNonZeros);
    MessageInterface::ShowMessage("--- iGfun:\n");
    for (Integer ii = 0; ii < iGfun.size(); ii++)
        MessageInterface::ShowMessage("------ (%d)  = %d\n",
        ii+1, iGfun.at(ii));
    MessageInterface::ShowMessage("--- jGvar:\n");
    for (Integer ii = 0; ii < jGvar.size(); ii++)
        MessageInterface::ShowMessage("------ (%d)  = %d\n",
        ii+1, jGvar.at(ii));
#endif
    Problem.setG(numNonLinearNonZeros, numTotalNonZeros,
        iGfun.data(), jGvar.data());
    // Tell SNOPT how many decision variables there are and how many constraints
    Problem.setProblemSize(decVec.GetSize(), F.GetSize());

    // Pass a pointer to this object so that the user function can call it
    Problem.setUserR((SNOPT_DOUBLE*)this, 500);

    // Run SNOPT and store the exit code
    SNOPT_INTEGER snoptExitFlag = Problem.solve(0); 

    // Put the std::vector data back into the Rvectors
    for (Integer ii = 0; ii < decVec.GetSize(); ii++)
    {
        decVec[ii] = x[ii];
        xmul[ii] = xmul_local[ii];
    }
    for (Integer ii = 0; ii < F.GetSize(); ii++)
    {
        F[ii] = F_local[ii];
        Fmul[ii] = Fmul_local[ii];
    }

    std::string exitMessage;

    // Set the return code "exitFlag" based on the SNOPT exit code
    // See SNOPT User guide V7 pgs 93-99 for detailed explanation of exit codes.
    // Optimizer finished successfully
    if (snoptExitFlag >= 1 && snoptExitFlag  <=  9)   
    {
        //exitFlag = 1;
        exitMessage = "optimality conditions satisfied";
    }
    // Optimizer did not converge, but mesh refinement may resolve the issue
    else if ((snoptExitFlag >= 31 && snoptExitFlag <= 39) ||  // Resource limit error
        (snoptExitFlag == 41 && snoptExitFlag <= 49))   // Terminated after numerical difficulties
    {
        //exitFlag = 0;

        if (snoptExitFlag >= 31 && snoptExitFlag <= 39)
           exitMessage = "resource limit error";
        else
           exitMessage = "terminated after numerical difficulties";
    }
    // Optimizer was stopped by the user
    else if (snoptExitFlag == 71 || snoptExitFlag == 74)
    {
       //exitFlag = -2;
       exitMessage = "optimization stopped by user";
    }
    // Unrecoverable failure
    else
    {
        //exitFlag = -1;
        exitMessage = "unrecoverable failure";
    }

    exitFlag = snoptExitFlag;

    // If this is a GUI application, print the ending data, otherwise just
    // print to the log
    std::string snoptEndMsg = "\nSNOPT Exit Condition: " +
       exitMessage + "\n\nProblem name                    "
       "CSALT\n";
    if (traj->GetApplicationType() != "Console")
    {
       MessageInterface::ShowMessage(snoptEndMsg);
       MessageInterface::ShowMessage("No. of iterations           %7d   "
          "Objective           %17.10e\n", totalNumIter, objFinalVal);
       MessageInterface::ShowMessage("No. of major iterations     %7d\n",
          totalNumMajorIter);

       MessageInterface::ShowMessage("==========  SNOPT Data End  ==========\n");
    }
    else
    {
       MessageInterface::ToggleConsolePrinting(false);
       MessageInterface::LogMessage(snoptEndMsg);
       MessageInterface::LogMessage("No. of iterations           %7d   "
          "Objective           %17.10e\n", totalNumIter, objFinalVal);
       MessageInterface::LogMessage("No. of major iterations     %7d\n",
          totalNumMajorIter);

       MessageInterface::LogMessage("==========  SNOPT Data End  ==========\n");
       MessageInterface::ToggleConsolePrinting(true);
    }

   #ifdef DEBUG_SnoptOptimizer
      MessageInterface::ShowMessage(
                                 " ----- EXITING SnoptOptimizer::Optimize<<<< \n");
   #endif
}
