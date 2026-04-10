//------------------------------------------------------------------------------
//                                  TestTextParser
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
// Author: Linda Jun
// Created: 2006/08/21
//
/**
 * Test driver for TextParser.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "StringUtil.hpp"
#include "GmatBaseException.hpp"
#include "TestOutput.hpp"
#include "TextParser.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace GmatStringUtil;

//------------------------------------------------------------------------------
// void WriteStringArray(TestOutput &out, const StringArray &parts,
//                       const std::string &exp1, const std::string &exp2,
//                       const std::string &exp3, bool addNewLine)
//------------------------------------------------------------------------------
void WriteStringArray(TestOutput &out, const StringArray &parts,
                      const std::string &exp1 = "", const std::string &exp2 = "",
                      const std::string &exp3 = "", const std::string &exp4 = "",
                      bool addNewLine = true)
{
   out.SetAddNewLine(addNewLine);
   int count = parts.size();
   out.Put("count = ", count);
   for (unsigned int i=0; i<parts.size(); i++)
      out.Put(parts[i]);
   
   if (count == 0)
   {
      out.Put("Array size is zero");
      return;
      //throw GmatBaseException
      //      (">>>>> The count of parts must be greater than 0\n");
   }
   
   if (addNewLine == false)
      out.Put("\n");
   else
      out.Put("");
   
   if (exp1 != "")
      out.Validate(parts[0], exp1);
   
   if (exp2 != "")
      out.Validate(parts[1], exp2);
   
   if (exp3 != "")
      out.Validate(parts[2], exp3);
   
   if (exp4 != "")
      out.Validate(parts[3], exp4);
   
   out.SetAddNewLine(true);
}


//------------------------------------------------------------------------------
// void WriteParts(TestOutput &out, TextParser &tp, const std::string &exp)
//------------------------------------------------------------------------------
void WriteParts(TestOutput &out, TextParser &tp, const std::string &exp)
{
   //out.SetAddNewLine(false);
   out.Put("prefaceComment = ", tp.GetPrefaceComment());
   //out.SetAddNewLine(false);
   out.Put("inlineComment  = ", tp.GetInlineComment());
   out.Put("theInstruction = ", tp.GetInstruction());
   out.Put("");
   
   out.Validate(tp.GetInstruction(), exp);
}


//------------------------------------------------------------------------------
// StringArray SeparateMultipleProps(StringArray chunks)
//------------------------------------------------------------------------------
StringArray SeparateMultipleProps(StringArray chunks)
{
   StringArray parts;
   
   return parts;
}


//--------------------------------------------------------------------
// void TestBlockType(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestBlockType(TextParser &tp, TestOutput &out)
{
   const std::string GMAT = "GMAT ";
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestBlockType()\n");
   out.Put("");
   
   out.Put("----------------------------------------");
   str =
      "% comment line 1\n"
      "% comment line 2\n";
   out.Put(str);
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 0);
   WriteParts(out, tp, "");
   
   out.Put("----------------------------------------");
   inst = "Create Spacecraft sat1";
   str =
      "% Build first spacecraft\n" + inst + "    % my first spacecraft\n";
   out.Put(str);
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 1);
   WriteParts(out, tp, inst);
   
   out.Put("----------------------------------------");
   str = "Save sat1 sat2 sat3";
   out.Put(str);
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("----------------------------------------");
   inst = "Propagate prop(Sat1, Sat2, {Sat1.ElapsedDays = 10})";
   str =
      "%This is propagate command\n"
      "% Propagate Sat1 and Sat2\n" + inst + ";  % propagate for 10 days\n";
   out.Put(str);
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, inst);
   
   
   out.Put("----------------------------------------");
   inst = "a=b";
   str =
      "%This is assignment command\n" + GMAT + inst + "  % assign b to a\n";
   out.Put(str);
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 3);
   WriteParts(out, tp, inst);
}


//--------------------------------------------------------------------
// void TestEvaluateDefinitionBlock(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestEvaluateDefinitionBlock(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestEvaluateDefinitionBlock\n");   
   out.Put("----------------------------------------");
   cmdName = "Create";
   typStr = "Spacecraft";
   objStr = "Sat1 Sat2,Sat3";
   str = cmdName + " " + typStr + " " + objStr;
   out.Put(str);
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 1);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, typStr, objStr);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[2], "()");
   WriteStringArray(out, parts, "Sat1", "Sat2", "Sat3");
   
   out.Put("----------------------------------------");
   typStr = "Spacecraft";
   objStr = "Sat1 Sat2,Sat3";
   str1 = cmdName + "      " + typStr + "    " + objStr;
   str = "     " + str1 + "   ";
   out.Put(str);
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 1);
   WriteParts(out, tp, str1);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, typStr, objStr);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[2], "()");
   WriteStringArray(out, parts, "Sat1", "Sat2", "Sat3");
   
   out.Put("----------------------------------------");
   typStr = "Array";
   objStr = "Mat1[3,3], Mat2[6,3]";
   str = cmdName + " " + typStr + " " + objStr;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 1);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, typStr, objStr);
   
//    out.Put("===== Decompose('()')");
//    parts = tp.Decompose(chunks[2], "()");
//    WriteStringArray(out, parts, "Mat1[3,3],", "Mat2[6,3]");
   
//    out.Put("----------------------------------------");
//    typStr = "Array";
//    objStr = "Vec1[3],Mat1[3,3], Mat2[6,3]";
//    str = cmdName + " " + typStr + " " + objStr;
//    out.Put(str);
//    chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 1);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, typStr, objStr);
   
//    out.Put("===== Decompose('()')");
//    parts = tp.Decompose(chunks[2], "()");
//    WriteStringArray(out, parts, "Vec1[3],", "Mat1[3,3],", "Mat2[6,3]");
}


//--------------------------------------------------------------------
// void TestEvaluateAssignmentBlock(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestEvaluateAssignmentBlock(TextParser &tp, TestOutput &out)
{
   const std::string GMAT = "GMAT ";
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestEvaluateAssignmentBlock()");
   out.Put("======================================== ASSIGNMENT_BLOCK");
   out.Put("----------------------------------------");
   lhs = "Sat1.X";
   rhs = "7000";
   str1 = lhs + "=" + rhs;
   str = GMAT + str1 + ";";
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 3);
   WriteParts(out, tp, str1);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, lhs, rhs);
   
   out.Put("----------------------------------------");
   lhs = "Sat1.VZ";
   rhs = "Mat1(1,3)";
   str = lhs + " = " + rhs;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 3);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, lhs, rhs);
   
   out.Put("----------------------------------------");
   lhs = "Sat1.Z";
   rhs = "vec(2)";
   str = lhs + " = " + rhs;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 3);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, lhs, rhs);
   
   out.Put("----------------------------------------");
   lhs = "Mat1(1,1)";
   rhs = "Sqrt(a+b+c+mat1(1,1)^2)";
   str1 = lhs + " = " + rhs;
   str = GMAT + str1 + ";";
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 3);
   WriteParts(out, tp, str1);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, lhs, rhs);

   out.Put("----------------------------------------");
   lhs = "";
   rhs = "StoreData(State1)";
   str1 = rhs;
   str = GMAT + str1 + ";";
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   // block type should be COMMAND_BLOCK because Function call without output
   out.Validate(blockType, 2);
   WriteParts(out, tp, str1);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, lhs, rhs);
   
   out.Put("----------------------------------------");
   lhs = "var2";
   rhs = "MyFunction(a, b, c)";
   str1 = lhs + " = " + rhs;
   str = GMAT + str1 + ";";
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 3);
   WriteParts(out, tp, str1);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, lhs, rhs);
   
   out.Put("----------------------------------------");
   lhs = "[S1,S2,S3,S1dot,S2dot,S3dot]";
   rhs = "GetLISAData(x,y,z,v(1),vv(1,1),vz)";
   str1 = lhs + "=" + rhs;
   str = GMAT + str1 + ";";
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   // block type should be COMMAND_BLOCK because LHS has []
   out.Validate(blockType, 2);
   WriteParts(out, tp, str1);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, lhs, rhs);
}


//------------------------------------------------------------------------------
// void TestPropagateCommand1(TestOutput &out, TextParser &tp, ...)
//------------------------------------------------------------------------------
/**
 * Parses Propagate command with the following syntax:
 *    Propagate Prop(Sat1, {Sat1.ElapsedSecs=8640.0})
 *    Propagate Prop1(Sat1,Sat2, {Sat1.ElapsedSecs=8640, Sat2.MA=90}) Prop2(Sat3, {Sat3.TA=0.0})
 */
