#!/bin/sh

# This script assembles a complete executable copy of GMAT from the network,
# ready for packaging.
#
# Usage: $0 [option] ...
# Options:
#   -d dest       Place files in specified directory dest (default: current
#                 directory)
#   -t type       Assemble a particular type of distribution:
#                 full: everything included (default)
#                 public: publicly-releasable version
#                 full-release: full excluding pre-release items
#                 public-release: public excluding pre-release items
#
# Prerequisites:
#   standard POSIX utilities

# Initializations
TYPE=full

usage() {
cat <<END
Usage: $0 [option] ...
Options:
  -d dest       Place files in specified directory dest (default: gmat-<date>)
  -t type       Assemble a particular type of distribution:
                full: everything included (default)
                public: publicly-releasable version
                full-release: full excluding pre-release items
                public-release: public excluding pre-release items
END
}

# Argument handling
while getopts d:t: o
do
    case "$o" in
        d) dest="$OPTARG";;
        t) TYPE=`echo "$OPTARG" | tr '[:upper:]' '[:lower:]'`;;
        ?) usage; exit 1;;
    esac
done

# File sources
gmatshare='//mesa-file.gsfc.nasa.gov/595/GMAT'
buildpath="$gmatshare/Builds/windows/VS2017_build_64/LatestCompleteVersion"

# Validate build
exepath="$buildpath/bin/GMAT.exe"
if [ ! -e "$exepath" ]
then
    echo "Invalid build: cannot find $exepath"
    exit 2;
fi

# Create destination directory
if [ -z "$dest" ]
then
    dest="gmat-$buildname"
fi
if [ ! -e "$dest" ]
then
    mkdir -p "$dest"
fi
    
# Copy build files
cp -pRv "$buildpath"/* "$dest"

# Remove Windows hidden files
find "$dest" -iname thumbs.db -delete
    
# remove GmatConsole.exe
rm -f "$dest/bin/GmatConsole.exe"

# remove proprietary data if necessary
if [ $TYPE = 'public' -o $TYPE = 'public-release' ]
then
    # Mars-GRAM data
    rm -rf "$dest/data/atmosphere/MarsGRAM2005"

    # proprietary plugins
    rm -rf "$dest"/plugins/proprietary/*
    sed '/plugins\/proprietary/s/^\( *PLUGIN\) /#\1/g' \
        "$dest"/bin/gmat_startup_file.txt \
        > "$dest"/bin/gmat_startup_file.public.txt
    unix2dos "$dest"/bin/gmat_startup_file.public.txt
    mv "$dest"/bin/gmat_startup_file.public.txt \
        "$dest"/bin/gmat_startup_file.txt
fi

# Remove git files
find "$dest" -iname .gitignore -delete
