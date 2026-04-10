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


#include "SparseMatrixUtil.hpp"
#include <iostream>
#include "NLPFunctionData.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

#ifndef TestNLPFunctionData_hpp
#define TestNLPFunctionData_hpp

class MATLABData
{
public:
    
   //  The nlp functions are computed using
   //       nlpFuncs = conAMat*DecVector + conBMat*userFuns
   //  The sparsity pattern is determined using
   //       sparsePattern = conAMat + conBMat*conDMat
   //  The Jacobian is calculated using 
   //       nlpJac = conAMat + conBMat*conQMat
    
   //default constructor
   MATLABData() {};
   ~MATLABData() {};

   
   RSMatrix MATLABData::GetSparseAMatrix();
   RSMatrix MATLABData::GetSparseAMatrix11();
   RSMatrix MATLABData::GetSparseAMatrix12();
   RSMatrix MATLABData::GetSparseAMatrix21();
   RSMatrix MATLABData::GetSparseAMatrix22();

   RSMatrix MATLABData::GetSparseparQMatrix();
   RSMatrix MATLABData::GetSparseBMatrix();
   RSMatrix MATLABData::GetSparseDMatrix();

   std::vector<double> MATLABData::GetQVector();
   std::vector<double> MATLABData::GetDecVector();
   
   std::vector<double> MATLABData::GetFuncValues();
   RSMatrix MATLABData::GetFuncJacMatrix();
   RSMatrix MATLABData::GetFuncJacSparsityPattern();

   RSMatrix GetSparsityPattern(RSMatrix spMat); 

   bool GetSparsityPattern(RSMatrix spMat, 
                           std::vector<unsigned int> &rowIdxVec, 
	                        std::vector<unsigned int> &colIdxVec); 

   // private methods go; they will be set to be private after unit-testing
   size_t GetNumNonZeroElements(RSMatrix spMat); 
   
   bool GetThreeVectorForm(RSMatrix spMat, 
                           std::vector<unsigned int> &rowIdxVec, 
	                        std::vector<unsigned int> &colIdxVec, 
                           std::vector<double> &valueVec); 
};

