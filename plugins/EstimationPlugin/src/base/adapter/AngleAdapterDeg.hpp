//------------------------------------------------------------------------------
//                           AngleAdapterDeg
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
// Author: 
// Created: Aug 31, 2018
/**
* A measurement adapter for angles in degrees
*/
//------------------------------------------------------------------------------

#ifndef AngleAdapterDeg_hpp
#define AngleAdapterDeg_hpp

#include "TrackingDataAdapter.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"


/**
* A measurement adapter for angles in degrees
*/
class ESTIMATION_API AngleAdapterDeg : public TrackingDataAdapter
{
public:
   AngleAdapterDeg(const std::string& name);
   virtual ~AngleAdapterDeg();
   AngleAdapterDeg (const AngleAdapterDeg& rak);
   AngleAdapterDeg& operator=(const AngleAdapterDeg& rak);

   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetMeasurement(MeasureModel* meas);

   virtual bool         Initialize();

   // Preserve interfaces in the older measurement model code
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                              ObservationData* forObservation = NULL,
                              std::vector<RampTableData>* rampTB = NULL,
                              bool forSimulation = false);

   //virtual const MeasurementData&
   //                     CalculateMeasurementAtOffset(bool withEvents = false,
   //                           Real dt = 0.0, ObservationData* forObservation = NULL,
   //                           std::vector<RampTableData>* rampTB = NULL,
   //                           bool forSimulation = false);

   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                              Integer id);
   virtual bool         WriteMeasurements();
   virtual bool         WriteMeasurement(Integer id);

   // Covariance handling code
   virtual Integer      HasParameterCovariances(Integer parameterId);

   virtual Integer      GetEventCount();
   virtual void         SetCorrection(const std::string& correctionName,
                              const std::string& correctionType);

   Real                 GetIonoCorrection();
   Real                 GetTropoCorrection();

   void                 TopocentricSEZToAzEl(const Rvector3& topovec, Real &azimuth, Real &elevation);
   void                 TopocentricSEZToXEYN(const Rvector3& topovec, Real &xEast, Real &yNorth);
   void                 TopocentricSEZToXSYE(const Rvector3& topovec, Real &xSouth, Real &yEast);
   void                 GetENZUnitVectors(Rvector& E, Rvector& N, Rvector& Z);
   void                 MJ2000ToRaDec(const Rvector3& mj2000Vector, Real &rightAscension, Real &declination);
   void                 BodyFixedToLongDec(const Rvector3& bfVector, Real &longitude, Real &declination);
   void                 BodyFixedToRaDec(const Rvector3& bfVector, Real longStn, Real &rightAscension, Real &declination);
   Rvector3             TopocentricToMJ2000T3(const Rvector3 &topoVector);
   Rvector3             BodyFixedToMJ2000T3(const Rvector3 &bfVector);

   void                 PartialXEPartialEl(const Rvector3& topovec, Real &partial);
   void                 PartialYNPartialEl(const Rvector3& topovec, Real &partial);
   void                 PartialXSPartialEl(const Rvector3& topovec, Real &partial);
   void                 PartialYEPartialEl(const Rvector3& topovec, Real &partial);

   DEFAULT_TO_NO_CLONES
protected:
   /// Converter to convert between frames
   CoordinateConverter         converter;
   /// Coordinate system in which observation is expressed
   CoordinateSystem           *ocs;
   /// J2K coordinate system
   CoordinateSystem           *j2k;
   /// Range vector in topocentric South-East-Z coordinates
   Rvector3                    topoRange;
   /// Range vector in body centered body-fixed coordinates
   Rvector3                    bfRange;
   /// Range vector in body centered MJ2000 coordinates
   Rvector3                    mj2000Range;
   /// Apply annual aberration correction?
   bool                        useAnnual;
   /// Apply diurnal aberration correction?
   bool                        useDiurnal;

   virtual Real         CalcMeasValue() = 0;
   virtual Rvector6     CalcDerivValue() = 0;
   void                 GetAberrationVel(bool useAnnual, bool useDiurnal,
                           const SignalData& theData, Rvector& aberrationVel);
   void                 ApplyAberrationCorrection(const Rvector3& lssb,
                           const Rvector3& gsvel, Rvector3& corrected_lssb);
   Rmatrix              Calc_dR_t2_dR_t3(GmatBase *forObj, const std::string &paramName,
                           SignalData *current);
   void                 CalculateBiasDerivatives(GmatBase* obj, Integer id,
                           SignalData *current, Integer derivSize);
   Real                 GetCrDerivative(GmatBase *forObj, SignalData *current);
   Real                 GetCdDerivative(GmatBase *forObj, SignalData *current);
   Real                 GetADSFDerivative(GmatBase *forObj, SignalData *current);
   Real                 GetTSFDerivative(GmatBase *forObj, const std::string &paramName,
                           SignalData *current);
   void                 GetCDerivativeVector(GmatBase *forObj, Rvector &deriv,
                           const std::string &solveForType, SignalData *current);
   SignalData *         GetLastSignalData();

   /// Parameter IDs for the AngleAdapterDeg
   enum
   {
      AngleAdapterDegParamCount = AdapterParamCount,
   };
};

#endif /* AngleAdapterDeg_hpp */
