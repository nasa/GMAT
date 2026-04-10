//------------------------------------------------------------------------------
//                         NLPFuncUtil_AlgPath
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
// Author: Youngkwang Kim
// Created: 2015/11/30 (mods 2016.05.20 WCS)
//
/**
 * Developed based on NLPFuncUtil_AlgPath.m
 */
//------------------------------------------------------------------------------

#include "NLPFuncUtil_AlgPath.hpp" 
//#define DEBUG
//#define DEBUG_ALGPATH
//#define DEBUG_ALGPATH_INIT

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
NLPFuncUtil_AlgPath::NLPFuncUtil_AlgPath() :
   NLPFuncUtil_Path(),
   numFunctionsPerPoint (0),
   numFunctionPoints    (0),
   numFunctions         (0),
   numDecisionParams    (0),
   hasStateVars         (false),
   hasControlVars       (false)
{
   #ifdef DEBUG_ALGPATH_INIT
      MessageInterface::ShowMessage("CREATING a new NLPFuncUtil_AlgPath\n");
   #endif
   // all Rvector items are initially empty
   // all Rmatrix items are initially empty
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
NLPFuncUtil_AlgPath::NLPFuncUtil_AlgPath(const NLPFuncUtil_AlgPath &copy) :
   NLPFuncUtil_Path(copy),
   dataFuncUtil         (copy.dataFuncUtil),
   numFunctionsPerPoint (copy.numFunctionsPerPoint),
   numFunctionPoints    (copy.numFunctionPoints),
   numFunctions         (copy.numFunctions),
   numDecisionParams    (copy.numDecisionParams),
   hasStateVars         (copy.hasStateVars),
   hasControlVars       (copy.hasControlVars)
{
   #ifdef DEBUG_ALGPATH_INIT
      MessageInterface::ShowMessage(
                     "COPYING a new NLPFuncUtil_AlgPath from an old one <%p>\n",
                     &copy);
   #endif
	// mod by YK for the initialization issue
	Integer sz;

	if (copy.dTimedTi.IsSized() == true)
	{
		sz = copy.dTimedTi.GetSize();
		dTimedTi.SetSize(sz);
		dTimedTi = copy.dTimedTi;
	}

	if (copy.dTimedTf.IsSized() == true)
	{
		sz = copy.dTimedTf.GetSize();
		dTimedTf.SetSize(sz);
		dTimedTf = copy.dTimedTf;
	}
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
NLPFuncUtil_AlgPath& NLPFuncUtil_AlgPath::operator=(
                                                const NLPFuncUtil_AlgPath &copy)
{
   #ifdef DEBUG_ALGPATH_INIT
      MessageInterface::ShowMessage("operator= on a NLPFuncUtil_AlgPath\n");
   #endif
   
   if (&copy == this)
      return *this;
   
   NLPFuncUtil_Path::operator=(copy);

   dataFuncUtil         = copy.dataFuncUtil;
   numFunctionsPerPoint = copy.numFunctionsPerPoint;
   numFunctionPoints    = copy.numFunctionPoints;
   numFunctions         = copy.numFunctions;
   numDecisionParams    = copy.numDecisionParams;
   hasStateVars         = copy.hasStateVars;
   hasControlVars       = copy.hasControlVars;

   // mod by YK for the initialization issue
   Integer sz;

   if (copy.dTimedTi.IsSized() == true)
   {
	   sz = copy.dTimedTi.GetSize();
	   dTimedTi.SetSize(sz);
	   dTimedTi = copy.dTimedTi;
   }

   if (copy.dTimedTf.IsSized() == true)
   {
	   sz = copy.dTimedTf.GetSize();
	   dTimedTf.SetSize(sz);
	   dTimedTf = copy.dTimedTf;
   }

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
NLPFuncUtil_AlgPath::~NLPFuncUtil_AlgPath()
{
   #ifdef DEBUG_ALGPATH_INIT
      MessageInterface::ShowMessage("DESTRUCTING a NLPFuncUtil_AlgPath <%p>\n",
                                    this);
   #endif
}


//------------------------------------------------------------------------------
//  void Initialize(UserFunctionProperties *funcProps,
//                  const std::vector<FunctionOutputData*> &funcData,
//                  Integer                numDecisionParameters,
//                  Integer                numFuncPoints,
//                  const Rvector          &dTi,
//                  const Rvector          &dTf)
//------------------------------------------------------------------------------
/**
 * This method initializes the algpath
 *
 * @param <funcProps>             user function properties
 * @param <funcData>              a vector of output data classes
 * @param <numDecisionParameters> number of decision parameters
 * @param <numFuncPoints>         number of function points
 * @param <dTi>                   vector of ???? @todo
 * @param <dTf>                   vector of ???? @todo
 *
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_AlgPath::Initialize(UserFunctionProperties *funcProps,
                          const std::vector<FunctionOutputData*> &funcData,
                          Integer                numDecisionParameters,
                          Integer                numFuncPoints,
                          const Rvector          &dTi,
                          const Rvector          &dTf)
{
   #ifdef DEBUG_ALGPATH
      MessageInterface::ShowMessage(
            "In AlgPath::Initialize, numDecisionParameters = %d, numFuncPoints "
            "= %d, dTi size = %d, dTf size = %d\n",
            numDecisionParameters, numFuncPoints, (Integer) dTi.GetSize(),
            (Integer) dTf.GetSize());
      MessageInterface::ShowMessage("   funcProps = <%p>\n", funcProps);
   #endif

   Integer numFuncDependencies = 1;
   
   // Set data from inputs
   numFunctionPoints    = numFuncPoints;
   numFunctionsPerPoint = funcProps->GetNumberOfFunctions();
   numFunctions         = numFunctionsPerPoint * numFunctionPoints;
   numDecisionParams    = numDecisionParameters;
   hasStateVars         = funcProps->HasStateVars();
   hasControlVars       = funcProps->HasControlVars();
   if (dTimedTi.IsSized() == true)
   {
      int sz = dTi.GetSize();
      dTimedTi.SetSize(sz);
      dTimedTi = dTi;
   }
   else
      dTimedTi = dTi;

   if (dTimedTf.IsSized() == true)
   {
      int sz = dTf.GetSize();
      dTimedTf.SetSize(sz);
      dTimedTf = dTf;
   }
   else
      dTimedTf = dTf;

   #ifdef DEBUG_ALGPATH
      MessageInterface::ShowMessage(
            "In AlgPath::Initialize, trying to initialize dataFuncUtil ...\n");
   #endif
   dataFuncUtil.Initialize(numFunctions,numDecisionParams,
                           numFuncDependencies*numFunctions);
   #ifdef DEBUG_ALGPATH
      MessageInterface::ShowMessage(
            "In AlgPath::Initialize, about to call "
            "InitializeConstantNLPMatrices\n");
   #endif
   
   InitializeConstantNLPMatrices(funcProps, funcData);
   #ifdef DEBUG_ALGPATH
      MessageInterface::ShowMessage(
            "Leaving AlgPath::Initialize\n");
   #endif
}

//------------------------------------------------------------------------------
//  void InitializeConstantNLPMatrices(
//                 UserFunctionProperties                 *funcProps,
//                 const std::vector<FunctionOutputData*> &funcData)
//------------------------------------------------------------------------------
/**
 * This method initializes the constant NLP matrices
 *
 * @param <funcProps>   user function properties
 * @param <funcData>    a vector of output data classes
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_AlgPath::InitializeConstantNLPMatrices(
							UserFunctionProperties *funcProps,
							const std::vector<FunctionOutputData*> &funcData)

														
{
	// Initialize arrays for state partials
	// Note that A matrix is a zero matrix
	// A is all zeros so nothing to do.
	// Loop over the number of function points
   Integer lowIdx  = 0;
   Integer numRows = 0;
   Integer numCols = 0;
	Rvector ones(numFunctionsPerPoint);
	IntegerArray funcIdxs, stateIdxs, controlIdxs;
	Rmatrix jacPatternS, jacPatternC, jacPatternT;
	for (Integer pointIdx = 0; pointIdx < numFunctionPoints; pointIdx++)
	{
	   funcIdxs.clear(); // Do I need this here?  YES!
		for (Integer jj = 0; jj < numFunctionsPerPoint; jj++)
		{
			ones(jj) = 1.0;
			funcIdxs.push_back(lowIdx + jj);
		}

		dataFuncUtil.InsertBMatPartition(0, 0, &funcIdxs, &funcIdxs, &ones);
      #ifdef DEBUG_ALGPATH
         MessageInterface::ShowMessage(
               "-=-=-=-= inserted into B matrix OK\n");
      #endif

		// insert D matrix
		if (hasStateVars)
		{
		   jacPatternS = funcProps->GetStateJacobianPattern();
			numRows     = jacPatternS.GetNumRows();
			numCols     = jacPatternS.GetNumColumns();

			stateIdxs = funcData.at(pointIdx)->GetStateIdxs();
			for (Integer rowIndex = 0; rowIndex < numRows; ++rowIndex)
			{
				for (Integer colIndex = 0; colIndex < numCols; ++colIndex)
				{
					if (jacPatternS(rowIndex, colIndex) != 0)
						dataFuncUtil.InsertDMatElement(funcIdxs[rowIndex], 
													   stateIdxs[colIndex], 1.0);
               #ifdef DEBUG_ALGPATH
                  MessageInterface::ShowMessage(
                        "-=-=-=-= inserted element into D matrix OK\n");
               #endif
				}
			}							
		}

		// Intialize arrays for control partials
		if (hasControlVars)
		{
         jacPatternC = funcProps->GetControlJacobianPattern();
			numRows     = jacPatternC.GetNumRows();
			numCols     = jacPatternC.GetNumColumns();
         #ifdef DEBUG_ALGPATH
            Rmatrix tmpPat = funcProps->GetControlJacobianPattern();
            Integer r, c;
            jacPatternC.GetSize(r, c);
            MessageInterface::ShowMessage(
                  "-=-=-=-= HAS control vars, size of jacPattern = %d x %d\n",
                                          r, c);
            tmpPat.GetSize(r, c);
            MessageInterface::ShowMessage(
                  "-=-=-=-=                   size of tmpPat = %d x %d\n",
                                          r, c);
            MessageInterface::ShowMessage(
                  "-=-=-=-= CONTROL numRows = %d, numCols = %d\n",
                                          numRows, numCols);
         #endif

			controlIdxs = funcData.at(pointIdx)->GetControlIdxs();
			for (Integer rowIndex = 0; rowIndex < numRows; ++rowIndex)
			{
				for (Integer colIndex= 0; colIndex < numCols; ++colIndex)
				{
					if (jacPatternC(rowIndex, colIndex) != 0)
						dataFuncUtil.InsertDMatElement(funcIdxs[rowIndex],
						controlIdxs[colIndex], 1.0);
               #ifdef DEBUG_ALGPATH
                  MessageInterface::ShowMessage(
                        "-=-=-=-= inserted CONTROL element into D matrix OK\n");
               #endif
				}
			}
		}

		// Initialize arrays for time Jacobian partials
		// D matrix sub element for initial time Jacobian
		jacPatternT = funcProps->GetTimeJacobianPattern();
		numRows     = jacPatternT.GetNumRows();
		numCols     = jacPatternT.GetNumColumns();

      #ifdef DEBUG_ALGPATH
         MessageInterface::ShowMessage(
               "-=-=-=-= For time, numRows = %d, numCols = %d\n",
                                       numRows, numCols);
      #endif
		for (Integer rowIndex = 0; rowIndex < numRows; ++rowIndex)
		{
			for (Integer colIndex = 0; colIndex < numCols; ++colIndex)
			{
				if (jacPatternT(rowIndex, colIndex) != 0)
				{
               #ifdef DEBUG_ALGPATH
                  MessageInterface::ShowMessage(
                        "-=-=-=-= TRYING to set to row = %d ...\n",
                                                funcIdxs[rowIndex]);
               #endif
					dataFuncUtil.InsertDMatElement(funcIdxs[rowIndex],
													0, 1.0);
               #ifdef DEBUG_ALGPATH
                  MessageInterface::ShowMessage(
                        "-=-=-=-= 1st 1.0 set on Dmat(time) ...\n");
               #endif
					dataFuncUtil.InsertDMatElement(funcIdxs[rowIndex],
													1, 1.0);
               #ifdef DEBUG_ALGPATH
                  MessageInterface::ShowMessage(
                        "-=-=-=-= 2nd 1.0 set on Dmat(time) ...\n");
               #endif
				}					
			}
		}

		// now update indices
		lowIdx += numFunctionsPerPoint;
	}
   #ifdef DEBUG_ALGPATH
      MessageInterface::ShowMessage(
            "LEAVING InitializeConstantNLPMatrices\n");
   #endif
}

//------------------------------------------------------------------------------
//  IntegerArray GetMatrixNumNonZeros()
//------------------------------------------------------------------------------
/**
 * This method returns the number of elements that are non-zero
 *
 * @return   number of matrix elements that are non-zero
 *
 */
//------------------------------------------------------------------------------
IntegerArray NLPFuncUtil_AlgPath::GetMatrixNumNonZeros()
{
   return dataFuncUtil.GetMatrixNumNonZeros();
}


//------------------------------------------------------------------------------
//  void ComputeFuncAndJac(const std::vector<FunctionOutputData*> &funcData,
//                         Rvector                &funcValues,
//                         RSMatrix               &jacArray)
//------------------------------------------------------------------------------
/**
 * This method computes the functions and jacobian given the input function data
 *
 * @param <funcData>   vector of function output data
 * @param <funcValues> output - the resulting function values
 * @param <jacArray>   output - the resulting Jacobian matrix
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_AlgPath::ComputeFuncAndJac(
                          const std::vector<FunctionOutputData*> &funcData,
                          Rvector                &funcValues,
                          RSMatrix               &jacArray)
{
	Rvector QVector;
	RSMatrix parQMatrix;
	FillUserNLPMatrices(funcData, QVector, parQMatrix);


   #ifdef DEBUG
      MessageInterface::ShowMessage(
                        "the parQMatrix of defect constraints is given as \n");
   	SparseMatrixUtil::PrintNonZeroElements(&parQMatrix);
      if (!QVector.IsSized())
         MessageInterface::ShowMessage("QVector is UNSIZED!!!\n");
      MessageInterface::ShowMessage(
                           "the QVector of defect constraints is given as: \n");
      for (Integer idx = 0; idx < QVector.GetSize(); idx++)
         MessageInterface::ShowMessage(" %d     %12.10f\n", idx, QVector(idx));
   #endif

   dataFuncUtil.ComputeFunctions(&QVector, funcValues);
   #ifdef DEBUG
      MessageInterface::ShowMessage("done with ComputeFunctions\n");
   #endif
   dataFuncUtil.ComputeJacobian(&parQMatrix, jacArray);
   #ifdef DEBUG
      MessageInterface::ShowMessage("LEAVING ComputeFunAndJac\n");
   #endif
}


//------------------------------------------------------------------------------
//  RSMatrix* ComputeSparsity()
//------------------------------------------------------------------------------
/**
 * This method computes the sparsity pattern and returns a pointer to it
 *
 * @return   a pointer to the sparsity pattern
 *
 */
//------------------------------------------------------------------------------
RSMatrix* NLPFuncUtil_AlgPath::ComputeSparsity()
{
   return dataFuncUtil.GetJacSparsityPatternPointer();
}

//------------------------------------------------------------------------------
// proteced methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void FillUserNLPMatrices(const std::vector<FunctionOutputData*> &funcData,
//                           Rvector  &QVector,
//                           RSMatrix &parQMatrix)
//------------------------------------------------------------------------------
/**
 * This method fills the matrices
 *
 * @param <funcData>   input function data
 * @param <QVector>    output - the Q Vector
 * @param <parQMatrix> output - the par Q Matrix
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_AlgPath::FillUserNLPMatrices(
                          const std::vector<FunctionOutputData*> &funcData,
                          Rvector   &QVector,
                          RSMatrix  &parQMatrix)
{
   // Update arrays that are not constant (q, and dq/dz)
   Integer      lowIdx = 0;
   IntegerArray stateIdxs;
   IntegerArray controlIdxs;
   Rmatrix      tempMat, tempMat2, tempMat3, tempMat4;
   Real         tempValue;

   Rvector tempVec; //, QVector(numFunctions); // by YKK
   QVector.SetSize(numFunctions); // by YKK

   // set the sparsity pattern of the parQMatrix before setting values to save
   // time.; by YKK
   const RSMatrix* ptrDMatrix = dataFuncUtil.GetDMatrixPtr();
   bool hasZeros = true;
   parQMatrix =SparseMatrixUtil::GetSparsityPattern(ptrDMatrix, hasZeros);

   #ifdef DEBUG
      MessageInterface::ShowMessage(
                  "FillUserNLPMatrices: about to enter pointIdx loop ... \n");
      MessageInterface::ShowMessage(
                  "size of funcData = %d\n", (Integer) funcData.size());
      MessageInterface::ShowMessage(
                  "numFunctionPoints = %d\n", numFunctionPoints);
      MessageInterface::ShowMessage(
                  "numFunctionsPerPoint = %d\n", numFunctionsPerPoint);
      MessageInterface::ShowMessage(
                  "   parQMatrix has been sized to %d x %d\n",
                  numFunctions, numDecisionParams);
   #endif
   for (Integer pointIdx = 0; pointIdx < numFunctionPoints; pointIdx++)
   {
      tempVec = funcData.at(pointIdx)->GetFunctionValues();

      for (Integer funcIdx = 0; funcIdx < numFunctionsPerPoint; ++funcIdx)
      {
         // add QVector
         QVector(lowIdx + funcIdx) = tempVec[funcIdx];

         // accumulate partial Q matrix
         if (hasStateVars)
         {
            #ifdef DEBUG
               MessageInterface::ShowMessage(
                  " --- At pointIdx = %d, attempting to get funcData at that "
                  "index ...\n", pointIdx);
            #endif
            stateIdxs = funcData.at(pointIdx)->GetStateIdxs();
            tempMat = funcData.at(pointIdx)->GetJacobian(UserFunction::STATE);
            #ifdef DEBUG
               Integer r, c;
               tempMat.GetSize(r,c);
               MessageInterface::ShowMessage(
                  "FillUserNLPMatrices: (funcIdx = %d) size of state jacobian "
                  "is %d x %d ... \n", funcIdx, r, c);
               MessageInterface::ShowMessage(
                  "                     size of stateIdxs is %d\n",
                  stateIdxs.size());
            #endif

            for (UnsignedInt colIdx = 0; colIdx < stateIdxs.size(); ++colIdx)
            {
               #ifdef DEBUG
                  MessageInterface::ShowMessage(
                                       " --- At funcIdx = %d, colIdx = %d\n",
                                       funcIdx, colIdx);
               #endif
               tempValue = tempMat(funcIdx, colIdx);
               #ifdef DEBUG
                  MessageInterface::ShowMessage(
                     " --- (funcIdx = %d, colIdx = %d) tempValue = %12.10f\n",
                     funcIdx, colIdx, tempValue);
               #endif
               if (tempValue != 0)
               {
                  #ifdef DEBUG
                     MessageInterface::ShowMessage(
                        " --- Attempting to add element (in state part) to "
                        "parQMatrix at (%d, %d)\n",
                        (lowIdx+funcIdx),stateIdxs[colIdx]);
                  #endif
                  SparseMatrixUtil::SetElement(parQMatrix, lowIdx + funcIdx,
                                               stateIdxs[colIdx], tempValue);
                  #ifdef DEBUG
                     MessageInterface::ShowMessage(
                              " --- Element added to parQMatrix at (%d, %d)\n",
                              (lowIdx+funcIdx),stateIdxs[colIdx]);
                                 
                  #endif
               }
            }
         }

         if (hasControlVars)
         {
            #ifdef DEBUG
               MessageInterface::ShowMessage(
                  " HAS control vars, now getting the control jacobian ...\n");
                           
            #endif
            controlIdxs  = funcData.at(pointIdx)->GetControlIdxs();
            tempMat2 = funcData.at(pointIdx)->GetJacobian(UserFunction::CONTROL);

            #ifdef DEBUG
               Integer r, c;
               tempMat2.GetSize(r,c);
               MessageInterface::ShowMessage(
                     "FillUserNLPMatrices: (funcIdx = %d) size of control "
                     "jacobian is %d x %d ... \n", funcIdx, r, c);
               MessageInterface::ShowMessage("                     size of "
                                    "controlIdxs is %d\n", controlIdxs.size());
            #endif
            for (UnsignedInt colIdx = 0; colIdx < controlIdxs.size(); ++colIdx)
            {
               tempValue = tempMat2(funcIdx, colIdx);
               #ifdef DEBUG
                  MessageInterface::ShowMessage(
                     " --- (funcIdx = %d, colIdx = %d) tempValue = %12.10f\n",
                     funcIdx, colIdx, tempValue);
               #endif
               if (tempValue != 0)
               {
                  #ifdef DEBUG
                     MessageInterface::ShowMessage(
                           " --- Attempting to add element (in control part) "
                           "to parQMatrix at (%d, %d)\n",
                           (lowIdx+funcIdx),controlIdxs[colIdx]);
                  #endif
                  SparseMatrixUtil::SetElement(parQMatrix, lowIdx + funcIdx,
                  controlIdxs[colIdx], tempValue);
               }
            }
         }

         // for Ti
         #ifdef DEBUG
            MessageInterface::ShowMessage(" --- getting tempMat3\n");
            MessageInterface::ShowMessage(" --- dTimedTi(pointIdx) = %12.10f\n",
                                          dTimedTi(pointIdx));
            Rmatrix tmpppp = funcData.at(pointIdx)->GetTimeJacobian();
            Integer rrr, ccc;
            tmpppp.GetSize(rrr, ccc);
            MessageInterface::ShowMessage("   tmpppp is of size %d x %d\n",
                                          rrr, ccc);
            
         #endif
         tempMat3 = funcData.at(pointIdx)->GetJacobian(UserFunction::TIME) *
                    dTimedTi(pointIdx);
         #ifdef DEBUG
            Integer r, c;
            tempMat3.GetSize(r,c);
            MessageInterface::ShowMessage(
                        "FillUserNLPMatrices (TIME): (funcIdx = %d) size of "
                        "time jacobian is %d x %d ... \n", funcIdx, r, c);
         #endif

         if (tempMat3(funcIdx, 0) != 0)
            SparseMatrixUtil::SetElement(parQMatrix, lowIdx + funcIdx, 
                                         0, tempMat3(funcIdx, 0));
         
         // for Tf
         tempMat4 = funcData.at(pointIdx)->GetJacobian(UserFunction::TIME) *
                    dTimedTf(pointIdx);

         if (tempMat4(funcIdx, 0) != 0)
            SparseMatrixUtil::SetElement(parQMatrix, lowIdx + funcIdx, 
                                         1, tempMat4(funcIdx, 0));
      }

      lowIdx += numFunctionsPerPoint;
   
   }
}
