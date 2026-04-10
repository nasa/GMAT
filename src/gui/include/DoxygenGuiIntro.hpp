/**
 * \mainpage GMAT GUI Code Overview
 *
 * This documentation provides a description of the contents of the graphical
 * user interface (GUI) code for the General Mission Analysis Tool, GMAT.
 *
 * GMAT's GUI is built into an application that provides system users with
 * access to all of the resources and commands needed to model a Spacecraft
 * mission in GMAT.  This application can be built for Windows, Mac, and Linux
 * systems using the code documented here.  The resulting application interfaces
 * the core GMAT engine, contained in shared libraries named libGmatUtil and 
 * libGmatBase, with a wxWidgets based GUI documented here.
 *
 * The documentation presented here describes the GUI structures and data
 * elements that access GMAT's classes and functions contained in the engine
 * and the core user configured elements comprising the resources and mission
 * control sequence.  The interface presented to users through this code
 * provides the means needed to interact with these elements to create mission
 * elements, design a mission timeline that uses those elements, and then run
 * the timeline to model a mission, producing both graphical output and reports
 * of the mission execution based on the user's specifications.
 *
 * By studying this documentation, you can learn how GMAT's GUI works and
 * interacts with the core code to accomplish mission modeling tasks.
 * Programmers that want to extend GMAT's functionality should read this
 * documentation while writing their own extensions to the GMAT code so that
 * they can ensure that derived elements used in custom code conforms to GMAT's
 * interfaces.
 *
 * This documentation comprises low level documentation for
 * the components of the GMAT wxWidgets based GUI.  Every member of all classes
 * in GMAT's GUI code has an entry in this documentation, along with the
 * documentation extracted from the code.  The documentation in the code
 * conforms to the needs of the Doxygen open source documentation system.  This
 * conformity is enforced through the GMAT coding standards, as described in the
 * GMAT Style Guide.
 *
 * The GMAT engine is documented in a separate set of pages.
 */
