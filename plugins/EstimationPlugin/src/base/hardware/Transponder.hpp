//$Id: Transponder.cpp 2010-04-02$
//------------------------------------------------------------------------------
//                             Transponder
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
// Author: Tuan Dang Nguyen
// Created: 2010/04/02 by Tuan Dang Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Transponder class
 */
//------------------------------------------------------------------------------

#ifndef Transponder_hpp
#define Transponder_hpp

#include "estimation_defs.hpp"
#include "RFHardware.hpp"

class ESTIMATION_API Transponder: public RFHardware
{
public:
   Transponder(const std::string &ofType, const std::string &name);
   virtual ~Transponder();
   Transponder(const Transponder& trans);
   Transponder& operator=(const Transponder& trans);

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
//   virtual Real         GetRealParameter(const Integer id,
//                                      const Integer index) const;
//   virtual Real         GetRealParameter(const Integer id, const Integer row,
//                                      const Integer col) const;
//   virtual Real         SetRealParameter(const Integer id,
//                                      const Real value,
//                                      const Integer index);
//   virtual Real         SetRealParameter(const Integer id, const Real value,
//                                      const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
//   virtual Real         GetRealParameter(const std::string &label,
//                                      const Integer index) const;
//   virtual Real         SetRealParameter(const std::string &label,
//                                      const Real value,
//                                      const Integer index);
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

//   Real                  GetOutPutFrequency();

   virtual bool         Initialize();

   virtual Real         GetDelay(Integer whichOne=0);
   virtual bool         SetDelay(Real delay, Integer whichOne=0);
   virtual bool         IsFeasible(Integer whichOne=0);
   virtual Integer      GetSignalCount();
   virtual bool         IsTransmitted(Integer whichOne=0);
   virtual Signal*      GetSignal(Integer whichOne=0);
   virtual bool         SetSignal(Signal* s,Integer whichOne=0);

   Real                 GetTurnAroundRatio();

   DEFAULT_TO_NO_REFOBJECTS

protected:
   std::string  inputFrequencyModel;
   Real         inputCenterFrequency;
   Real         inputBandwidth;
   std::string  outputFrequencyModel;
   std::string  turnAroundRatio;

   /// Published parameters for the RF hardware
   enum
   {
      INPUT_FREQUENCY_MODEL = RFHardwareParamCount,
      INPUT_CENTER_FREQUENCY,
      INPUT_BANDWIDTH,
      OUTPUT_FREQUENCY_MODEL,
      TURN_AROUND_RATIO,
      TransponderParamCount,
   };

   static const std::string
      PARAMETER_TEXT[TransponderParamCount - RFHardwareParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[TransponderParamCount - RFHardwareParamCount];
};

#endif /* Transponder_hpp */
