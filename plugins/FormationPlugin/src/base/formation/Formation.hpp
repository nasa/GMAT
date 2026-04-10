//------------------------------------------------------------------------------
//                              Formation
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/7/24
//
/**
 * Defines the class used for formations. 
 */
//------------------------------------------------------------------------------


#ifndef Formation_hpp
#define Formation_hpp

#include "FormationDefs.hpp"
#include "FormationInterface.hpp"

class FORMATION_API Formation : public FormationInterface
{
public:
   Formation(UnsignedInt typeId, const std::string &typeStr,
             const std::string &instName);
   virtual ~Formation();
   Formation(const Formation& orig);
   Formation&           operator=(const Formation& orig);
   
   virtual const Rvector6 GetMJ2000State(const A1Mjd &atTime);
   virtual const Rvector6 GetMJ2000State(const GmatTime &atTimeGT);

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   virtual void         ParametersHaveChanged(bool flag);
   
   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const Integer id, const bool value);
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const Integer id, const Real value);
   virtual Real         SetRealParameter(const std::string &label, 
                                         const Real value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray& 
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& 
                        GetStringArrayParameter(const std::string &label) const;

   virtual const        ObjectTypeArray& GetRefObjectTypeArray();

   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);
   
   virtual void         BuildState();
   virtual void         UpdateElements();
   virtual void         UpdateState();
   virtual bool         IsManeuvering();
   virtual const StringArray&
                        GetManeuveringMembers();

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   virtual void         ClearLastStopTriggered();
   // virtual void SetLastStopTriggered(const std::string &stopCondName);
   // virtual bool WasLastStopTriggered(const std::string &stopCondName);
   
//   virtual Integer         GetPropItemID(const std::string &whichItem);
   virtual Integer         SetPropItem(const std::string &propItem);
   virtual StringArray     GetDefaultPropItems();
   virtual Real*           GetPropItem(const Integer item);
   virtual Integer         GetPropItemSize(const Integer item);
   
   DEFAULT_TO_NO_CLONES

protected:
   /// List of the object names used in the formation
   StringArray                      componentNames;
   /// Pointers to the formation members
   std::vector <SpaceObject *>      components;
   /// Size of the state vector used in propagation
   Integer                          dimension;
   /// Number of spacecraft in the state
   UnsignedInt                      satCount;
   
   /// Enumerated parameter IDs   
   enum
   {
      ADDED_SPACECRAFT = SpaceObjectParamCount,
      REMOVED_SPACECRAFT,
      CLEAR_NAMES,
      FORMATION_STM,
      FORMATION_CARTESIAN_STATE,
      FormationParamCount
   };
   
   /// Array of supported parameters
   static const std::string
      PARAMETER_TEXT[FormationParamCount - SpaceObjectParamCount];
   /// Array of parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[FormationParamCount - SpaceObjectParamCount];
   
   bool                 ClearSpacecraftList();
   bool                 RemoveSpacecraft(const std::string &name);
};

#endif // Formation_hpp
