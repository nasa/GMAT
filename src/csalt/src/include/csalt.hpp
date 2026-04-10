//------------------------------------------------------------------------------
//                           csalt
//------------------------------------------------------------------------------
// GMAT:  General Mission Analysis Tool
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Apr 10, 2017
/**
 * Include file for systems that build wth csalt
 */
//------------------------------------------------------------------------------


#ifndef csalt_hpp
#define csalt_hpp

// Boost and (maybe) SNOPT
#include "boost/config.hpp"

// GMAT includes
#include "utildefs.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "FileManager.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"
#include "MessageInterface.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "Rmatrix33.hpp"
#include "TimeTypes.hpp"

// CSALT
#include "LowThrustException.hpp"   // CSALT error handling

#include "ImplicitRKPhase.hpp"
#include "Phase.hpp"
#include "RadauPhase.hpp"
#include "Trajectory.hpp"
#include "UserPathFunction.hpp"
#include "UserPointFunction.hpp"


// Pieces not currently needed, but found in the include lists for test cases
//#include "BoundData.hpp"
//#include "DecisionVector.hpp"
//#include "DecVecTypeBetts.hpp"
//#include "DummyPathFunction.hpp"
//#include "DummyPathFunction2.hpp"
//#include "FunctionOutputData.hpp"
//#include "GuessGenerator.hpp"
//#include "JacobianData.hpp"
//#include "PathFunctionContainer.hpp"
//#include "PointFunctionContainer.hpp"
//#include "ProblemCharacteristics.hpp"
//#include "SparseMatrixLibraryHeader.hpp"
//#include "UserFunctionManager.hpp"
//#include "UserFunctionProperties.hpp"
//#include "UserPathFunctionManager.hpp"
//#include "UserPointFunctionManager.hpp"


#endif /* csalt_hpp */
