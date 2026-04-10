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


#include "NLPFuncUtil.hpp"
#include <iostream>
#include "Phase.hpp"
#include "Rvector.hpp"

class TestNLPFuncUtil
{
public:
   static void RunTests()
   {
  
      Phase pseudoPhase;
      pseudoPhase.Initialize();
      NLPFuncUtil nlpFuncUtil;
      nlpFuncUtil.SetPhasePointer(&pseudoPhase);

      Rvector decVector;

      nlpFuncUtil.GetDecisionVector(decVector);
      
      std::cout << "The number of decision variables is " 
                << decVector.GetSize() << std::endl;

      
      bool hasStateVars = nlpFuncUtil.HasStateVars();
      bool hasControlVars = nlpFuncUtil.HasControlVars();
      unsigned int numStateVars = nlpFuncUtil.GetNumStateVars();

      std::cout << "HasStateVars?:" << hasStateVars << std::endl;
      std::cout << "HasControlVars?:" << hasStateVars << std::endl;
      std::cout << "The number of stateVars?:" << numStateVars << std::endl;


      std::cout << "Test on the assignment operator begins ......."
                << std::endl;

      NLPFuncUtil nlpFuncUtil2;

      nlpFuncUtil2 = nlpFuncUtil;
      //decVector.SetSize(0);

      nlpFuncUtil2.GetDecisionVector(decVector);
      
      std::cout << "The number of decision variables is " 
                << decVector.GetSize() << std::endl;

      
      
      hasStateVars = nlpFuncUtil2.HasStateVars();
      hasControlVars = nlpFuncUtil2.HasControlVars();
      numStateVars = nlpFuncUtil2.GetNumStateVars();

      std::cout << "HasStateVars?:" << hasStateVars << std::endl;
      std::cout << "HasControlVars?:" << hasStateVars << std::endl;
      std::cout << "The number of stateVars?:" << numStateVars << std::endl;


      std::cout << "Test on the copy constructor begins ......."
                << std::endl;

      NLPFuncUtil nlpFuncUtil3(nlpFuncUtil);
      //decVector.SetSize(0);

      nlpFuncUtil2.GetDecisionVector(decVector);
      
      std::cout << "The number of decision variables is " 
                << decVector.GetSize() << std::endl;

      
      hasStateVars = nlpFuncUtil3.HasStateVars();
      hasControlVars = nlpFuncUtil3.HasControlVars();
      numStateVars = nlpFuncUtil3.GetNumStateVars();

      std::cout << "HasStateVars?:" << hasStateVars << std::endl;
      std::cout << "HasControlVars?:" << hasStateVars << std::endl;
      std::cout << "The number of stateVars?:" << numStateVars << std::endl;


   };
};