class TestNLPFunctionData
{
public:

static void RunTests()
   {
      std::cout << "\nNLPFunctionData Unit-Tester: test begins" << std::endl;
      NLPFunctionData nlpFD;

      nlpFD.Initialize(178,359,1);

      MATLABData dummyData;
      RSMatrix AMatrix = dummyData.GetSparseAMatrix();
      RSMatrix AMatrix11 = dummyData.GetSparseAMatrix11();
      RSMatrix AMatrix12 = dummyData.GetSparseAMatrix12();
      RSMatrix AMatrix21 = dummyData.GetSparseAMatrix21();
      RSMatrix AMatrix22 = dummyData.GetSparseAMatrix22();

      RSMatrix BMatrix = dummyData.GetSparseBMatrix();
      RSMatrix DMatrix = dummyData.GetSparseDMatrix();
      RSMatrix parQMatrix = dummyData.GetSparseparQMatrix();
      std::vector<double> DecVector = dummyData.GetDecVector();
      std::vector<double> QVector   = dummyData.GetQVector();
      std::vector<double> funcValues= dummyData.GetFuncValues();
      RSMatrix funcJacSparisityPattern   = dummyData.GetFuncJacSparsityPattern();
      RSMatrix funcJacMatrix   = dummyData.GetFuncJacMatrix();
      // body1 begins............................................

      IntegerArray ARowIdxVec, AColIdxVec, 
                           ARowIdxVec11, ARowIdxVec12, ARowIdxVec21, ARowIdxVec22,
                           AColIdxVec11, AColIdxVec12, AColIdxVec21, AColIdxVec22;
      Rvector AValueVec, AValueVec11, AValueVec12, AValueVec21, AValueVec22;


      SparseMatrixUtil::GetThreeVectorForm(&AMatrix, ARowIdxVec, AColIdxVec, AValueVec); 
      SparseMatrixUtil::GetThreeVectorForm(&AMatrix11, ARowIdxVec11, AColIdxVec11, AValueVec11); 
      SparseMatrixUtil::GetThreeVectorForm(&AMatrix12, ARowIdxVec12, AColIdxVec12, AValueVec12); 
      SparseMatrixUtil::GetThreeVectorForm(&AMatrix21, ARowIdxVec21, AColIdxVec21, AValueVec21); 
      SparseMatrixUtil::GetThreeVectorForm(&AMatrix22, ARowIdxVec22, AColIdxVec22, AValueVec22); 

      nlpFD.InsertAMatPartition(0, 0, 
                                 &ARowIdxVec11, 
                                 &AColIdxVec11, 
						               &AValueVec11); 

      nlpFD.InsertAMatPartition(0, 200, 
                                 &ARowIdxVec12, 
                                 &AColIdxVec12, 
						               &AValueVec12); 

      nlpFD.InsertAMatPartition(100, 0, 
                                 &ARowIdxVec21, 
                                 &AColIdxVec21, 
						               &AValueVec21); 

      nlpFD.InsertAMatPartition(100, 200, 
                                 &ARowIdxVec22, 
                                 &AColIdxVec22, 
						               &AValueVec22);

      RSMatrix testResult = AMatrix - nlpFD.GetAMatrix();

      double error;

      error = SparseMatrixUtil::GetAbsTotalSum(&testResult);

      std::cout << "\nBlockwise insertion error is:" << error << std::endl;
      // body2 begins...........................................

      SparseMatrixUtil::GetThreeVectorForm(&AMatrix, ARowIdxVec, AColIdxVec, AValueVec); 
      nlpFD.InsertAMatPartition(0, 0, 
                                 &ARowIdxVec, 
                                 &AColIdxVec, 
						               &AValueVec); 

      SparseMatrixUtil::GetThreeVectorForm(&BMatrix, ARowIdxVec, AColIdxVec, AValueVec); 
      nlpFD.InsertBMatPartition(0, 0, 
                                 &ARowIdxVec, 
                                 &AColIdxVec, 
						               &AValueVec); 

      //nlpFD.SetDecisionVector(DecVector);
      //nlpFD.SetQVector(QVector);
      // body3 begins...........................................
   
      Rvector funcValueVec, stdQVector, stdDecVector;
      stdQVector.SetSize(QVector.size());
      stdDecVector.SetSize(DecVector.size());

      for (int idx = 0; idx < QVector.size(); ++idx)
      {
         stdQVector(idx) = QVector[idx];
      
      }

      for (int idx = 0; idx < DecVector.size(); ++idx)
      {
         stdDecVector(idx) = DecVector[idx];      
      }

      nlpFD.ComputeFunctions(&stdQVector, &stdDecVector, funcValueVec);
   
      error = 0;
      //double funcValueAbsSum = 0;
      for (int idx = 0; idx < funcValues.size(); ++idx)
      {
         error += abs(funcValueVec(idx) - funcValues[idx]);
         //funcValueAbsSum += abs(funcValues[idx]);
         //std::cout << "funcValueVec: " << funcValueVec(idx) << ", funcValues: " << funcValues[idx]  << std::endl;
         //error =  funcValues[idx];
      }   
      std::cout << "funcValue computation error is:" << error << std::endl;
      //std::cout << "funcValue Abs Total Sum is:" << funcValueAbsSum << std::endl;
      
      // body3 begins...........................................

      IntegerArray rowIdxVec, colIdxVec;
      Rvector       valueVec;
      RSMatrix jacArray;


      nlpFD.ComputeJacobian(&parQMatrix, jacArray);


      SparseMatrixUtil::GetThreeVectorForm(&jacArray, rowIdxVec, colIdxVec, valueVec); 

      SparseMatrixUtil::GetThreeVectorForm(&funcJacMatrix, ARowIdxVec, AColIdxVec, AValueVec); 
      error = 0;


      for (int idx = 0; idx < valueVec.GetSize(); idx++)
      {
         if ((ARowIdxVec[idx] == rowIdxVec[idx]) 
               && (AColIdxVec[idx] == colIdxVec[idx]))
         {
            //std::cout << "MATLAB result is:" << AValueVec[idx] <<std::endl;
            //std::cout << "C++ result is:" << valueVec[idx] <<std::endl;
            error += abs(AValueVec(idx) - valueVec(idx));
         }
         else
            std::cout << "error! sparsity pattern does not match!" << std::endl;
      }   

      std::cout << "funcJacMatrix computation error is:" << error << std::endl;
      // body3 begins...........................................
      /// here, summation will be tested.
      SparseMatrixUtil::GetThreeVectorForm(&BMatrix, ARowIdxVec, AColIdxVec, AValueVec); 
   
      /// here I set BMatrix = BMatrix + BMatrix;
      nlpFD.SumBMatPartition(0, 0, &ARowIdxVec, &AColIdxVec, &AValueVec); 
   
      testResult = nlpFD.GetBMatrix() - BMatrix*2;

      error = SparseMatrixUtil::GetAbsTotalSum(&testResult);
      std::cout << "BMatrix summation error is:" << error << std::endl;

      // body4 begins...........................................
      /// here, sparsity pattern determination will be tested.
      
      /// test for IsZeroMatrix method; if D is not set, report an error.

      /// reset Matrices
      SparseMatrixUtil::GetThreeVectorForm(&AMatrix, ARowIdxVec, AColIdxVec, AValueVec); 
      nlpFD.InsertAMatPartition(0, 0, 
                                 &ARowIdxVec, 
                                 &AColIdxVec, 
						               &AValueVec);  

      SparseMatrixUtil::GetThreeVectorForm(&BMatrix, ARowIdxVec, AColIdxVec, AValueVec); 
      nlpFD.InsertBMatPartition(0, 0, 
                                 &ARowIdxVec, 
                                 &AColIdxVec, 
						               &AValueVec); 

      SparseMatrixUtil::GetThreeVectorForm(&DMatrix, ARowIdxVec, AColIdxVec, AValueVec); 
      nlpFD.InsertDMatPartition(0, 0, 
                                 &ARowIdxVec, 
                                 &AColIdxVec, 
						               &AValueVec); 

      
      RSMatrix* jacSparsityPattern = nlpFD.GetJacSparsityPatternPointer();  
      SparseMatrixUtil::GetThreeVectorForm(jacSparsityPattern, rowIdxVec, colIdxVec, valueVec); 

      // JacSparsityPattern obtained from MATLAB
      IntegerArray rowIdxVecMATLAB, colIdxVecMATLAB;
      SparseMatrixUtil::GetThreeVectorForm(&funcJacSparisityPattern, rowIdxVecMATLAB, colIdxVecMATLAB, AValueVec); 
      



      bool isSuccessful = true;
      if (isSuccessful == true)
      {

         for (unsigned int idx = 0; idx < rowIdxVecMATLAB.size(); idx++) 
         {
            if ((rowIdxVecMATLAB[idx] != rowIdxVec[idx]) 
               || (colIdxVecMATLAB[idx] != colIdxVec[idx]))
            {
               std::cout << "sparsity pattern determination error in (" << rowIdxVecMATLAB[idx] << "," << colIdxVecMATLAB[idx] << ")" << std::endl;
            }

         }
         std::cout << "sparsity pattern check is done" <<std::endl;
      }
      else
      {
         std::cout << "sparsity pattern could not be obtained." <<std::endl;
      }

      // test for assignment and copy constructor
      NLPFunctionData nlpData2 = nlpFD, nlpData3;

      nlpData3 = nlpData2;
      nlpData3.ComputeFunctions(&stdQVector, &stdDecVector, funcValueVec);
   }
};

#endif TestNLPFunctionData_hpp