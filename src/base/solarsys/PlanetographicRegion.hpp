//------------------------------------------------------------------------------
//                             PlanetographicRegion.hpp
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the 
// FDSS III Contract
//
// Author: David E. Washington, Pearl River Technologies, Inc.
// Created: 2022.3.9
//
/// PlanetographicRegion: a defined region over the surface of a celestial body.
/** It is not dependent on the surface features, but merely a region over which
    spacecraft will do something.  It is defined as a collection of positions 
    (Lat, long [,alt]) in the fixed coordinates of the celestial body.
**/
//------------------------------------------------------------------------------
#ifndef planetographicRegion_hpp
#define planetographicRegion_hpp

#include "Parameter.hpp"
#include "GmatBase.hpp"
#include "CelestialBody.hpp"
#include "PlanetParameters.hpp"
//#include "SolarSystem.hpp"
#include "BodyFixedPoint.hpp"

class GMAT_API PlanetographicRegion : public BodyFixedPoint
{
public:


   // Constructors.
   PlanetographicRegion(const std::string& objectName = "");
   PlanetographicRegion(const PlanetographicRegion& regionToCopy);

	// Assignment operator
	PlanetographicRegion& operator=(const PlanetographicRegion& regionToCopy);

   // Destructor
   virtual ~PlanetographicRegion();

   // Information about the region.
   bool isWithin(Real lat, Real lon, Real height, Real & distance);
   static Rvector3 EllipsoidToCartesian(const Rvector3 & latLongHeight, Real flattening, Real equatorialRadius);
   static Rvector3 CartesianToEllipsoid(const Rvector3 & cartesian, Real flattening, Real equatorialRadius);

   // Inherited from GmatBase
   virtual const Rvector & GetRvectorParameter(const std::string & label) const;
   virtual const Rvector & GetRvectorParameter(const Integer id) const;
   virtual bool SetStringParameter(const Integer id, const std::string& value);
   virtual bool SetStringParameter(const std::string& field, const std::string& value);
   virtual std::string GetStringParameter(const std::string& field) const;
   virtual std::string GetStringParameter(const Integer id) const;

   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string& label) const;
   virtual Real GetRealParameter(const Integer id, 
      const Integer index) const;
   virtual GmatBase * GetRefObject(const UnsignedInt type, 
      const std::string & name);

   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const Integer id, const Real value, 
      const Integer index);
   virtual Real SetRealParameter(const std::string& label, const Real value);
   virtual bool SetRefObject(GmatBase * object, const UnsignedInt type, 
      const std::string & name);

   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool IsParameterReadOnly(const Integer id) const;
   virtual bool IsParameterReadOnly(const std::string &label) const;
   virtual bool IsValidID(const std::string & id);

   virtual bool Initialize();
   virtual bool HasLocalClones();
   virtual GmatBase* Clone() const;
   virtual bool RenameRefObject(const UnsignedInt type,
      const std::string &oldName, const std::string &newName);

protected:
   /// Parameter IDs
   enum
   {
      LATITUDE = BodyFixedPointParamCount,
      LONGITUDE,
      FILENAME,
      PlanetographicRegionParamCount
   };

   static const std::string
      PARAMETER_TEXT[PlanetographicRegionParamCount - BodyFixedPointParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[PlanetographicRegionParamCount - BodyFixedPointParamCount];

 //---------------------------------------------------------------------------
   virtual const Rvector6 GetMJ2000State(const A1Mjd &atTime);
   virtual const Rvector6 GetMJ2000State(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000Position(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Position(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Velocity(const GmatTime &atTime);


private:
   void AddExtraPointIfMovingAlongSegmentPoleToPole(Integer currentIndex);
   void CheckLatitudeAndLongitudeList() const;
   void DuplicateDefinitionError() const;
   static bool DoLinesIntersect(Real a[2], Real b[2], Real c[2], Real d[2], Real & distance);
   std::vector<Rvector3> EliminateHiddenVertices(const std::vector<Rvector3> & region);
   static void HandleRealParseError(const Integer &errorCode, std::string &message);
   void InterpolateTheCoordinates();
   static StringArray Parse(const std::string & input, const char delimeter);
   static StringArray Parse(const std::string & input, const std::string & delimeters);
   void ReadLatitudeLongitudeFile();
   static void RotateYX(const Real angle, Rvector3 & theVector);
   static void RotateZX(const Real angle, Rvector3 & theVector);
   static void RotateZY(const Real angle, Rvector3 & theVector);

   // Defining parameters
   RealArray latitudeList;
   RealArray longitudeList;
   std::string filename;

   // Calculated parameters
   std::vector<Rvector3> vertices;
};

#endif

