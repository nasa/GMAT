//------------------------------------------------------------------------------
//                              TestFunction
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
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/04/29
//
/**
 * File Header of Unit test program 
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "Spacecraft.hpp"
#include "TimeConverter.hpp"
#include "UtcDate.hpp"
#include "DateUtil.hpp"
 
// Declare functions 

void printEpoch(const Spacecraft *sc);

void printState(const std::string &title, const Real *state);

void printState(const std::string &title, const Rvector6 state);

void printState(const Spacecraft *s);

void printState(const std::string &title, const Spacecraft *s);
 
void printTime(const Real time);

void printTime(const Real time,const std::string &fromTime,
               const std::string &toTime);

Real julianDate(const Integer yr, const Integer mon, const Integer day, 
                const Integer hour, const Integer min, const Real sec);

std::string jdToGregorian(const Real jd);

std::string mjdToGregorian(const Real mjd);
