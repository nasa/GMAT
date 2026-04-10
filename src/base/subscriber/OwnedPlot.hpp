//$Id$
//------------------------------------------------------------------------------
//                                  OwnedPlot
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
// number NNG06CA54C
//
// Author: Darrel Conway, based on XyPlot code by Linda Jun
// Created: 2009/09/28
//
/**
 * Defines XyPlot class controlled by Sandbox elements rather than Subscribers.
 */
//------------------------------------------------------------------------------
#ifndef OwnedPlot_hpp
#define OwnedPlot_hpp

#include "GmatBase.hpp"
#include "Subscriber.hpp"


/**
 * The OwnedPlot class provides access to plotting capabilities for components
 * that need to display graphical information for that component only, without
 * generating object to object interactions or data passing through GMAT's
 * Publisher.  This component is wholly owned by another, which passes in data
 * and controls plot updates using the interfaces defined here.
 *
 * OwnedPlot objects talk to the graphical components through the PlotInterface
 * and PlotReceiver classes.  The standard GMAT GUI interface for this
 * communication is the GuiPlotReceiver, which connects the plot messages to a
 * XyPlot object displayed on the GUI.
 *
 * OwnedPlots are not Subscribers that show up on the Resource tree in GMAT.
 * They exist inside of other objects that own them and control their
 * allocation, processing, and deallocation.
 */
class GMAT_API OwnedPlot : public Subscriber
{
public:
   OwnedPlot(const std::string &name, const std::string &plotTitle = "",
          const std::string &xAxisTitle = "",
          const std::string &yAxisTitle = "");
   OwnedPlot(const OwnedPlot &orig);
   OwnedPlot& operator=(const OwnedPlot& orig);
   virtual ~OwnedPlot(void);
   
   // methods inherited from Subscriber
   virtual bool         Initialize();
   
   // methods inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         SetName(const std::string &who,
                                const std::string &oldName = "");
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   
   virtual bool         Activate(bool state = true);
   bool                 Deactivate();

   // Methods used to access the plot
   virtual void         SetData(std::vector<RealArray*> &dataBlast,
                              RealArray hiErrors, RealArray lowErrors);
   virtual void         SetCurveData(const Integer forCurve, RealArray *xData,
                              RealArray *yData, const RealArray *yhis = NULL,
                              const RealArray *ylows = NULL);

   virtual bool         MarkPoint(Integer whichOne = -1, Integer forCurve = -1);

   virtual Integer      SetUsedDataID(Integer id, Integer forCurve = -1);
   virtual void         SetUsedObjectID(Integer id);
   virtual Integer      UsesData(Integer id);
   virtual Integer      UsesObject(Integer id);

protected:
   StringArray  curveNames;
   IntegerArray curveDataIDs;
   
   // These arrays store the default settings, curve by curve  They default to
   // the plot level settings unless the user overrides them

   /// Curve colors; default is 0xFF0000 (Blue)
   IntegerArray curveColor;
   /// Line width used to draw lines and markers; default is 1
   IntegerArray curveLineWidth;
   /// Line style for each line; default is wxSOLID
   IntegerArray curveLineStyle;
   /// Marker styles for the curves; these iterate through the defined styles
   IntegerArray curveMarker;
   /// Marker size; default is 3
   IntegerArray curveMarkerSize;

   /// Curve by curve useLines setting
   std::vector<bool> curveUseLines;
   /// Curve by curve useMarkers setting
   std::vector<bool> curveUseMarkers;
   /// Curve by curve useHiLow setting
   std::vector<bool> curveUseHiLow;

   std::string mOldName;
   std::string mPlotTitle;
   std::string mXAxisTitle;
   std::string mYAxisTitle;
   std::string mDrawGrid;
   bool mIsOwnedPlotWindowSet;
   
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;

   /// Default color
   IntegerArray defaultColor;
   /// Default marker size; initialized to 3
   Integer markerSize;
   /// Default marker style; initialized to -1, meaning set by curve number
   Integer markerStyle;
   /// Default line width used to draw lines and markers; default is 1
   Integer lineWidth;
   /// Default line style; initialized to a solid line
   Integer lineStyle;   // If this breaks, check the enum value for wxSOLID

   bool useLines;
   bool useMarkers;
   bool useHiLow;

   IntegerArray supportedData;
   IntegerArray supportedObjects;

   bool                 active;
   bool                 showLegend;
   bool                 isEndOfReceive;
   bool                 isEndOfRun;
   bool                 isInitialized;
   std::string          mSolverIterations;
   Gmat::RunState       runstate;

   void BuildPlotTitle();
   bool ClearYParameters();
   bool RemoveYParameter(const std::string &name);
   bool ResetYParameters();
   bool PenUp();
   bool PenDown();
   bool RescaleData();

   void DeletePlotCurves();

   enum
   {
      ADD = GmatBaseParamCount,
      PLOT_TITLE,
      X_AXIS_TITLE,
      Y_AXIS_TITLE,
      DRAW_GRID,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      SHOW_PLOT,
      SHOW_LEGEND,
      DEFAULT_COLOR,
      USE_LINES,
      LINE_WIDTH,
      LINE_STYLE,
      USE_MARKERS,
      MARKER_SIZE,
      MARKER_STYLE,
      USE_HI_LOW,
      OwnedPlotParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[OwnedPlotParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[OwnedPlotParamCount - GmatBaseParamCount];
};

#endif
