//$Id: SolarSailForce.hpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            SolarSailForce
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2008/07/03
//
/**
 * Declaration code for the SolarSailForce class, which extends the SRP model 
 * for solar sailing.
 * 
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------


#ifndef SolarSailForce_hpp
#define SolarSailForce_hpp

#include "SolarRadiationPressure.hpp"

class SolarSailForce : public SolarRadiationPressure
{
public:
	SolarSailForce(const std::string &name = "");
	virtual ~SolarSailForce();
	SolarSailForce(const SolarSailForce& ssf);
	SolarSailForce& operator=(const SolarSailForce& ssf);

	virtual bool IsUserForce();
	
	virtual bool Initialize();
   virtual bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1);

   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   
protected:
   /// Flag used when completing initialization at first call to GetDerivatives 
   bool firedOnce;
   /// The number of satellites in the propagation
   Real satCount;
   /// Vector normal to the surface in sunlight
   Real* norm;
   
   void CheckParameters();
};

#endif /* SolarSailForce_hpp*/
