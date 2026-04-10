//$Id: Oscillator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             Oscillator
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
// Author: Tuan D. Nguyen
// Created: 2021/02/19 by Tuan D. Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Oscillator class
 */
//------------------------------------------------------------------------------


#ifndef Oscillator_hpp
#define Oscillator_hpp

#include "estimation_defs.hpp"
#include "Transmitter.hpp"

/**
 * Oscillators used in the Estimation processes
 */
class ESTIMATION_API Oscillator : public Transmitter
{
public:
   Oscillator(const std::string &ofType, const std::string &name);
   virtual ~Oscillator();
   Oscillator(const Oscillator& osc);
   Oscillator& operator=(const Oscillator& osc);

   virtual GmatBase* Clone() const;
   virtual void      Copy(const GmatBase* osc);

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

   virtual const Rvector& 
                        GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                        SetRvectorParameter(const Integer id,
                                            const Rvector& value);


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
   virtual const StringArray& 
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   virtual Integer      HasParameterCovariances(Integer parameterId);
   virtual Rmatrix*     GetParameterCovariances(Integer parameterId);

   virtual Integer      GetEstimationParameterSize(const Integer item);
   virtual bool         IsEstimationParameterValid(const Integer id);
   
   virtual bool         Initialize();

   virtual void         SetEpoch(GmatTime epoch);
   Real                 GetFrequency();
   Real                 GetFrequency(const GmatTime atTime);
   Real                 GetFrequencyDriftDerivative();
   Real                 GetFrequencyDriftDerivative(const GmatTime atTime);
   const RealArray&     GetFrequencyPolynomialCoefficientsDerivative() const;
   const RealArray&     GetFrequencyPolynomialCoefficientsDerivative(const GmatTime atTime) const;

   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Osicllator frequency drift is the drifted time of frequency due to the delay of the oscillator (unit: second)
   Real          freqDrift;
   /// Osicllator frequency drift noise sigma is noise sigma of the frequency drift (unit: second)
   Real          freqDriftSigma;
   /// Osicllator frequency polynomial coefficients are Taylor's series coeffcicients when oscillator frequency presented in Taylor's series
   Rvector       freqPolynomialCoeffs;
   /// Osicllator frequency polynomial coefficient noise sigma are noise sigmas of the frequency polynomial coefficients  
   Rvector       freqPolynomialCoeffSigmas;
   /// Oscillator initial time. It is presented in a string
   std::string   initialEpochStr;
   /// Epoch type of oscillator initial time
   std::string   initialEpochTypeStr;

   /// Oscillator initial time presented in GmatTime
   GmatTime      initialEpoch;
   /// Current time which is used to specify transmit frequency
   GmatTime      currentEpoch;

   /// List of Oscillator's solve-for variables
   StringArray   solveforNames;

   /// Published parameters for the Oscillator
   enum
   {
      OSC_FREQUENCY_DRIFT = TransmitterParamCount,
      OSC_FREQUENCY_DRIFT_NOISESIGMA,
      OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS,
      OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA,
      OSC_INITIAL_EPOCH,
      OSC_INITIAL_EPOCH_TYPE,
      OSC_SOLVEFORS,
      OscillatorParamCount,
   };

   static const std::string
      PARAMETER_TEXT[OscillatorParamCount - TransmitterParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[OscillatorParamCount - TransmitterParamCount];
};

#endif /* Oscillator_hpp */
