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


#include "Rvector.hpp"
#include "RadauMathUtil.hpp"
#include <iostream>

#ifndef TestRadauMathUtil_hpp
#define TestRadauMathUtil_hpp

class TestRadauMathUtil
{
public:
   static void RunTests()
   {     
      // body 7: this is for LGR math util
      std::cout << "\nRadauMathUtil Unit-Tester: test begins" << std::endl;
      
      std::vector<double> stdMeshIntervalFractions;
      RSMatrix lagDiffMat;
      IntegerArray meshIntervalNumPoints;

      stdMeshIntervalFractions.push_back(-1);
      stdMeshIntervalFractions.push_back(0);
      stdMeshIntervalFractions.push_back(0.25);
      stdMeshIntervalFractions.push_back(1);
      meshIntervalNumPoints.push_back(5);
      meshIntervalNumPoints.push_back(3);
      meshIntervalNumPoints.push_back(4);
   
      Rvector meshIntervalFractions, lgrNodes, lgrWeigthVec;

      meshIntervalFractions.SetSize(stdMeshIntervalFractions.size());

      for (int idx = 0; idx < stdMeshIntervalFractions.size(); ++idx)
      {
         meshIntervalFractions(idx) = stdMeshIntervalFractions[idx];
      }

      RadauMathUtil::ComputeMultiSegmentLGRNodes(&meshIntervalFractions, &meshIntervalNumPoints, 
                                             lgrNodes,
                                             lgrWeigthVec,
                                             lagDiffMat);
      std::vector<double> stdNGRNodes;
      stdNGRNodes.resize(13);
      stdNGRNodes[0] = -1.00000000000000000e+00;
      stdNGRNodes[1] = -8.60240135656219480e-01;
      stdNGRNodes[2] = -5.83590432368916830e-01;
      stdNGRNodes[3] = -2.76843013638123800e-01;
      stdNGRNodes[4] = -5.71041961145176690e-02;
      stdNGRNodes[6] = 8.87627564304205450e-02;
      stdNGRNodes[7] = 2.11237243569579460e-01;
      stdNGRNodes[8] = 2.50000000000000000e-01;
      stdNGRNodes[9] = 4.09255403679364690e-01;
      stdNGRNodes[10] = 6.92899851669449010e-01;
      stdNGRNodes[11] = 9.33559030365472050e-01;
      stdNGRNodes[12] = 1.00000000000000000e+00;

      std::vector<double> stdWeigthVec;
      stdWeigthVec.resize(12);
      stdWeigthVec[0] = 4.00000000000000010e-02;
      stdWeigthVec[1] = 2.23103901083570920e-01;
      stdWeigthVec[2] = 3.11826522975741200e-01;
      stdWeigthVec[3] = 2.81356015149462120e-01;
      stdWeigthVec[4] = 1.43713560791225660e-01;
      stdWeigthVec[5] = 2.77777777777777760e-02;
      stdWeigthVec[6] = 1.28121456547105380e-01;
      stdWeigthVec[7] = 9.41007656751167150e-02;
      stdWeigthVec[8] = 4.68750000000000000e-02;
      stdWeigthVec[9] = 2.46633239985044860e-01;
      stdWeigthVec[10] = 2.91145101632378940e-01;
      stdWeigthVec[11] = 1.65346658382575950e-01;
 
      double error = 0;     
      for (int idx = 0; idx < lgrNodes.GetSize(); ++idx)
         error += abs(lgrNodes(idx)- stdNGRNodes[idx]);

      std::cout << "\ntotal lgrNodes error is:" << error <<std::endl;
   
      error = 0;
      for (int idx = 0; idx < lgrWeigthVec.GetSize(); ++idx)
         error += abs(lgrWeigthVec(idx)- stdWeigthVec[idx]);

      std::cout << "total lgr weigths error is:" << error << std::endl;
      RSMatrix stdLagDiffMat(12,13,62);
      stdLagDiffMat(0,0) = -1.3000000000000000e+01;
      stdLagDiffMat(1,0) = -2.8099836552797135e+00;
      stdLagDiffMat(2,0) = 6.5706275713436046e-01;
      stdLagDiffMat(3,0) = -2.7433807777519426e-01;
      stdLagDiffMat(4,0) = 1.3370616384921585e-01;
      stdLagDiffMat(0,1) = 1.8219282311088101e+01;
      stdLagDiffMat(1,1) = -5.8123305258081404e-01;
      stdLagDiffMat(2,1) = -2.5183209492110650e+00;
      stdLagDiffMat(3,1) = 8.6590078028313444e-01;
      stdLagDiffMat(4,1) = -3.9970520793996539e-01;
      stdLagDiffMat(0,2) = -8.7771142041504699e+00;
      stdLagDiffMat(1,2) = 5.1883409064071850e+00;
      stdLagDiffMat(2,2) = -8.5676524539717702e-01;
      stdLagDiffMat(3,2) = -2.3637971760686072e+00;
      stdLagDiffMat(4,2) = 8.7518639620026473e-01;
      stdLagDiffMat(0,3) = 6.9702561166566586e+00;
      stdLagDiffMat(1,3) = -3.3931519180649539e+00;
      stdLagDiffMat(2,3) = 4.4960171258133954e+00;
      stdLagDiffMat(3,3) = -1.8060777240836461e+00;
      stdLagDiffMat(4,3) = -2.8919426153801169e+00;
      stdLagDiffMat(0,4) = -8.4124242235942877e+00;
      stdLagDiffMat(1,4) = 3.8786632197240105e+00;
      stdLagDiffMat(2,4) = -4.1221652462433749e+00;
      stdLagDiffMat(3,4) = 7.1613807201453881e+00;
      stdLagDiffMat(4,4) = -8.7559239779383624e+00;
      stdLagDiffMat(0,5) = 4.9999999999999991e+00;
      stdLagDiffMat(1,5) = -2.2826355002057142e+00;
      stdLagDiffMat(2,5) = 2.3441715579038611e+00;
      stdLagDiffMat(3,5) = -3.5830685225010748e+00;
      stdLagDiffMat(4,5) = 1.1038679241208964e+01;
      stdLagDiffMat(5,5) = -1.9999999999999996e+01;
      stdLagDiffMat(6,5) = -4.2127890589687240e+00;
      stdLagDiffMat(7,5) = 1.0127890589687234e+00;
      stdLagDiffMat(5,6) = 3.0127890589687230e+01;
      stdLagDiffMat(6,6) = -3.1010205144336460e+00;
      stdLagDiffMat(7,6) = -4.6713603387616214e+00;
      stdLagDiffMat(5,7) = -2.2127890589687233e+01;
      stdLagDiffMat(6,7) = 1.4271360338761625e+01;
      stdLagDiffMat(7,7) = -1.2898979485566361e+01;
      stdLagDiffMat(5,8) = 1.2000000000000000e+01;
      stdLagDiffMat(6,8) = -6.9575507653592545e+00;
      stdLagDiffMat(7,8) = 1.6557550765359260e+01;
      stdLagDiffMat(8,8) = -1.1333333333333334e+01;
      stdLagDiffMat(9,8) = -2.4295167979123393e+00;
      stdLagDiffMat(10,8) = 5.7972194828344181e-01;
      stdLagDiffMat(11,8) = -2.3183780343232760e-01;
      stdLagDiffMat(8,9) = 1.6228999217576920e+01;
      stdLagDiffMat(9,9) = -8.4638946020695816e-01;
      stdLagDiffMat(10,9) = -2.3395746516811147e+00;
      stdLagDiffMat(11,9) = 7.8120197613842135e-01;
      stdLagDiffMat(8,10) = -8.7936502261708576e+00;
      stdLagDiffMat(9,10) = 5.3126905277099947e+00;
      stdLagDiffMat(10,10) = -1.6281333718595903e+00;
      stdLagDiffMat(11,10) = -2.5646763694062833e+00;
      stdLagDiffMat(8,11) = 9.2313176752606072e+00;
      stdLagDiffMat(9,11) = -4.6566215448339179e+00;
      stdLagDiffMat(10,11) = 6.7322861845218815e+00;
      stdLagDiffMat(11,11) = -7.5254771679334578e+00;
      stdLagDiffMat(8,12) = -5.3333333333333357e+00;
      stdLagDiffMat(9,12) = 2.6198372752432206e+00;
      stdLagDiffMat(10,12) = -3.3443001092646183e+00;
      stdLagDiffMat(11,12) = 9.5407893646336479e+00;

      error = 0;
      double tmp;
      for (int idx1 = 0; idx1 < 12; ++idx1)
      {
         for (int idx2 = 0; idx2 < 13; ++idx2)
         {
            tmp = abs(lagDiffMat(idx1,idx2) - stdLagDiffMat(idx1,idx2));
            if (lagDiffMat(idx1,idx2) != 0)
               tmp /= abs(lagDiffMat(idx1,idx2));
            error += tmp;
            //std::cout << "("<< idx1 << "," << idx2 << "):" << lagDiffMat(idx1,idx2) - stdLagDiffMat(idx1,idx2) <<std::endl;
 
         }
      }
      
      std::cout << "total differentiation matrix error is:" << error << "\n" << std::endl;
   };

};
#endif // TestRadauMathUtil_hpp