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
// Author: Ed Behn
// Created: 2025/12/22
//
// Developed jointly by NASA/GSFC and Pearl River Technologies

// This class launches OpenFramesInterface in place of OrbitView in order
// to improve performance and stability.

#ifndef OVtoOFI_hpp
#define OVtoOFI_hpp

#include "OrbitView.hpp"
#include "OVtoOFI_defs.hpp"

#include "OpenFramesInterface.hpp"
#include "OpenFramesView.hpp"

// Replacement for OpenFramesView that produces no script output

/// The view interface object is needed because the OpenFramesView is created automatically
class OVTOOFI_API InvisOpenFramesView : public OpenFramesView
{
public:
    InvisOpenFramesView(const std::string& typeName, const std::string& name);
    InvisOpenFramesView(const InvisOpenFramesView&);

    InvisOpenFramesView& operator=(const InvisOpenFramesView&);

    virtual GmatBase* Clone(void) const;

    virtual const std::string& GetGeneratingString(
        Gmat::WriteMode mode = Gmat::SCRIPTING,
        const std::string& prefix = "",
        const std::string& useName = "");
};

/// Core mapping component for the conversion of an OrbitView to an OpenFramesInterface
class OVTOOFI_API OVtoOFI : public OpenFramesInterface
{
public:
    OVtoOFI(const std::string& typeName, const std::string& name);
    OVtoOFI(const OVtoOFI&);

    OVtoOFI& operator=(const OVtoOFI&);

    virtual GmatBase* Clone(void) const;

    // methods for parameters
    virtual std::string  GetParameterText(const Integer id) const;
    virtual Integer      GetParameterID(const std::string& str) const;
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string  GetParameterTypeString(const Integer id) const;
    virtual bool         IsParameterReadOnly(const Integer id) const;
    virtual bool         IsSquareBracketAllowedInSetting(const Integer id) const;
    virtual bool         IsParameterCommandModeSettable(const Integer id) const;

    virtual Integer      SetIntegerParameter(const Integer id, const Integer value);
    virtual Integer      SetIntegerParameter(const std::string& label, const Integer value);
    virtual Integer      GetIntegerParameter(const Integer id) const;
    virtual Integer      GetIntegerParameter(const std::string& label) const;

    virtual Real         SetRealParameter(const Integer id, const Real value);
    virtual Real         SetRealParameter(const std::string& label, const Real value);
    virtual Real         GetRealParameter(const Integer id) const;
    virtual Real         GetRealParameter(const std::string& label) const;

    virtual Real         SetRealParameter(const Integer id, const Real value, const Integer index);
    virtual Real         GetRealParameter(const Integer id, const Integer index) const;
    virtual Real         SetRealParameter(const std::string& label, const Real value, const Integer index);
    virtual Real         GetRealParameter(const std::string& label, const Integer index) const;

    virtual const Rvector& GetRvectorParameter(const Integer id) const;
    virtual const Rvector& SetRvectorParameter(const Integer id, const Rvector& value);
    virtual const Rvector& GetRvectorParameter(const std::string& label) const;
    virtual const Rvector& SetRvectorParameter(const std::string& label, const Rvector& value);

    virtual bool         SetStringParameter(const Integer id, const std::string& value);
    virtual std::string  GetStringParameter(const Integer id) const;
    virtual std::string  GetStringParameter(const std::string& label) const;
    virtual bool         SetStringParameter(const std::string& label, const char* value);
    virtual bool         SetStringParameter(const std::string& label, const std::string& value);

    virtual bool         SetStringParameter(const Integer id, const std::string& value, const Integer index);
    virtual bool         SetStringParameter(const std::string& label, const std::string& value, const Integer index);

    virtual const StringArray& GetStringArrayParameter(const Integer id) const;
    virtual bool SetStringArrayParameter(const Integer id, const StringArray& stringArray);

    virtual bool         SetOnOffParameter(const Integer id, const std::string& value);
    virtual bool         SetOnOffParameter(const std::string& label, const std::string& value);
    virtual std::string  GetOnOffParameter(const Integer id) const;
    virtual std::string  GetOnOffParameter(const std::string& label) const;

    virtual bool         GetBooleanParameter(const std::string& label) const;
    virtual bool         GetBooleanParameter(const Integer id) const;
    virtual bool         SetBooleanParameter(const Integer id,        const bool value);
    virtual bool         SetBooleanParameter(const std::string& label, const bool value);

    virtual const BooleanArray& GetBooleanArrayParameter(const Integer id) const;
    virtual const BooleanArray& GetBooleanArrayParameter(const std::string& label) const;
    virtual bool         SetBooleanArrayParameter(const Integer id, const BooleanArray& valueArray);
    virtual bool         SetBooleanArrayParameter(const std::string& label, const BooleanArray& valueArray);

    virtual UnsignedInt GetPropertyObjectType(const Integer id) const;

    virtual const std::string& GetGeneratingString(
                          Gmat::WriteMode mode = Gmat::SCRIPTING,
                          const std::string& prefix = "",
                          const std::string& useName = "");

protected:
    OpenFramesView *theView;
    Rvector3 defaultUp;
    Rvector3 viewPoint;
    std::string viewUp;
    Integer addCount;
    bool showLabels;
    bool drawGrid;
    Real viewScaleFactor;

    bool setupGrid();
    bool setupLabels();
    Rvector setupDefaultEye();

    enum
    {
        ORBIT_COLOR = OpenFramesInterfaceParamCount,
        TARGET_COLOR,
        DATA_COLLECT_FREQUENCY,
        UPDATE_PLOT_FREQUENCY,
        NUM_POINTS_TO_REDRAW,
        MAX_DATA,
        SHOW_LABELS,
        VIEWPOINT_REF,
        VIEWPOINT_REFERENCE,
        VIEWPOINT_REF_TYPE,
        VIEWPOINT_REF_VECTOR,
        VIEWPOINT_VECTOR,
        VIEWPOINT_VECTOR_TYPE,
        VIEWPOINT_VECTOR_VECTOR,
        VIEW_DIRECTION,
        VIEW_DIRECTION_TYPE,
        VIEW_DIRECTION_VECTOR,
        VIEW_SCALE_FACTOR,
        VIEW_UP_COORD_SYSTEM,
        VIEW_UP_AXIS,
        CELESTIAL_PLANE,
        WIRE_FRAME,
        GRID,
        EARTH_SUN_LINES,
        SUN_LINE,
        OVERLAP_PLOT,
        USE_INITIAL_VIEW,
        ENABLE_CONSTELLATIONS,
        MIN_FOV,
        MAX_FOV,
        INITIAL_FOV,
        OVtoOFIParamCount
    };

    static const Gmat::ParameterType
        PARAMETER_TYPE[OVtoOFIParamCount - OpenFramesInterfaceParamCount];
    static const std::string
        PARAMETER_TEXT[OVtoOFIParamCount - OpenFramesInterfaceParamCount];
};

#endif // OVtoOFI_hpp
