//$Id$
//------------------------------------------------------------------------------
//                              GravityField
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: D. Conway
// Created: 2003/03/17
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed quad-t to Real, int
//           to Integer, etc.
//           2011.05.20 W. Shoan - updates based on code from John P. Downing to add Earth tides
//
/**
 * This is the base class for celestial bodies.
 *
 * @note This is an abstract class.
 * @note original prolog information included at end of file prolog.
 */
//
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: D. Conway
// Created: 2003/03/17
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed quad-t to Real, int
//           to Integer, etc.
//
/**
 * This is the GravityField class.
 *
 * @note original prolog information included at end of file prolog.
 */
//
// *** File Name : val_gravityfield.h
// *** Created   : March 17, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C/C++ Source
// Development Environment   : Borland C++ 5.02
// Modification History      : 3/17/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery.  Happy St. Patrick's Day!
// **************************************************************************
//------------------------------------------------------------------------------

#ifndef GravityField_hpp   /* protect against multiple includes */
#define GravityField_hpp

#include "HarmonicField.hpp"
#include "CelestialBody.hpp"
#include "HarmonicGravity.hpp"
#include "ODEModelException.hpp"


class GMAT_API GravityField : public HarmonicField
{
public:
   GravityField(const std::string &name, const std::string &forBodyName,
                Integer maxDeg = DEFAULT_DEGREE,
                Integer maxOrd = DEFAULT_ORDER);
   virtual ~GravityField();
   GravityField (const GravityField & gf);
   GravityField&   operator=(const GravityField & gf);

   virtual bool    Initialize();
   
   virtual bool    GetDerivatives(Real *state, Real dt = 0.0, 
                                  Integer order = 1, 
                                  const Integer id = -1);
   virtual Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc);

   virtual bool    GetBodyAndMu(std::string &itsName, Real &itsMu);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   // override access methods derived from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool        IsParameterReadOnly(const Integer id) const;
   virtual bool        IsParameterValid(const Integer id, const std::string &value);
   virtual bool        IsParameterValid(const std::string &label, const std::string &value);

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual Real        GetRealParameter(const std::string &label) const;
   virtual Real        SetRealParameter(const std::string &label,
                                        const Real value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const std::string &label,
                                          const char *value);
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);
   
   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index, 
                         Integer quantity, Integer sizeOfType);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

   static HarmonicGravity* GetHarmonicGravity 
     (const std::string& filename, const std::string& tideFilename,
      const Real &radius, const Real &mukm, const std::string& bodyname,
      const bool& loadCoefficients);

   // constants defining maximum degree and order
   static const Integer DEFAULT_DEGREE = 360;
   static const Integer DEFAULT_ORDER  = 360;

   enum GravityModelType
      {
      // Earth Models
      GFM_EGM96,
      GFM_JGM2,
      GFM_JGM3,
      // Luna Models
      GFM_LP165P,
      // Mars Models
      GFM_MARS50C,
      // Venus Models
      GFM_MGNP180U,
      // Other or unknown models
      GFM_OTHER,
      GFM_NONE,
      NumGravityModels
      };
   /// names of the gravity models
   static const std::string GRAVITY_MODEL_NAMES[NumGravityModels];
   static GravityModelType GetModelType(const char *filename, const char *forBody);
   static GravityModelType GetModelType(const std::string &filename, const std::string &forBody);

   virtual bool SetTideFilename(const std::string &fn, bool validateOnly = false);

   virtual Rvector3 GetTorquesForSpacecraft(Spacecraft *sc);

protected:

   enum
   {
      MU = HarmonicFieldParamCount,
      A,
      TIDE_FILENAME,
      TIDE_FILE_FULLPATH,
      TIDE_MODEL,
      GravityFieldParamCount
   };

   static std::vector<HarmonicGravity*> cache;

   static const std::string PARAMETER_TEXT[
      GravityFieldParamCount - HarmonicFieldParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
      GravityFieldParamCount - HarmonicFieldParamCount];

   /// gravity parameter of central body
   Real               mu;
   /// radius of central body ( mean equatorial )
   Real               a;
   /// The name of the tide file
   std::string        tideFilename;
   /// The full path file name of the tide file
   std::string        tideFilenameFullPath;
   /// string for tide model
   std::string        TideModel;
   /// default mu
   Real               defaultMu;
   /// default equatorial radius
   Real               defaultA;
//   /// The minimum distance required from body origin for computation
//   Real               minimumDistance;
   ///
   bool               gfInitialized;
   /// Flag used to keep from scrolling the "truncating to order" message
   bool               orderTruncateReported;   // no longer needed - only truncated in Initialize
   /// Flag used to keep from scrolling the "truncating to degree" message
   bool               degreeTruncateReported;  // no longer needed - only truncated in Initialize
   /// Flag indicating the ODE model origin is not the GravityField origin
   bool               offsetBodyOrigin;

   HarmonicGravity    *gravityModel;    // JPD
   

   bool          IsBlank(char* aLine);  // leaving this one in for now
   bool          IsBlank(const std::string &aLine);  // leaving this one in for now

   static const Integer stateSize             = 6;
   
   // stuff moved in here for performance
   Rvector6 frv;
   Rvector6 trv;
   A1Mjd    now;
   GmatTime nowGT;
   
   CoordinateConverter cc;
   CoordinateSystem    *j2k;

   //  JPD added these ...............
   void GetTideData (Real dt, const std::string bodyname, 
      Real pos[3], Real& mukm);
   void Calculate (Real dt, Real state[6],
      Real force[3], Rmatrix33& grad);
   void InverseRotate(Rmatrix33& rot, const Real in[3], Real out[3]);
   
};


#endif // GravityField_hpp