//------------------------------------------------------------------------------
void TestPropagateCommand1(TestOutput &out, TextParser &tp,
                           const std::string &prop1,
                           const std::string &prop1Sat1, const std::string &prop1Sat2,
                           const std::string &prop1Stop1, const std::string &prop1Stop2,
                           const std::string &prop2 = "",
                           const std::string &prop2Sat1 = "", const std::string &prop2Sat2 = "",
                           const std::string &prop2Stop1 = "", const std::string &prop2Stop2 = "")
{
   StringArray chunks, parts;
   Integer blockType;
   std::string cmdName = "Propagate";
   
   std::string prop1SatStr = prop1Sat1;
   if (prop1Sat2 != "")
      prop1SatStr = prop1SatStr + "," + prop1Sat2;
   
   std::string prop2SatStr = prop2Sat1;
   if (prop2Sat2 != "")
      prop2SatStr = prop2SatStr + "," + prop2Sat2;
   
   std::string prop1StopStr = prop1Stop1;
   if (prop1Stop2 != "")
      prop1StopStr = prop1StopStr + "," + prop1Stop2;
   
   std::string prop2StopStr = prop2Stop1;
   if (prop2Stop2 != "")
      prop2StopStr = prop2StopStr + "," + prop2Stop2;
   
   std::string stopStr1 = "{" + prop1StopStr + "}";
   std::string prop1Str = "(" + prop1SatStr + "," + stopStr1 + ")";
   
   std::string stopStr2 = "{" + prop2StopStr + "}";
   std::string prop2Str = "(" + prop2SatStr + "," + stopStr2 + ")";
   
   std::string cmdStr1 = prop1 + prop1Str;
   std::string cmdStr2 = prop2 + prop2Str;
   
   std::string cmdStr = cmdStr1;
   if (prop2 != "")
      cmdStr = cmdStr1 + " " + cmdStr2; // It should work without " "
   
   std::string str = cmdName + " " + cmdStr;
   out.Put(str);
   
   out.Put("========== Get block type 1");
   out.Put("===== EvaluateBlock() 1");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("========== Separate command and description 1");
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdStr);
   
   out.Put("========== Separate multiple prop setups 1");
   out.Put("===== SeparateBrackets(() by ' ' ", chunks[1]);
   parts = tp.SeparateBrackets(chunks[1], "()", " ", false);
   if (prop2 == "")
      WriteStringArray(out, parts, cmdStr1);
   else
      WriteStringArray(out, parts, cmdStr1, cmdStr2);
   
   out.Put("========== Separate proagator and the rest 1");
   StringArray satStopStrs;
   StringArray satsStops;
   for (Integer i = 0; i < (Integer)parts.size(); i++)
   {
      out.Put("===== Decompose() () 1", parts[i]);
      satStopStrs = tp.Decompose(parts[i], "()");
      if (i == 0)
         WriteStringArray(out, satStopStrs, prop1, prop1Str);
      else
         WriteStringArray(out, satStopStrs, prop2, prop2Str);
      
      out.Put("========== Separate spacecraft and stop conditions 1");
      out.Put("===== SeparateBrackets(() by ','", satStopStrs[1]);
      satsStops = tp.SeparateBrackets(satStopStrs[1], "()", ",");
      
      StringArray sats;
      StringArray stopStrs;
      // Rearrange into spacecrafts and stop conditions
      for (Integer j = 0; j < (Integer)satsStops.size(); j++)
      {
         // stop conditions are enclosed with {}
         if (satsStops[j].find("{") == std::string::npos)
            sats.push_back(satsStops[j]);
         else
            stopStrs.push_back(satsStops[j]);
      }
      
      out.Put("===== Validate spacecrafts 1", i+1);
      if (i == 0)
      {
         if (sats.size() == 1)
            WriteStringArray(out, sats, prop1Sat1);
         else
            WriteStringArray(out, sats, prop1Sat1, prop1Sat2);
      }
      else
      {
         if (sats.size() == 1)
            WriteStringArray(out, sats, prop2Sat1);
         else
            WriteStringArray(out, sats, prop2Sat1, prop2Sat2);
      }
      
      out.Put("===== Validate stop conditions 1", i+1);
      if (i == 0)
         WriteStringArray(out, stopStrs, stopStr1);
      else
         WriteStringArray(out, stopStrs, stopStr2);
      
      out.Put("========== Separate stop conditions 1", i+1);
      for (Integer k = 0; k < (Integer)stopStrs.size(); k++)
      {
         out.Put("===== SeparateBrackets({} by ,) 1", stopStrs[k]);
         StringArray stops = tp.SeparateBrackets(stopStrs[k], "{}", ",");
         if (i == 0)
            WriteStringArray(out, stops, prop1Stop1);
         else
            WriteStringArray(out, stops, prop2Stop1);
      }
   }
}


//------------------------------------------------------------------------------
// void TestPropagateCommand2(TestOutput &out, TextParser &tp, ...)
//------------------------------------------------------------------------------
/**
 * Parses Propagate command with the following syntax:
 *    Propagate Prop(Sat1) {Sat1.MA=90}
 *    Propagate Prop1(Sat1,Sat2) {Sat1.ElapsedSecs=8640, Sat2.MA=90} Prop2(Sat3), {Sat3.TA=0.0}
 */
