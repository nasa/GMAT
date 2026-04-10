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

#include "OVtoOFI.hpp"
#include "Moderator.hpp"

// GCC places max in the std namespace
#ifndef max
   #define max std::max
#endif

#define FALL_THROUGH_OFFSET 1000

const std::string
OVtoOFI::PARAMETER_TEXT[OVtoOFIParamCount - OpenFramesInterfaceParamCount] =
{
   "OrbitColor",
   "TargetColor",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
   "NumPointsToRedraw",
   "MaxPlotPoints",
   "ShowLabels",
   "ViewPointRef",
   "ViewPointReference",
   "ViewPointRefType",
   "ViewPointRefVector",
   "ViewPointVector",
   "ViewPointVectorType",
   "ViewPointVectorVector",
   "ViewDirection",
   "ViewDirectionType",
   "ViewDirectionVector",
   "ViewScaleFactor",
   "ViewUpCoordinateSystem",
   "ViewUpAxis",
   "CelestialPlane",
   "WireFrame",
   "Grid",
   "EarthSunLines",
   "SunLine",
   "Overlap",
   "UseInitialView",
   "EnableConstellations",
   "MinFOV",
   "MaxFOV",
   "InitialFOV",
};

const Gmat::ParameterType
OVtoOFI::PARAMETER_TYPE[OVtoOFIParamCount - OpenFramesInterfaceParamCount] =
{
   Gmat::UNSIGNED_INTARRAY_TYPE, //"OrbitColor",
   Gmat::UNSIGNED_INTARRAY_TYPE, //"TargetColor",
   Gmat::INTEGER_TYPE,           //"DataCollectFrequency"
   Gmat::INTEGER_TYPE,           //"UpdatePlotFrequency"
   Gmat::INTEGER_TYPE,           //"NumPointsToRedraw"
   Gmat::INTEGER_TYPE,           //"MaxPlotPoints"

   Gmat::BOOLEAN_TYPE,           //"ShowLabels"
   Gmat::OBJECT_TYPE,            //"ViewPointRef", - deprecated
   Gmat::OBJECT_TYPE,            //"ViewPointReference",
   Gmat::STRING_TYPE,            //"ViewPointRefType"
   Gmat::RVECTOR_TYPE,           //"ViewPointRefVector",
   Gmat::RVECTOR_TYPE,           //"ViewPointVector",
   Gmat::STRING_TYPE,            //"ViewPointVectorType",
   Gmat::RVECTOR_TYPE,           //"ViewPointVectorVector", - deprecated
   Gmat::OBJECT_TYPE,            //"ViewDirection",
   Gmat::STRING_TYPE,            //"ViewDirectionType",
   Gmat::RVECTOR_TYPE,           //"ViewDirectionVector", - deprecated
   Gmat::REAL_TYPE,              //"ViewScaleFactor",
   Gmat::OBJECT_TYPE,            //"ViewUpCoordinaetSystem"
   Gmat::ENUMERATION_TYPE,       //"ViewUpAxis"

   Gmat::ON_OFF_TYPE,            //"CelestialPlane" - deprecated
   Gmat::ON_OFF_TYPE,            //"WireFrame"
   Gmat::ON_OFF_TYPE,            //"Grid"
   Gmat::ON_OFF_TYPE,            //"EarthSunLines" - deprecated
   Gmat::ON_OFF_TYPE,            //"SunLine"
   Gmat::ON_OFF_TYPE,            //"Overlap"
   Gmat::ON_OFF_TYPE,            //"UseInitialView"

   Gmat::ON_OFF_TYPE,            //"EnableConstellations"

   Gmat::INTEGER_TYPE,           //"MinFOV"
   Gmat::INTEGER_TYPE,           //"MaxFOV"
   Gmat::INTEGER_TYPE,           //"InitialFOV"
};

// Default constructor
InvisOpenFramesView::InvisOpenFramesView(const std::string& typeName, const std::string& name) :
    OpenFramesView(typeName, name)
{
}

