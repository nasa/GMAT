//------------------------------------------------------------------------------
//                           TestOptCtrl
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
// Created: Jul 11, 2017
/**
 * Header file defining the CSALT test system driver functions
 */
//------------------------------------------------------------------------------


#ifndef CPLUSPLUS_TEST_TESTOPTCTRL_SRC_TESTOPTCTRL_HPP_
#define CPLUSPLUS_TEST_TESTOPTCTRL_SRC_TESTOPTCTRL_HPP_

#include "drivers/CsaltTestDriver.hpp"


int main(int argc, char *argv[]);
bool RunTest(CsaltTestDriver **driver, const std::string &outputPath = "");


#endif /* CPLUSPLUS_TEST_TESTOPTCTRL_SRC_TESTOPTCTRL_HPP_ */
