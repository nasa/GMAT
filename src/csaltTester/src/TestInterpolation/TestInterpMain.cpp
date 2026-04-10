//------------------------------------------------------------------------------
//                           TestInterpMain
//------------------------------------------------------------------------------
// GMAT:  General Mission Analysis Tool
// CSALT: Collocation Standalone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 14, 2017.  Happy Valentine's Day!
/**
 * Simple driver to demonstrate use of a couple of GMAT interpolators
 */
//------------------------------------------------------------------------------

#include "TestInterpMain.hpp"
#include "LinearInterpolator.hpp"
#include "CubicSplineInterpolator.hpp"
#include "NotAKnotInterpolator.hpp"

#include <iostream>
#include <iomanip>

int main()
{
   std::cout << "Interpolator Demonstration\n\n";

   // Build the interpolators: parms are object name and dim of dependent vars
   // Defaults are "" and 1; made explicit here just because...
   LinearInterpolator liner("",1);
   CubicSplineInterpolator cuber("",1);
   NotAKnotInterpolator knoter("",1);

   // Load the interpolation buffers -- 1 dim for now
   std::cout << "Seed values:\n";
   double ind, sq[1];
   for (int i = 0; i < 5; ++i)
   {
      ind = 2.0 * i;
      sq[0] = ind * ind;

      std::cout << ind << " -> " << sq[0] << std::endl;

      liner.AddPoint(ind, sq);
      cuber.AddPoint(ind, sq);
      knoter.AddPoint(ind, sq);
   }

   // Now build a table of interpolated squares
   double lineval[1], cubeval[1], knotval[1];
   // Init to -1; if seen, intetrpolator not active
   lineval[0] = cubeval[0] = knotval[0] = -1.0;

   std::cout << "\nInterpolated Values:\n\n"
             << "Value    Square   Linear   Cubic    NotAKnot\n"
             << "--------------------------------------------\n";

   for (double n = 0.0; n <= 8.0; n += 0.5)
   {
      liner.Interpolate(n, lineval);
      cuber.Interpolate(n, cubeval);
      knoter.Interpolate(n, knotval);

      std::cout << std::left << std::setw(8) << n << " "
                << std::left << std::setw(8) << n*n << " "
                << std::left << std::setw(8) << lineval[0] << " "
                << std::left << std::setw(8) << cubeval[0] << " "
                << std::left << std::setw(8) << knotval[0] << "\n";
   }

   std::cout << std::endl;
   return 0;
}