// Copy constructor
InvisOpenFramesView::InvisOpenFramesView(const InvisOpenFramesView& iofv) :
    OpenFramesView(iofv)
{
}

//------------------------------------------------------------------------------
// InvisOpenFramesView& InvisOpenFramesView::operator=(const InvisOpenFramesView& iofv)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
InvisOpenFramesView& InvisOpenFramesView::operator=(const InvisOpenFramesView& iofv)
{
    if (this == &iofv)
        return *this;

    OpenFramesView::operator=(iofv);
    return *this;
}

//------------------------------------------------------------------------------
// GmatBase* InvisOpenFramesView::Clone(void) const
//------------------------------------------------------------------------------
/*
 * Clone object
 */
//------------------------------------------------------------------------------
GmatBase* InvisOpenFramesView::Clone(void) const
{
    return new InvisOpenFramesView(*this);
}

//------------------------------------------------------------------------------
// const std::string& InvisOpenFramesView::GetGeneratingString(Gmat::WriteMode mode, const std::string& prefix, const std::string& useName)
//------------------------------------------------------------------------------
/*
 * Ignore all input and return an empy string.
 * This because the object is automatically regenerated when the script is loaded
 *
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string containing the text.
 */
//------------------------------------------------------------------------------
const std::string& InvisOpenFramesView::GetGeneratingString(Gmat::WriteMode mode, const std::string& prefix, const std::string& useName)
{
    return OpenFramesView::GetGeneratingString(mode, prefix);
}


// ----------------------------------------------------------------------------
// Main OVtoOFI wrapping code
// ----------------------------------------------------------------------------

// Default constructor
OVtoOFI::OVtoOFI(const std::string& type, const std::string& name) :
    OpenFramesInterface(type, name),
    defaultUp(0.0, 0.0, 1.0),
    viewPoint(30000.0, 0.0, 0.0),
    viewUp("Z"),
    addCount(0),
    showLabels(true),
    drawGrid(false),
    viewScaleFactor(1.0)
{
    parameterCount = OVtoOFIParamCount;

    objectTypeNames.push_back("OVtoOFI");

    std::string ofvName = name + "_View";

    // Build a configured View object
    UnsignedInt viewType = GmatType::GetTypeId("OpenFramesView");
    theView = (OpenFramesView*)(Moderator::Instance()->CreateObject(viewType, "OpenFramesView", ofvName));

    // Set default OV settings on the view object
    theView->SetStringParameter("ViewFrame", "CoordinateSystem");
    theView->SetOnOffParameter("SetDefaultLocation", "On");
    theView->SetOnOffParameter("SetCurrentLocation", "Off");

    setupDefaultEye();

    Rvector3 vec(0.0, 0.0, 0.0);
    theView->SetRvectorParameter("DefaultCenter", vec);

    theView->SetRvectorParameter("DefaultUp", defaultUp);

    OpenFramesInterface::SetStringParameter(OpenFramesInterface::VIEW, ofvName, 0);
}

// Copy constructor
OVtoOFI::OVtoOFI(const OVtoOFI& ov_to_ofi) :
    OpenFramesInterface(ov_to_ofi),
    defaultUp(ov_to_ofi.defaultUp),
    viewPoint(ov_to_ofi.viewPoint),
    viewUp(ov_to_ofi.viewUp),
    addCount(ov_to_ofi.addCount),
    showLabels(ov_to_ofi.showLabels),
    drawGrid(ov_to_ofi.drawGrid),
    viewScaleFactor(ov_to_ofi.viewScaleFactor)
{
    std::string ofvName = ov_to_ofi.instanceName + "_View";

    // Build a configured View object
    UnsignedInt viewType = GmatType::GetTypeId("OpenFramesView");
    theView = (OpenFramesView*)(Moderator::Instance()->CreateObject(viewType, "OpenFramesView", ofvName));

    // Set default OV settings on the view object
    theView->SetStringParameter("ViewFrame", "CoordinateSystem");
    theView->SetOnOffParameter("SetDefaultLocation", "On");
    theView->SetOnOffParameter("SetCurrentLocation", "Off");
   
    setupDefaultEye();

    Rvector3 vec(0.0, 0.0, 0.0);
    theView->SetRvectorParameter("DefaultCenter", vec);

    theView->SetRvectorParameter("DefaultUp", defaultUp);

    OpenFramesInterface::SetStringParameter(OpenFramesInterface::VIEW, ofvName, 0);
}

