//$Id$
//------------------------------------------------------------------------------
//                              FiniteThrust
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Defines the FiniteThrust class used to model the acceleration during a finite 
 * burn. 
 */
//------------------------------------------------------------------------------


#ifndef FiniteThrust_hpp
#define FiniteThrust_hpp

#include "PhysicalModel.hpp"
#include "FiniteBurn.hpp"


/**
 * The force used for finite burns.
 */
class GMAT_API FiniteThrust : public PhysicalModel
{
public:
   FiniteThrust(const std::string &name = "");
   virtual ~FiniteThrust();
   FiniteThrust(const FiniteThrust& ft);
   FiniteThrust&           operator=(const FiniteThrust& ft);
   bool                    operator==(const FiniteThrust& ft) const;

   virtual GmatBase*       Clone() const;

   virtual void            Clear(const UnsignedInt
                                 type = Gmat::UNKNOWN_OBJECT);
   virtual bool            SetRefObjectName(const UnsignedInt type,
                                            const std::string &name);
   virtual const StringArray&
                           GetRefObjectNameArray(const UnsignedInt type);
   virtual bool            SetRefObject(GmatBase *obj, 
                              const UnsignedInt type,
                              const std::string &name = "");
   virtual bool            SetRefObject(GmatBase *obj,
                              const UnsignedInt type,
                              const std::string &name, const Integer index);
   virtual bool            RenameRefObject(const UnsignedInt type,
                                           const std::string &oldName,
                                           const std::string &newName);
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name);
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name,
                                        const Integer index);

   virtual bool            IsTransient();
   virtual bool            DepletesMass();
   virtual void            SetPropList(ObjectArray *soList);
   virtual bool            Initialize();

   // Make defual value of dt and order the same as other force models' setting 
   //virtual bool            GetDerivatives(Real * state, Real dt, Integer order, 
   virtual bool            GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1, 
                                          const Integer id = -1);
   virtual Rvector6        GetDerivativesForSpacecraft(Spacecraft *sc);

   
   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index, 
                         Integer quantity, Integer sizeOfType);

   DEFAULT_TO_NO_CLONES

protected:
   // Pieces needed for bookkeeping
   /// The finite burn objects this model uses
   std::vector <FiniteBurn *>    burns;
   /// Names for the FiniteBurn objects used for this force 
   StringArray                   burnNames;
   /// Names of the spacecraft accessed by this force
   StringArray                   mySpacecraft;
   /// Propagated objects used in the ODEModel
   ObjectArray                   spacecraft;
   /// Indexes (in the spacecraft vector) for the Spacecraft used by this force
   std::vector<Integer>          scIndices;
   /// Number of spacecraft in the state vector that use CartesianState
   Integer                       satCount;
   /// Start index for the Cartesian state
   Integer                       cartIndex;
   /// Flag indicating if the Cartesian state should be populated
   bool                          fillCartesian;

   /// Number of spacecraft in the state vector that deplete mass
   Integer                       satThrustCount;
   /// Start index for the dm/dt data
   Integer                       mDotIndex;
   /// Flag indicating if any thrusters are set to deplete mass
   bool                          depleteMass;
};

#endif // FiniteThrust_hpp

