//$Id$
//------------------------------------------------------------------------------
//                                  SpacePoint
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
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/08
//
/**
 * Definition of the SpacePoint class.  This is the base class for SpaceObject
 * (base class for Spacecraft and Formation), CelestialBody (base class for Star,
 * Planet, and Moon), and CalculatedPoint (base class for LibrationPoint and
 * Barycenter)  - any object that can be used as an origin, primary, or
 * secondary when defining a CoordinateSystem.
 *
 * @note Methods returning the point's state information at time atTime return
 * the state in a frame with axes aligned with the MJ2000 Earth Equatorial 
 * Coordinate System but based at the origin j2000Body.
 *
 */
//------------------------------------------------------------------------------

#ifndef SpacePoint_hpp
#define SpacePoint_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"

#include "GmatTime.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton


// forward reference for SolarSystem
class SolarSystem;

class GMAT_API SpacePoint : public GmatBase
{
public:

   // default constructor
   SpacePoint(UnsignedInt ofType, const std::string &itsType,
              const std::string &itsName = "");
   // copy constructor
   SpacePoint(const SpacePoint &sp);
   // operator = 
   const SpacePoint& operator=(const SpacePoint &sp);
   // destructor
   virtual ~SpacePoint();
   
   virtual void       SetSolarSystem(SolarSystem *ss);
   
   virtual Real       GetEpoch();
   virtual GmatTime   GetEpochGT();
   virtual Real       SetEpoch(const Real ep);

   virtual GmatTime   SetEpochGT(const GmatTime ep);

   virtual Rvector6   GetLastState();

   // methods for accessing the bodyName or body pointer
   virtual bool       RequiresJ2000Body();
   const std::string  GetJ2000BodyName() const;
   SpacePoint*        GetJ2000Body() const;

   // WARNING: The J200Body must be set identically for all objects in a GMAT run;
   // not doing so will give incorrect results.
   // In addition, the setting of a body other than Earth as the J2000Body has
   // not been tested.
   bool               SetJ2000BodyName(const std::string &toName);
   void               SetJ2000Body(SpacePoint* toBody);

   virtual bool       IsParameterCloaked(const Integer id) const;
   virtual bool       IsParameterEqualToDefault(const Integer id) const;
   virtual bool       SaveAllAsDefault();
   virtual bool       SaveParameterAsDefault(const Integer id);
   
   // methods for accessing attitude
   virtual bool       HasAttitude() const;
   virtual const Rmatrix33& GetAttitude(Real a1mjdTime);
   
   // methods for accessing colors
   UnsignedInt        GetDefaultOrbitColor();
   UnsignedInt        GetDefaultTargetColor();
   UnsignedInt        GetCurrentOrbitColor();
   UnsignedInt        GetCurrentTargetColor();
   std::string        GetOrbitColorString();
   std::string        GetTargetColorString();
   void               SetCurrentOrbitColor(UnsignedInt color);
   void               SetCurrentTargetColor(UnsignedInt color);
   void               SetDefaultColors(UnsignedInt orbColor, UnsignedInt targColor);
   void               SetSpacecraftDefaultColors();
   // static method to clear instance count
   static void ClearInstanceCount();
   
   //---------------------------------------------------------------------------
   //  const Rvector6 GetMJ2000State(const A1Mjd &atTime)
   //---------------------------------------------------------------------------
   /**
    * Method returning the MJ2000 state of the SpacePoint at the time atTime.
    *
    * @param atTime Time for which the state is requested.
    *
    * @return state of the SpacePoint at time atTime.
    *
    * @note This method is pure virtual and must be implemented by the 
    *       'leaf' (non-abstract derived) classes.
    */
   //---------------------------------------------------------------------------
   virtual const Rvector6 GetMJ2000State(const A1Mjd &atTime)    = 0;
   virtual const Rvector6 GetMJ2000State(const Real atTime){ return GetMJ2000State(A1Mjd(atTime));};
   virtual const Rvector6 GetMJ2000State(const GmatTime &atTime) = 0;

