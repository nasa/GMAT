//------------------------------------------------------------------------------
//                           GPSPointMeasureModel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Tuan Dang Nguyen, GSFC/NASA.
// Created: Oct 3, 2016
/**
 * The GPS Point Measurement Model class for estimation
 */
//------------------------------------------------------------------------------

#ifndef GPSPointMeasureModel_hpp
#define GPSPointMeasureModel_hpp

#include "MeasureModel.hpp"

//#include "estimation_defs.hpp"
//#include "GmatBase.hpp"
//#include "SolarSystem.hpp"
//#include "SignalData.hpp"
//
//// Forward references
//class SignalBase;
//class ProgressReporter;
//
//class ObservationData;
//class RampTableData;
//class MeasurementData;
//class PropSetup;


/**
 * The estimation measurement model
 *
 * This class is the reworked measurement model for GMAT's estimation subsystem.
 * It uses the signal classes to model the path of a measurement.  The output
 * resulting from the modeling is built by an Adapter that uses the raw data to
 * generate measurement information.
 */
class ESTIMATION_API GPSPointMeasureModel : public MeasureModel
{
public:
   GPSPointMeasureModel(const std::string &name);
   virtual ~GPSPointMeasureModel();
   GPSPointMeasureModel(const GPSPointMeasureModel& mm);
   GPSPointMeasureModel& operator=(const GPSPointMeasureModel& mm);

   virtual GmatBase* Clone() const;
//   // Access methods derived classes can override
//   virtual std::string  GetParameterText(const Integer id) const;
//   virtual Integer      GetParameterID(const std::string &str) const;
//   virtual Gmat::ParameterType
//                        GetParameterType(const Integer id) const;
//   virtual std::string  GetParameterTypeString(const Integer id) const;

   DEFAULT_TO_NO_CLONES

   virtual bool         Initialize();

   virtual bool         CalculateMeasurement(bool withEvents = false,
                           bool withMediaCorrection = true,
                           ObservationData* forObservation = NULL,
                           std::vector<RampTableData>* rampTB = NULL,
                           bool forSimulation = false, 
                           Real atTimeOffset = 0.0, 
                           Integer forStrand = -1);

   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                                                        Integer id,
                                                        Integer forStrand = -1);


protected:
   ///// Parameter IDs for the BatchEstimators
   //enum
   //{
   //   GPSPointMeasurementParamCount = MeasurementParamCount,
   //};

   ///// Strings describing the BatchEstimator parameters
   //static const std::string PARAMETER_TEXT[GPSPointMeasurementParamCount -
   //                                        MeasurementParamCount];
   ///// Types of the BatchEstimator parameters
   //static const Gmat::ParameterType PARAMETER_TYPE[GPSPointMeasurementParamCount -
   //                                                MeasurementParamCount];

private:
   bool         InitializePointModel();

//   void         PointMeasurementDerivatives(std::vector<RealArray>& derivative, GmatBase *obj, Integer id, Integer forStrand = -1);

   //const std::vector<RealArray>&
   //             SignalPathMeasurementDerivatives(GmatBase *obj, Integer id, Integer forStrand = -1);

   void         ModelPointSignalDerivative(GmatBase* obj, Integer forId, SignalBase* sb, std::vector<RealArray>& derivative);
   void         GetDerivativeWRTState(GmatBase *forObj, bool wrtR, bool wrtV, GmatTime measTime, std::vector<RealArray>& derivative);
   void         GetDerivativeWRTC(GmatBase *forObj, GmatTime measTime, std::vector<RealArray>& derivative);
   void         GetDerivativeWRTCr(GmatBase *forObj, GmatTime measTime, std::vector<RealArray>& derivative);
   void         GetDerivativeWRTCd(GmatBase *forObj, GmatTime measTime, std::vector<RealArray>& derivative);

   Real         GetParamDerivative(GmatBase *forObj, std::string paramName, GmatBase *associateObj, SignalData *theData);
   void         GetCDerivativeVector(GmatBase *forObj, Rvector &deriv, const std::string &solveForType, SignalData *theData);

};

#endif /* GPSPointMeasureModel_hpp */