//------------------------------------------------------------------------------
void TestPropagateCommand2(TestOutput &out, TextParser &tp,
                            const std::string &prop1,
                            const std::string &prop1Sat1, const std::string &prop1Sat2,
                            const std::string &prop1Stop1, const std::string &prop1Stop2,
                            const std::string &prop2 = "",
                            const std::string &prop2Sat1 = "", const std::string &prop2Sat2 = "",
                            const std::string &prop2Stop1 = "", const std::string &prop2Stop2 = "")
{
   StringArray chunks, parts;
   Integer blockType;
   std::string cmdName = "Propagate";
   
   std::string prop1SatStr = prop1Sat1 + ",";
   if (prop1Sat2 != "")
      prop1SatStr = prop1SatStr + prop1Sat2;
   
   std::string prop2SatStr = prop2Sat1 + ",";
   if (prop2Sat2 != "")
      prop2SatStr = prop2SatStr + prop2Sat2;
   
   std::string prop1SatsWithParen = "(" + prop1SatStr + ")";
   std::string prop2SatsWithParen = "(" + prop2SatStr + ")";
   std::string prop1Str1 = prop1 + prop1SatsWithParen;
   std::string prop2Str1 = prop2 + prop2SatsWithParen;
   
   std::string prop1StopStr = prop1Stop1;
   if (prop1Stop2 != "")
      prop1StopStr = prop1StopStr + "," + prop1Stop2;
   
   std::string prop2StopStr = prop2Stop1;
   if (prop2Stop2 != "")
      prop2StopStr = prop2StopStr + "," + prop2Stop2;
   
   std::string stopStr1 = "{" + prop1StopStr + "}";
   ///std::string prop1Str = prop1SatsWithParen + " " + stopStr1;
   std::string prop1Str = prop1SatsWithParen + stopStr1;
   
   std::string stopStr2 = "{" + prop2StopStr + "}";
   ///std::string prop2Str = prop2SatsWithParen + " " + stopStr2;
   std::string prop2Str = prop2SatsWithParen + stopStr2;
   
   std::string cmdStr1 = prop1 + prop1Str;
   std::string cmdStr2 = prop2 + prop2Str;
   
   std::string cmdStr = cmdStr1;
   if (prop2 != "")
      //cmdStr = cmdStr1 + " " + cmdStr2; // It should work without " "
      cmdStr = cmdStr1 + cmdStr2; // It should work without " "
   
   std::string str = cmdName + " " + cmdStr;
   out.Put(str);
   
   out.Put("========== Get block type 2");
   out.Put("===== EvaluateBlock() 2");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("========== Separate command and description 2");
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdStr);
   
   out.Put("========== Separate prop+spacecrafts and stop conditions 2");
   out.Put("===== SeparateAllBrackets({} 2 =>", chunks[1]);
   parts = tp.SeparateAllBrackets(chunks[1], "{}");
   if (prop2 == "")
      WriteStringArray(out, parts, prop1Str1, stopStr1);
   else
      WriteStringArray(out, parts, prop1Str1, stopStr1, prop2Str1, stopStr2);
   
   out.Put("========== Separate propagator spacecrafts and stop conditions 2");
   for (UnsignedInt i=0; i<parts.size(); i++)
   {
      // If it does not starts with {, it is propagator and spacecrafts
      if (parts[i][0] != '{')
      {
         out.Put("========== Separate prop_sats 2");
         out.Put("===== SeparateBy() () 2 => ", parts[i]);
         StringArray propSats = tp.SeparateBy(parts[i], "(),");
         if (i == 0)
            WriteStringArray(out, propSats, prop1, prop1Sat1, prop1Sat2);
         else
            WriteStringArray(out, propSats, prop2, prop2Sat1, prop2Sat2);
      }
      else
      {
         out.Put("========== Separate stop conditions 2");
         out.Put("===== SeparateBrackets({} by ,) 2 => ", parts[i]);
         StringArray stops = tp.SeparateBrackets(parts[i], "{}", ",", true);
         if (i == 1)
            WriteStringArray(out, stops, prop1Stop1);
         else
            WriteStringArray(out, stops, prop2Stop1);
      }
   }
}


//--------------------------------------------------------------------
// void TestDecomposeBlock(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestDecomposeBlock(TextParser &tp, TestOutput &out)
{
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("");
   out.Put("======================================== TestDecomposeBlock()\n");
   
   out.Put("----------------------------------------");
   cmt1 = "% Build first spacecraft\n";
   cmdName = "Create";
   typStr = "Spacecraft";
   inst = cmdName + " " + typStr + " sat1    % my first spacecraft\n";
   str = cmt1 + inst;
   out.Put(str);
   
   out.Put("===== DecomposeBlock()");
   chunks = tp.DecomposeBlock(str);
   WriteStringArray(out, chunks, cmt1, inst, "", "", false);
   
   out.Put("----------------------------------------");
   inst = "Create " + typStr + " sat2    % my second pacecraft\n";
   cmt1 = "% Build second spacecraft\n";
   cmt2 = "% Second line comment\r";
   str = cmt1 + cmt2 + inst;
   out.Put(str);
   
   out.Put("===== DecomposeBlock()");
   chunks = tp.DecomposeBlock(str);
   WriteStringArray(out, chunks, cmt1, cmt2, inst, "", false);
   
   out.Put("----------------------------------------");
   cmt1 = "     % Build third spacecraft\n";
   cmt2 = "     % Second line comment\r";
   inst = "     Create " + typStr + " sat3    % my third spacecraft\n";
   str = cmt1 + cmt2 + inst;
   out.Put(str);
   
   out.Put("===== DecomposeBlock()");
   chunks = tp.DecomposeBlock(str);
   WriteStringArray(out, chunks, cmt1, cmt2, inst, "", false);
}


//--------------------------------------------------------------------
// void TestReportCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestReportCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestReportCommand()\n");
   cmdName = "Report";
   typStr = "reportObject";
   objStr = " Mat3(2,1), Vec(1) Vec(2)";
   cmdExp = typStr + objStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   try
   {
      parts = tp.Decompose(chunks[1], "()");
      WriteStringArray(out, parts, typStr, "Mat3(2,1)", "Vec(1)");
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage());
   }
   
   out.Put("----------------------------------------");
   objStr = " Vec1(5),Mat1,Mat2(1,1) Mat3(2,1)";
   cmdExp = typStr + objStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   try
   {
      parts = tp.Decompose(chunks[1], "()");
      WriteStringArray(out, parts, typStr, "Vec1(5)", "Mat1");
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage());
   }
}

//--------------------------------------------------------------------
// void TestSaveCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestSaveCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestSaveCommand()\n");
   cmdName = "Save";
   cmdExp = "Sat1 Sat2,Sat3 Sat4";
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()");
   WriteStringArray(out, parts, "Sat1", "Sat2", "Sat3");
   
}


//--------------------------------------------------------------------
// void TestEndIfCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestEndIfCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestEndIfCommand()\n");
   cmdName = "EndIf";
   cmdExp = "";
   str = cmdName;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks.clear();
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName);
}


