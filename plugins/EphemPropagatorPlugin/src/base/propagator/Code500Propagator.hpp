//------------------------------------------------------------------------------
//                             Code500Propagator
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
// Author: Darrel J. Conway
// Created: Apr 08, 2016 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task 26
//
/**
 * Implementation for the Code500Propagator class
 */
//------------------------------------------------------------------------------


#ifndef Code500Propagator_hpp
#define Code500Propagator_hpp

/**
 * Code500Propagator ...
 */
#include "ephempropagator_defs.hpp"
#include "EphemerisPropagator.hpp"
#include "Code500EphemerisFile.hpp"
#include "Interpolator.hpp"


class EPHEM_PROPAGATOR_API Code500Propagator : public EphemerisPropagator
{
public:
   Code500Propagator(const std::string &name = "");
   virtual ~Code500Propagator();
   Code500Propagator(const Code500Propagator& spk);
   Code500Propagator& operator=(const Code500Propagator& spk);

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
   Code500EphemerisFile    ephem;
   /// Interpolator used to propagate to points off of the ephem nodes
   Interpolator            *interp;
   /// The Satellite ID for the propagated spacecraft (always 101.0 for now)
   Real                    satID;
   /// Flag indicating the file handle status
   bool                    fileDataLoaded;
   /// Ephemeris data from the file
   std::vector<Code500EphemerisFile::EphemData>
                           *ephemRecords;
   /// Index for the current record in use
   Integer                 record;
   /// Last used state index (the index of the ephem state <= current epoch)
   Integer                 stateIndex;
   /// Array of A.1 epochs at the start of each data block
   RealArray               startEpochs;

   /// Array of time steps in each data block
   RealArray               timeSteps;
   /// Time from the start of the ephem, in seconds
   Real                    timeFromEphemStart;
   /// Most recent epoch used from this propagator
   GmatEpoch               lastEpoch;
   GmatTime                lastEpochGT;
   /// Time spanned by each data block
   RealArray               timeSpans;

   /// CoordinateConverter instance
   CoordinateConverter     cc;
   /// Coordinate system used in the ephemeris file
   CoordinateSystem        *ephemCoord;
   /// MJ2000 Coordinate System about the ephemeris file's central body
   CoordinateSystem        *j2k;

   virtual void            UpdateState();
   virtual void            SetEphemSpan(Integer whichOne = 0);

   void                    FindRecord(GmatEpoch forEpoch);
   void                    FindRecord(GmatTime forEpoch);
   void                    UpdateInterpolator(const GmatEpoch &forEpoch);
   void                    UpdateInterpolator(const GmatTime &forEpoch);
   void                    GetState(GmatEpoch forEpoch, Rvector6 &outstate);
   void                    GetState(GmatTime forEpoch, Rvector6 &outstate);

   /// Parameter IDs
   enum
   {
      EPHEMERISFILENAME = EphemerisPropagatorParamCount,
      Code500PropagatorParamCount,
   };

   /// Code500Propagator parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[Code500PropagatorParamCount -
                                                   EphemerisPropagatorParamCount];
   /// Code500Propagator parameter labels
   static const std::string PARAMETER_TEXT[Code500PropagatorParamCount -
                                           EphemerisPropagatorParamCount];

   static const Real DUL_TO_KM;
   static const Real DUL_DUT_TO_KM_SEC;
   static const Real DUT_TO_DAY;
   static const Real DUT_TO_SEC;

};

#endif /* Code500Propagator_hpp */
