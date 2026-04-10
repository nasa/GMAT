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


// This is an example of a user guess class

#ifndef ExampleUserGuessClass_hpp
#define ExampleUserGuessClass_hpp

#include "TrajectoryData.hpp"


class ExampleUserGuessClass : public TrajectoryData
{
public:
    ExampleUserGuessClass() :
        TrajectoryData()
    {
        numStateParams = 2;
        numControlParams = 1;
        numIntegralParams = 0;
    };
    
    Rmatrix GetState(Rvector requestedTimes)
    {
        Rmatrix output;
        output.SetSize(requestedTimes.GetSize(),numStateParams);
        for (Integer idx = 0; idx < requestedTimes.GetSize(); idx++)
            for (Integer jdx = 0; jdx < numStateParams; jdx++)
                output(idx,jdx) = 1.0;
        return output;
    };
    Rmatrix GetControl(Rvector requestedTimes)
    {
        Rmatrix output;
        output.SetSize(requestedTimes.GetSize(),numControlParams);
        for (Integer idx = 0; idx < requestedTimes.GetSize(); idx++)
            for (Integer jdx = 0; jdx < numControlParams; jdx++)
                output(idx,jdx) = 0.0;
        return output;
    };
    Rmatrix GetIntegral(Rvector requestedTimes)
    {
        Rmatrix output;
        output.SetSize(numIntegralParams,requestedTimes.GetSize());
        for (Integer idx = 0; idx < requestedTimes.GetSize(); idx++)
            for (Integer jdx = 0; jdx < numIntegralParams; jdx++)
                output(jdx,idx) = 1.0;
        return output;
    };

    Integer numStateParams;
    Integer numControlParams;
    Integer numIntegralParams;
};

#endif