//$Id$
//------------------------------------------------------------------------------
//                             SpaceWeatherParameters
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Actalent under the FDSS-III contract
// 
//
// Author: Allen Fu
// Created: 2026/09/15
//

#include "SpaceWeatherParameters.hpp"

//------------------------------------------------------------------------------
// F107(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
F107::F107(const std::string &name, GmatBase *obj) :
   OdeReal(name, "F107", obj, "F10.7 Solar Flux", "Watts/m^2/Hz",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// F107(const F107 &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
F107::F107(const F107 &copy)
   : OdeReal(copy)
{
}

//------------------------------------------------------------------------------
// F107& operator=(const F107 &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const F107& F107::operator=(const F107 &right)
{
   if (this != &right)
      OdeReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~F107()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
F107::~F107()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool F107::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("F107");

   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* F107::Clone() const
{
   return new F107(*this);
}


//------------------------------------------------------------------------------
// F107Avg(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
F107Avg::F107Avg(const std::string &name, GmatBase *obj) :
   OdeReal(name, "F107Avg", obj, "F10.7 Centered Running Average Solar Flux",
             "Watts/m^2/Hz", Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// F107Avg(const F107Avg &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
F107Avg::F107Avg(const F107Avg &copy)
   : OdeReal(copy)
{
}

//------------------------------------------------------------------------------
// F107Avg& operator=(const F107Avg &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const F107Avg& F107Avg::operator=(const F107Avg &right)
{
   if (this != &right)
      OdeReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~F107Avg()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
F107Avg::~F107Avg()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool F107Avg::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("F107Avg");

   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* F107Avg::Clone() const
{
   return new F107Avg(*this);
}


//------------------------------------------------------------------------------
// Kp(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
Kp::Kp(const std::string &name, GmatBase *obj) :
   OdeReal(name, "Kp", obj, "Planetary K Index", "",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// Kp(const Kp &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Kp::Kp(const Kp &copy)
   : OdeReal(copy)
{
}

//------------------------------------------------------------------------------
// Kp& operator=(const Kp &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const Kp& Kp::operator=(const Kp &right)
{
   if (this != &right)
      OdeReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~Kp()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Kp::~Kp()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool Kp::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("Kp");

   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* Kp::Clone() const
{
   return new Kp(*this);
}


//------------------------------------------------------------------------------
// Ap(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
Ap::Ap(const std::string &name, GmatBase *obj) :
   OdeReal(name, "Ap", obj, "Planetary A Index", "",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// Ap(const Ap &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Ap::Ap(const Ap &copy)
   : OdeReal(copy)
{
}

//------------------------------------------------------------------------------
// Ap& operator=(const Ap &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const Ap& Ap::operator=(const Ap &right)
{
   if (this != &right)
      OdeReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~Ap()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Ap::~Ap()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool Ap::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("Ap");

   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* Ap::Clone() const
{
   return new Ap(*this);
}


//------------------------------------------------------------------------------
// ApAvg(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
ApAvg::ApAvg(const std::string &name, GmatBase *obj) :
   OdeReal(name, "ApAvg", obj, "Average Planetary A Index", "",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// ApAvg(const ApAvg &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ApAvg::ApAvg(const ApAvg &copy)
   : OdeReal(copy)
{
}

//------------------------------------------------------------------------------
// ApAvg& operator=(const ApAvg &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ApAvg& ApAvg::operator=(const ApAvg &right)
{
   if (this != &right)
      OdeReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~ApAvg()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ApAvg::~ApAvg()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool ApAvg::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("ApAvg");

   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* ApAvg::Clone() const
{
   return new ApAvg(*this);
}