//--------------------------------------------------------------------
// void TestBeginFiniteBurnCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestBeginFiniteBurnCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestBeginFiniteBurnCommand()\n");
   cmdName = "BeginFiniteBurn";
   typStr = "burn1";
   cmdExp = typStr + "(Sat1 Sat2)";
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()", false);
   WriteStringArray(out, parts, typStr, "(Sat1 Sat2)");
   
   out.Put("===== SeparateBrackets('()')");
   parts = tp.SeparateBrackets(parts[1], "()", " ,");
   WriteStringArray(out, parts, "Sat1", "Sat2");
}

//--------------------------------------------------------------------
// void TestForCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestForCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestForCommand()\n");
   cmdName = "For";
   typStr = "step";
   cmdExp = typStr + " = 1 : 10 : 2";
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   out.Validate(blockType, 2);
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()");
   WriteStringArray(out, parts, typStr, "=", "1");
   
   out.Put("----------------------------------------");
   cmdName = "For";
   typStr = "step";
   cmdExp = typStr + "=1:10:2";
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   out.Validate(blockType, 2);
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()");
   WriteStringArray(out, parts, "step=1:10:2");
}


//--------------------------------------------------------------------
// void TestWhileCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestWhileCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestWhileCommand()");
   cmdName = "While";
   cmdExp = "var1 < Sat1.X";
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()");
   WriteStringArray(out, parts, "var1", "<", "Sat1.X");
   
   out.Put("----------------------------------------");
   cmdExp = "var1 == Sat1.X";
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()");
   WriteStringArray(out, parts, "var1", "==", "Sat1.X");
}

//--------------------------------------------------------------------
// void TestIfCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestIfCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestIfCommand()");
   cmdName = "If";
   cmdExp = "var1 ~= var2";
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()");
   WriteStringArray(out, parts, "var1", "~=", "var2");
}


//--------------------------------------------------------------------
// void TestPropagateCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestPropagateCommand(TextParser &tp, TestOutput &out)
{
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestPropagateCommand()");
   
   out.Put("---------------------------------------- Propagate 1");
   TestPropagateCommand1(out, tp, "Prop1", "Sat1", "Sat2", "Sat1.Periapsis", "");
   
   out.Put("---------------------------------------- Propagate 2");
   TestPropagateCommand1(out, tp, "-prop", "Sat1", "", "Sat1.Periapsis", "");
   
   out.Put("---------------------------------------- Propagate 3");
   TestPropagateCommand1(out, tp, "Prop1", "Sat1", "", "Sat1.ElapsedSecs=8640", "");
   
   out.Put("---------------------------------------- Propagate 4");
   TestPropagateCommand1(out, tp, "Prop1", "Sat1", "Sat2", "Sat1.ElapsedSecs=8640.0", "Sat2.MA=90");
   
   out.Put("---------------------------------------- Propagate 5");
   TestPropagateCommand2(out, tp, "Prop1", "Sat1", "Sat2", "Sat1.ElapsedSecs=8640.0", "Sat2.MA=90");
   
   out.Put("---------------------------------------- Propagate 6");
   TestPropagateCommand1(out, tp, "Prop1", "Sat1", "Sat2", "Sat1.ElapsedSecs=8640.0", "Sat2.MA=90",
                          "Prop2", "Sat3", "Sat4", "Sat3.ElapsedSecs=8640.0", "Sat4.MA=90");
   
   out.Put("---------------------------------------- Propagate 7");
   TestPropagateCommand2(out, tp, "Prop1", "Sat1", "Sat2", "Sat1.ElapsedSecs=8640.0", "Sat2.MA=90",
                          "Prop2", "Sat3", "Sat4", "Sat3.ElapsedSecs=8640.0", "Sat4.MA=90");
}


//------------------------------------------------------------------------------
// void ParsePropagateCommand(TestOutput &out, TextParser &tp, ...)
//------------------------------------------------------------------------------
/**
 * Parses Propagate command with the following syntax:
 *    Propagate Prop(Sat1, {Sat1.ElapsedSecs=8640.0})
 *    Propagate Prop1(Sat1,Sat2, {Sat1.ElapsedSecs=8640, Sat2.MA=90}) Prop2(Sat3, {Sat3.TA=0.0})
 *    Propagate Prop(Sat1) {Sat1.MA=90}
 *    Propagate Prop1(Sat1,Sat2) {Sat1.ElapsedSecs=8640, Sat2.MA=90} Prop2(Sat3), {Sat3.TA=0.0}
 */
//------------------------------------------------------------------------------
void ParsePropagateCommand(TestOutput &out, TextParser &tp, const std::string &str,
                           const std::string &expProp1 = "", const std::string &expStop1 = "",
                           const std::string &expProp2 = "", const std::string &expStop2 = "")
{
   StringArray chunks, parts;
   
   out.Put(str);
   // Remove blank spaces
   std::string str1 = GmatStringUtil::RemoveAll(str, ' ');
   out.Put(str1);
   std::string str2;
   StringArray props, stops;
   
   out.Put("========== GmatStringUtil::SeparateBy( ) ) => ", str1);
   chunks = GmatStringUtil::SeparateBy(str1, ")", true, true, false);
   WriteStringArray(out, chunks);
   
   for (UnsignedInt i = 0; i < chunks.size(); i++)
   {
      str2 = chunks[i];
      out.Put("str2 = ", str2);
      std::string::size_type lastCloseParen = str2.find_last_of(")");
      
      // Remove last ) after }
      if (lastCloseParen == (str2.size() - 1) && str2[lastCloseParen - 1] == '}')
      {
         // Remove last )
         str2 = GmatStringUtil::RemoveLastString(str2, ")");
         
         // Replace last comma before { with )
         out.Put("--- Replace last comma before { with )");
         std::string::size_type openBrace = str2.find("{");
         std::string::size_type lastComma = str2.find_last_of(",", openBrace);
         if (lastComma != str2.npos && str2[lastComma-1] != ')')
            str2[lastComma] = ')';
         else if (lastComma != str2.npos)
            str2.erase(lastComma, 1);
         
         out.Put(str2);
      }
      
      out.Put("========== SeparateAllBrackets( {} ) => ", str2);
      parts = tp.SeparateAllBrackets(str2, "{}");
      
      WriteStringArray(out, parts);
      
      
      for (UnsignedInt i=0; i<parts.size(); i++)
      {
         // If it does not starts with {, it is propagator and spacecrafts
         if (parts[i][0] != '{')
         {
            out.Put("========== Add propagator setups");
            parts[i] = GmatStringUtil::Trim(parts[i]);
            props.push_back(parts[i]);
         }
         else
         {
            out.Put("========== Add stop conditions");
            out.Put("===== SeparateBrackets({} by ,) => ", parts[i]);
            StringArray tempStops = tp.SeparateBrackets(parts[i], "{}", ",", true);
            copy(tempStops.begin(), tempStops.end(), back_inserter(stops));
         }
      }
   }
   
   WriteStringArray(out, props, expProp1);
   WriteStringArray(out, stops, expStop1);
}