//------------------------------------------------------------------------------
// OVtoOFI& OVtoOFI::operator=(const OVtoOFI& ov_to_ofi)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
OVtoOFI& OVtoOFI::operator= (const OVtoOFI& ov_to_ofi)
{
    if (&ov_to_ofi != this)
    {
       OpenFramesInterface::operator=(ov_to_ofi);

       theView = ov_to_ofi.theView;
       defaultUp = ov_to_ofi.defaultUp;
       viewPoint = ov_to_ofi.viewPoint;
       viewUp = ov_to_ofi.viewUp;
       addCount = ov_to_ofi.addCount;
       showLabels = ov_to_ofi.showLabels;
       drawGrid = ov_to_ofi.drawGrid;
       viewScaleFactor = ov_to_ofi.viewScaleFactor;
    }

    return *this;
}

//------------------------------------------------------------------------------
// GmatBase* OVtoOFI::Clone() const
//------------------------------------------------------------------------------
/*
 * Clone object
 */
//------------------------------------------------------------------------------
GmatBase* OVtoOFI::Clone() const
{
    return new OVtoOFI(*this);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script label of a parameter
 *
 * @param id The ID of the parameter
 *
 * @return The label
 */
//------------------------------------------------------------------------------
std::string OVtoOFI::GetParameterText(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetParameterText(id - FALL_THROUGH_OFFSET);

    if (id >= OpenFramesInterfaceParamCount && id < OVtoOFIParamCount)
        return PARAMETER_TEXT[id - OpenFramesInterfaceParamCount];
    else
        return OpenFramesInterface::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the integer ID of a parameter
 *
 * @param str The label for the parameter
 *
 * @return The parameter's ID
 */
//------------------------------------------------------------------------------
Integer OVtoOFI::GetParameterID(const std::string& str) const
{
    if (str == "OrbitColor" || str == "TargetColor" ||
        str == "PerspectiveMode" || str == "UseFixedFov" || str == "FixedFovAngle" ||
        str == "MinFOV" || str == "MaxFOV" || str == "InitialFOV")
        return Gmat::PARAMETER_REMOVED;

    for (int i = OpenFramesInterfaceParamCount; i < OVtoOFIParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - OpenFramesInterfaceParamCount])
            return i;
    }

    Integer id = OpenFramesInterface::GetParameterID(str);
    if (id < OpenFramesInterfaceParamCount)
        return id;

    return id + FALL_THROUGH_OFFSET;
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type of a parameter
 *
 * @param id The parameter's ID
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType OVtoOFI::GetParameterType(const Integer id) const
{
    if (id >= OpenFramesInterfaceParamCount && id < OVtoOFIParamCount)
        return PARAMETER_TYPE[id - OpenFramesInterfaceParamCount];

    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetParameterType(id - FALL_THROUGH_OFFSET);

    return OpenFramesInterface::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description of a parameter's type
 *
 * @param id The parameter's ID
 *
 * @return The description
 */
//------------------------------------------------------------------------------
std::string OVtoOFI::GetParameterTypeString(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetParameterTypeString(id - FALL_THROUGH_OFFSET);

    return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param id Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool OVtoOFI::IsParameterReadOnly(const Integer id) const
{
    // Legacy ID's from OrbitView
    if (id == OVERLAP_PLOT || id == CELESTIAL_PLANE ||
        id == EARTH_SUN_LINES || id == VIEWPOINT_REF || id == VIEWPOINT_REF_VECTOR ||
        id == VIEWPOINT_VECTOR_VECTOR || id == VIEW_DIRECTION_VECTOR ||
        id == VIEWPOINT_REF_TYPE || id == VIEWPOINT_VECTOR_TYPE ||
        id == MIN_FOV || id == MAX_FOV || id == INITIAL_FOV ||
        id == VIEW_DIRECTION_TYPE || id == ORBIT_COLOR || id == TARGET_COLOR)
        return true;

    // Only write OFI settings on save
    if ((id >= OpenFramesInterfaceParamCount) && (id < OVtoOFIParamCount))
       return true;

    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::IsParameterReadOnly(id - FALL_THROUGH_OFFSET);

    return OpenFramesInterface::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
// bool IsSquareBracketAllowedInSetting(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not a value with square bracket can be
 * assigned to the specified object property.
 *
 * @param id ID for the property.
 *
 * @return true, if a value with square bracket can be assigned; false otherwise
 */
//---------------------------------------------------------------------------
bool OVtoOFI::IsSquareBracketAllowedInSetting(const Integer id) const
{
    if (id == VIEW_DIRECTION || id == VIEWPOINT_VECTOR || id == VIEWPOINT_REFERENCE)
        return true;

    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::IsSquareBracketAllowedInSetting(id - FALL_THROUGH_OFFSET);

    return OpenFramesInterface::IsSquareBracketAllowedInSetting(id);
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool OVtoOFI::IsParameterCommandModeSettable(const Integer id) const
{
    // Why are we making this special case here?
    if (id == SOLVER_ITERATIONS)
        return true;

    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::IsParameterCommandModeSettable(id - FALL_THROUGH_OFFSET);

    return OpenFramesInterface::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Integer OVtoOFI::SetIntegerParameter(const Integer id, const Integer value)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetIntegerParameter(id - FALL_THROUGH_OFFSET, value);

    switch (id)
    {
    case DATA_COLLECT_FREQUENCY:
    case UPDATE_PLOT_FREQUENCY:
    case NUM_POINTS_TO_REDRAW:
    case MAX_DATA:
        return value; // Not used
    default:
        return OpenFramesInterface::SetIntegerParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Integer OVtoOFI::SetIntegerParameter(const std::string& label, const Integer value)
{
    Integer id = GetParameterID(label);
    return SetIntegerParameter(id, value);
}

//------------------------------------------------------------------------------
// Integer OVtoOFI::GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for an integer parameter
 *
 * @param id The ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
Integer OVtoOFI::GetIntegerParameter(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetIntegerParameter(id - FALL_THROUGH_OFFSET);

    switch (id)
    {
    case DATA_COLLECT_FREQUENCY:
    case UPDATE_PLOT_FREQUENCY:
    case NUM_POINTS_TO_REDRAW:
    case MAX_DATA:
        return 0; // Not used
//    case STAR_COUNT :
//        return OpenFramesInterface::GetIntegerParameter(OpenFramesInterface::STAR_COUNT);
    default:
        return OpenFramesInterface::GetIntegerParameter(id);
    }
}

//------------------------------------------------------------------------------
// Integer OVtoOFI::GetIntegerParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for an integer parameter
 *
 * @param label The name for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
Integer OVtoOFI::GetIntegerParameter(const std::string& label) const
{
    Integer id = GetParameterID(label);
    return GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Real OVtoOFI::SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an real parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
Real OVtoOFI::SetRealParameter(const Integer id, const Real value)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetRealParameter(id - FALL_THROUGH_OFFSET, value);

    if (id == VIEW_SCALE_FACTOR)
    {
        viewScaleFactor = value;
        setupDefaultEye();
        return value;
    }

    return OpenFramesInterface::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real OVtoOFI::SetRealParameter(const std::string& label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an real parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
Real OVtoOFI::SetRealParameter(const std::string& label, const Real value)
{
    Integer id = GetParameterID(label);
    return SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real OVtoOFI::GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real OVtoOFI::GetRealParameter(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetRealParameter(id - FALL_THROUGH_OFFSET);

    if (id == VIEW_SCALE_FACTOR)
        return viewScaleFactor;

    return OpenFramesInterface::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real OVtoOFI::GetRealParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param label The name for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real OVtoOFI::GetRealParameter(const std::string& label) const
{
    Integer id = GetParameterID(label);
    return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real OVtoOFI::SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 * @param index Index for the object's location
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real OVtoOFI::SetRealParameter(const Integer id, const Real value, const Integer index)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetRealParameter(id - FALL_THROUGH_OFFSET, value, index);

    if (id == VIEWPOINT_VECTOR && index < 3)
    {
        viewPoint[index] = value;
        setupDefaultEye();
        return value;
    }

    return OpenFramesInterface::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
// Real OVtoOFI::GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param id The ID for the parameter
 * @param index Index for the object's location
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real OVtoOFI::GetRealParameter(const Integer id, const Integer index) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetRealParameter(id - FALL_THROUGH_OFFSET, index);

    if (id == VIEWPOINT_VECTOR)
        return viewPoint[index];

    return OpenFramesInterface::GetRealParameter(id, index);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 * @param index Index for the object's location
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real OVtoOFI::SetRealParameter(const std::string& label, const Real value, const Integer index)
{
    Integer id = GetParameterID(label);
    return SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
// Real OVtoOFI::GetRealParameter(const std::string& label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a real parameter
 *
 * @param label The name for the parameter
 * @param index Index for the object's location
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
Real OVtoOFI::GetRealParameter(const std::string& label, const Integer index) const
{
    Integer id = GetParameterID(label);
    return GetRealParameter(id, index);
}

//------------------------------------------------------------------------------
// const Rvector& OVtoOFI::GetRvectorParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a Rvector parameter
 *
 * @param id The ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
const Rvector& OVtoOFI::GetRvectorParameter(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetRvectorParameter(id - FALL_THROUGH_OFFSET);

    if (id == VIEWPOINT_VECTOR)
        return viewPoint;

    return OpenFramesInterface::GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
// const Rvector& OVtoOFI::SetRvectorParameter(const Integer id, const Rvector& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Rvector parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
const Rvector& OVtoOFI::SetRvectorParameter(const Integer id, const Rvector& value)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetRvectorParameter(id - FALL_THROUGH_OFFSET, value);

    if (id == VIEWPOINT_VECTOR)
    {
        viewPoint.Set(value[0], value[1], value[2]);
        setupDefaultEye();
        return value;
    }
    
    return OpenFramesInterface::SetRvectorParameter(id, value);
}

//------------------------------------------------------------------------------
// const Rvector& OVtoOFI::GetRvectorParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a Rvector parameter
 *
 * @param label The name for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
const Rvector& OVtoOFI::GetRvectorParameter(const std::string& label) const
{
    Integer id = GetParameterID(label);
    return GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
// const Rvector& OVtoOFI::SetRvectorParameter(const std::string& label, const Rvector& value)
//------------------------------------------------------------------------------
/**
 *
 * Sets the value for a Rvector parameter
 * 
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
const Rvector& OVtoOFI::SetRvectorParameter(const std::string& label, const Rvector& value)
{
    Integer id = GetParameterID(label);
    return SetRvectorParameter(id, value);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetStringParameter(const Integer id, const std::string& value)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetStringParameter(id - FALL_THROUGH_OFFSET, value);

    switch (id)
    {
    case COORD_SYSTEM:
        return OpenFramesInterface::SetStringParameter(OpenFramesInterface::COORD_SYSTEM, value);
    case VIEWPOINT_REFERENCE :
        return true; // Leave as CoordinateFrame
    case VIEWPOINT_VECTOR:
    case VIEW_DIRECTION:
    case VIEW_UP_COORD_SYSTEM:
        return true; // Not used
    case VIEW_UP_AXIS:
        if (value == "X")
            defaultUp.Set(1.0, 0.0, 0.0);
        else if (value == "-X")
            defaultUp.Set(-1.0, 0.0, 0.0);
        else if (value == "Y")
            defaultUp.Set(0.0, 1.0, 0.0);
        else if (value == "-Y")
            defaultUp.Set(0.0, -1.0, 0.0);
        else if (value == "Z")
            defaultUp.Set(0.0, 0.0, 1.0);
        else if (value == "-Z")
            defaultUp.Set(0.0, 0.0, -1.0);
        else
            return false;

        viewUp = value;
        theView->SetRvectorParameter("DefaultUp", defaultUp);

        return true;
    default :
        return OpenFramesInterface::SetStringParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// std::string OVtoOFI::GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a string parameter
 *
 * @param id The ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string OVtoOFI::GetStringParameter(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetStringParameter(id - FALL_THROUGH_OFFSET);

    switch(id)
    {
    case COORD_SYSTEM:
        return OpenFramesInterface::GetStringParameter(OpenFramesInterface::COORD_SYSTEM);
    case VIEWPOINT_REFERENCE:
    case VIEWPOINT_VECTOR:
    case VIEW_DIRECTION:
    case VIEW_UP_COORD_SYSTEM:
        return ""; // Not used
    case VIEW_UP_AXIS:
        return viewUp;
    default :
        return OpenFramesInterface::GetStringParameter(id);
    }
}

//------------------------------------------------------------------------------
// std::string OVtoOFI::GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a string parameter
 *
 * @param label The name for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string OVtoOFI::GetStringParameter(const std::string& label) const
{
    Integer id = GetParameterID(label);
    return GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetStringParameter(const std::string& label, const char* value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetStringParameter(const std::string& label, const char* value)
{
    if (label == "View")
        return OpenFramesInterface::SetStringParameter(OpenFramesInterface::VIEW, value);

    if (label == "Add")
    {
        addCount++;

        return OpenFramesInterface::SetStringParameter(OpenFramesInterface::ADD, value) && setupGrid() && setupLabels();
    }

    Integer id = GetParameterID(label);
    return SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetStringParameter(const std::string& label, const std::string& value)
{
    if (label == "View")
        return OpenFramesInterface::SetStringParameter(OpenFramesInterface::VIEW, value);

    if (label == "Add")
    {
        addCount++;

        return OpenFramesInterface::SetStringParameter(OpenFramesInterface::ADD, value) && setupGrid() && setupLabels();
    }

    Integer id = GetParameterID(label);
    return SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetStringParameter(const Integer id, const std::string& value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 * @param index Index for the object's location
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetStringParameter(const Integer id, const std::string& value, const Integer index)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetStringParameter(id - FALL_THROUGH_OFFSET, value, index);

    if (id == ADD)
    {
        addCount = max(addCount, index + 1);

        return OpenFramesInterface::SetStringParameter(OpenFramesInterface::ADD, value) && setupGrid() && setupLabels();
    }

    if (id == VIEWPOINT_VECTOR)
        return true;

    return OpenFramesInterface::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetStringParameter(const std::string& label, const std::string& value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 * @param index Index for the object's location
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetStringParameter(const std::string& label, const std::string& value, const Integer index)
{
    Integer id = GetParameterID(label);
    return SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::GetStringArrayParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * Gets the value for a StringArray parameter
 *
 * @param id The ID for the parameter
 *
 *  @return the value of the parameter
 */
 //------------------------------------------------------------------------------
const StringArray& OVtoOFI::GetStringArrayParameter(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetStringArrayParameter(id - FALL_THROUGH_OFFSET);

    return OpenFramesInterface::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetStringArrayParameter(const Integer id, const StringArray& stringArray)
//------------------------------------------------------------------------------
/**
 * Sets the value for a OnOff parameter
 *
 * @param id The ID for the parameter
 * @param stringArray The value for the parameter
 *
 * @return true if successful
 */
 //------------------------------------------------------------------------------
bool OVtoOFI::SetStringArrayParameter(const Integer id, const StringArray& stringArray)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetStringArrayParameter(id - FALL_THROUGH_OFFSET, stringArray);

    return OpenFramesInterface::SetStringArrayParameter(id, stringArray);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetOnOffParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a OnOff parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetOnOffParameter(const Integer id, const std::string& value)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetOnOffParameter(id - FALL_THROUGH_OFFSET, value);

    switch (id)
    {
    case WIRE_FRAME:
        return true; // Not used
    case GRID:
        drawGrid = (value == "On");
        return setupGrid();
    case SUN_LINE:
        return true; // Not used
    case USE_INITIAL_VIEW:
        return true; // Not used
    case ENABLE_CONSTELLATIONS:
        return true; // Not used
    default:
        return OpenFramesInterface::SetOnOffParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetOnOffParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a OnOff parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetOnOffParameter(const std::string& label, const std::string& value)
{
    Integer id = GetParameterID(label);
    return SetOnOffParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string OVtoOFI::GetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a OnOff parameter
 *
 * @param id The ID for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
std::string OVtoOFI::GetOnOffParameter(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetOnOffParameter(id - FALL_THROUGH_OFFSET);

    switch (id)
    {
    case WIRE_FRAME:
        return "Off"; // Not used
    case GRID:
        return drawGrid ? "On" : "Off";
    case SUN_LINE:
        return "Off"; // Not used
    case USE_INITIAL_VIEW:
        return "Off"; // Not used
    case ENABLE_CONSTELLATIONS:
        return "Off"; // Not used
    default:
        return OpenFramesInterface::GetOnOffParameter(id);
    }
}

//------------------------------------------------------------------------------
// std::string OVtoOFI::GetOnOffParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a OnOff parameter
 *
 * @param label The name for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string OVtoOFI::GetOnOffParameter(const std::string& label) const
{
    Integer id = GetParameterID(label);
    return GetOnOffParameter(id);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::GetBooleanParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a bool parameter
 *
 * @param label The name for the parameter
 *
 * @return the value
 */
//------------------------------------------------------------------------------
bool OVtoOFI::GetBooleanParameter(const std::string& label) const
{
    if (label == "ShowPlot")
        return OpenFramesInterface::GetBooleanParameter(OpenFramesInterface::SHOW_PLOT);

    Integer id = GetParameterID(label);
    return GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::GetBooleanParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a bool parameter
 *
 * @param label The name for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
bool OVtoOFI::GetBooleanParameter(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetBooleanParameter(id - FALL_THROUGH_OFFSET);

    switch(id)
    {
    case SHOW_LABELS:
        return showLabels;
    default :
        return OpenFramesInterface::GetBooleanParameter(id);
    }
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a bool parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetBooleanParameter(const Integer id, const bool value)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetBooleanParameter(id - FALL_THROUGH_OFFSET, value);

    switch (id)
    {
    case SHOW_LABELS:
        showLabels = value;
        return setupLabels();
    default:
        return OpenFramesInterface::SetBooleanParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetBooleanParameter(const std::string& label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a bool parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetBooleanParameter(const std::string& label, const bool value)
{
    Integer id = GetParameterID(label);
    return SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
// const BooleanArray& OVtoOFI::GetBooleanArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a bool array parameter
 *
 * @param id The ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
const BooleanArray& OVtoOFI::GetBooleanArrayParameter(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetBooleanArrayParameter(id - FALL_THROUGH_OFFSET);

    return OpenFramesInterface::GetBooleanArrayParameter(id);
}

//------------------------------------------------------------------------------
// const BooleanArray& OVtoOFI::GetBooleanArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a bool array parameter
 *
 * @param label The name for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
const BooleanArray& OVtoOFI::GetBooleanArrayParameter(const std::string& label) const
{
    Integer id = GetParameterID(label);
    return GetBooleanArrayParameter(id);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetBooleanArrayParameter(const Integer id, const BooleanArray& valueArray)
//------------------------------------------------------------------------------
/**
 * Sets the value for a bool array parameter
 *
 * @param id The ID for the parameter
 * @param valueArray The value for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetBooleanArrayParameter(const Integer id, const BooleanArray& valueArray)
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::SetBooleanArrayParameter(id - FALL_THROUGH_OFFSET, valueArray);

    return OpenFramesInterface::SetBooleanArrayParameter(id, valueArray);
}

//------------------------------------------------------------------------------
// bool OVtoOFI::SetBooleanArrayParameter(const std::string& label, const BooleanArray& valueArray)
//------------------------------------------------------------------------------
/**
 * Sets the value for a bool array parameter
 *
 * @param label The name for the parameter
 * @param valueArray The value for the parameter
 *
 * @return true if successful
 */
//------------------------------------------------------------------------------
bool OVtoOFI::SetBooleanArrayParameter(const std::string& label, const BooleanArray& valueArray)
{
    Integer id = GetParameterID(label);
    return SetBooleanArrayParameter(id, valueArray);
}

//------------------------------------------------------------------------------
// UnsignedInt OVtoOFI::GetPropertyObjectType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a object parameter
 *
 * @param id The ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
UnsignedInt OVtoOFI::GetPropertyObjectType(const Integer id) const
{
    if (id >= FALL_THROUGH_OFFSET)
        return OpenFramesInterface::GetPropertyObjectType(id - FALL_THROUGH_OFFSET);

    return OpenFramesInterface::GetPropertyObjectType(id);
}

//------------------------------------------------------------------------------
// const std::string& OVtoOFI::GetGeneratingString(Gmat::WriteMode mode,
//       const std::string& prefix, const std::string& useName)
//------------------------------------------------------------------------------
/**
 * Script writing interface: calls the OFI writer
 *
 * @param mode Configuration for the returned daya
 * @param prefix: Text to place at the start of each line
 * @param useName Override for the object type (is needed)
 *
 * @return The scripting for this object
 */
//------------------------------------------------------------------------------
const std::string& OVtoOFI::GetGeneratingString(Gmat::WriteMode mode,
      const std::string& prefix, const std::string& useName)
{
   return OpenFramesInterface::GetGeneratingString(mode, prefix);
}

//---------------------------------------------------------------------------
//  bool setupGrid()
//---------------------------------------------------------------------------
/**
 * Set the DRAG_GRID parameter
 *
 * @return true iff sucessful
 */
//---------------------------------------------------------------------------
bool OVtoOFI::setupGrid()
{
    BooleanArray valueArray(addCount, drawGrid);
    return OpenFramesInterface::SetBooleanArrayParameter(OpenFramesInterface::DRAW_GRID, valueArray);
}

//---------------------------------------------------------------------------
//  bool setupLabels()
//---------------------------------------------------------------------------
/**
 * Set the DRAW_LABEL parameter
 *
 * @return true iff sucessful
 */
//---------------------------------------------------------------------------
bool OVtoOFI::setupLabels()
{
    BooleanArray valueArray(addCount, showLabels);
    return OpenFramesInterface::SetBooleanArrayParameter(OpenFramesInterface::DRAW_LABEL, valueArray);
}

//---------------------------------------------------------------------------
//  Rvector setupDefaultEye()
//---------------------------------------------------------------------------
/**
 * Set the DefaultEye parameter
 *
 * @return passed value
 */
//---------------------------------------------------------------------------
Rvector OVtoOFI::setupDefaultEye()
{
    return theView->SetRvectorParameter("DefaultEye", viewScaleFactor * viewPoint);
}
