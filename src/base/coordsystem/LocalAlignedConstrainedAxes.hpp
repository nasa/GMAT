//$Id:  $
//------------------------------------------------------------------------------
//                                  LocalAlignedConstrainedAxes
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
// Created: 2013.04.29
//
/**
 * Definition of the LocalAlignedConstrainedAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef LocalAlignedConstrainedAxes_hpp
#define LocalAlignedConstrainedAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"

class GMAT_API LocalAlignedConstrainedAxes : public DynamicAxes
{
public:

   // default constructor
   LocalAlignedConstrainedAxes(const std::string &itsName = "");
   // another constructor - for derived classes to call
   LocalAlignedConstrainedAxes(const std::string &itsType,
                               const std::string &itsName);
   // copy constructor
   LocalAlignedConstrainedAxes(const LocalAlignedConstrainedAxes &copy);
   // operator = for assignment
   const LocalAlignedConstrainedAxes& operator=(const LocalAlignedConstrainedAxes &copy);
   // destructor
   virtual ~LocalAlignedConstrainedAxes();

   // methods to set parameters for the AxisSystems
   // (inherited from CoordinateBase)

   // method to initialize the data
   virtual bool Initialize();

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;

   virtual GmatCoordinate::ParameterUsage
                           UsesReferenceObject() const;
   virtual SpacePoint*     GetReferenceObject() const;
   virtual std::string     GetReferenceObjectName() const;
   virtual void            SetReferenceObject(SpacePoint *reObj);
   virtual bool            UsesSpacecraft(const std::string &withName = "") const;


   // Parameter access methods - overridden from GmatBase
   virtual std::string     GetParameterText(const Integer id) const;
   virtual Integer         GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;

   virtual Real            GetRealParameter(const Integer id) const;
   virtual Real            GetRealParameter(const std::string &label) const;
   virtual Real            SetRealParameter(const Integer id,
                                            const Real value);
   virtual Real            SetRealParameter(const std::string &label,
                                            const Real value);

   virtual std::string     GetStringParameter(const Integer id) const;
   virtual bool            SetStringParameter(const Integer id,
                                              const std::string &value);
   virtual std::string     GetStringParameter(const std::string &label) const;
   virtual bool            SetStringParameter(const std::string &label,
                                              const std::string &value);

   virtual const ObjectTypeArray&
                           GetRefObjectTypeArray();
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name);
   const StringArray&      GetRefObjectNameArray(const UnsignedInt type);
   virtual bool            SetRefObject(GmatBase *obj, const UnsignedInt type,
                                        const std::string &name = "");
   virtual bool            RenameRefObject(const UnsignedInt type,
                                           const std::string &oldName,
                                           const std::string &newName);

protected:

   // types and data
   enum
   {
      REFERENCE_OBJECT = DynamicAxesParamCount,
      ALIGNMENT_VECTOR_X,
      ALIGNMENT_VECTOR_Y,
      ALIGNMENT_VECTOR_Z,
      CONSTRAINT_VECTOR_X,
      CONSTRAINT_VECTOR_Y,
      CONSTRAINT_VECTOR_Z,
      CONSTRAINT_COORDSYS,
      CONSTRAINT_REF_VECTOR_X,
      CONSTRAINT_REF_VECTOR_Y,
      CONSTRAINT_REF_VECTOR_Z,
      LocalAlignedConstrainedAxesParamCount
   };

   static const std::string         PARAMETER_TEXT[LocalAlignedConstrainedAxesParamCount -
                                                   DynamicAxesParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[LocalAlignedConstrainedAxesParamCount -
                                                   DynamicAxesParamCount];

   static const Real                MAGNITUDE_TOL;
   static const Real                ORTHONORMAL_TOL;

   std::string       referenceObjName;
   SpacePoint        *referenceObject;
   std::string       constraintCSName;
   CoordinateSystem  *constraintCS;

   Rvector3          alignmentVector;
   Rvector3          constraintVector;
   Rvector3          constraintRefVector;

   /// methods to compute the rotation matrix (and its derivative) to transform
   /// from this LocalAlignedConstrainedAxesParamCount system to MJ2000EqAxes
   /// system; inherited from AxisSystem
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

};
#endif // ObjectReferencedAxes_hpp
