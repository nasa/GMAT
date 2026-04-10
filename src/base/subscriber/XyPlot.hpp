//$Id$
//------------------------------------------------------------------------------
//                                  XyPlot
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/22
//
/**
 * Declares XyPlot class.
 */
//------------------------------------------------------------------------------
#ifndef XyPlot_hpp
#define XyPlot_hpp

#include "Subscriber.hpp"
#include "Parameter.hpp"


/**
 * Subscriber class used to drive the XyPlot components on the GUI
 */
class GMAT_API XyPlot : public Subscriber 
{
public:
   XyPlot(const std::string &name, Parameter *xParam = NULL,
          Parameter *firstYParam = NULL, const std::string &plotTitle = "",
          const std::string &xAxisTitle = "", const std::string &yAxisTitle = "");
   XyPlot(const XyPlot &orig);
   XyPlot& operator=(const XyPlot& orig);
   virtual ~XyPlot(void);
   
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
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   
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
   
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   
   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   
   // methods for setting up the items to subscribe
   virtual const StringArray&
                        GetWrapperObjectNameArray(bool completeSet = false);

   virtual void         SetRunState(Gmat::RunState rs);

protected:

   bool SetXParameter(const std::string &paramName);
   bool AddYParameter(const std::string &paramName, Integer index);
   void BuildPlotTitle();
   bool ClearYParameters();
   bool RemoveYParameter(const std::string &name);
   bool ResetYParameters();
   bool PenUp();
   bool PenDown();
   bool MarkPoint();
   bool Darken(Integer factor);
   bool Lighten(Integer factor);
   bool MarkBreak();
   bool ClearFromBreak();
   
   void DeletePlotCurves();
   void WriteDeprecatedMessage(Integer id) const;

   Parameter *mXParam;
   std::vector<Parameter*> mYParams;
   
   Integer mNumXParams;
   Integer mNumYParams;
   
   std::string mXParamName;
   StringArray mYParamNames;
   StringArray mAllParamNames;
   
   std::string mOldName;
   std::string mPlotTitle;
   std::string mXAxisTitle;
   std::string mYAxisTitle;
   bool mDrawGrid;
   bool mIsXyPlotWindowSet;
   
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;
   
   Integer mNumDataPoints;
   Integer mNumCollected;
   
   bool useLines;
   Integer lineWidth;
   bool useMarkers;
   Integer markerSize;
   bool drawing;
   Integer breakCount;

   // methods inherited from Subscriber
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);

public:
   enum
   {
      XVARIABLE = SubscriberParamCount,
      YVARIABLES,
//      PLOT_TITLE,
//      X_AXIS_TITLE,
//      Y_AXIS_TITLE,
      SHOW_GRID,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      SHOW_PLOT,
//      USE_LINES,
//      LINE_WIDTH,
//      USE_MARKERS,
//      MARKER_SIZE,
      DRAWING,
      IND_VAR,                 // deprecated
      ADD,                     // deprecated
      DRAW_GRID,               // deprecated
      XyPlotParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[XyPlotParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[XyPlotParamCount - SubscriberParamCount];

};

#endif
