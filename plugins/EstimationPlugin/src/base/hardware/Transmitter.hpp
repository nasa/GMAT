//$Id: Transmitter.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             Transmitter
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
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the Transmitter class
 */
//------------------------------------------------------------------------------


#ifndef Transmitter_hpp
#define Transmitter_hpp

#include "estimation_defs.hpp"
#include "RFHardware.hpp"

/**
 * Transmitters used in the Estimation processes
 */
class ESTIMATION_API Transmitter : public RFHardware
{
public:
   Transmitter(const std::string &ofType, const std::string &name);
   virtual ~Transmitter();
   Transmitter(const Transmitter& trans);
   Transmitter& operator=(const Transmitter& trans);

   virtual GmatBase* Clone() const;
   virtual void      Copy(const GmatBase* trans);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   // These are the parameter accessors.  The commented out versions may be
   // needed if derived classes implement them in order to prevent compiler
   // confusion.
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
//   virtual Real         GetRealParameter(const Integer id, const Integer row,
//                                      const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
//   virtual Real         SetRealParameter(const Integer id, const Real value,
//                                      const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
//   virtual Real         GetRealParameter(const std::string &label,
//                                      const Integer row,
//                                      const Integer col) const;
//   virtual Real         SetRealParameter(const std::string &label,
//                                      const Real value, const Integer row,
//                                      const Integer col);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);

   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const Rvector& GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&
                         GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                         SetRvectorParameter(const Integer id,
                         const Rvector& value);
   virtual const StringArray&
      GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
      GetStringArrayParameter(const std::string &label) const;

   virtual bool         Initialize();

   virtual Real         GetDelay(Integer whichOne=0);
   virtual bool         SetDelay(Real delay, Integer whichOne=0);
   virtual Integer      GetSignalCount();
   virtual bool         IsTransmitted(Integer whichOne=0);
   virtual Signal*      GetSignal(Integer whichOne=0);
   virtual bool         SetSignal(Signal* s,Integer whichOne=0);


   Real                 GetOutPutFrequency();

   const RealArray&     GetFrequencyBiasDerivative() const;
   const RealArray&     GetFrequencyBiasDerivative(const GmatTime atTime) const;

   virtual Integer      HasParameterCovariances(Integer parameterId);
   virtual Rmatrix*     GetParameterCovariances(Integer parameterId);

   virtual Integer      GetEstimationParameterSize(const Integer item);
   virtual bool         IsEstimationParameterValid(const Integer id);
   Real                 GetFrequency();
   Real                 GetFrequency(const GmatTime atTime);
   Real                 GetFrequencyOscillatorBias();
   Real                 GetFrequencyOscillatorBias(const GmatTime atTime);
   virtual void         SetEpoch(GmatTime epoch);

   DEFAULT_TO_NO_REFOBJECTS

protected:
   std::string   frequencyModel;
   Real          frequency;
   std::string   frequencyBand;

   bool modelFreqBias;
   /// Transmitter frequency polynomial coefficients are Taylor's series coeffcicients when Transmitter frequency presented in Taylor's series
   Rvector       freqPolynomialCoeffs;
   /// Transmitter frequency polynomial coefficient noise sigma are noise sigmas of the frequency polynomial coefficients  
   Rvector       freqPolynomialCoeffSigmas;

   /// Transmitter initial time. It is presented in a string
   std::string   initialEpochStr;
   /// Epoch type of Transmitter initial time
   std::string   initialEpochTypeStr;

   /// Transmitter initial time presented in GmatTime
   GmatTime      initialEpoch;
   /// Current time which is used to specify transmit frequency
   GmatTime      currentEpoch;
   /// List of Transmitter's solve-for variables
   StringArray   solveforNames;

   std::vector<std::string> supportedFrequencyBands;
   /// Published parameters for the RF hardware

   enum
   {
      FREQUENCY_MODEL = RFHardwareParamCount,
      FREQUENCY,
      FREQUENCY_BAND,
      FREQUENCY_POLYNOMIAL_COEFFICIENTS,
      FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA,
      TRANS_INITIAL_EPOCH,
      TRANS_INITIAL_EPOCH_TYPE,
      TRANS_SOLVEFORS,
      TransmitterParamCount,
   };

   static const std::string
      PARAMETER_TEXT[TransmitterParamCount - RFHardwareParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[TransmitterParamCount - RFHardwareParamCount];
};

#endif /* Transmitter_hpp */
