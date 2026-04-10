//------------------------------------------------------------------------------
//                             InterpolatorTest  
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2003/12/12
//
/**
 * Test code for the Interpolator classes
 */
//------------------------------------------------------------------------------


#include <iostream>
#include <math.h>

#include "ConsoleAppException.hpp"
#include "LinearInterpolator.hpp"
#include "CubicSplineInterpolator.hpp"

#define DATASIZE 100
#define DIM      2 
#define POINTS   81
#define START    25


void RunInterpolator(Interpolator *interp, Integer dim)
{
    if (dim > DIM)
        throw ConsoleAppException("Dimension exceeds defined maximum");
  
    // Data that gets interpolated in the test
    Real indep[DATASIZE], data[DATASIZE][DIM], temp1, temp2;
    
    for (Integer i = 0; i < DATASIZE; ++i) {
        indep[i] = i;
        for (Integer j = 0; j < DIM; ++j) {
            temp1 = i + M_PI / ((j+1) * sqrt(i + 0.0001));
            temp2 = 0.03 * M_PI * ((i+j+1) + cos(i/17));
            data[i][j] = (j - 0.5) * sin(temp1) + cos(temp2) * (2 * j - 1.0);
        }
        std::cout << indep[i] << "   " << data[i][0] 
                  << "   " << data[i][1] << "\n" ;
    } 
    
    
    // Feed in data
    for (Integer i = 0; i < START; ++i)
        interp->AddPoint(indep[i], &data[0][i]);
    for (Integer i = 0; i < 5; ++i)
        interp->AddPoint(indep[START+i], &data[START+i][0]);
        
    std::cout << "Setup:\n";
    for (Integer m = 0; m < dim; ++m)
        std::cout << "  Domain ranges from " << indep[START]
                  << " to " << indep[START+4] << "\n"
                  << "  Data ranges from " << data[START][m] 
                  << " to " << data[START+4][m] 
                  << "\n\nInterpolated data:\n";

    // Now interpolate
    Real intermed[DIM], interdep, domain = indep[START+4] - indep[START];
    for (Integer i = 0; i < POINTS; ++i) {
        interdep = indep[START] + domain * i / (POINTS-1);
        if (!interp->Interpolate(interdep, intermed))
            std::cout << "   Interpolate (" << interdep << ") returned false\n";

        std::cout << interdep;
        for (Integer k = 0; k < dim; ++k) {
            std::cout << " " << intermed[k];
        }
        std::cout << "\n";
    }

    // Success!
    std::cout << "\n\n*** GMAT Interpolator test successful! ***\n";
}



int main(int argc, char *argv[])
{
    try {
        std::cout << "********************************************\n"
                  << "***  Interpolator Tests\n"
                  << "********************************************\n"
                  << std::endl;

        std::cout << "Testing linear interpolator\n\n";

        Interpolator *interp = new LinearInterpolator;
        RunInterpolator(interp, 1);
        delete interp;

        std::cout << "Testing cubic spline interpolator\n\n";

        interp = new CubicSplineInterpolator(2);
        RunInterpolator(interp, 2);
        delete interp;
    }
    catch (BaseException &ex) {
        std::cout << ex.GetMessage() << std::endl;
    }

    std::cout << "\nPress 'Enter' to finish . . .";
    std::cin.get();
    return 0;
}