//--------------------------------------------------------------------
// void TestParsePropagateCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestParsePropagateCommand(TextParser &tp, TestOutput &out)
{
   std::string str, str1, str2;
   std::string prop1, prop2, stop1, stop2;
   std::string expProp1, expProp2;
   std::string expStop1, expStop2, expStop3, expStop4;

   #if 1
   out.Put("======================================== TestParsePropagateCommand()");
   out.Put("---------------------------------------- ParsePropagate 1");
   prop1 = "Prop1(Sat1";
   expProp1 = prop1 + ")";
   expStop1 = "Sat1.ElapsedSecs=8640.0";
   stop1 = "{" + expStop1 + "}";
   str = prop1 + ", " + stop1 + ")";
   ParsePropagateCommand(out, tp, str, expProp1, expStop1);
   
   out.Put("---------------------------------------- ParsePropagate 2");
   str = expProp1 + " " + stop1 + ")";
   ParsePropagateCommand(out, tp, str, expProp1, expStop1);
   
   out.Put("---------------------------------------- ParsePropagate 3");
   str = expProp1 + ", " + stop1 + ")";
   ParsePropagateCommand(out, tp, str, expProp1, expStop1);
   
   out.Put("---------------------------------------- ParsePropagate 4");
   ParsePropagateCommand(out, tp, "Prop1(Sat1, Sat2, {Sat1.ElapsedSecs = 8640.0, Sat2.MA=90.0})",
                         "Prop1(Sat1,Sat2)", "Sat1.ElapsedSecs=8640.0", "Sat2.MA=90.0");
   
   out.Put("---------------------------------------- ParsePropagate 5");
   ParsePropagateCommand(out, tp, "Prop1(Sat1, Sat2) {Sat1.ElapsedSecs = 8640.0, Sat2.MA = 90.0}",
                         "Prop1(Sat1,Sat2)", "Sat1.ElapsedSecs=8640.0", "Sat2.MA=90.0");
   
   out.Put("---------------------------------------- ParsePropagate 6");
   prop1 = "Prop1(Sat1)";
   expProp1 = prop1;
   expStop1 = "Sat1.TA=stopArray(1,1)";
   expStop2 = "StopTolerance=1e-005";
   stop1 = "{" + expStop1 + ", " + expStop2 + "}";
   str = prop1 + stop1;
   ParsePropagateCommand(out, tp, str, expProp1, expStop1);
   
   out.Put("---------------------------------------- ParsePropagate 7");
   prop1 = "Prop1(Sat1)";
   expProp1 = prop1;
   prop2 = "Prop2(Sat3, Sat4)";
   expProp2 = prop2;
   expStop1 = "Sat1.TA=stopArray(1,1)";
   expStop2 = "StopTolerance=A(1,1)";
   expStop3 = "Sat3.TA=B(a,b)";
   expStop4 = "A(a,b)=B(c,d)";
   str1 = prop1 + " " + "{" + expStop1 + ", " + expStop2 + "}";
   str2 = prop2 + " " + "{" + expStop3 + ", " + expStop4 + "}";
   str  = str1 + str2;
   ParsePropagateCommand(out, tp, str, expProp1, expStop1, expProp2, expStop2);
   
   out.Put("---------------------------------------- ParsePropagate 8");
   ParsePropagateCommand(out, tp, "Prop1(Sat1)", "Prop1(Sat1)");
   
   out.Put("---------------------------------------- ParsePropagate 9");
   str = "Prop1(SC1, SC2, {SC1.ElapsedDays = 0.2}) Prop2(SC3, {SC3.ElapsedDays = 0.25})";
   ParsePropagateCommand(out, tp, str, "Prop1(SC1,SC2)", "SC1.ElapsedDays=0.2",
                         "Prop2(SC3)", "SC3.ElapsedDays=0.25");
   
   out.Put("---------------------------------------- ParsePropagate 10");
   str = "Prop1(Sat1, {One = Sat.EarthMJ2000Eq.Z})";
   ParsePropagateCommand(out, tp, str, "Prop1(Sat1)", "One=Sat.EarthMJ2000Eq.Z");
   
   #endif
   
   out.Put("---------------------------------------- ParsePropagate 11");
   prop1 = "Prop1(Sat1)";
   expProp1 = prop1;
   prop2 = "";
   expProp2 = prop2;
   expStop1 = "stopArray(1,1)=Sat1.TA";
   expStop2 = "StopTolerance=1e-005";
   expStop3 = "";
   expStop4 = "";
   str1 = prop1 + " " + "{" + expStop1 + ", " + expStop2 + "}";
   str  = str1;
   ParsePropagateCommand(out, tp, str, expProp1, expStop1, expStop2);
   
   out.Put("---------------------------------------- ParsePropagate 12");
   prop1 = "Prop1(Sat1)";
   expProp1 = prop1;
   prop2 = "";
   expProp2 = prop2;
   expStop1 = "Sat1.Luna.RMAG=65000.0";
   expStop2 = "";
   expStop3 = "";
   expStop4 = "";
   str1 = prop1 + " " + "{" + expStop1 + ", " + expStop2 + "}";
   str  = str1;
   ParsePropagateCommand(out, tp, str, expProp1, expStop1, expStop2);
   
   out.Put("---------------------------------------- ParsePropagate 13");
   prop1 = "Prop1(Sat1";
   expProp1 = prop1 + ")";
   prop2 = "";
   expProp2 = prop2;
   expStop1 = "Sat1.Apoapsis";
   expStop2 = "";
   expStop3 = "";
   expStop4 = "";
   str1 = prop1 + ", " + " {" + expStop1 + "} )";
   str  = str1;
   ParsePropagateCommand(out, tp, str, expProp1, expStop1, expStop2);
   
   out.Put("---------------------------------------- ParsePropagate 14");
   prop1 = "Prop1(Sat1";
   expProp1 = prop1 + ")";
   prop2 = "";
   expProp2 = prop2;
   expStop1 = "Sat1.Apoapsis";
   expStop2 = "";
   expStop3 = "";
   expStop4 = "";
   str1 = prop1 + ", " + " {" + expStop1 + "} )";
   str  = str1;
   ParsePropagateCommand(out, tp, str, expProp1, expStop1, expStop2);
}


