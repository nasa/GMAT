// PolyhedronGravityModel.hpp
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
//  Created on: Jul 16, 2012
//      Author: tdnguye2

#ifndef PolyhedronGravityModel_hpp
#define PolyhedronGravityModel_hpp

#include "polyhedrongravitymodel_defs.hpp"
#include "gmatdefs.hpp"
#include "GravityBase.hpp"
//#include "GravityField.hpp"
#include "Rmatrix66.hpp"
#include "PolyhedronBody.hpp"


class POLYHEDRONGRAVITYMODEL_API PolyhedronGravityModel : public GravityBase
{
public:

   PolyhedronGravityModel(const std::string &name);
   virtual ~PolyhedronGravityModel();
   PolyhedronGravityModel(const PolyhedronGravityModel& polgm);
   PolyhedronGravityModel& operator= (const PolyhedronGravityModel& polgm);

   virtual bool Initialize();

   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);

   virtual bool         HasLocalClones();
   virtual bool         IsUserForce();
//   virtual bool         SupportsDerivative(Gmat::StateElementId id);
//   virtual bool         SetStart(Gmat::StateElementId id, Integer index, Integer quantity);

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   bool SetDensity(Real density);								// set density for the body
   Real GetDensity();											   // get density of the body
   Real GetSolidAngle(Rvector3 &r, GmatEpoch time);
   Real GetAltitude(Rvector3 &r, GmatEpoch time);

   bool SetBodyShapeFileName(const std::string &filename);		// set bodyShapeFilename of the body
   std::string GetBodyShapeFileName(); 						    	// get bodyShapeFilename of the body

   virtual bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1, 
         const Integer id = -1);

   // Parameter definition and accessor methods inherited from GmatBase
   virtual Integer      GetParameterCount() const;

   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &value) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual Real        GetRealParameter(const std::string &label) const;
   virtual Real        SetRealParameter(const std::string &label,
                                        const Real value);


   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index,
                         Integer quantity, Integer sizeOfType);

public:
   // Parameter IDs
   enum
   {
      CREATE_FORCE_BODY = GravityBaseParamCount,
	  /// name of the body generating gravity field
	  SHAPE_FILENAME,
	  /// File containing data specifying shape of the body
	  BODY_DENSITY,
	  /// Desity of the body
	  //FORCE_APPLIED_ONOBJECTS,
	  /// list of spacecrafts need to specify gravity acceleration
      PolyhedronGravityModelParamCount
   };

   static const std::string 
   PARAMETER_TEXT[PolyhedronGravityModelParamCount - GravityBaseParamCount];
   static const Gmat::ParameterType 
   PARAMETER_TYPE[PolyhedronGravityModelParamCount - GravityBaseParamCount];

   static const Real 	UniverisalGravityConstant;

private:
   Rmatrix33            CalculateTransformationMatrix_UsingIAUSimplified() const;
   const std::vector<Rmatrix33> CalculateTransformationMatrix() const;
   bool					   Calculation(Rvector6 x, Rvector6& xdot, Rmatrix66& A);		// calculate gravity
   bool                 firstcalculation;

   std::string          createForceBodyName;		// name of the body generating gravity field
   CelestialBody*       createForceBody;			// the body generating gravity field
   Real                 bodyDensity;				// density of the body             // unit: Kg/(m^3)
   std::string 			bodyShapeFilename;			// name of the file containing information about shape of the body
   PolyhedronBody* 		polybody;					// object defining shape of the body
   Rvector6             bodyOrientation;			// orientation of the body
   Rvector6             bodyState;					// state of the body

   Real					   now;                    // current time
   Real                 initialtime;				// initial time
   /// Calculation value used to detect when we are inside of the body
   Real                 sumWf;
   bool                 isPHGMInitialized;       
   bool                 isShapeLoaded;
};

#endif /* PolyhedronGravityModel_hpp */
