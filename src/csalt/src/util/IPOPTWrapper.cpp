// Copyright (C) 2004, 2006 International Business Machines and others.
// All Rights Reserved.
// This code is published under the Eclipse Public License.
//
// $Id: IPOPTWrapper.cpp 2005 2011-06-06 12:55:16Z stefan $
//
// Authors:  Carl Laird, Andreas Waechter     IBM    2004-11-05

#include "IPOPTWrapper.hpp"

#include <cassert>

//#define DEBUG_IPOPT
using namespace Ipopt;

/* Constructor. */
IPOPTWrapper::IPOPTWrapper(Trajectory * trajectory_in) :
   TNLP()
{
   traj = trajectory_in;
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
IPOPTWrapper::IPOPTWrapper(const IPOPTWrapper &copy) :
   traj(copy.traj)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
IPOPTWrapper& IPOPTWrapper::operator=(const IPOPTWrapper &copy)
{
   if (&copy == this)
      return *this;

   traj = copy.traj;
   return *this;
}


//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
IPOPTWrapper::~IPOPTWrapper() 
{
   // nothing to do here 
}

bool IPOPTWrapper::get_nlp_info(Ipopt::Index& numVars, Ipopt::Index& numConstr, 
                         Ipopt::Index& numNZerosInConstrJac,
                         Ipopt::Index& numNZerosInHess, IndexStyleEnum& index_style)
{
   // this method delivers the basic NLP information on the sizes of functions, jac., hess.,
   // and variables, and indexing setting

   Rvector decVec = traj->GetDecisionVector();

   // n is the number of variables
   numVars = decVec.GetSize();

   // m is the number of constraints,
   
   // note that traj->GetConstraintVector() gives only 
   // the path constraints not including point constraints.
   Rvector costConstr = traj->GetCostConstraintFunctions();

   numConstr = costConstr.GetSize() - 1;

   RSMatrix jac = traj->GetConstraintSparsityPattern();

   // check the number of elements in constraint jacobian. 
   numNZerosInConstrJac = SparseMatrixUtil::GetNumNonZeroElements(&jac);

   RSMatrix hessPattern = traj->GetHessianSparsityPattern();
   // the effective nonzeros in Lag. (lower triangular part)
   numNZerosInHess = SparseMatrixUtil::GetNumNonZeroElements(&hessPattern);
   // assume that the maximum memory, and no zeros in contracted hessian matrix
   //numNZerosInHess = numVars*(numVars + 1) / 2;

   // indexing style for jacobian and hessian matrix in three vector form. 
   // CSALT always uses c style indexing starting from zero.
   // index_style = 0 for C style
   // index_style = 1 for fortran style
   index_style = C_STYLE;

   return true;
}

bool IPOPTWrapper::get_bounds_info(Ipopt::Index numVars, Ipopt::Number* varLB, Ipopt::Number* varUB,
                            Ipopt::Index numConstr, Ipopt::Number* constrLB, Ipopt::Number* constrUB)
{
   // this method delivers the var/constr bounds to IPOPT

   // here, the numVars and numConstr we gave IPOPT in get_nlp_info are passed back to us.
   // If desired, we could assert to make sure they are what we think they are.



   Rvector varLBVec = traj->GetDecisionVectorLowerBound();
   Rvector varUBVec = traj->GetDecisionVectorUpperBound();
   RealArray constrLBVec = traj->GetAllConLowerBound();
   RealArray constrUBVec = traj->GetAllConUpperBound();

   for (Integer idx = 0; idx < numVars; idx++)
   {
      varUB[idx] = varUBVec(idx);
      varLB[idx] = varLBVec(idx);
   }
   for (Integer idx = 0; idx < numConstr; idx++)
   {
      constrUB[idx] = constrUBVec[idx];
      constrLB[idx] = constrLBVec[idx];
   }

   return true;
}

bool IPOPTWrapper::get_starting_point(Ipopt::Index numVars, bool hasInitGuess, Ipopt::Number* initGuess,
                               bool hasInitBndMultpliers, Ipopt::Number* initLBMultpliers,
                               Ipopt::Number* initUBMultpliers,
                               Ipopt::Index numConstr, bool hasInitConstrMultpliers,
                               Ipopt::Number* lambda)
{
   // Here, we assume we only have starting values for x, if you code
   // your own NLP, you can provide starting values for the others if
   // you wish.
   assert(hasInitGuess == true);
   assert(hasInitBndMultpliers == false);
   assert(hasInitConstrMultpliers == false);

   // we initialize x in bounds, in the upper right quadrant

   Rvector decVec = traj->GetDecisionVector();

   for (Integer idx = 0; idx < numVars; idx++)
   {
      initGuess[idx] = decVec(idx);
   }

   return true;
}

bool IPOPTWrapper::eval_f(Ipopt::Index numVars, const Ipopt::Number* decVec, bool isNewDecVec, Ipopt::Number& costValue)
{
   // return the value of the objective function

   //RSMatrix            GetJacobian();
   //Rvector             GetCostConstraintFunctions();

   /*
   // CSALT internally handle this by itself. 
   if (isNewDecVec)
   {
      Rvector decRVec(numVars);
      for (Integer idx = 0; idx < numVars; idx++)
      {
         decRVec(idx) = decVec[idx];
      }

      traj->SetDecisionVector(decRVec);
   }
   */
   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
   {
      decRVec(idx) = decVec[idx];
   }

   traj->SetDecisionVector(decRVec);
   
   costValue = traj->GetCostFunction();
   return true;
}

bool IPOPTWrapper::eval_grad_f(Ipopt::Index numVars, const Ipopt::Number* decVec, bool isNewDecVec, Ipopt::Number* costJac)
{
   // return the cost jacobian to IPOPT
   /*
   // CSALT internally handle this by itself.
   if (isNewDecVec)
   {
   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
   {
   decRVec(idx) = decVec[idx];
   }

   traj->SetDecisionVector(decRVec);
   }
   */
   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
   {
      decRVec(idx) = decVec[idx];
   }

   traj->SetDecisionVector(decRVec);

   RSMatrix costJacRSMat = traj->GetCostJacobian();
   Rmatrix costJacRmat = SparseMatrixUtil::RSMatrixToRmatrix(&costJacRSMat);

   for (Integer idx = 0; idx < numVars; idx++)
   {
      costJac[idx] = costJacRmat(0,idx);
   }

   return true;
}

bool IPOPTWrapper::eval_g(Ipopt::Index numVars, const Ipopt::Number* decVec, bool isNewDecVec, Ipopt::Index numConstr, Ipopt::Number* constrVec)
{
   // return the constraint vector to IPOPT

   /*
   // CSALT internally handle this by itself.
   if (isNewDecVec)
   {
   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
   {
   decRVec(idx) = decVec[idx];
   }

   traj->SetDecisionVector(decRVec);
   }
   */
   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
   {
      decRVec(idx) = decVec[idx];
   }

   traj->SetDecisionVector(decRVec);

   Rvector constrRVec = traj->GetAllConstraintFunctions();

   for (Integer idx = 0; idx < numConstr; idx++)
   {
      constrVec[idx] = constrRVec(idx);
   }

   return true;
}

bool IPOPTWrapper::eval_jac_g(Ipopt::Index numVars, const Ipopt::Number* decVec, bool isNewDecVec,
                       Ipopt::Index numConstr, Ipopt::Index nele_jac, Ipopt::Index* iRow, Ipopt::Index *jCol,
                       Ipopt::Number* values)
{
   // provide constraint jacobian here
   IntegerArray  rowIdxs, colIdxs;
   if (values == NULL) {

      RSMatrix constJacPattern = traj->GetConstraintSparsityPattern();
      SparseMatrixUtil::GetSparsityPattern(&constJacPattern, rowIdxs, colIdxs);
      for (Integer idx = 0; idx < rowIdxs.size(); idx++)
      {
         iRow[idx] = rowIdxs[idx];
         jCol[idx] = colIdxs[idx];
      }
      return true;
   }

   /*
   // CSALT internally handle this by itself.
   if (isNewDecVec)
   {
   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
   {
   decRVec(idx) = decVec[idx];
   }

   traj->SetDecisionVector(decRVec);
   }
   */
   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
   {
      decRVec(idx) = decVec[idx];
   }

   traj->SetDecisionVector(decRVec);

   std::vector<Real> valueVec;
   RSMatrix constrJacRSMat = traj->GetConstraintJacobian();
   SparseMatrixUtil::GetThreeVectorForm(&constrJacRSMat, rowIdxs, colIdxs,valueVec);
   for (Integer idx = 0; idx < valueVec.size(); idx++)
   {
      values[idx] = valueVec[idx];
   }
   


   return true;
}

bool IPOPTWrapper::eval_h(Ipopt::Index numVars, const Ipopt::Number* decVec, bool isNewDecVec,
                   Ipopt::Number costFactor, Ipopt::Index numConstr, const Ipopt::Number* constrFactor,
                   bool isNewConstrFactor, Ipopt::Index numNZerosInHess, Ipopt::Index* iRow,
                   Ipopt::Index* jCol, Ipopt::Number* values)
{
   // now no hessian. in the future, this method provides hessian lagrangian in three vector form (lower triangular form)
   // note that hessian is symmetric.
   // rowIdxs and colIdxs must be NULL when no hessian is provided. 
   
   // return false when there is no hessian function provided.
   // return true when there is the user-defined hessian function provided.

   // hessian pattern computation
   IntegerArray  rowIdxs, colIdxs;
   if (values == NULL) {
      // this is for sparsity pattern check
#ifdef DEBUG_IPOPT
      //MessageInterface::ShowMessage("The current Hessian pattern is: \n");
      //SparseMatrixUtil::PrintNonZeroElements(&hessPattern,true);
#endif

      RSMatrix hessPattern = traj->GetHessianSparsityPattern();
      SparseMatrixUtil::GetSparsityPattern(&hessPattern, rowIdxs, colIdxs);

      for (Integer idx = 0; idx < rowIdxs.size(); idx++)
      {
         iRow[idx] = rowIdxs[idx];
         jCol[idx] = colIdxs[idx];
      }
      return true;
   }

   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
      decRVec(idx) = decVec[idx];

   RealArray valueVec;
   // this is for hessian computation      
   Rvector lambdaVec(numConstr + 1);
   lambdaVec(0) = costFactor;
   for (Integer idx = 0; idx < numConstr ; idx++)
   {
      Real tmp = constrFactor[idx];
      lambdaVec(idx+1) = constrFactor[idx];
   }
   RSMatrix hessMat = traj->ComputeHessianContraction(decRVec, lambdaVec);
   SparseMatrixUtil::GetThreeVectorForm(&hessMat, rowIdxs, colIdxs, valueVec);

   for (Integer idx = 0; idx < valueVec.size(); idx++)
   {
      values[idx] = valueVec[idx];
   }
      
   

   return true;
}

void IPOPTWrapper::finalize_solution(SolverReturn status,
                              Ipopt::Index numVars, const Ipopt::Number* decVec, const Ipopt::Number* lbMultpliers, const Ipopt::Number* ubMultpliers,
                              Ipopt::Index numConstr, const Ipopt::Number* constrVec, const Ipopt::Number* constrMultipliers,
                              Ipopt::Number costValue, const IpoptData* ip_data, IpoptCalculatedQuantities* ip_cq)
{
   // deliver solution to the trajectory here.

   //status: (in), gives the status of the algorithm as specified in IpAlgTypes.hpp
   //   SUCCESS: Algorithm terminated successfully at a locally optimal point, satisfying the convergence tolerances . 
   //   MAXITER_EXCEEDED: Maximum number of iterations exceeded. 
   //   CPUTIME_EXCEEDED: Maximum number of CPU seconds exceeded. 
   //   STOP_AT_TINY_STEP: Algorithm proceeds with very little progress. 
   //   STOP_AT_ACCEPTABLE_POINT: Algorithm stopped at a point that was converged, not to desired tolerances, but to acceptable tolerances (see the acceptable-... options). 
   //   LOCAL_INFEASIBILITY: Algorithm converged to a point of local infeasibility. Problem may be infeasible. 
   //   USER_REQUESTED_STOP: The user call-back function  intermediate_callback (see Section 3.3.4) returned false, i.e., the user code requested a premature termination of the optimization. 
   //   DIVERGING_ITERATES: It seems that the iterates diverge. 
   //   RESTORATION_FAILURE: Restoration phase failed, algorithm doesn't know how to proceed. 
   //   ERROR_IN_STEP_COMPUTATION: An unrecoverable error occurred while IPOPT tried to compute the search direction. 
   //   INVALID_NUMBER_DETECTED: Algorithm received an invalid number (such as NaN or Inf) from the NLP; see also option check_derivatives_for_naninf. 
   //   INTERNAL_ERROR: An unknown internal error occurred. Please contact the IPOPT authors through the mailing list. 
   

   Rvector decRVec(numVars);
   for (Integer idx = 0; idx < numVars; idx++)
   {
      decRVec(idx) = decVec[idx];
   }

   traj->SetDecisionVector(decRVec);
   traj->GetCostConstraintFunctions();
   traj->GetJacobian();
}

