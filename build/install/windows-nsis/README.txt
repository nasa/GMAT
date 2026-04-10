This folder contains files that build GMAT's Windows installer and .zip bundles.
The current installer is based on NSIS (http://nsis.sf.net), but a new installer
based on MSI (using WiX) is a long-term goal.

This script bundles files from the \\mesa-file\595\GMAT network share and the
central git repository on gs-mesajade.gsfc.nasa.gov. It does not use the local
repository at all, except for the packaging scripts themselves.


Prerequisites
-------------
To build the installer, you need:

1. MSYS (installed with MinGW: http://sf.net/projects/mingw)
2. zip: in the MinGW Installation Manager, install "msys-zip" (bin package)
2. Git command-line tools (git.exe) in your system path
3. NSIS (makensis.exe) in your system path


Configuration
-------------
The gmat.nsi file contains installer properties like the output filename and
project URLs.

The ../assemblegmat.sh script actually assembles the active copy of GMAT. That
file should be checked to make sure it is fetching all the correct files.


Building the installer
----------------------
From the MinGW/MSYS shell, run:

make VERSION=<version string>

This will assemble the latest copy of GMAT and build an installer file and .zip
bundles from it.


Available make targets
----------------------
The following make targets are available:

	all: build everything

	internal: build installer & zip for internal version
	public: build installer & zip for public version

	assemble: assemble the files, but don't package anything
	installer: make the installer
	zip: make the zip bundle

	assemble-internal: assemble only, internal only
	(etc.)