/**
 * \mainpage GMAT Base Code Overview
 *
 * This documentation provides a description of the contents of the core engine
 * code for the General Mission Analysis Tool, GMAT.
 *
 * GMAT's engine is built into two shared libraries that provides all of the core
 * functionality of the system, as described in the opening chapters to the GMAT
 * Architectural Specification.  The libraries, libGmatUtil and libGmatBase, contain the
 * classes,structures, and other elements described in that documentation.
 * Interested readers should review the Architectural Specification to gain an
 * understanding of how the components described in this documentation behave.
 *
 * The documentation presented here describes the low level structures and data
 * elements of GMAT's classes and functions contained in the engine and the core
 * user configured elements comprising the resources and mission control
 * sequence.  By studying this documentation, you can learn how GMAT's classes
 * work and interact to accomplish mission modeling tasks.  Programmers that
 * want to extend GMAT's functionality should read this documentation while
 * writing their own classes derived from the GMAT code so that they can ensure
 * that derived elements used in custom code conforms to GMAT's interfaces.
 *
 * This documentation comprises the calls available to the GMAT Application 
 * Programming Interface (API) for the components of the core GMAT system.  
 * Every member of all classes in GMAT's base code has an entry in this 
 * documentation, along with the documentation extracted from the code.  The 
 * documentation in the code conforms to the needs of the Doxygen open source 
 * documentation system.  This conformity is enforced through the GMAT coding 
 * standards, as described in the GMAT Style Guide.
 *
 * The wxWidgets based GMAT graphical user interface (GUI) is documented in a
 * separate set of pages.
 */
