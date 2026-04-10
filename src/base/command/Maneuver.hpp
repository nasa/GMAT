//$Id$
//------------------------------------------------------------------------------
//                                 Maneuver
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
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/20
//
/**
 * Definition for the Maneuver command class
 */
//------------------------------------------------------------------------------


#ifndef Maneuver_hpp
#define Maneuver_hpp

#include "GmatCommand.hpp"
#include "Burn.hpp"
#include "Spacecraft.hpp"


/**
 * This class implements the Maneuver command.  Maneuvers are performed using
 * pre-configured Burn objects.  A typical segment of script that implements
 * a maneuver for a spacecraft named Sat1 looks like this:
 *
 *     Create ImpulsiveBurn burn;
 *     burn.CoordinateSystem = Local;
 *     burn.Origin = Earth;
 *     burn.Axes = VNB;
 *     burn.Element1 = 0.125;         % km/s
 *     ...
 *     Maneuver burn(Sat1);
 */
class GMAT_API Maneuver : public GmatCommand
{
public:
   Maneuver();
   virtual ~Maneuver();
   Maneuver(const Maneuver& m);
   Maneuver&            operator=(const Maneuver& m);
   
   // Methods used for configuration
//   virtual bool         SetObject(const std::string &name,
//                                  const UnsignedInt type,
//                                  const std::string &associate = "",
//                                  const UnsignedInt associateType =
//                                  Gmat::UNKNOWN_OBJECT);
//
//   virtual bool         SetObject(GmatBase *obj,
//                                  const UnsignedInt type);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode =
                                                           Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   
   // Parameter accessor methods -- overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);

   
   // Methods used to run the command
   virtual bool         InterpretAction();
   
   virtual bool         Initialize();
   virtual bool         Execute();
   
   DEFAULT_TO_NO_CLONES

protected:
   /// The burn object used for the maneuver
   std::string              burnName;
   /// The burn object used for the maneuver
   Burn                    *burn;
   /// The name of the spacecraft that gets maneuvered
   std::string             satName;
   /// The spacecraft
   Spacecraft              *sat;
   /// Flag used to tell if the summary can be built yet
   bool                    firedOnce;

   /// Saved data for Command Summary
   /// flag indicating whether or not the Burn is using a Local
   /// coordinate system
   bool                    localCS;
   /// Spacecraft that maneuvers, as retrieved from the Burn object
   std::string             scNameM;
   /// Maneuver coordinate system
   std::string             csNameM;
   /// Maneuver origin
   std::string             originNameM;
   /// Maneuver axis system
   std::string             axesNameM;

   /// Spacecraft tanks used for mass depletion from the maneuver
   StringArray             tankNamesM;
   /// Flag indicating if mass was depleted
   bool                    decMassM;
   /// Flag indicating if the maneuver should be applied as if in backprop
   bool                    maneuverBackwards;

   /// Maneuver data used in the summary
   Real                    *elementIspMassData;

   // Parameter IDs 
   enum
   {
      burnNameID = GmatCommandParamCount,
      satNameID,
      backPropID,
      ManeuverCommandParamCount
   };
   static const std::string
         PARAMETER_TEXT[ManeuverCommandParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
         PARAMETER_TYPE[ManeuverCommandParamCount - GmatCommandParamCount];

   virtual void            BuildCommandSummary(bool commandCompleted = true);
   virtual void            BuildCommandSummaryString(
                                 bool commandCompleted = true);
};

#endif // Maneuver_hpp
