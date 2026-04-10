//$Id$
//------------------------------------------------------------------------------
//                                  DecoratedTree
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/12
//
// Developed by Thinking Systems, Inc. to support the internal Unit Test 
// framework.
//
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
/**
 * A custom extension to the wxTree class
 * 
 * A wxWindows based Tree component that adds text data in columns on the right
 * side of the displayed tree.  This component has been built and tested under 
 * wxWindows 2.4.2, running on Windows XP, Linux (RedHat 8, 2.4.20 kernel), and
 * MacIntosh OSX 10.2.  The current list of bugs and enhancements is provided 
 * below.
 *
 * @todo The current form of the DecoratedTree class does not provide methods 
 * to rework the tree after it is built.  This means the tree "decorations" on
 * the right don't get repopulated when the wxTree base changes.
 * 
 * @bug  Adding a decoration string with an index that has not been populated 
 *       results in a segmenttion fault.  The vector should be checked and 
 *       resized if needed when the decoration string is added.
 * 
 * @bug  The indices of the decoration strings is not very intuitive.  Strings
 *       are numbered right to left, top to bottom.
 * 
 * @bug  Under Linux, the box drawing for the component doesn't expand and 
 *       contract correctly in this version of the DecoratedTree.  (It was 
 *       working before, so it just needs a careful walkthrough...)
 */
//------------------------------------------------------------------------------



#ifndef DecoratedTree_hpp
#define DecoratedTree_hpp

#include <wx/wx.h> // inheriting class's header file
#include <wx/treectrl.h> // inheriting class's header file

#include <vector>
#include <string>


/** Enumeration for the types of drawing that can be performed on the tree */
enum treeDecorations {
                        DRAWOUTLINE,
                        DRAWBOXES,
                        BOXCOUNT,
                        BOXWIDTH,
                        PARMCOUNT
                     };


/**
 * This class adds text-based columns on the right of a wxTree component that 
 * can be used to provide additional information about lines in the tree.
 * 
 * The initial use of this component is to provide status information about the 
 * tests run in the tester application.  GMAT will use this component or 
 * something similar to display targeter data -- the number of variables and 
 * goals specified at each command in the mission sequence, for example.
 */
class DecoratedTree : public wxTreeCtrl
{
	public:
		DecoratedTree(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT |
                              wxTR_HIDE_ROOT, // | wxTR_EXTENDED,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = "listCtrl");
		virtual ~DecoratedTree();

        void				Initialize(void);

        // SetModes is used to check the GMAT targeter functionality        
        void 				SetNodes(void);

        void                ExpandAll(void);
        void                ExpandAll(wxTreeItemId root);
        wxTreeItemId        Find(wxString str);
        wxTreeItemId        Find(wxString str, wxTreeItemId root);

        wxTreeItemId        AddItem(wxString txt, wxString parent = "");
        
        // Configuration methods
        void                SetParameter(int id, int value);
        int                 GetParameter(int id);
        void                SetString(int line, std::string value);

        // Message overrides
        virtual void        OnPaint(wxPaintEvent& ev);
        int offset;

    protected:
        /// Indicates if the tree has been setup
        bool                initialized;
        /// Flag indicating if a line should be drawn aroung the tree node
        bool                drawOutline;
        /// Flag indicating if a box is drawn aroung the decorations
        bool                drawBoxes;
        /// Number of decorations added to the right side of the tree
        int                 boxCount;
        /// Width of each of the decoration boxes
        int                 boxWidth;
        /// The text placed in the decorations
        std::vector<std::string *> 
                            boxData;
        /// The height of each of the tree nodes
        int                 rowHeight;
        /// Internal variable used when drawing decorations
        int                 lineNumber;
                
        void                DrawOutline(wxTreeItemId id);
        void                DrawBoxes(wxTreeItemId id);
        
        
        DECLARE_EVENT_TABLE()
};

#endif // DecoratedTree_hpp