//--------------------------------------------------------------------
// void TestAchieveCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestAchieveCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType, count;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestAchieveCommand()");
   
   out.Put("---------------------------------------- Achieve 1");
   cmdName = "Achieve";
   typStr = "myDC";
   braceStr = "{Tolerance=0.1}";
   parenStr = "(Sat1.SMA=21545.0," + braceStr + ")";
   cmdExp = typStr + parenStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);

   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()");
   WriteStringArray(out, parts, typStr, parenStr);
   
   out.Put("===== SeparateBrackets('()')");
   parts = tp.SeparateBrackets(parts[1], "()", ",");
   WriteStringArray(out, parts, "Sat1.SMA=21545.0", braceStr);
   
   count = parts.size();
   out.Put("===== SeparateBrackets('{}')");
   parts = tp.SeparateBrackets(parts[count-1], "{}", "=");
   WriteStringArray(out, parts, "Tolerance", "0.1");
   
   out.Put("---------------------------------------- Achieve 2");
   cmdName = "Achieve";
   typStr = "myDC";
   braceStr = "{Tolerance=0.1}";
   parenStr = "(Sat1.SMA=Mat1(1,1)," + braceStr + ")";
   cmdExp = typStr + parenStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);

   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()");
   WriteStringArray(out, parts, typStr, parenStr);
   
   out.Put("===== SeparateBrackets('()')");
   parts = tp.SeparateBrackets(parts[1], "()", ",");
   WriteStringArray(out, parts, "Sat1.SMA=Mat1(1,1)", braceStr);
   
   count = parts.size();
   out.Put("===== SeparateBrackets('{}')");
   parts = tp.SeparateBrackets(parts[count-1], "{}", "=");
   WriteStringArray(out, parts, "Tolerance", "0.1");
}

//--------------------------------------------------------------------
// void TestVaryCommand(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestVaryCommand(TextParser &tp, TestOutput &out)
{
   Integer blockType, count;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== TestVaryCommand()");
   
   out.Put("---------------------------------------- Vary 1");
   cmdName = "Vary";
   typStr = "DC";
   braceStr = "{Pert=1,MaxStep=1000,Lower=6000,Upper=100000}";
   parenStr = "(DefaultSC.SMA=Vec(3)," + braceStr + ")";
   cmdExp = typStr + parenStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()", true, true);
   WriteStringArray(out, parts, typStr, parenStr);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(parts[1], "()", true, true);
   WriteStringArray(out, parts, "DefaultSC.SMA=Vec(3)", braceStr);
   
   count = parts.size();
   out.Put("===== SeparateBrackets('{}')");
   parts = tp.SeparateBrackets(parts[count-1], "{}", ",");
   WriteStringArray(out, parts, "Pert=1", "MaxStep=1000", "Lower=6000");
   
   
   out.Put("---------------------------------------- Vary 2");
   cmdName = "Vary";
   typStr = "DC";
   braceStr = "{Pert=1,MaxStep=1000,Lower=6000,Upper=100000}";
   parenStr = "(DefaultSC.SMA=6500," + braceStr + ")";
   cmdExp = typStr + parenStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()", true, true);
   WriteStringArray(out, parts, typStr, parenStr);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(parts[1], "()", true, true);
   WriteStringArray(out, parts, "DefaultSC.SMA=6500", braceStr); //<<<<<
   
   count = parts.size();
   out.Put("===== SeparateBrackets('{}')");
   parts = tp.SeparateBrackets(parts[count-1], "{}", ",");
   WriteStringArray(out, parts, "Pert=1", "MaxStep=1000", "Lower=6000");
   
   out.Put("---------------------------------------- Vary 3");
   cmdName = "Vary";
   typStr = "DC";
   braceStr = "{Pert=Pert(1,1),MaxStep=MaxStep(1,1),Lower=Lower(1,1),Upper=Upper(1,1)}";
   parenStr = "(DefaultSC.SMA=GOAL(1,1)," + braceStr + ")";
   cmdExp = typStr + parenStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
      
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()", true, true);
   WriteStringArray(out, parts, typStr, parenStr);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(parts[1], "()", true, true);
   WriteStringArray(out, parts, "DefaultSC.SMA=GOAL(1,1)", braceStr);
   
   count = parts.size();
   out.Put("===== SeparateBrackets('{}')");
   parts = tp.SeparateBrackets(parts[count-1], "{}", ",");
   WriteStringArray(out, parts, "Pert=Pert(1,1)", "MaxStep=MaxStep(1,1)", "Lower=Lower(1,1)");
   
   out.Put("---------------------------------------- Vary 4");
   cmdName = "Vary";
   typStr = "DC";
   braceStr = "{Pert=Pert(1,1),MaxStep=MaxStep(1,1),Lower=Lower(1,1),Upper=Upper(1,1)}";
   parenStr = "(array(1,1)=GOAL(1,1)," + braceStr + ")";
   cmdExp = typStr + parenStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
      
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()", true, true);
   WriteStringArray(out, parts, typStr, parenStr);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(parts[1], "()", true, true);
   WriteStringArray(out, parts, "array(1,1)=GOAL(1,1)", braceStr);
   
   count = parts.size();
   out.Put("===== SeparateBrackets('{}')");
   parts = tp.SeparateBrackets(parts[count-1], "{}", ",");
   WriteStringArray(out, parts, "Pert=Pert(1,1)", "MaxStep=MaxStep(1,1)", "Lower=Lower(1,1)");


   out.Put("---------------------------------------- Vary 5");
   cmdName = "Vary";
   typStr = "DC";
   braceStr = "{Perturbation=array1(array2(1,1),array2(1,1)),MaxStep=var2,"
      "Lower=0,Upper=array1(array2(2,2),array2(2,2))}";
   parenStr = "(TOI.Element1=var1," + braceStr + ")";
   cmdExp = typStr + parenStr;
   str = cmdName + " " + cmdExp;
   out.Put(str);
   chunks.clear();
   
   out.Put("===== EvaluateBlock()");
   blockType = tp.EvaluateBlock(str);
   out.Validate(blockType, 2);
   WriteParts(out, tp, str);
   
   out.Put("===== ChunkLine()");
   chunks = tp.ChunkLine();
   WriteStringArray(out, chunks, cmdName, cmdExp);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(chunks[1], "()", true, true);
   WriteStringArray(out, parts, typStr, parenStr);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(parts[1], "()", true, true);
   WriteStringArray(out, parts, "TOI.Element1=var1", braceStr);
   
   count = parts.size();
   out.Put("===== SeparateBrackets('{}')");
   parts = tp.SeparateBrackets(parts[count-1], "{}", ",");
   WriteStringArray(out, parts, "Perturbation=array1(array2(1,1),array2(1,1))",
                    "MaxStep=var2", "Lower=0");
   
   out.Put("----------------------------------------");
   str = "State1(1,1) State2(1,1) State(2,2)";
   out.Put("===== SeparateBrackets('{}')");
   out.Put(str);
   parts = tp.SeparateBrackets(str, "{}", " ,", false);
   WriteStringArray(out, parts, "State1(1,1)", "State2(1,1)", "State(2,2)");
}


