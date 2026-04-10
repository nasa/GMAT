//$Id$
//------------------------------------------------------------------------------
//                                  TimeData
//------------------------------------------------------------------------------
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
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/12
//
/**
 * Declares Time related data class.
 */
//------------------------------------------------------------------------------
#ifndef TimeData_hpp
#define TimeData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SpacePoint.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton


class GMAT_API TimeData : public RefData
{
public:

   TimeData(const std::string &name = "", const std::string &typeName = "",
            UnsignedInt paramOwnerType = Gmat::SPACECRAFT);
   TimeData(const TimeData &td);
   TimeData& operator= (const TimeData& td);
   virtual ~TimeData();
   
   bool IsInitialEpochSet();
   void ClearIsInitialEpochSet();
   Real GetInitialEpoch() const;
   void SetInitialEpoch(const Real &initialEpoch);
   
   Real GetTimeReal(Integer id);
   void SetTimeReal(Integer id, Real value);
   
   std::string GetTimeString(Integer id);
   void        SetTimeString(Integer id, const std::string &value);
   
   Real GetElapsedTimeReal(Integer id);
   
   // The inherited methods from RefData
   virtual std::string        GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);

   virtual bool               SetRefObjectName(const UnsignedInt type,
                                               const std::string &name);
   virtual GmatBase*          GetRefObject(const UnsignedInt type,
                                           const std::string &name = "");
   virtual bool               SetRefObject(GmatBase *obj, const UnsignedInt type,
                                           const std::string &name = "");

   virtual bool               ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
protected:
    
   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);
   virtual bool AddRefObject(const UnsignedInt type,
                             const std::string &name, GmatBase *obj = NULL,
                             bool replaceName = false);
   
   Real mInitialEpoch;
   bool mIsInitialEpochSet;
   Spacecraft *mSpacecraft;
   SpacePoint *mSpacePoint;
   
   bool handleLeapSecond;
   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;
   
   const static Real TIME_REAL_UNDEFINED;
   const static std::string TIME_STRING_UNDEFINED;
   
   enum TimeDataID
   {
      A1, TAI, TT, TDB, UTC,
      YEARS, MONTHS, DAYS, HOURS, MINS, SECS
   };
   
   enum
   {
      SPACECRAFT = 0,
      SPACE_POINT,
      TimeDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[TimeDataObjectCount];
   
};
#endif // TimeData_hpp

