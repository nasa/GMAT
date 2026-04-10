//$Id$
//------------------------------------------------------------------------------
//                                  ItrfCoefficientsFile
//------------------------------------------------------------------------------
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/01/31
//
/**
 * Definition of the ItrfCoefficientsFile class.  This is the code that reads 
 * the nutation and planetary coefficients from the file.
 *
 * The source of the nutation and planetary coefficient data is 
 * http://www.celestrak.com/software/vallado-sw.asp 
 *
 */
//------------------------------------------------------------------------------

#ifndef ItrfCoefficientsFile_hpp
#define ItrfCoefficientsFile_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"

namespace GmatItrf
{
   enum NutationTerms
   {
      NUTATION_1980,
      NUTATION_1996,
      NUTATION_2000
   };
   enum PlanetaryTerms
   {
      PLANETARY_1980,
      PLANETARY_1996     // CURRENTLY not used wcs 2013.03.04
      // not available for 2000?
   };
};

class GMAT_API ItrfCoefficientsFile
{
public:

   // default constructor
   ItrfCoefficientsFile(const std::string &nutFileName = "NUTATION.DAT", 
               const std::string planFileName = "NUT85.DAT",      // CURRENTLY not used wcs 2013.03.04
               GmatItrf::NutationTerms  nutTerms  = GmatItrf::NUTATION_1980,
               GmatItrf::PlanetaryTerms planTerms = GmatItrf::PLANETARY_1980);
   // copy constructor
   ItrfCoefficientsFile(const ItrfCoefficientsFile &itrfF);
   // operator = 
   const ItrfCoefficientsFile& operator=(const ItrfCoefficientsFile &itrfF);
   // destructor
   virtual ~ItrfCoefficientsFile();
   
   
   // initializes the ItrfCoefficientsFile
   virtual void Initialize();

   // method to return the name of the coefficients file
   virtual GmatItrf::NutationTerms  GetNutationTermsSource() const;
   virtual GmatItrf::PlanetaryTerms GetPlanetaryTermsSource() const;
   virtual std::string GetNutationFileName() const;
   virtual std::string GetPlanetaryFileName() const;
      
   virtual Integer GetNumberOfNutationTerms();
   virtual Integer GetNumberOfPlanetaryTerms();
   virtual bool GetNutationTerms(std::vector<IntegerArray> &a5, Rvector &Aval, 
                                 Rvector &Bval, Rvector &Cval, Rvector &Dval, 
                                 Rvector &Eval, Rvector &Fval);
   virtual bool GetPlanetaryTerms(std::vector<IntegerArray> &ap10, 
                                  Rvector &Apval, Rvector &Bpval, 
                                  Rvector &Cpval, Rvector &Dpval);
   
  
protected:

   // additional protected data
   // (NOTE - static const strings are initialized in source file)
   static const Integer MAX_1980_NUT_TERMS;//         = 106;
   static const Real    MULT_1980_NUT;//              = 1.0e-04;
   static const std::string FIRST_NUT_PHRASE_1980;
   
   static const Integer MAX_1996_NUT_TERMS;//         = 263;
   static const Real    MULT_1996_NUT;//              = 1.0e-06;
   static const std::string FIRST_NUT_PHRASE_1996;
   
   static const Integer MAX_2000_NUT_TERMS;//         = 106;
   static const Real    MULT_2000_NUT;//              = 1.0e-04;
   static const std::string FIRST_NUT_PHRASE_2000;
   
   static const Integer MAX_1980_PLANET_TERMS;//      = 85;
   static const Real    MULT_1980_PLANET;//           = 1.0e-04;
   static const std::string FIRST_PLAN_PHRASE_1980;
   
   static const Integer MAX_1996_PLANET_TERMS;//      = 112;
   static const Real    MULT_1996_PLANET;//           = 1.0e-04;
   static const std::string FIRST_PLAN_PHRASE_1996;
   
   static const Integer MAX_2000_PLANET_TERMS;// = 112;            // ????
   static const Real    MULT_2000_PLANET;//      = 1.0e-04;        // ????
   static const std::string FIRST_PLAN_PHRASE_2000; // ????
   
   
   /// number of terms in nutation longitude series <=MAX_NUT_TERMS
   Integer    nut;  
   /// number of terms in nutation planetary series <=MAX_PLANET_TERMS
   Integer    nutpl;
   
   /// multipliers for the nutation and planetary coefficients
   Real       nutMult;
   Real       planMult;
   
   std::string firstNutPhrase;
   std::string firstPlanPhrase;
   
   GmatItrf::NutationTerms  nutation;
   GmatItrf::PlanetaryTerms planetary;

   std::string nutationFileName;
   std::string planetaryFileName;
   bool filesAreInitialized;

   std::vector<IntegerArray> a;
   /// reduction terms
   Rvector*    A;
   Rvector*    B;
   Rvector*    C;
   Rvector*    D;
   Rvector*    E;
   Rvector*    F;
   /// planetary terms (F0-F9) multipliers
   std::vector<IntegerArray> ap;
   Rvector*    Ap;
   Rvector*    Bp;
   Rvector*    Cp;
   Rvector*    Dp;

   bool InitializeArrays(GmatItrf::NutationTerms nutT,
                         GmatItrf::PlanetaryTerms planT);
   
   bool IsBlank(const char* aLine);
   
};
#endif // ItrfCoefficientsFile_hpp