//--------------------------------------------------------------------
// void TestDecompose(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestDecompose(TextParser &tp, TestOutput &out)
{
   std::string str, str1, str2, str3, str4;
   StringArray chunks;
   
   out.Put("");
   out.Put("======================================== TestDecompose()\n");

   out.Put("----------------------------------------");
   str = "Vec[3] Mat1[3,1], Mat2[6,1]";
   out.Put("===== Decompose() by ()", str);
   chunks = tp.Decompose(str, "()");
   WriteStringArray(out, chunks, "Vec[3]", "Mat1[3,1],", "Mat2[6,1]", "", false);
   
   out.Put("----------------------------------------");
   str = "Prop1(Sat1,Sat2, {Sat1.ElapsedSecs = 8640.0})";
   out.Put("===== Decompose() ", str);
   chunks = tp.Decompose(str, "()");
   WriteStringArray(out, chunks, "Prop1", "(Sat1,Sat2,{Sat1.ElapsedSecs=8640.0})");
   
   out.Put("----------------------------------------");
   str1 = "Prop1";
   str2 = "(Sat1,Sat2,{Sat1.ElapsedSecs=8640.0,Sat2.MA=90.0})";
   str = str1 + str2;
   out.Put("===== Decompose() by () ", str);
   chunks = tp.Decompose(str, "()");
   WriteStringArray(out, chunks, str1, str2);
   
   out.Put("----------------------------------------");
   str1 = "Prop1(Sat1)";
   str2 = "{Sat1.ElapsedSecs=8640.0,Sat2.MA=90.0}";
   str = str1 + "," + str2;
   out.Put("===== Decompose() by {} ", str);
   chunks = tp.Decompose(str, "{}");
   WriteStringArray(out, chunks, str1, str2);
   
   out.Put("----------------------------------------");
   str1 = "Prop1(Sat1)";
   str2 = "{Sat1.ElapsedSecs=8640.0,Sat2.MA=90.0}";
   str = str1 + " " + str2;
   out.Put("===== Decompose() by {} ", str);
   chunks = tp.Decompose(str, "{}");
   WriteStringArray(out, chunks, str1, str2);
   
   out.Put("----------------------------------------");
   str1 = "Prop1(Sat1)";
   str2 = "{Sat1.ElapsedSecs=8640.0,Sat2.MA=90.0}";
   str = str1 + "" + str2;
   out.Put("===== Decompose() by {} ", str);
   chunks = tp.Decompose(str, "{}");
   WriteStringArray(out, chunks, str1, str2);      
}


//--------------------------------------------------------------------
// void TestSeparateBrackets(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestSeparateBrackets(TextParser &tp, TestOutput &out)
{
   std::string str, str1, str2, str3, str4;
   StringArray parts;

   #if 0
   out.Put("======================================== TestSeparateBrackets()\n");
   out.Put("----------------------------------------");
   str = "{sat.EarthMJ2000Eq.X}";
   out.Put("===== SeparateBrackets('{}') ", str);
   parts = tp.SeparateBrackets(str, "{}", " ,", false);
   WriteStringArray(out, parts, "sat.EarthMJ2000Eq.X");
   
//    out.Put("----------------------------------------");
//    str = "(Prop1(Sat1, {Sat1.ElapsedSecs = 8640.0})";
//    out.Put("===== SeparateBrackets('{}') ", str);
//    parts = tp.SeparateBrackets(str, "{}", " ,", false);
//    WriteStringArray(out, parts, "Prop1(Sat1", "{Sat1.ElapsedSecs=8640.0}");
   
   out.Put("----------------------------------------");
   str = "Prop1(Sat1) {Sat1.ElapsedSecs = 8640.0}";
   out.Put("===== SeparateBrackets('{}')");
   out.Put(str);
   parts = tp.SeparateBrackets(str, "{}", " ,", false);
   WriteStringArray(out, parts, "Prop1(Sat1)", "{Sat1.ElapsedSecs=8640.0}");
   
   out.Put("----------------------------------------");
   str = "Prop1(Sat1) {Sat1.ElapsedSecs = 8640.0} Prop2(Sat2) {Sat2.MA = 90.0}";
   out.Put("===== SeparateBrackets('{}')");
   out.Put(str);
   parts = tp.SeparateBrackets(str, "{}", " ,", false);
   WriteStringArray(out, parts, "Prop1(Sat1)", "{Sat1.ElapsedSecs=8640.0}", "Prop2(Sat2)",
                    "{Sat2.MA=90.0}");
   #endif
   
   out.Put("======================================== TestSeparateAllBrackets()\n");
   out.Put("----------------------------------------");
   str1 = "Prop1(Sat1,Sat2)";
   str2 = "{Sat1.ElapsedSecs=abc(1,2),Sat2.MA=def(3,4)}";
   str = str1 + str2 + str3 + str4;
   out.Put("===== SeparateAllBrackets() by { ", str);
   parts = tp.SeparateAllBrackets(str, "{}");
   WriteStringArray(out, parts, str1, str2, str3, str4);
   
   str1 = "Prop1(Sat1,Sat2)";
   str2 = "{Sat1.ElapsedSecs=8640.0,Sat2.MA=90.0}";
   str3 = "Prop2(Sat3,Sat4)";
   str4 = "{Sat3.ElapsedSecs=8640.0,Sat4.MA=90.0}";
   str = str1 + str2 + str3 + str4;
   out.Put("===== SeparateAllBrackets() by { ", str);
   parts = tp.SeparateAllBrackets(str, "{}");
   WriteStringArray(out, parts, str1, str2, str3, str4);
   
   out.Put("----------------------------------------");
   str1 = "Prop1(Sat1,Sat2)";
   str2 = "{Sat1.ElapsedSecs=A(1,1),Sat2.MA=90.0}";
   str3 = "Prop2(Sat3,Sat4)";
   str4 = "{Sat3.ElapsedSecs=8640.0,Sat4.MA=B(2,2)}";
   str = str1 + str2 + str3 + str4;
   out.Put("===== SeparateAllBrackets() by { ", str);
   parts = tp.SeparateAllBrackets(str, "{}");
   WriteStringArray(out, parts, str1, str2, str3, str4);
}

//--------------------------------------------------------------------
// void TestSeparateSpaces(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestSeparateSpaces(TextParser &tp, TestOutput &out)
{
   std::string str;
   StringArray parts;
   
   out.Put("");
   out.Put("======================================== TestSeparateSpaces()\n");
   
   out.Put("----------------------------------------");
   str = "Sat1 , Sat2 , Sat3";
   out.Put(str);
   
   out.Put("===== SeparateSpaces()");
   parts = tp.SeparateSpaces(str);
   WriteStringArray(out, parts, "Sat1", "Sat2", "Sat3");
   
   out.Put("----------------------------------------");
   str = "A[1, 3], B[3, 3],  C[6, 6]";
   out.Put(str);
   
   out.Put("===== Decompose('()')");
   parts = tp.Decompose(str, "()");
   WriteStringArray(out, parts, "A[1, 3],", "B[3, 3],", "C[6, 6]");
   
   out.Put("===== SeparateSpaces()");
   parts = tp.SeparateSpaces(str);
   WriteStringArray(out, parts, "A[1", "3]", "B[3");
}


