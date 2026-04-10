
Welcome to the wonderful world of GMAT!
 
GMAT is a software system for space mission design, navigation, and optimization 
applicable to missions anywhere in the solar system ranging from low Earth orbit 
to lunar, libration point, and deep space missions. The system contains high-
fidelity space system models, optimization and targeting, built-in scripting and 
programming infrastructure, and customizable plots, reports and data products 
that enable flexible analysis and solutions for custom and unique applications. 
GMAT can be driven from a fully featured, interactive Graphical User Interface 
(GUI) or from a custom script language. 
 
The system is implemented in ANSI standard C++ using an Object Oriented 
methodology, with a rich class structure designed to make new features easy to 
incorporate. GMAT has been used extensively as a design tool for many missions 
including ARTEMIS, LCROSS, and LRO and for operational support of ACE, RST, 
SDO, SOHO, SOLAR-1, TESS, and WIND.
 
-----------------------------------------------------------------------
                          License and Copyright
-----------------------------------------------------------------------
 
“NASA Docket No. GSC-19468-1, identified as “General Mission Analysis Tool 
(GMAT) Version R2026a” 
 
Copyright (c) 2002-2026 United States Government as represented by the
Administrator of the National Aeronautics and Space Administration.
All Rights Reserved.
 
Licensed under the Apache License, Version 2.0 (the "License");
You may not use this file except in compliance with the License.
You may obtain a copy of the License at:
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
express or implied.   See the License for the specific language
governing permissions and limitations under the License.
 
-----------------------------------------------------------------------
                           Contact Information
-----------------------------------------------------------------------
 
For general project info see: https://gmat.atlassian.net/wiki/spaces/GW/overview
 
For source code and application distributions see:
- https://sourceforge.net/projects/gmat/ (primary)
- https://github.com/nasa/GMAT/ (secondary)
 
For other comments and questions, email: gmat-developers@lists.sourceforge.net
 
-----------------------------------------------------------------------
                      Credits and Acknowledgments
