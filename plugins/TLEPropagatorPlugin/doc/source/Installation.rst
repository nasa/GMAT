********************
Installation and Use
********************

Setup
=====

Using Packaged Builds
---------------------
Starting with GMAT R2020a, Thinking Systems provides a precompiled implementation
of the TLE propagator built for use with each of the binary packages distributed 
on SourceForge.  The precompiled component includes sample scripts and data files
that demonstrate the TLEPropagator running inside of GMAT.

Python Users
^^^^^^^^^^^^
Installation of the component is a straightforward process.  Users that have Python 3 installed need only run the installation script:

   python install.py Path-to-GMAT

where Path-to-GMAT is the path to the GMAT installation.  This procedure adds the TLE propagator and sample scripts to your GMAT installation.  The GMAT startup file is automatically updated to import the TLE propagator library into your GMAT installation.  The original startup file is preserved with the name "gmat_startup_file.txt.orig".

You can omit the startup file editing using the command

   python install.py --no-startup Path-to-GMAT 


Manual Installation
^^^^^^^^^^^^^^^^^^^
Manual installation is a 2-step process: first, copy the files into place, then edit the GMAT startup file to load the plugin..

The complete TLEPropagator package includes a folder named "TLEPropagator."  That folder contains subfolders that match GMAT's folder structure.  Simply copy the contents of that folder into your GMAT installation top level folder:

   cp -R TLEPropagator/* Path-to-GMAT

where Path-to-GMAT is the path to the GMAT installation (e.g. "~/GMAT/R2020a."  Once the files have been copied, add this line to your GMAT startup file:

   PLUGINS = ../plugins/thinksys/TLEPropagator

and the plugin is ready for use.

Building the Plugin
-------------------


Scripting
=========

