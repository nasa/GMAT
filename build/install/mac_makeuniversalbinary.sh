#!/bin/zsh

# mac_makeuniversalbinary.sh <intel_binarypath> <silicon_binarypath>
#     intel_binarypath: Absolute path to Intel binary
#                       This file will not be altered
#     silicon_binarypath: Absolute path to Silicon binary
#                         This file will be converted to a universal binary
#
# This script converts an Apple Silicon version of the MatlabInterface plugin into a universal binary
# by merging in a provided Intel version of the same plugin. This script is commonly used to
# merge together the GMAT MatlabInterface plugin, which must be compiled as separate Intel/Silicon
# binaries as described in https://gmat.atlassian.net/browse/GMT-8560.
# Author: Ravi Mathur, Intuitive Machines

# 1. Check if exactly 2 arguments were provided
if [[ $# -ne 2 ]]; then
    print "Usage: $0 <intel_binarypath> <silicon_binarypath>"
    exit 1
fi

# Assign arguments to variables
local intel_file="$1"
local silicon_file="$2"
local temp_file="$2_universal"

# 2. Check if input files exist
if [[ ! -f "$intel_file" || -L "$intel_file" ]]; then
  print "Error: Intel input must be a regular file: '$intel_file'"
  exit 1
fi

if [[ ! -f "$silicon_file" || -L "$silicon_file" ]]; then
  print "Error: Silicon input must be a regular file: '$silicon_file'"
  exit 1
fi

# Function to check if a binary is already universal
is_universal() {
  # zsh: -q in grep stays quiet; returns 0 if found
  file "$1" 2>/dev/null | grep -q "universal binary"
}

# Function to check if a binary has Intel architecture
has_intel() {
  # zsh: -q in grep stays quiet; returns 0 if found
  file "$1" 2>/dev/null | grep -q "x86_64"
}

# Function to check if a binary has Silicon architecture
has_silicon() {
  # zsh: -q in grep stays quiet; returns 0 if found
  file "$1" 2>/dev/null | grep -q "arm64"
}

# 3. Check input file validity
if is_universal "$intel_file"; then
  print "Error: Intel input is a universal binary. Please provide an Intel-only binary. '$intel_file'"
  exit 1
fi

if has_silicon "$intel_file"; then
  print "Error: Intel input is a Silicon binary. Please provide an Intel-only binary. '$intel_file'"
  exit 1
fi

if is_universal "$silicon_file"; then
  print "Error: Silicon input is a universal binary. Please provide a Silicon-only binary. '$silicon_file'"
  exit 1
fi

if has_intel "$silicon_file"; then
  print "Error: Silicon input is an Intel binary. Please provide a Silicon-only binary. '$silicon_file'"
  exit 1
fi

# 4. Merge the binaries
print "Merging '$intel_file' and '$silicon_file' into '$temp_file'..."

if lipo -create -output "$temp_file" "$intel_file" "$silicon_file"; then
  if mv "$temp_file" "$silicon_file"; then
    print "Success! Verification:"
    file "$silicon_file"
  else
    print "Error: universal file could not be renamed"
    print "$temp_file"
  fi
else
  print "Error: lipo failed to merge the files."
  exit 1
fi
