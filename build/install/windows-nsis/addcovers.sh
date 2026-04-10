#!/bin/sh

# This script merges the User Guide cover into the PDF.
#
# Requirements: sejda-console (http://www.sejda.org/)

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-A4-Trimmed.pdf gmat-internal/GMAT/docs/help/help-a4.pdf -o gmat-internal/GMAT/docs/help/help-a4-new.pdf -s all:all:
mv gmat-internal/GMAT/docs/help/help-a4-new.pdf gmat-internal/GMAT/docs/help/help-a4.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-Letter-Trimmed.pdf gmat-internal/GMAT/docs/help/help-letter.pdf -o gmat-internal/GMAT/docs/help/help-letter-new.pdf -s all:all:
mv gmat-internal/GMAT/docs/help/help-letter-new.pdf gmat-internal/GMAT/docs/help/help-letter.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-A4-Trimmed.pdf gmat-public/GMAT/docs/help/help-a4.pdf -o gmat-public/GMAT/docs/help/help-a4-new.pdf -s all:all:
mv gmat-public/GMAT/docs/help/help-a4-new.pdf gmat-public/GMAT/docs/help/help-a4.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-Letter-Trimmed.pdf gmat-public/GMAT/docs/help/help-letter.pdf -o gmat-public/GMAT/docs/help/help-letter-new.pdf -s all:all:
mv gmat-public/GMAT/docs/help/help-letter-new.pdf gmat-public/GMAT/docs/help/help-letter.pdf
