//------------------------------------------------------------------------------
//                                ThrustSegmentReal
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Peter Candell
// Created: 2022/09/06
//
/**
 * Declares ThrustSegmentReal class which provides base class for environment realated
 * Real Parameters
 */
 //------------------------------------------------------------------------------
#ifndef ThrustSegmentParameters_hpp
#define ThrustSegmentParameters_hpp

#include "ThrustFileDefs.hpp"
#include "ThrustSegmentReal.hpp"
#include "ThrustSegmentRvector.hpp"


class THRUSTFILE_API ThrustSegmentScaleFactor : public ThrustSegmentReal
{
   public:

      ThrustSegmentScaleFactor(const std::string &name = "", GmatBase *obj = NULL);
      ThrustSegmentScaleFactor(const ThrustSegmentScaleFactor &copy);
      ThrustSegmentScaleFactor& operator=(const ThrustSegmentScaleFactor &right);
      virtual ~ThrustSegmentScaleFactor();

      // methods inherited from Parameter
      virtual bool Evaluate();
      virtual void SetReal(Real val);

      // methods inherited from GmatBase
      virtual GmatBase* Clone(void) const;

   protected:
};

class THRUSTFILE_API TSFSigma : public ThrustSegmentReal
{
public:

   TSFSigma(const std::string &name = "", GmatBase *obj = NULL);
   TSFSigma(const TSFSigma &copy);
   TSFSigma& operator=(const TSFSigma &right);
   virtual ~TSFSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};


class THRUSTFILE_API TSFEpsilon : public ThrustSegmentReal
{
public:

   TSFEpsilon(const std::string &name = "", GmatBase *obj = NULL);
   TSFEpsilon(const TSFEpsilon &copy);
   TSFEpsilon& operator=(const TSFEpsilon &right);
   virtual ~TSFEpsilon();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};


class THRUSTFILE_API StartEpoch : public ThrustSegmentReal
{
public:

   StartEpoch(const std::string &name = "", GmatBase *obj = NULL);
   StartEpoch(const StartEpoch &copy);
   StartEpoch& operator=(const StartEpoch &right);
   virtual ~StartEpoch();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};


class THRUSTFILE_API EndEpoch : public ThrustSegmentReal
{
public:

   EndEpoch(const std::string &name = "", GmatBase *obj = NULL);
   EndEpoch(const EndEpoch &copy);
   EndEpoch& operator=(const EndEpoch &right);
   virtual ~EndEpoch();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};


class THRUSTFILE_API MassFlowScaleFactor : public ThrustSegmentReal
{
public:

   MassFlowScaleFactor(const std::string &name = "", GmatBase *obj = NULL);
   MassFlowScaleFactor(const MassFlowScaleFactor &copy);
   MassFlowScaleFactor& operator=(const MassFlowScaleFactor &right);
   virtual ~MassFlowScaleFactor();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};


class THRUSTFILE_API ThrustAngleConstraint : public ThrustSegmentRvector
{
public:

   ThrustAngleConstraint(const std::string &name = "", GmatBase *obj = NULL);
   ThrustAngleConstraint(const ThrustAngleConstraint &copy);
   ThrustAngleConstraint& operator=(const ThrustAngleConstraint &right);
   virtual ~ThrustAngleConstraint();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetRvector(Rvector& val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};

class THRUSTFILE_API ThrustAngle1 : public ThrustSegmentRvector
{
public:

   ThrustAngle1(const std::string &name = "", GmatBase *obj = NULL);
   ThrustAngle1(const ThrustAngle1 &copy);
   ThrustAngle1& operator=(const ThrustAngle1 &right);
   virtual ~ThrustAngle1();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetRvector(Rvector& val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};


class THRUSTFILE_API ThrustAngle2 : public ThrustSegmentRvector
{
public:

   ThrustAngle2(const std::string &name = "", GmatBase *obj = NULL);
   ThrustAngle2(const ThrustAngle2 &copy);
   ThrustAngle2& operator=(const ThrustAngle2 &right);
   virtual ~ThrustAngle2();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetRvector(Rvector& val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};

class THRUSTFILE_API ThrustAngle1Sigma : public ThrustSegmentRvector
{
public:

   ThrustAngle1Sigma(const std::string &name = "", GmatBase *obj = NULL);
   ThrustAngle1Sigma(const ThrustAngle1Sigma &copy);
   ThrustAngle1Sigma& operator=(const ThrustAngle1Sigma &right);
   virtual ~ThrustAngle1Sigma();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetRvector(Rvector& val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};


class THRUSTFILE_API ThrustAngle2Sigma : public ThrustSegmentRvector
{
public:

   ThrustAngle2Sigma(const std::string &name = "", GmatBase *obj = NULL);
   ThrustAngle2Sigma(const ThrustAngle2Sigma &copy);
   ThrustAngle2Sigma& operator=(const ThrustAngle2Sigma &right);
   virtual ~ThrustAngle2Sigma();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetRvector(Rvector& val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
};


#endif