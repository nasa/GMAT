#!/bin/sh

# Generate the list of files for inclusion in gmat.nsi (both in the "GMAT" and
# "Uninstaller" sections).
# Expects the path to the GMAT directory as the first argument.

# install files (not working for now)
# find gmat \
    # \( -type d -printf 'SetOutPath "$INSTDIR/%p"\n' \) , \
    # \( -type f -printf 'File "${SRCDIR}/%p"\n' \) \
    # | sed 's/\//\\/g' \
    # | sed 's/\\gmat//' \
    # > install.nsh


# delete files
find $1 -type f \
    | sed 's|^'$1'|Delete "\$INSTDIR|' \
    | sed 's|/|\\|g' \
    | sed 's|$|"|' \
    | unix2dos \
    > uninstall.nsh

# delete directories
find $1 -depth -type d \
    | sed 's|^'$1'|RMDir "\$INSTDIR|' \
    | sed 's|/|\\|g' \
    | sed 's|$|"|' \
    | unix2dos \
    >> uninstall.nsh
