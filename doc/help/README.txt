-----------------------------------------------------------------
Prerequisites
-----------------------------------------------------------------
To generate the GMAT help documentation, the following tools must be installed
and present on the system path for the current user:

* MSYS
    (bundled with MinGW: http://sourceforge.net/mingw)

* Java
    (http://java.com)

* HTML Help Workshop (for Windows Help output)
    (http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=21138)


-----------------------------------------------------------------
Usage
-----------------------------------------------------------------
Windows:
    1. Open the MinGW Shell (Start > All Programs > MinGW)
    2. Navigate to the folder containing Makefile.
    3. Run "make".

Mac/Linux:
    1. Open a shell.
    2. Navigate to the folder containing Makefile.
    3. Run "make".

You can perform specific actions by running:
    "make <target>"

Some common targets are (see the Makefile for the full list):
    all:        build everything
    chm:        build the HTML Help docs
    html:       build the HTML docs
    pdf:        build the PDF docs
    help:       build only the user guide
    training:   build only the training manual
    <filename>: build a specific file (e.g. help.chm, help-letter.pdf)
    clean:      delete all automatically-created files
    validate:   validate the DocBook XML
