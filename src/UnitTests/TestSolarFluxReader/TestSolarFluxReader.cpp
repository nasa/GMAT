//------------------------------------------------------------------------------
//                             TestSolarFluxReader
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
// Author: Peter Cotroneo
// Created: 2026/09/05
//
/**
 * Unit test for SolarFluxReader flux-selection, covering GitHub issue #4 /
 * GMT-8612: PrepareKpData selected the F10.7 81-day centered average
 * (obsCtrF107a) from the previous historic row (f107index-1) instead of the
 * detected-day row (f107index), diverging from PrepareApData for the same epoch.
 *
 * Invariant under test: for a given epoch, the Kp path (used by
 * JacchiaRobertsAtmosphere) and the Ap path (used by the base AtmosphereModel /
 * MSISE90) must select the SAME F10.7 and F10.7a from the historic CSSI file.
 * Only the geomagnetic arrays (kp/ap) legitimately differ between them.
 *
 * All checks use the public SolarFluxReader API only.  The historic space
 * weather file is taken from argv[1], or defaults to the bundled
 * application/data/atmosphere/earth/SpaceWeather-All-v1.2.txt.
 */
//------------------------------------------------------------------------------

#include "SolarFluxReader.hpp"
#include "TestOutput.hpp"
#include "BaseException.hpp"

#include <string>
#include <iostream>

//------------------------------------------------------------------------------
// void CheckRow(TestOutput &out, SolarFluxReader &reader, Real startEpoch,
//               Integer N, bool preEightAm)
//------------------------------------------------------------------------------
/**
 * Runs the flux-selection checks for historic row N.  At noon (preEightAm ==
 * false) the epoch is past the 8am F10.7 validity boundary, so the detected-day
 * index inside Prepare*Data equals N and matches the raw row from GetInputs.
 */
//------------------------------------------------------------------------------
void CheckRow(TestOutput &out, SolarFluxReader &reader, Real startEpoch,
              Integer N, bool preEightAm)
{
   const Real epoch = startEpoch + (Real)N + (preEightAm ? 0.10 : 0.50);

   SolarFluxReader::FluxData raw = reader.GetInputs(epoch);
   SolarFluxReader::FluxData fdKp = raw;
   SolarFluxReader::FluxData fdAp = raw;
   reader.PrepareKpData(fdKp, epoch);
   reader.PrepareApData(fdAp, epoch);

   out.Put("--- historic row ", (int)N, (preEightAm ? " (pre-8am)" : " (noon)"));
   out.Put("   raw detected-day F10.7a: ", raw.obsCtrF107a);
   out.Put("   Kp-path  F10.7a:         ", fdKp.obsCtrF107a);
   out.Put("   Ap-path  F10.7a:         ", fdAp.obsCtrF107a);
   out.Put("   Kp-path  F10.7 (daily):  ", fdKp.obsF107);
   out.Put("   Ap-path  F10.7 (daily):  ", fdAp.obsF107);

   // T1 (regression guard): Kp and Ap paths must agree on the F10.7a *average*.
   //     FAILS before the fix (Kp reads row N-1), PASSES after.  Holds in both
   //     the noon and pre-8am cases because both paths apply the same offset.
   //     Note: the daily obsF107 legitimately differs between the paths
   //     (PrepareApData interpolates F10.7, PrepareKpData does not); that is a
   //     pre-existing design difference unrelated to this fix, so it is not
   //     asserted here.
   out.Validate(fdKp.obsCtrF107a, fdAp.obsCtrF107a);

   // T2 (detected-day selection): at noon the raw GetInputs row equals the
   //     detected-day index, so the Kp path must return that raw F10.7a.
   if (!preEightAm)
      out.Validate(fdKp.obsCtrF107a, raw.obsCtrF107a);
}

//------------------------------------------------------------------------------
// void RunTest(TestOutput &out, const std::string &histFile)
//------------------------------------------------------------------------------
void RunTest(TestOutput &out, const std::string &histFile)
{
   out.Put("Historic space weather file: ", histFile.c_str());

   SolarFluxReader reader;
   reader.SetHistoricDataSource(1);     // 1 = CSSI historic file (else the load is skipped)
   reader.LoadFluxData(histFile, "");   // opens + parses the historic CSSI file

   // Anchor to row 0: a very early epoch is clamped by GetInputs to the first
   // record, whose .epoch gives us historicStart without a private accessor.
   SolarFluxReader::FluxData row0 = reader.GetInputs(1.0);
   const Real startEpoch = row0.epoch;   // MJD of the first historic day
   out.Put("First historic record epoch (MJD): ", startEpoch);

   // Sample rows across the span: quiet and active solar periods, plus the
   // measured worst case (row 23840, 2023-01-08, ~3 sfu divergence).
   CheckRow(out, reader, startEpoch, 100,   false);
   CheckRow(out, reader, startEpoch, 5000,  false);
   CheckRow(out, reader, startEpoch, 10000, false);
   CheckRow(out, reader, startEpoch, 20000, false);
   CheckRow(out, reader, startEpoch, 23840, false);

   // T5 (pre-8am path): both paths apply the same -1 detected-day adjustment, so
   // F10.7a must still agree even before the 8am boundary.
   CheckRow(out, reader, startEpoch, 5000, true);

   // T4 (boundary): first record, no underflow to histFluxData[-1].
   {
      const Real epoch0 = startEpoch + 0.50;
      SolarFluxReader::FluxData raw0 = reader.GetInputs(epoch0);
      SolarFluxReader::FluxData fdKp0 = raw0;
      reader.PrepareKpData(fdKp0, epoch0);
      out.Put("--- boundary historic row 0 (noon) ---");
      out.Validate(fdKp0.obsCtrF107a, raw0.obsCtrF107a);
   }
}

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string histFile = "../../../application/data/atmosphere/earth/"
                          "SpaceWeather-All-v1.2.txt";
   if (argc > 1)
      histFile = argv[1];

   TestOutput out("TestSolarFluxReader.out");

   try
   {
      RunTest(out, histFile);
      out.Put("");
      out.Put("Successfully ran TestSolarFluxReader");
   }
   catch (BaseException &be)
   {
      out.Put("** EXCEPTION: ", be.GetFullMessage());
      std::cout << "Exception: " << be.GetFullMessage() << std::endl;
      return 1;
   }
   catch (...)
   {
      out.Put("** Unknown exception");
      return 1;
   }

   return 0;
}
