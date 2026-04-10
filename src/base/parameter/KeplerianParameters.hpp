//$Id$
//------------------------------------------------------------------------------
//                             File: KeplerianParameters.hpp
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
// Created: 2004/03/12
//
/**
 * Declares Keplerian related parameter classes.
 *   KepSMA, KepEcc, KepInc, KepAOP, KepRAAN, KepTA, KepMA, KepEA, KepHA,
 *   KepMM, KepElem, ModKepRadApo, ModKepRadPer, ModKepElem
 */
//------------------------------------------------------------------------------
#ifndef KeplerianParameters_hpp
#define KeplerianParameters_hpp

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

//==============================================================================
//                              KepSMA
//==============================================================================
/**
 * Declares Keplerian Semimajor Axis class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepSMA : public OrbitReal
{
public:

   KepSMA(const std::string &name = "", GmatBase *obj = NULL);
   KepSMA(const KepSMA &copy);
   const KepSMA& operator=(const KepSMA &right);
   virtual ~KepSMA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              KepEcc
//==============================================================================
/**
 * Declares Keplerian Eccentricity class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepEcc : public OrbitReal
{
public:

   KepEcc(const std::string &name = "", GmatBase *obj = NULL);
   KepEcc(const KepEcc &param);
   const KepEcc& operator=(const KepEcc &right);
   virtual ~KepEcc();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepInc
//==============================================================================
/**
 * Declares Keplerian Inclinatin class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepInc : public OrbitReal
{
public:

   KepInc(const std::string &name = "", GmatBase *obj = NULL);
   KepInc(const KepInc &copy);
   const KepInc& operator=(const KepInc &right);
   virtual ~KepInc();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              KepAOP
//==============================================================================
/**
 * Declares Keplerian Argument of Periapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepAOP : public OrbitReal
{
public:

   KepAOP(const std::string &name = "", GmatBase *obj = NULL);
   KepAOP(const KepAOP &copy);
   const KepAOP& operator=(const KepAOP &right);
   virtual ~KepAOP();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              KepRAAN
//==============================================================================
/**
 * Declares Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepRAAN : public OrbitReal
{
public:

   KepRAAN(const std::string &name = "", GmatBase *obj = NULL);
   KepRAAN(const KepRAAN &copy);
   const KepRAAN& operator=(const KepRAAN &right);
   virtual ~KepRAAN();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepRADN
//==============================================================================
/**
 * Declares Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepRADN : public OrbitReal
{
public:

   KepRADN(const std::string &name = "", GmatBase *obj = NULL);
   KepRADN(const KepRADN &copy);
   const KepRADN& operator=(const KepRADN &right);
   virtual ~KepRADN();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepTA
//==============================================================================
/**
 * Declares Keplerian True Anomaly class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepTA : public OrbitReal
{
public:

   KepTA(const std::string &name = "", GmatBase *obj = NULL);
   KepTA(const KepTA &copy);
   const KepTA& operator=(const KepTA &right);
   virtual ~KepTA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepMA
//==============================================================================
/**
 * Declares Keplerian Mean Anomaly class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepMA : public OrbitReal
{
public:

   KepMA(const std::string &name = "", GmatBase *obj = NULL);
   KepMA(const KepMA &copy);
   const KepMA& operator=(const KepMA &right);
   virtual ~KepMA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepEA
//==============================================================================
/**
 * Declares Keplerian Mean Anomaly class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepEA : public OrbitReal
{
public:

   KepEA(const std::string &name = "", GmatBase *obj = NULL);
   KepEA(const KepEA &copy);
   const KepEA& operator=(const KepEA &right);
   virtual ~KepEA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepHA
//==============================================================================
/**
 * Declares Keplerian Mean Anomaly class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepHA : public OrbitReal
{
public:

   KepHA(const std::string &name = "", GmatBase *obj = NULL);
   KepHA(const KepHA &copy);
   const KepHA& operator=(const KepHA &right);
   virtual ~KepHA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepMM
//==============================================================================
/**
 * Declares Keplerian Mean Motion class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepMM : public OrbitReal
{
public:

   KepMM(const std::string &name = "", GmatBase *obj = NULL);
   KepMM(const KepMM &copy);
   const KepMM& operator=(const KepMM &right);
   virtual ~KepMM();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              KepElem
//==============================================================================
/**
 * Declares Keplerian Elements class.
 *   6 elements: KepSMA, KepEcc, KepInc, KepRAAN, KepAOP, KepTA
 */
//------------------------------------------------------------------------------

class GMAT_API KepElem : public OrbitRvec6
{
public:

   KepElem(const std::string &name = "", GmatBase *obj = NULL);
   KepElem(const KepElem &copy);
   const KepElem& operator=(const KepElem &right);
   virtual ~KepElem();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
    
};


//==============================================================================
//                              ModKepRadApo
//==============================================================================
/**
 * Declares ModKepRadApo class.
 */
//------------------------------------------------------------------------------

class GMAT_API ModKepRadApo : public OrbitReal
{
public:

   ModKepRadApo(const std::string &name = "", GmatBase *obj = NULL);
   ModKepRadApo(const ModKepRadApo &copy);
   const ModKepRadApo& operator=(const ModKepRadApo &right);
   virtual ~ModKepRadApo();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              ModKepRadPer
//==============================================================================
/**
 * Declares ModKepRadPer class.
 */
//------------------------------------------------------------------------------

class GMAT_API ModKepRadPer : public OrbitReal
{
public:

   ModKepRadPer(const std::string &name = "", GmatBase *obj = NULL);
   ModKepRadPer(const ModKepRadPer &copy);
   const ModKepRadPer& operator=(const ModKepRadPer &right);
   virtual ~ModKepRadPer();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//loj: 2/16/05 Added
//==============================================================================
//                              ModKepElem
//==============================================================================
/**
 * Declares Keplerian Elements class.
 *   6 elements: RadPeriapais, ModKepRadApo, KepInc, KepRAAN, KepAOP, KepTA
 */
//------------------------------------------------------------------------------

class GMAT_API ModKepElem : public OrbitRvec6
{
public:

   ModKepElem(const std::string &name = "", GmatBase *obj = NULL);
   ModKepElem(const ModKepElem &copy);
   const ModKepElem& operator=(const ModKepElem &right);
   virtual ~ModKepElem();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
    
};

#endif // KeplerianParameters_hpp
