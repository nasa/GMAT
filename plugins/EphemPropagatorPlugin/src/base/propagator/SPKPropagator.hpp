//------------------------------------------------------------------------------
//                             SPKPropagator
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
// Created: Mar 26, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the SPKPropagator class
 */
//------------------------------------------------------------------------------


#ifndef SPKPropagator_hpp
#define SPKPropagator_hpp

/**
 * SPKPropagator ...
 */
#include "ephempropagator_defs.hpp"
#include "EphemerisPropagator.hpp"
#include "SpiceOrbitKernelReader.hpp"


class EPHEM_PROPAGATOR_API SPKPropagator : public EphemerisPropagator
{
public:
   SPKPropagator(const std::string &name = "");
   virtual ~SPKPropagator();
   SPKPropagator(const SPKPropagator& spk);
   SPKPropagator& operator=(const SPKPropagator& spk);

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

   virtual bool Initialize();
   // virtual bool Step(Real dt);
   // virtual void SetAsFinalStep(bool fs);
   virtual bool LoadSpans();

   virtual GmatBase* Clone() const;
   virtual bool Step();
   virtual bool RawStep();
   virtual Real GetStepTaken();

protected:
   /// List of the SPICE files that are needed by this propagator
   StringArray spkFileNames;
   /// ID's used to lookup data for the prop objects
   IntegerArray naifIds;
   /// J2000 Epoch from the SPICE library
   SpiceDouble j2ET;
   /// Most recent epoch used from this propagator
   GmatEpoch               lastEpoch;

   // We need code corresponding to this:
   GmatTime                lastEpochGT;
   /// Time from the start of the ephem, in seconds
   Real                    timeFromEphemStart;
   /// Central body string used by SPK (needed because of Luna/Moon names)
   std::string spkCentralBody;
   /// NAIF ID for the central body
   Integer     spkCentralBodyNaifId;
   /// Flag triggering "outside of ephem" warning messages, allowing a single message
   bool        warnOffEphem;

   SpiceOrbitKernelReader *skr;


   /// Parameter IDs
   enum
   {
      SPKFILENAMES = EphemerisPropagatorParamCount,
      SPKPropagatorParamCount,
   };

   /// SPKPropagator parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[SPKPropagatorParamCount -
                                                   EphemerisPropagatorParamCount];
   /// SPKPropagator parameter labels
   static const std::string PARAMETER_TEXT[SPKPropagatorParamCount -
                                           EphemerisPropagatorParamCount];

   virtual void         UpdateState();

   virtual void         SetEphemSpan(Integer whichOne = 0);
};

#endif /* SPKPropagator_hpp */
