//------------------------------------------------------------------------------
//                           GroundTrack
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Sep 12, 2025
/**
 * Base code subscriber for the new ground track component/
 *
 * This implementation is ported from Astrodynamics Workbench(R).
 */
//------------------------------------------------------------------------------

#ifndef SRC_BASE_SUBSCRIBER_GROUNDTRACK_HPP_
#define SRC_BASE_SUBSCRIBER_GROUNDTRACK_HPP_

#include "gmatdefs.hpp"
#include "Subscriber.hpp"
#include "GroundstationInterface.hpp"

#include "CelestialBody.hpp"
#include "Spacecraft.hpp"
#include "GroundstationInterface.hpp"

class GMAT_API GroundTrack : public Subscriber
{
public:
   GroundTrack(const std::string &name);
   virtual ~GroundTrack();
   GroundTrack(const GroundTrack& gt);
   GroundTrack& operator=(const GroundTrack& gt);

   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);

   virtual bool Initialize();

   // methods for parameters
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterValid(const Integer id, const std::string &value);
   virtual bool         IsParameterValid(const std::string &label, const std::string &value);
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;

   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value);

   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);

   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label, const std::string &value);

   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   virtual Real GetRealParameter(const Integer id) const;
   virtual Real SetRealParameter(const Integer id, const Real value);

   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const std::string &label, const Real value);

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                         const Integer value);

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);

   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);

   virtual const BooleanArray&
                        GetBooleanArrayParameter(const Integer id) const;
   virtual const BooleanArray&
                        GetBooleanArrayParameter(const std::string &label) const;
   virtual bool         SetBooleanArrayParameter(const Integer id,
                                                 const BooleanArray &valueArray);
   virtual bool         SetBooleanArrayParameter(const std::string &label,
                                                 const BooleanArray &valueArray);

   // Initialization setup methods
   virtual bool HasRefObjectTypeArray();
   virtual const ObjectTypeArray& GetRefObjectTypeArray();

   virtual std::string GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);

   virtual bool SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);

   virtual bool RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   std::vector<GroundstationInterface*> *GetStations();

   virtual void SetDataLabels(const StringArray& elements);
   virtual bool Distribute(const Real * dat, Integer len);
   virtual bool TakeAction(const std::string &action, const std::string &actionData);

   virtual void SetSolarSystem(SolarSystem *ss);

protected:
   /// Central body for the track
   std::string theBodyName;
   /// The solar system.  theBody should be a member.
   SolarSystem *theSS;
   /// The body, used for the body fixed CS and texture map
   CelestialBody *theBody;
   /// Objects added for viewing
   StringArray theObjects;
   /// Array indicating is the name is displayed
   BooleanArray showName;
   /// The map file
   std::string mapFile;
   /// Line width for the trace
   int lineWidth;

   // Ground track plot legacy items
   /// Setting for how often to add a data point to the track
   int collectFrequency;
   /// Frequency for updates to teh display
   int updateFrequency;
   /// Length of the redraw on a track
   int redrawCount;
   /// (Not used; legacy setting) Maximum number of points on the track
   int maxData;
   /// Toggle used to turn the plot on and off
   bool showPlot;
   /// Setting for sensor footprints; not currently hooked up
   std::string footprintType;

   // Latitude and longitude grid line control
   /// Toggle for the grid lines
   bool useGrid;
   /// Number of latitude lines
   Integer latLineCount;
   /// Number of longitude lines
   Integer longLineCount;

   /// Body fixed coordinate system for gtorigin
   CoordinateSystem *viewCoordinates;
   /// Inertial system for the incoming data
   CoordinateSystem *inertialSystem;
   /// Converter used to build the track data
   CoordinateConverter ccvt;

   /// Ground track body radius
   double bodyRadius;
   /// Ground track body flattening factor
   double bodyFlattening;

   /// Data structure used to pass data to the view
   double *longlat;

   /// Data structure used for point locations marked on the map
   double *pointlonglat;

   /// Spacecraft that are tracked
   std::vector<Spacecraft*> theSats;

   /// Map of state X indices by spacecraft
   std::map<Spacecraft*, int> stateMap;
   std::vector<int> xIndex;

   /// Celestial bodies tracked, omitting central body and spacecraft
   std::vector<SpacePoint*> theBodies;

   /// Other points tracked
   std::vector<SpacePoint*> thePoints;

   // List of groundstations
   std::vector<GroundstationInterface*> theStations;

   enum
   {
      CENTRAL_BODY = SubscriberParamCount,
      TEXTURE_MAP,
      ADD,
      SHOWNAME,
      LINE_WIDTH,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      NUM_POINTS_TO_REDRAW,
      MAX_DATA,
      SHOW_PLOT,
      SHOW_FOOTPRINTS,
      USE_GRID,
      LATLINE_COUNT,
      LONGLINE_COUNT,
      GroundTrackParamCount
   };

   static const Gmat::ParameterType
      PARAMETER_TYPE[GroundTrackParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[GroundTrackParamCount - SubscriberParamCount];

   void CalculateLatLong(const double *state, double &longitude, double &latitude);
};

#endif /* SRC_BASE_SUBSCRIBER_GROUNDTRACK_HPP_ */
