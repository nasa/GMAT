//$Id$
//------------------------------------------------------------------------------
//                                  ObjectReferencedAxes
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/03/01
//
/**
 * Definition of the ObjectReferencedAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef ObjectReferencedAxes_hpp
#define ObjectReferencedAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"

class GMAT_API ObjectReferencedAxes : public DynamicAxes
{
public:

   // default constructor
   ObjectReferencedAxes(const std::string &itsName = "");
   // another constructor - for derived classes to call
   ObjectReferencedAxes(const std::string &itsType,
                        const std::string &itsName);
   // copy constructor
   ObjectReferencedAxes(const ObjectReferencedAxes &orAxes);
   // operator = for assignment
   const ObjectReferencedAxes& operator=(const ObjectReferencedAxes &orAxes);
   // destructor
   virtual ~ObjectReferencedAxes();

   // methods to set parameters for the AxisSystems
   // (inherited from CoordinateBase)
   virtual void SetPrimaryObject(SpacePoint *prim);
   virtual void SetSecondaryObject(SpacePoint *second);
   virtual void SetXAxis(const std::string &toValue);
   virtual void SetYAxis(const std::string &toValue);
   virtual void SetZAxis(const std::string &toValue);   
   virtual SpacePoint* GetPrimaryObject() const;
   virtual SpacePoint* GetSecondaryObject() const;
   virtual std::string GetPrimaryObjectName() const;
   virtual std::string GetSecondaryObjectName() const;
   virtual std::string GetXAxis() const;
   virtual std::string GetYAxis() const;
   virtual std::string GetZAxis() const;
   virtual void        ResetAxes();
   
   virtual GmatCoordinate::ParameterUsage UsesXAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesYAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesZAxis() const;
   
   // method to initialize the data
   virtual bool Initialize();
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;
   
   virtual bool RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName);
   
   // Parameter access methods - overridden from GmatBase
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
   virtual std::string     GetStringParameter(const Integer id) const;
   virtual bool            SetStringParameter(const Integer id, 
                                              const std::string &value);
   virtual std::string     GetStringParameter(const std::string &label) const;
   virtual bool            SetStringParameter(const std::string &label, 
                                              const std::string &value);
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name);
   const StringArray&      GetRefObjectNameArray(const UnsignedInt type);
   virtual bool            SetRefObject(GmatBase *obj, const UnsignedInt type,
                                        const std::string &name = "");
protected:

   // types and data
   enum
   {
      X_AXIS = DynamicAxesParamCount,
      Y_AXIS,
      Z_AXIS,
      PRIMARY_OBJECT_NAME,
      SECONDARY_OBJECT_NAME,
      ObjectReferencedAxesParamCount
   };
   
   static const std::string         PARAMETER_TEXT[ObjectReferencedAxesParamCount -
                                                   DynamicAxesParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[ObjectReferencedAxesParamCount - 
                                                   DynamicAxesParamCount];

   static const Real                MAGNITUDE_TOL;
   static const Real                ORTHONORMAL_TOL;

   std::string primaryName;
   std::string secondaryName;
   SpacePoint  *primary;
   SpacePoint  *secondary;
   
   std::string xAxis;
   std::string yAxis;
   std::string zAxis;

   // methods
   // methods to compute the rotation matrix (and its derivative) to transform
   // from this ObjectReferencedAxes system to MJ2000EqAxes system; inherited
   // from AxisSystem
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

};
#endif // ObjectReferencedAxes_hpp
