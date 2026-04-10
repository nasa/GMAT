//------------------------------------------------------------------------------
//                           IntervalEventReport
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
 * Definition of the Interval Event Report class.  
 */
//------------------------------------------------------------------------------
#ifndef IntervalEventReport_hpp
#define IntervalEventReport_hpp

#include "gmatdefs.hpp"
#include "AbsoluteDate.hpp"
#include "VisiblePOIReport.hpp"


class IntervalEventReport
{
public:
   
   /// class construction/destruction
   IntervalEventReport();
   IntervalEventReport( const IntervalEventReport &copy);
   IntervalEventReport& operator=(const IntervalEventReport &copy);
   
   virtual ~IntervalEventReport();
   
   /// Set the start date
   virtual void    SetStartDate(const AbsoluteDate &toDate);
   /// Set the end date
   virtual void    SetEndDate(const AbsoluteDate &toDate);
   
   /// Get the start date
   virtual const   AbsoluteDate& GetStartDate();
   /// Get the end date
   virtual const   AbsoluteDate& GetEndDate();

   /// Set the POI index
   virtual void    SetPOIIndex(Integer toIdx);
   /// Get the POI index
   virtual Integer GetPOIIndex();

   /// Add a POI event
   void            AddPOIEvent(const VisiblePOIReport &theReport);
   /// Get an array of POI event reports
   virtual std::vector<VisiblePOIReport>
                   GetPOIEvents();
   /// Set all of the POI events at once
   void            SetAllPOIEvents(std::vector<VisiblePOIReport>);

protected:
   
   /// Index of point of interest
   Integer poiIndex;
   /// Start date of the interval event
   AbsoluteDate startDate;
   /// End date of the interval event
   AbsoluteDate endDate;
   /// Vector of discrete event reports (VisiblePOIReports)
   std::vector<VisiblePOIReport> discretePOIEvents;
   
};
#endif // IntervalEventReport_hpp