//--------------------------------------------------------------------
// void TestSeparateDots(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void TestSeparateDots(TextParser &tp, TestOutput &out)
{
   std::string str;
   StringArray parts;
   
   out.Put("");
   out.Put("======================================== TestSeparateDots()\n");
   
   out.Put("----------------------------------------");
   str = "Sat1.Earth.RMAG";
   out.Put(str);
   
   out.Put("===== SeparateDots()");
   parts = tp.SeparateDots(str);
   WriteStringArray(out, parts, "Sat1", "Earth", "RMAG");
   
   out.Put("----------------------------------------");
   str = "Sat1. Earth. RMAG";
   out.Put(str);
   
   out.Put("===== SeparateDots()");
   parts = tp.SeparateDots(str);
   WriteStringArray(out, parts, "Sat1", " Earth", " RMAG");
}


//--------------------------------------------------------------------
// void CheckErrors(TextParser &tp, TestOutput &out)
//--------------------------------------------------------------------
void CheckErrors(TextParser &tp, TestOutput &out)
{
   Integer blockType;
   std::string str, str1, inst;
   std::string cmt1, cmt2;
   std::string cmdName, typStr, objStr, braceStr, parenStr, cmdExp;
   std::string lhs, rhs;
   StringArray chunks;
   StringArray parts;
   StringArray theCommandList;
   
   out.Put("======================================== CheckErrors()");
   //-----------------------------------------------------------------
   // Error condition
   //-----------------------------------------------------------------
   out.Put("----------------------------------------");
   cmdName = "Create";
   typStr = "Spacecraft";
   str = cmdName + " " + typStr;
   out.Put(str);
   chunks.clear();

   try
   {
      out.Put("===== EvaluateBlock()");
      blockType = tp.EvaluateBlock(str);
      out.Validate(blockType, 1);
      WriteParts(out, tp, str);
      
      out.Put("===== ChunkLine()");
      chunks = tp.ChunkLine();
      WriteStringArray(out, chunks, "Error", "Error");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   out.Put("----------------------------------------");
   typStr = "Propagator";
   str = cmdName + " " + typStr;
   out.Put(str);
   chunks.clear();
   
   try
   {
      out.Put("===== EvaluateBlock()");
      blockType = tp.EvaluateBlock(str);
      out.Validate(blockType, 1);
      WriteParts(out, tp, str);
   
      out.Put("===== ChunkLine()");
      chunks = tp.ChunkLine();
      WriteStringArray(out, chunks, "Error", "Error");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   out.Put("----------------------------------------");
   cmdName = "Propagate";
   typStr = "";
   str = cmdName + " " + typStr;
   out.Put(str);
   chunks.clear();
   
   try
   {
      out.Put("===== EvaluateBlock()");
      blockType = tp.EvaluateBlock(str);
      out.Validate(blockType, 2);
      WriteParts(out, tp, str);
   
      out.Put("===== ChunkLine()");
      chunks = tp.ChunkLine();
      WriteStringArray(out, chunks, "Error", "Error");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   out.Put("----------------------------------------");
   cmdName = "While";
   typStr = "";
   str = cmdName + " " + typStr;
   out.Put(str);
   chunks.clear();
   
   try
   {
      out.Put("===== EvaluateBlock()");
      blockType = tp.EvaluateBlock(str);
      out.Validate(blockType, 2);
      WriteParts(out, tp, str);
   
      out.Put("===== ChunkLine()");
      chunks = tp.ChunkLine();
      WriteStringArray(out, chunks, "Error", "Error");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   out.Put("----------------------------------------");
   str = "   A1 = ";
   out.Put(str);
   chunks.clear();
   
   try
   {
      out.Put("===== EvaluateBlock()");
      blockType = tp.EvaluateBlock(str);
      out.Validate(blockType, 3);
   
      out.Put("===== ChunkLine()");
      chunks = tp.ChunkLine();
      WriteStringArray(out, chunks, "Error", "Error");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   out.Put("----------------------------------------");
   str = "= Sat.X   ";
   out.Put(str);
   chunks.clear();
   
   try
   {
      out.Put("===== EvaluateBlock()");
      blockType = tp.EvaluateBlock(str);
      out.Validate(blockType, 3);
   
      out.Put("===== ChunkLine()");
      chunks = tp.ChunkLine();
      WriteStringArray(out, chunks, "Error", "Error");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   out.Put("----------------------------------------");
   str = "   = Sat.X";
   out.Put(str);
   chunks.clear();
   
   try
   {
      out.Put("===== EvaluateBlock()");
      blockType = tp.EvaluateBlock(str);
      out.Validate(blockType, 3);
   
      out.Put("===== ChunkLine()");
      chunks = tp.ChunkLine();
      WriteStringArray(out, chunks, "Error", "Error");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   out.Put("----------------------------------------");
   str = "   = ";
   out.Put(str);
   chunks.clear();
   
   try
   {
      out.Put("===== EvaluateBlock()");
      blockType = tp.EvaluateBlock(str);
      out.Validate(blockType, 3);
   
      out.Put("===== ChunkLine()");
      chunks = tp.ChunkLine();
      WriteStringArray(out, chunks, "Error", "Error");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   MessageInterface::SetLogFile("../../TestTextParser/GmatLog.txt");
   MessageInterface::ShowMessage
      ("================================================== TestTextParser\n");
   
   TextParser tp;
   StringArray theCommandList;
   
   theCommandList.push_back("Propagate");
   theCommandList.push_back("Maneuver");
   theCommandList.push_back("BeginFiniteBurn");
   theCommandList.push_back("EndFiniteBurn");
   theCommandList.push_back("BeginScript");
   theCommandList.push_back("EndScript");
   theCommandList.push_back("Target");
   theCommandList.push_back("Achieve");
   theCommandList.push_back("Vary");
   theCommandList.push_back("If");
   theCommandList.push_back("EndIf");
   theCommandList.push_back("While");
   theCommandList.push_back("EndWhile");
   theCommandList.push_back("For");
   theCommandList.push_back("EndFor");
   theCommandList.push_back("Save");
   theCommandList.push_back("Report");
   
   tp.Initialize(theCommandList);
   
//    TestBlockType(tp, out);
//    TestEvaluateDefinitionBlock(tp, out);
//    TestEvaluateAssignmentBlock(tp, out);
//    TestDecomposeBlock(tp, out);
//    TestDecompose(tp, out);
//    TestSeparateBrackets(tp, out);
//    TestSeparateSpaces(tp, out);
//    TestSeparateDots(tp, out);
//    TestReportCommand(tp, out);
//    TestSaveCommand(tp, out);
//    TestEndIfCommand(tp, out);
//    TestBeginFiniteBurnCommand(tp, out);
//    TestForCommand(tp, out);
//    TestWhileCommand(tp, out);
//    TestIfCommand(tp, out);
//    TestAchieveCommand(tp, out);
//    TestVaryCommand(tp, out);
//    TestPropagateCommand(tp, out);
   TestParsePropagateCommand(tp, out);
//    CheckErrors(tp, out);
   
   return 0;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string outPath = "../../TestTextParser/";
   std::string outFile = outPath + "TestTextParserOut.txt";   
   TestOutput out(outFile);
   
//    char *buffer;
//    buffer = getenv("OS");
//    if (buffer  != NULL)
//       printf("Current OS is %s\n", buffer);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of TextParser!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
