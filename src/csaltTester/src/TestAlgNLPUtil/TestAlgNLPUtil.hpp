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


#include <iostream>
#include "gmatdefs.hpp"
#include "DecisionVector.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "PathFuncProperties.hpp"
#include "UserPathFunctionData.hpp"

class TestAlgNLPUtil
{
public:
   static int RunTests()
   { 
      // =======================================================
      // Set up the function properties class
      // =======================================================
      PathFuncProperties funcProps;

      funcProps.SetNumFunctions(1);
      funcProps.SetHasStateVars(true);
      funcProps.SetHasControlVars(true);

      // set jacobian patterns
      Rmatrix stateJacPattern(1,5);
      funcProps.SetStateJacPattern(stateJacPattern);

      Rmatrix controlJacPattern(1,2);
      controlJacPattern(1,1) = 1;
      controlJacPattern(1,2) = 1;
      funcProps.SetControlJacPattern(controlJacPattern);

      Rmatrix timeJacPattern(1,1);
      timeJacPattern(1,1) = 1;
      funcProps.SetControlJacPattern(timeJacPattern);

      // =======================================================
      // Set up the function data vector
      // =======================================================
      UserPathFunctionData thePointData1;
      thePointData1.SetIsInitializing(true);
      thePointData1.SetNumFunctions(1);
      //Set Function Values
      Rvector funValues1(1,1);
      funValues1(1) = 2.679418114163102;
      thePointData1.SetFunctions(funValues1);
      //Set State Jacobian
      Rmatrix jacValuesState1(1,5);
      jacValuesState1(1,1) = 0;
      jacValuesState1(1,2) = 0;
      jacValuesState1(1,3) = 0;
      jacValuesState1(1,4) = 0;
      jacValuesState1(1,5) = 0;
      thePointData1.SetStateJacobian(jacValuesState1);
      //Set Control Jacobian
      Rmatrix jacValuesControl1(1,2);
      jacValuesControl1(1,1) = 3.187544275818019;
      jacValuesControl1(1,2) = 0.7464810421920909;
      thePointData1.SetControlJacobian(jacValuesControl1);
      //Set Time Values
      Rmatrix jacValuesTime1(1,1);
      jacValuesTime1(1,1) = 0;
      thePointData1.SetControlJacobian(jacValuesTime1);
      //Set State Indeces
      IntegerArray stateIdxValues1(5,0);
      int arrstateIdxValues1 [] = { 3 , 4 , 5 , 6 , 7 };
      stateIdxValues1.insert(stateIdxValues1.begin(),arrstateIdxValues1,arrstateIdxValues1+5);
      //Set Control Indeces
      IntegerArray controlIdxValues1(2,0);
      int arrcontrolIdxValues1 [] = { 8 , 9 };
      controlIdxValues1.insert(controlIdxValues1.begin(),arrcontrolIdxValues1,arrcontrolIdxValues1+2);
      thePointData1.SetNLPData(1,0,stateIdxValues1,controlIdxValues1);
      thePointData1.SetIsInitializing(false);
 
      UserPathFunctionData thePointData2;
      thePointData2.SetIsInitializing(true);
      thePointData2.SetNumFunctions(1);
      //Set Function Values
      Rvector funValues2(1,1);
      funValues2(1) = 2.679418114163102;
      thePointData2.SetFunctions(funValues2);
      //Set State Jacobian
      Rmatrix jacValuesState2(1,5);
      jacValuesState2(1,1) = 0;
      jacValuesState2(1,2) = 0;
      jacValuesState2(1,3) = 0;
      jacValuesState2(1,4) = 0;
      jacValuesState2(1,5) = 0;
      thePointData2.SetStateJacobian(jacValuesState2);
      //Set Control Jacobian
      Rmatrix jacValuesControl2(1,2);
      jacValuesControl2(1,1) = 3.187544275818019;
      jacValuesControl2(1,2) = 0.7464810421920909;
      thePointData2.SetControlJacobian(jacValuesControl2);
      //Set Time Values
      Rmatrix jacValuesTime2(1,1);
      jacValuesTime2(1,1) = 0;
      thePointData2.SetControlJacobian(jacValuesTime2);
      //Set State Indeces
      IntegerArray stateIdxValues2(5,0);
      int arrstateIdxValues2 [] = { 10 , 11 , 12 , 13 , 14 };
      stateIdxValues2.insert(stateIdxValues2.begin(),arrstateIdxValues2,arrstateIdxValues2+5);
      //Set Control Indeces
      IntegerArray controlIdxValues2(2,0);
      int arrcontrolIdxValues2 [] = { 15 , 16 };
      controlIdxValues2.insert(controlIdxValues2.begin(),arrcontrolIdxValues2,arrcontrolIdxValues2+2);
      thePointData2.SetNLPData(2,0,stateIdxValues2,controlIdxValues2);
      thePointData2.SetIsInitializing(false);
 
      UserPathFunctionData thePointData3;
      thePointData3.SetIsInitializing(true);
      thePointData3.SetNumFunctions(1);
      //Set Function Values
      Rvector funValues3(1,1);
      funValues3(1) = 2.679418114163102;
      thePointData3.SetFunctions(funValues3);
      //Set State Jacobian
      Rmatrix jacValuesState3(1,5);
      jacValuesState3(1,1) = 0;
      jacValuesState3(1,2) = 0;
      jacValuesState3(1,3) = 0;
      jacValuesState3(1,4) = 0;
      jacValuesState3(1,5) = 0;
      thePointData3.SetStateJacobian(jacValuesState3);
      //Set Control Jacobian
      Rmatrix jacValuesControl3(1,2);
      jacValuesControl3(1,1) = 3.187544275818019;
      jacValuesControl3(1,2) = 0.7464810421920909;
      thePointData3.SetControlJacobian(jacValuesControl3);
      //Set Time Values
      Rmatrix jacValuesTime3(1,1);
      jacValuesTime3(1,1) = 0;
      thePointData3.SetControlJacobian(jacValuesTime3);
      //Set State Indeces
      IntegerArray stateIdxValues3(5,0);
      int arrstateIdxValues3 [] = { 17 , 18 , 19 , 20 , 21 };
      stateIdxValues3.insert(stateIdxValues3.begin(),arrstateIdxValues3,arrstateIdxValues3+5);
      //Set Control Indeces
      IntegerArray controlIdxValues3(2,0);
      int arrcontrolIdxValues3 [] = { 22 , 23 };
      controlIdxValues3.insert(controlIdxValues3.begin(),arrcontrolIdxValues3,arrcontrolIdxValues3+2);
      thePointData3.SetNLPData(3,0,stateIdxValues3,controlIdxValues3);
      thePointData3.SetIsInitializing(false);
 
      UserPathFunctionData thePointData4;
      thePointData4.SetIsInitializing(true);
      thePointData4.SetNumFunctions(1);
      //Set Function Values
      Rvector funValues4(1,1);
      funValues4(1) = 2.507768026686315;
      thePointData4.SetFunctions(funValues4);
      //Set State Jacobian
      Rmatrix jacValuesState4(1,5);
      jacValuesState4(1,1) = 0;
      jacValuesState4(1,2) = 0;
      jacValuesState4(1,3) = 0;
      jacValuesState4(1,4) = 0;
      jacValuesState4(1,5) = 0;
      thePointData4.SetStateJacobian(jacValuesState4);
      //Set Control Jacobian
      Rmatrix jacValuesControl4(1,2);
      jacValuesControl4(1,1) = 0.8963132368110889;
      jacValuesControl4(1,2) = 3.037712081199021;
      thePointData4.SetControlJacobian(jacValuesControl4);
      //Set Time Values
      Rmatrix jacValuesTime4(1,1);
      jacValuesTime4(1,1) = 0;
      thePointData4.SetControlJacobian(jacValuesTime4);
      //Set State Indeces
      IntegerArray stateIdxValues4(5,0);
      int arrstateIdxValues4 [] = { 24 , 25 , 26 , 27 , 28 };
      stateIdxValues4.insert(stateIdxValues4.begin(),arrstateIdxValues4,arrstateIdxValues4+5);
      //Set Control Indeces
      IntegerArray controlIdxValues4(2,0);
      int arrcontrolIdxValues4 [] = { 29 , 30 };
      controlIdxValues4.insert(controlIdxValues4.begin(),arrcontrolIdxValues4,arrcontrolIdxValues4+2);
      thePointData4.SetNLPData(4,0,stateIdxValues4,controlIdxValues4);
      thePointData4.SetIsInitializing(false);
 
      UserPathFunctionData thePointData5;
      thePointData5.SetIsInitializing(true);
      thePointData5.SetNumFunctions(1);
      //Set Function Values
      Rvector funValues5(1,1);
      funValues5(1) = 2.679418114163102;
      thePointData5.SetFunctions(funValues5);
      //Set State Jacobian
      Rmatrix jacValuesState5(1,5);
      jacValuesState5(1,1) = 0;
      jacValuesState5(1,2) = 0;
      jacValuesState5(1,3) = 0;
      jacValuesState5(1,4) = 0;
      jacValuesState5(1,5) = 0;
      thePointData5.SetStateJacobian(jacValuesState5);
      //Set Control Jacobian
      Rmatrix jacValuesControl5(1,2);
      jacValuesControl5(1,1) = 0.7464810421920909;
      jacValuesControl5(1,2) = 3.187544275818019;
      thePointData5.SetControlJacobian(jacValuesControl5);
      //Set Time Values
      Rmatrix jacValuesTime5(1,1);
      jacValuesTime5(1,1) = 0;
      thePointData5.SetControlJacobian(jacValuesTime5);
      //Set State Indeces
      IntegerArray stateIdxValues5(5,0);
      int arrstateIdxValues5 [] = { 31 , 32 , 33 , 34 , 35 };
      stateIdxValues5.insert(stateIdxValues5.begin(),arrstateIdxValues5,arrstateIdxValues5+5);
      //Set Control Indeces
      IntegerArray controlIdxValues5(2,0);
      int arrcontrolIdxValues5 [] = { 36 , 37 };
      controlIdxValues5.insert(controlIdxValues5.begin(),arrcontrolIdxValues5,arrcontrolIdxValues5+2);
      thePointData5.SetNLPData(5,0,stateIdxValues5,controlIdxValues5);
      thePointData5.SetIsInitializing(false);
 
      std::vector<UserPathFunctionData> funcData(5);
      funcData.at( 0 )=thePointData1;
      funcData.at( 1 )=thePointData2;
      funcData.at( 2 )=thePointData3;
      funcData.at( 3 )=thePointData4;
      funcData.at( 4 )=thePointData5;

      funcData.at(0).GetFunctionValues();
      return 1;
   } 
};