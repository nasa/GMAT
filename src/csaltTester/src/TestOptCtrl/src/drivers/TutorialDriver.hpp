//------------------------------------------------------------------------------
//                           TutorialDriver
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
//
// Author: Noble Hatten
// Created: 2019/07/03
/**
 * The Tutorial test case driver
 */
 //------------------------------------------------------------------------------

#ifndef TutorialDriver_hpp
#define TutorialDriver_hpp

#include "CsaltTestDriver.hpp"

/**
 * Driver for the Tutorial problem
 */
class TutorialDriver : public CsaltTestDriver
{
public:
	TutorialDriver();
	virtual ~TutorialDriver();
	void StandAloneTest(); // method suitable for a standalone test like what is written in the User Guide Tutorial

protected:
	virtual void SetPointPathAndProperties();
	virtual void SetupPhases();
};

#endif /* TutorialDriver_hpp */