   //---------------------------------------------------------------------------
   //  const Rvector3 GetMJ2000Position(const A1Mjd &atTime)
   //---------------------------------------------------------------------------
   /**
    * Method returning the MJ2000 position of the SpacePoint at the time atTime.
    *
    * @param atTime Time for which the position is requested.
    *
    * @return position of the SpacePoint at time atTime.
    *
    * @note This method is pure virtual and must be implemented by the 
    *       'leaf' (non-abstract derived) classes.
    */
   //---------------------------------------------------------------------------
   virtual const Rvector3 GetMJ2000Position(const A1Mjd &atTime) = 0;
   virtual const Rvector3 GetMJ2000Position(const Real atTime){ return GetMJ2000Position(A1Mjd(atTime));};
   virtual const Rvector3 GetMJ2000Position(const GmatTime &atTime) = 0;

   //---------------------------------------------------------------------------
   //  const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime)
   //---------------------------------------------------------------------------
   /**
    * Method returning the MJ2000 velocity of the SpacePoint at the time atTime.
    *
    * @param atTime Time for which the velocity is requested.
    *
    * @return velocity of the SpacePoint at time atTime.
    *
    * @note This method is pure virtual and must be implemented by the 
    *       'leaf' (non-abstract derived) classes.
    */
   //---------------------------------------------------------------------------
   virtual const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime) = 0;
   virtual const Rvector3 GetMJ2000Velocity(const Real atTime){ return GetMJ2000Velocity(A1Mjd(atTime)); };
   virtual const Rvector3 GetMJ2000Velocity(const GmatTime &atTime) = 0;
   
   virtual const Rvector3 GetMJ2000Acceleration(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Acceleration(const Real atTime){ return GetMJ2000Acceleration(A1Mjd(atTime)); };
   virtual const Rvector3 GetMJ2000Acceleration(const GmatTime &atTime);

   virtual const Rvector6 GetMJ2000PrecState(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000PrecPosition(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000PrecVelocity(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000PrecAcceleration(const GmatTime &atTime);

   virtual void           RemoveSpiceKernelName(const std::string &kernelType,
                                                const std::string &fileName);

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   //virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase 
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;

   virtual bool            IsParameterReadOnly(const Integer id) const;
   virtual bool            IsParameterReadOnly(const std::string &label) const;
   virtual bool            IsSquareBracketAllowedInSetting(const Integer id) const;
   virtual bool            IsParameterCommandModeSettable(const Integer id) const;
   
   virtual Integer         GetIntegerParameter(const Integer id) const;
   virtual Integer         GetIntegerParameter(const std::string &label) const;
   virtual Integer         SetIntegerParameter(const Integer id,
                                               const Integer value);

   virtual std::string     GetStringParameter(const Integer id) const;
   virtual bool            SetStringParameter(const Integer id, 
                                              const std::string &value);
   virtual std::string     GetStringParameter(const std::string &label) const;
   virtual bool            SetStringParameter(const std::string &label, 
                                              const std::string &value);
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name);
   virtual bool            SetRefObject(GmatBase *obj, const UnsignedInt type,
                                        const std::string &name = "");
   
   // DJC Added, 12/16/04
   virtual std::string     GetStringParameter(const Integer id, 
                                              const Integer index) const;
   virtual bool            SetStringParameter(const Integer id, 
                                              const std::string &value, 
                                              const Integer index);
   virtual std::string     GetStringParameter(const std::string &label, 
                                              const Integer index) const;
   virtual bool            SetStringParameter(const std::string &label, 
                                              const std::string &value, 
                                              const Integer index);
   const StringArray&      GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                           GetStringArrayParameter(const std::string &label) const;
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name, 
                                        const Integer index);
   virtual bool            SetRefObject(GmatBase *obj, const UnsignedInt type,
                                        const std::string &name, 
                                        const Integer index);
   
   virtual bool			   HasEphemerisFile();

protected:

   enum
   {
      J2000_BODY_NAME = GmatBaseParamCount,
      NAIF_ID,
      NAIF_ID_REFERENCE_FRAME,
      SPICE_FRAME_ID,
      ORBIT_SPICE_KERNEL_NAME,
      ATTITUDE_SPICE_KERNEL_NAME,
      SC_CLOCK_SPICE_KERNEL_NAME,
      FRAME_SPICE_KERNEL_NAME,
      EPOCH_PARAM,
      ORBIT_COLOR,
      TARGET_COLOR,
      SpacePointParamCount
   };
   
   static const std::string PARAMETER_TEXT[SpacePointParamCount - GmatBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[SpacePointParamCount - GmatBaseParamCount];
   
   static const Integer UNDEFINED_NAIF_ID;
   static const Integer UNDEFINED_NAIF_ID_REF_FRAME;
   
   /// the solar system to which this body belongs
   SolarSystem     *theSolarSystem;
   /// Inertial coordinate system to be used in GetAttitude()
   CoordinateSystem *inertialCS;
   /// BodyFixed coordinate system to be used in GetAttitude()
   CoordinateSystem *bodyFixedCS;
   ///A coordinate system based on Spice
   CoordinateSystem *spiceCS;
   /// Origin for the return coordinate system (aligned with the MJ2000 Earth
   /// Equatorial coordinate system)
   SpacePoint      *j2000Body;  
   /// Name for the J2000 body
   std::string     j2000BodyName;
   /// NAIF Id (for SPICE)
   Integer         naifId;
   /// NAIF Id for the body/spacecraft reference frame
   Integer         naifIdRefFrame;
   /// The name of the frame neede for SPICE calls
   std::string     spiceFrameID;
   /// NAIF Id for the observer
   Integer         naifIdObserver;

   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   // saved default values
   std::string     default_j2000BodyName;
   /// default value for NAIF ID
   Integer         default_naifId;
   /// default value for NAIF ID for the body/spacecraft reference frame
   Integer         default_naifIdRefFrame;
   /// default for the spice frame name
   std::string     default_spiceFrameID;
   /// Defaults for the kernel names
   StringArray     default_orbitSpiceKernelNames;
   StringArray     default_attitudeSpiceKernelNames;
   StringArray     default_frameSpiceKernelNames;
   StringArray     default_scClockSpiceKernelNames;
   
   /// flag indicating whether or not the SPICE code is setup
   bool            spiceSetupDone;
   
   /// Orbit SPICE kernel name(s)
   StringArray     orbitSpiceKernelNames;
   /// Attitude SPICE kernel name(s)
   StringArray     attitudeSpiceKernelNames;
   /// SC Clock SPICE kernel name(s)
   StringArray     scClockSpiceKernelNames;
   /// Frame SPICE kernel name(s)
   StringArray     frameSpiceKernelNames;
   
   /// full path to SPK kernels for this SpacePoint; derived classes must
   /// set  this, if the associated SPK kernels are not located in the default
   /// folder (SPK_PATH, as defined in the FileManager).
   std:: string    theSpkPath;

   /// Current rotation matrix (from inertial to body)
   bool      hasAttitude;
   Rmatrix33 cosineMat;
   
   /// orbit and target color (LOJ added 2013.10.22)
   static Integer spacecraftInstanceCount;
   bool useOrbitColorName;
   bool useTargetColorName;
   UnsignedInt defaultOrbitColor;
   UnsignedInt defaultTargetColor;
   UnsignedInt orbitColor;
   UnsignedInt targetColor;
   std::string orbitColorStr;
   std::string targetColorStr;
   
   /// Automatic default orbit and target colors
   static const int MAX_SP_COLOR = 20;
   static const UnsignedInt DEFAULT_ORBIT_COLOR[MAX_SP_COLOR];
   static const UnsignedInt DEFAULT_TARGET_COLOR[MAX_SP_COLOR];
   
   std::string ParseKernelName(const std::string &kernel);
   void        ValidateKernel(const std::string &kName,
                              const std::string label  = "OrbitSpiceKernelName",
                              const std::string ofType = "spk");

};
#endif // SpacePoint_hpp
