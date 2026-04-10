//$Id$
//------------------------------------------------------------------------------
//                              Plate
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
// number NNG04CI63P
//
// Author: Tuan D. Nguyen, GFSC-NASA, Code 583
// Created: 2019/05/28
//
/**
 * Defines Plate class. 
 */
//------------------------------------------------------------------------------


#ifndef Plate_hpp
#define Plate_hpp

#include "GmatBase.hpp"
#include "GmatBaseException.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "NPlateHistoryFileReader.hpp"


class GMAT_API Plate : public GmatBase
{
public:
   Plate(UnsignedInt typeId, const std::string &typeStr,
               const std::string &instName);
   virtual ~Plate();
   Plate(const Plate& pl);
   Plate&         operator=(const Plate& pl);
   
   virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase*);

   virtual bool         Initialize();
   
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Gmat::ParameterType 
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, const char *value);
   virtual bool         SetStringParameter(const Integer id, const std::string &value);
   
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label, const char *value);
   virtual bool         SetStringParameter(const std::string &label,   const std::string &value);

   
   // Get/Set for StringArray
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;

   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;


   
   virtual const Rvector& GetRvectorParameter(const Integer id) const;
   virtual const Rvector& GetRvectorParameter(const std::string &label) const;
   virtual const Rvector& SetRvectorParameter(const Integer id,
                                              const Rvector &value);
   virtual const Rvector& SetRvectorParameter(const std::string &label,
                                              const Rvector &value);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const Integer id, const Real value);
   virtual Real         SetRealParameter(const std::string &label, const Real value);
   
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);

   virtual Rvector3    GetLocation() const;

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         HasLocalClones();

   virtual void         UpdateClonedObject(GmatBase *obj);
   virtual void         UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId);

   virtual Integer      HasParameterCovariances(Integer parameterId);
   virtual bool         IsEstimationParameterValid(const Integer item);
   virtual Integer      GetEstimationParameterSize(const Integer item);


   Rvector3             GetReflectance(Rvector3 &sHatB, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3             GetReflectanceI(Rvector3 &sHatI, GmatTime &epochGT, Rmatrix33 &MT);
   std::vector<Rvector3>
                        GetReflectanceDerivative(const Rvector3 &sunSC, 
                                                 Rmatrix33 &MT, 
                                                 std::vector<Rmatrix33> &dMT,
                                                 GmatTime &epochGT);
   std::vector<Rvector3>
                        GetReflectanceDerivativeI(const Rvector3 &sunSC,
                                                  Rmatrix33 &MT,
                                                  std::vector<Rmatrix33> &dMT,
                                                  GmatTime &epochGT);

   std::vector<Rvector3>
                        GetReflectanceDerivativeB(const Rvector3 &sunSC,
                                                 Rmatrix33 &MT,
                                                 std::vector<Rmatrix33> &dMT,
                                                 GmatTime &epochGT);

   void                 StoreSpacecraftInertialCoordinateSystem(CoordinateSystem* inertialCS);

   Rvector3             GetFaceNormal(const GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3             GetFaceNormalI(const GmatTime &epochGT, Rmatrix33 &MT);

   bool                 SetErrorSelection(bool select);

   Integer              SetRunningCommandFlag(Integer runningCommand);


protected:
   /// Flag to indicate the Plate is running simulation, propagation, or estimation command 
   // 0: not running; 1: running simmulation command;
   // 2: running propagate command; 3: running estimation command
   Integer runningCommandFlag;

   /// Plate type. It has three posible value "FixedInBody", "SunFacing", and "File"
   std::string plateType;

   /// History file of Plate's normal vector. It is only used for plateType with "File" value 
   std::string historyFileName;
   NPlateHistoryFileReader* faceNormalFile;

   /// Plate's normal unit vector
   Rvector3 plateNormal;

   /// Plate's area
   Real plateArea;
   
   /// Plate's area coefficient
   Real areaCoeff;
   /// Plate's area coefficient sigma
   Real areaCoeffSigma;

   /// Lit fraction
   Real litFrac;

   /// Specular fraction
   Real specularFrac;
   /// Specular fraction sigma
   Real specularFracSigma;
   /// Diffuse fraction
   Real diffuseFrac;
   /// Diffuse fraction sigma
   Real diffuseFracSigma;

   /// true: for display error message, false: for display warning message
   bool                    errorSelection;

   StringArray             allowedSolveFors;

   /// Enumerated parameter IDs   
   enum
   {
      TYPE_ID = GmatBaseParamCount,
      PLATE_NORMAL_HISTORY_FILE_ID,
      PLATE_NORMAL_ID,
      AREA_ID,
      AREA_COEFFICIENT_ID,
      AREA_COEFFICIENT_SIGMA_ID,
      LIT_FRACTION_ID,
      SPECULAR_FRACTION_ID,
      SPECULAR_FRACTION_SIGMA_ID,
      DIFFUSE_FRACTION_ID,
      DIFFUSE_FRACTION_SIGMA_ID,
      SOLVEFORS_ID,
      //PLATE_POSITION,
      PLATEX,
      PLATEY,
      PLATEZ,
      PlateParamCount
   };
   /// Array of supported parameters
   static const std::string PARAMETER_TEXT[PlateParamCount -
                                           GmatBaseParamCount];
   /// Array of parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[PlateParamCount -
                                                   GmatBaseParamCount];
   
private:
   // This provide information about spacecraft's coordinate system which is used to calculate face normal from history file
   CoordinateSystem *      scInertialCS;

   /// Refectance of the plate
   Rvector3                reflectance;

   Rvector3                GetReflectanceDerivativeWRTAreaCoeff(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3                GetReflectanceDerivativeWRTPlateArea(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3                GetReflectanceDerivativeWRTLitFrac(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3                GetReflectanceDerivativeWRTSpecularFrac(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3                GetReflectanceDerivativeWRTDiffuseFrac(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);

   std::vector<Rvector3>   
                           GetReflectanceDerivativeWRTSpacecraftState(const Rvector3 &sunSC, 
                                  Rmatrix33 MT, std::vector<Rmatrix33> MTDeriv, GmatTime &epochGT);
   Rmatrix                 Calculate_sHatI_Deriv(const Rvector3& sunSC);

   Rvector3                GetReflectanceDerivativeWRTAreaCoeffI(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3                GetReflectanceDerivativeWRTPlateAreaI(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3                GetReflectanceDerivativeWRTLitFracI(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3                GetReflectanceDerivativeWRTSpecularFracI(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);
   Rvector3                GetReflectanceDerivativeWRTDiffuseFracI(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT);

   std::vector<Rvector3>
                           GetReflectanceDerivativeWRTSpacecraftStateI(const Rvector3 &sunSC,
                                  Rmatrix33 MT, std::vector<Rmatrix33> MTDeriv, GmatTime &epochGT);

   /// Location of center of the plate on the spacecraft, in meters.
   Real                    location[3];

};

#endif // Plate_hpp
