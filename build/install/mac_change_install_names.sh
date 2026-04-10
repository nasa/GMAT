#!/bin/sh
# Change the install names of specified Mac dynamic libraries
# Arguments:
#  ${1}: library location, typically GMAT bin directory
#  ${2}: library name wildcard, typically "libXXX*.dylib"
#  ${3}: old install path, typically library's lib directory
#  ${4}: new install path, typically "@rpath"
#  ${5+}: Additional GMAT binaries that use the libraries, e.g. Gui

# Find all specified dylibs
libnames=`cd ${1} ; find ${2} 2> /dev/null`
if [[ -z "$libnames" ]]; then
  exit
fi

# Collect all changes from old name to new name
changes=''
for i in ${libnames} ; do
  changes="${changes} -change ${3}/${i} ${4}/${i}"
done

# Change install names of libraries
for i in ${libnames} ; do
  install_name_tool ${changes} -id ${4}/${i} ${1}/${i}
done

# Change library names inside GMAT binaries
for i in ${@:5}; do
  install_name_tool ${changes} ${i}
done
