//------------------------------------------------------------------------------
//                             CcsdsEphPropagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Tuan D. Nguyen / GSFC-NASA Code 583
// Created: 2021.07.19
//
/**
 * Implementation for the CcsdsEphPropagator class
 */
//------------------------------------------------------------------------------


#ifndef CcsdsEphPropagator_hpp
#define CcsdsEphPropagator_hpp

/**
 * CcsdsEphPropagator ...
 */
#include "ephempropagator_defs.hpp"
#include "EphemerisPropagator.hpp"
#include "CCSDSEphemerisFile.hpp"
#include "Interpolator.hpp"


class EPHEM_PROPAGATOR_API CcsdsEphPropagator : public EphemerisPropagator
{
public:
   CcsdsEphPropagator(const std::string &name = "");
   virtual ~CcsdsEphPropagator();
   CcsdsEphPropagator(const CcsdsEphPropagator& prop);
   CcsdsEphPropagator& operator=(const CcsdsEphPropagator& prop);

   virtual GmatBase*    Clone() const;

   // Access methods for the scriptable parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual bool         Initialize();
   // virtual bool Step(Real dt);
   // virtual void SetAsFinalStep(bool fs);

   virtual bool         Step();
   virtual bool         RawStep();
   virtual Real         GetStepTaken();

protected:
   /// The ephem file that is propagated
   std::string             ephemName;
   /// The ephem reader
   CCSDSEphemerisFile      ephem;
   /// Flag indicating the file handle status
   bool                    fileDataLoaded;
   /// Ephemeris data from the file
   std::vector<CCSDSEphemerisFile::EphemData>
                           *ephemRecords;
   /// Last used state index (the index of the ephem state <= current epoch)
   Integer                 stateIndex;
   /// A.1 epoch of the data
   Real                    startEpoch;
   /// Time from the start of the ephem, in seconds
   Real                    timeFromEphemStart;
   /// Most recent epoch used from this propagator
   GmatEpoch               lastEpoch;

// We need code corresponding to this:
   GmatTime                lastEpochGT;

   /// CoordinateConverter instance
   CoordinateConverter     cc;
   /// Coordinate system used in the ephemeris file
   CoordinateSystem        *ephemCoord;
   /// MJ2000 Coordinate System about the ephemeris file's central body
   CoordinateSystem        *j2k;


   virtual void            UpdateState();
   virtual void            SetEphemSpan(Integer whichOne = 0);

   void BuildCoordinates(const std::string &coordinates = "J2000");

// We need code corresponding to these:
   //void                    FindRecord(GmatEpoch forEpoch);
   //void                    FindRecord(GmatTime forEpoch);
   //void                    UpdateInterpolator(const GmatEpoch &forEpoch);
   //void                    UpdateInterpolator(const GmatTime &forEpoch);
   //void                    GetState(GmatEpoch forEpoch, Rvector6 &outstate);
   //void                    GetState(GmatTime forEpoch, Rvector6 &outstate);
   
   /// Parameter IDs
   enum
   {
      EPHEMERISFILENAME = EphemerisPropagatorParamCount,
      CcsdsEphPropagatorParamCount,
   };

   /// CcsdsEphPropagator parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[CcsdsEphPropagatorParamCount -
                                                   EphemerisPropagatorParamCount];
   /// CcsdsEphPropagator parameter labels
   static const std::string PARAMETER_TEXT[CcsdsEphPropagatorParamCount -
                                           EphemerisPropagatorParamCount];
};

#endif /* CcsdsEphPropagator_hpp */
