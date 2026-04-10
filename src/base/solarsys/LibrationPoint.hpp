//$Id$
//------------------------------------------------------------------------------
//                                  LibrationPoint
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
// Author: Wendy C. Shoan
// Created: 2005.04.13
//
/**
 * This is the base class for libration points.
 */
//------------------------------------------------------------------------------

#ifndef LibrationPoint_hpp
#define LibrationPoint_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "CalculatedPoint.hpp"
#include "A1Mjd.hpp"
#include "Rmatrix.hpp"
#include "Rvector6.hpp"
#include "TimeTypes.hpp"

/**
 *
 * LibrationPoint derives from CalculatedPoint.
 *
 */
class GMAT_API LibrationPoint : public CalculatedPoint
{
public:
   // default constructor, specifying libration point name
   LibrationPoint(const std::string &itsName = "");
   // copy constructor
   LibrationPoint(const LibrationPoint &lp);
   // operator=
   LibrationPoint& operator=(const LibrationPoint &lp);
   // destructor
   virtual ~LibrationPoint();
   
   // methods inherited from SpacePoint, that must be implemented here
   // in the derived class
   virtual const Rvector6 GetMJ2000State(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Position(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime);

   virtual const Rvector6 GetMJ2000State(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000Position(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000Velocity(const GmatTime &atTime);

   virtual StringArray    GetBuiltInNames();

   
   // Parameter access methods - overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType 
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   // These two added to satisfy GCC, which apparently has trouble finding 
   // the methods inherited from CalculatedPoint
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value,
                                           const Integer index);
   
   virtual const        ObjectTypeArray& GetRefObjectTypeArray();
   virtual const StringArray& 
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObject(GmatBase *obj, 
                                     const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   
protected:

   enum
   {
      PRIMARY_BODY_NAME = CalculatedPointParamCount, 
      SECONDARY_BODY_NAME,
      WHICH_POINT,
      LibrationPointParamCount
   };
   static const std::string
      PARAMETER_TEXT[LibrationPointParamCount - CalculatedPointParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[LibrationPointParamCount - CalculatedPointParamCount];
   
   static const Real CONVERGENCE_TOLERANCE;  // = 1.0E-8;
   static const Real MAX_ITERATIONS;         // = 2000;
   static const Real ZERO_MASS_TOL;          // = 1.0E-15;
   static const Real ZERO_MAG_TOL;           // = 1.0E-12;

   std::string primaryBodyName;
   std::string secondaryBodyName;
   std::string whichPoint;
   
   SpacePoint  *primaryBody;
   SpacePoint  *secondaryBody;
   
private:
   
   //------------------------------------------------------------------------------
   //  void CheckBodies()
   //------------------------------------------------------------------------------
   /**
    * Method that checks to make sure the bodyList has
    * been defined appropriately (i.e. all are of a type that makes sense).
    *
    */
    //------------------------------------------------------------------------------
   virtual void CheckBodies();      
};
#endif // LibrationPoint_hpp

