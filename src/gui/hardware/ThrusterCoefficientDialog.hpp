//$Id$
//------------------------------------------------------------------------------
//                              ThrusterCoefficientDialog
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
// Author: Waka Waktola
// Created: 2005/01/13
//
/**
 * Declares ThrusterCoefficientDialog class. This class shows dialog window where
 * thruster coefficients can be modified.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ThrusterCoefficientDialog_hpp
#define ThrusterCoefficientDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class ThrusterCoefficientDialog : public GmatDialog
{
public:
   ThrusterCoefficientDialog(wxWindow *parent, wxWindowID id, 
           const wxString &title, GmatBase *obj, Integer numCoefs,
           const RealArray &coefs1, const RealArray &coefs2);

   RealArray&      GetCoefs1Values()  { return coefs1Values;    }
   RealArray&      GetCoefs2Values()  { return coefs2Values;    }

   bool            AreCoefs1Saved()  { return coefs1Modified; }
   bool            AreCoefs2Saved()  { return coefs2Modified; }
   
private:
   
   GmatBase    *theObject;
   Integer     coefsCount;
   StringArray coefs1Names;
   StringArray coefs2Names;
   RealArray   coefs1Values;
   RealArray   coefs2Values;

   bool        isElectric;

   bool        coefs1Modified;
   bool        coefs2Modified;

   wxGrid      *coefGrid1;
   wxGrid      *coefGrid2;

   wxNotebook  *coefNotebook;
   
   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_GRID1 = 30300,
      ID_GRID2,
      ID_NOTEBOOK,
   };
};

#endif
