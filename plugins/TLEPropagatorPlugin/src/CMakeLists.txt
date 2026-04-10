# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT TLE Propagator Plugin.
#
# Author: Darrel Conway
# Created: 2019/09/26
#
# Copyright (c) 2019, Thinking Systems, Inc.
# All rights reserved
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Edited 12/13/2021 by Jairus Elarbee to add Change TLEPropagator to SPICEPropagator to use SPICE v66 updated TLE algorithm


MESSAGE("==============================")
MESSAGE("GMAT TLE Propagator Plugin setup " ${VERSION}) 

SET(TargetName TLEPropagator)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DTLE_PROP_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin propagator)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/TlePropFactory.cpp
    plugin/GmatPluginFunctions.cpp
    propagator/TLEData.cpp
    propagator/SPICEPropagator.cpp
    propagator/TLEReader.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins/thinksys)