-----------------------------------------------------------------------
GMAT uses:
- wxWidgets 3.2.9 (http://www.wxwidgets.org/)
- TSPlot (http://sourceforge.net/projects/tsplot/)
- SOFA (http://www.iausofa.org/)
- Apache Xerces (http://xerces.apache.org)
- JPL SPICE (https://naif.jpl.nasa.gov/naif/) 
- OpenFramesInterface (https://gitlab.com/EmergentSpaceTechnologies/OpenFramesInterface)
- Boost (http://www.boost.org/)
- f2c (http://www.netlib.org/f2c)
- MSISE 1990 Density Model (https://doi.org/10.1029/90JA02125)
- NRLMSISE-00 Density Model (https://map.nrl.navy.mil/map/pub/nrl/NRLMSIS/NRLMSISE-00/)
- IRI 2007 Ionosphere Model (https://irimodel.org/)
 
Planetary images are courtesy of: 
- JPL/Caltech/USGS (http://maps.jpl.nasa.gov/)
- Celestia Motherlode (http://www.celestiamotherlode.net/)
- Bjorn Jonsson (https://bjj.mmedia.is/)
- NASA World Wind (http://worldwind.arc.nasa.gov/)
 
Some icons are courtesy of Mark James
 
-----------------------------------------------------------------------
                      Installation and Configuration
-----------------------------------------------------------------------
 
WINDOWS
 
The GMAT Windows distribution is a zip file. When unzipped, locate the bin 
directory then select GMAT.exe to open GMAT. 
 
LINUX
 
There are two precompiled Linux distributions of GMAT, packaged as compressed TAR 
files, ready for expansion and use. The precompiled releases were built on 
Ubuntu 22.04 LTS and on Centos 7. Linux users on releases 
compatible with those platforms can download the tarball file and uncompress it 
into place using the command

    tar -zxf <TarballPackageName>
 
where <TarballPackageName> is either gmat-ubuntu-x64-R2026a.tar.gz or 
gmat-rhel7-x64-R2026a.tar.gz.

Linux building was performed on the indicated distributions. The packaged 
executable files have been tested on Ubuntu 24.04 LTS, and on Red Hat Enterprise 
Linux 8, and have been run (but not exhaustively tested) on Ubuntu 22.04 LTS and 
Red Hat Enterprise Linux 7 and 9.
 
MACOS
 
The GMAT macOS R2026a distribution is provided as a universal DMG file and is 
compatible with macOS 14.5 (Sonoma) and above on Intel or Apple Silicon Macs.
 
After downloading and mounting (i.e. opening) the dmg file, installation of GMAT 
is as simple as copying the resulting "GMAT <version>" folder to one of the 
following two locations:

  - System-wide /Applications folder. This requires admin privileges.
  - User's $HOME/Applications folder. Admin privileges are not needed.
 
Copying GMAT to any other location may cause issues with loading certain plugins,
such as the MatlabInterface plugin.
 
BUILDING FROM SOURCE
 
GMAT is distributed in source form as well, and can be compiled on macOS,
Linux and Windows. Build instructions for GMAT can be found at
https://gmat.atlassian.net/wiki/spaces/GW/pages/380273355/Compiling+GMAT+CMake+Build+System.
 
USING GMAT OPTIMAL CONTROL / CSALT
 
Using the GMAT Optimal Control capability -- implemented in the EMTGModels and 
CsaltInterface plugins -- requires additional installation steps. See the 
"Software Organization and Compilation" section of the GMAT Optimal Control user 
guide in gmat/docs/GMAT_OptimalControl_Specification.pdf for complete instructions.
 
-----------------------------------------------------------------------
                           Running GMAT
-----------------------------------------------------------------------
 
WINDOWS
 
On Windows, locate your GMAT directory. From there, open the bin directory, then 
double click GMAT.exe to open GMAT.
 
Additionally, you can open a Command Prompt and change directories to the GMAT 
bin folder. Then type "GMAT.exe" and hit enter. This will open the GMAT GUI.

If you experience a Microsoft Defender SmartScreen when attempting to open GMAT,
right click GMAT.exe and select “Properties” from the context menu, then unblock 
the application by checking the “Unblock” checkbox and hitting “Apply”. 

To use the GMAT Console, open a Command Prompt and change directories to the 
GMAT bin folder. The GMAT command line program is executed by running the 
GmatConsole application in that folder:
 
GmatConsole.exe
 
LINUX
 
On Linux, open a terminal window and change directories to the GMAT bin folder. 
The GMAT command line program is executed by running the GmatConsole application
in that folder:
 
   ./GmatConsole
 
The Beta GUI can be run using the same terminal window by running Gmat_Beta:
 
   ./GMAT_Beta
 
The Linux GUI places its libraries in the GMAT lib folder. Users of the 
OpenFramesInterface component may see that texture maps are not displayed 
correctly. If that happens, launch the GMAT GUI application with a load library 
path, like this:  

   LD_LIBRARY_PATH=../lib:$LD_LIBRARY_PATH ./GMAT_Beta

Linux users can create a launcher for either the command line application or the 
GUI application by following the instructions for that process for their Linux 
distribution.
 
MACOS
 
On macOS, the Beta GUI application can be run by double-clicking the 
GMAT-R2026a_Beta.app in the Finder, or by using the ‘open’ Terminal command, 
executed in the bin folder:
  
   open GMAT-R2026a_Beta.app 
 
See note below if you have issues. The GmatConsole command line program is 
executed via the Terminal, from the bin folder:
 
   ./GmatConsole 
 
Please see the GMAT User Guide for important instructions on how to use MATLAB,
Python, SNOPT7, and gfortran for each GMAT distribution.
 
-----------------------------------------------------------------------
                           User Information
-----------------------------------------------------------------------
 
User docs are available in pdf and html format. The pdf documentation
is distributed in letter and A4 size in this package. The files are
located here: /doc/GMAT_UsersGuide.pdf, /doc/help/help-letter.pdf, and 
/doc/help/help-a4.pdf
 
For new users, see the Getting Started and Tour of GMAT sections first,
then take the tutorials. The tutorials are included in print versions
in the help documents, and are available in video form here:
https://www.youtube.com/channel/UCt-REODJNr2mB3t-xH6kbjg
 
For a list of new functionality, compatibility changes, and known issues,
see the Release Notes section in the User's Guide. 
 
-----------------------------------------------------------------------
                         Developer Information
----------------------------------------------------------------------
 
Source code is available here:
https://sourceforge.net/p/gmat/git/
 
Compilation instructions are available here: 
https://gmat.atlassian.net/wiki/spaces/GW/pages/380273355/Compiling+GMAT+CMake+Build+System
 
Design Documentation is available at the links below:
https://gmat.atlassian.net/wiki/spaces/GW/pages/380273312/Design+Documents
https://gmat.atlassian.net/wiki/spaces/GW/pages/380273320/How+To+Write+New+Components
 
You can sign up for mailing lists here:
https://sourceforge.net/p/gmat/mailman/
