/**
 * \mainpage GMAT C-Interface Overview
 *
 * This documentation provides a description of the contents of GMAT's
 * C-Interface plugin module.
 *
 * The GMAT C Interface provides a set of C-style functions that provide access
 * to core GMAT functionality.  The interface is currently in the early stages
 * of implementation, and provide facilities to start the GMAT engine, load a
 * script file, run the file, access the resulting ODE model, and retrieve
 * derivative data (accelerations, mass flow rates, state transition matrices,
 * etc) from that model.
 *
 * The interface provides low level access to GMAT functionality.  Accordingly,
 * model inputs should be made in the GMAT internal formats: states are in a
 * Cartesian body-centered Earth-equatorial coordinate system, epochs are in
 * GMAT's A.1 modified Julian format, and time offsets are in seconds.
 *
 * Goddard's ODTBX project uses a set of MATLAB based wrapper functions to
 * simplify the use of the interface.  This documentation discusses the usage
 * of the interface directly.
 *
 * @todo Joel, do you want to add information about how to use the interface
 * from the MATLAB wrappers?
 *
 * \section Setup Setting up GMAT for C-Interface use
 *
 * In order for GMAT to use the full functionality of the C interface, the
 * interface should be loaded into GMAT as a plug-in at run time.  To do this,
 * add the following line to your startup file:
 *
 * PLUGIN                 = ./libCInterface
 *
 * \section Running Running the interface
 *
 * The interface supplies functions as documented in the CInterfaceFunctions.h
 * header file description.  When using the interface, the user must first
 * load and initialize the GMAT engine in memory, then load a GMAT script file
 * to populate the engine with the objects that the user want to use.  Next the
 * user runs the script to establish internal connections between the member
 * objects in the script.  Once these steps have been run successfully, the
 * user can access the internal objects exposes through the interface.
 *
 * The files CInterfaceTester.h and CInterfaceTester.cpp provide sample code for
 * using the interface to call into GMAT's force model.  The test application
 * coded there runs on Mac and Linux systems.
 */
