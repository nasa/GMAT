TLE Propagator for the General Mission Analysis Tool (GMAT)
-----------------------------------------------------------
Provided by Thinking Systems, Inc.
www.thinksysinc.com

Licensed under the Apache 2.0 License

Release:  R2020a
Status:   Beta.  This component still needs testing and final debugging. 

Platforms tested:
   Windows 10
   Ubuntu 20.04
   Centos 7
   Red Hat 7
   OS X 10.14

License:  

This GMAT Plugin component is licensed under the Apache 2 license.

This component is an open source project.  The project is hosted on GitLab:

   https://gitlab.com/thinkingsystems/gmat-tle-propagator


Installation
------------
The TLE Propagator is provided as a GMAT plugin component for Windows, 
Linux, and Mac systems.  Installation is straightforward:

  + Unpack the TLE Propagator package.  If you are reading this file, you 
    probably did this already.

  + Move or copy the "plugins/thinksys" folder into the GMAT "plugins" folder

    Thinking Systems recommends placing the shared library in a subfolder 
    of the GMAT plugins folder to distinguish the plugins built outside of
    the GMAT installation from those included in the build from NASA.  The
    default folder for plugins from Thinking Systems is "plugins/thinksys."  

  + Move or copy the "samples" folder into the GMAT folder, adding samples 
    for this plugin.

  + Move or copy the "TLE" folder into the GMAT folder, adding element sets 
    needed to run the samples to your system.

  + Edit the GMAT startup file, gmat_startup_file.txt, located in the GMAT 
    bin folder, adding the line

        PLUGIN = ../plugins/thinksys/libTLEPropagator

    to the file, adjusting the path if needed.  Mac users that install the 
    propagator outside of the application bundle will need a line referencign 
    the external folder.  For example, if a plugins/thinksys folder at the level 
    of the GMAT bin folder is used, add this line instead:
    
        PLUGIN = ../../../../plugins/thinksys/libTLEPropagator

    The GMAT startup file includes a section labeled "# External plugins."   
    This section provides a convenient location for plugins built outside of
    the standard GMAT plugin components.

Testing the Installation
------------------------
Once installed, the scripts in the samples/NeedTLE Propagator folder provide a 
set of examples that can test the installation.  These scripts also provide 
examples illustrating the use of the new component.
