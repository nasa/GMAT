//------------------------------------------------------------------------------
//                           KeyValueStatistics
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
// Created: 2016.05.02
//
/**
 * Definition of the KeyValueStatistics class.
 */
//------------------------------------------------------------------------------
#ifndef KeyValueStatistics_hpp
#define KeyValueStatistics_hpp

#include "gmatdefs.hpp"

class KeyValueStatistics
{
public:
   
   /// class construction/destruction
   KeyValueStatistics(Real minVal, Real maxVal, Real avgVal);
   KeyValueStatistics( const KeyValueStatistics &copy);
   KeyValueStatistics& operator=(const KeyValueStatistics &copy);
   
   virtual ~KeyValueStatistics();
   
   // Get methods
   virtual Real   GetMinValue();
   virtual Real   GetMaxValue();
   virtual Real   GetAvgValue();
   
protected:
   
   /// Minimum value
   Real minValue;
   /// Maximum value
   Real maxValue;
   /// Average value
   Real avgValue;
   
};
#endif // KeyValueStatistics_hpp
