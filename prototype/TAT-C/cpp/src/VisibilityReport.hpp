//------------------------------------------------------------------------------
//                           VisibilityReport
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
// Created: 2016.04.29
//
/**
 * Definition of the visibility report base class.  This class reports and
 * stores visibilty data.
 */
//------------------------------------------------------------------------------
#ifndef VisibilityReport_hpp
#define VisibilityReport_hpp

#include "gmatdefs.hpp"
#include "AbsoluteDate.hpp"


class VisibilityReport
{
public:
   
   /// class construction/destruction
   VisibilityReport();
   VisibilityReport( const VisibilityReport &copy);
   VisibilityReport& operator=(const VisibilityReport &copy);
   
   virtual ~VisibilityReport();
   
   /// Set the start date
   virtual void  SetStartDate(const AbsoluteDate &toDate);
   /// Set the end date
   virtual void  SetEndDate(const AbsoluteDate &toDate);
   
   /// Get the start date
   virtual const AbsoluteDate& GetStartDate();
   /// Get the end date
   virtual const AbsoluteDate& GetEndDate();
   
protected:
   
   /// Start date of the interval event
   AbsoluteDate startDate;
   /// End date of the interval event
   AbsoluteDate endDate;
   
};
#endif // VisibilityReport_hpp
