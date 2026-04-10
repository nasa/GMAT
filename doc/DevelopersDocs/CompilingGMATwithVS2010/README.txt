These are the LaTeX sources for "Compiling GMAT using Visual Studio 2010".

The sources are platform-independent, but a Makefile exists for Windows using NMAKE.

To build the document:
1. Open a command window that has NMAKE in its path. NMAKE is installed with Visual Studio or Visual C++ Express Edition. You can either add it to your path manually, or open the Visual Studio Command Prompt.
2. Change to the directory containing Makefile.nmake (and this file).
3. Run 'nmake /f Makefile.nmake'

The document will be built in DVI, PS, and PDF formats.

You can run 'nmake /f Makefile.nmake <target>' to perform a specific task, where <target> is one of the following:
    all: build everything (same as without <target> specified)
    pdf: build everything (same as without <target> specified)
    ps: build DVI and PostScript formats
    dvi: build DVI format only
    clean: delete all generated files