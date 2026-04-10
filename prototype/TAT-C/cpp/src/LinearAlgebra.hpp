//------------------------------------------------------------------------------
//                           LinearAlgebra
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2017.03.23
//
/**
 * Definition of the LinearAlgebra class.
 * NOTE: This is a static class: No instances of this class may be declared.
 */
//------------------------------------------------------------------------------
#ifndef LinearAlgebra_hpp
#define LinearAlgebra_hpp

#include "gmatdefs.hpp"
#include "Rmatrix.hpp"


class LinearAlgebra
{
public:
   
   /// Set the Gregorian date
   static void  LineSegmentIntersect(const Rmatrix    &XY1,
                           const Rmatrix              &XY2,
                           std::vector<IntegerArray>  &adjacency,
                           Rmatrix                    &matrixX,
                           Rmatrix                    &matrixY,
                           Rmatrix                    &distance1To2,
                           Rmatrix                    &distance2To1,
                           std::vector<IntegerArray>  &parallelAdjacency,
                           std::vector<IntegerArray>  &coincidentAdjacency);
         
protected:
   
private:
   
   //------------------------------------------------------------------------------
   // private constructors, destructor, operator=
   //------------------------------------------------------------------------------

   /// class methods (unimplemented, since this is a static class)
   LinearAlgebra();
   LinearAlgebra( const LinearAlgebra &copy);
   LinearAlgebra& operator=(const LinearAlgebra &copy);
   
   ~LinearAlgebra();
   
};
#endif // LinearAlgebra_hpp
