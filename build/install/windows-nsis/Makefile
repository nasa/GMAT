# Standard settings recommended by GNU Coding Standards
SHELL = /bin/sh
.SUFFIXES:

# Customization
VERSION = dev

#
# Standard targets
#

all: installer-internal zip-internal installer-public zip-public

clean:
	rm -fr gmat-internal gmat-public *.exe *.zip uninstall.nsh

distclean: clean

mostlyclean: clean

maintainer-clean: clean

#
# Public targets
#

internal: installer-internal zip-internal

public: installer-public zip-public

assemble: assemble-internal assemble-public

assemble-internal: gmat-internal/GMAT/bin/GMAT.exe

assemble-public: gmat-public/GMAT/bin/GMAT.exe

installer: installer-internal installer-public

installer-internal: gmat.nsi assemble-internal
	./genfilelists.sh gmat-internal/GMAT
	cp -p gmat-internal/GMAT/bin/gmat_startup_file.txt \
	    gmat-internal/GMAT/bin/gmat_startup_file.wmatlab.txt
	sed '/libMatlabInterface/s/^\( *PLUGIN\) /#\1/g' \
	    gmat-internal/GMAT/bin/gmat_startup_file.wmatlab.txt \
	    | sed '/libFminconOptimizer/s/^\( *PLUGIN\) /#\1/g' \
	    | unix2dos \
	    > gmat-internal/GMAT/bin/gmat_startup_file.txt
	cmd /c "makensis /DVERSION=$(VERSION) $<"
	rm -f gmat-internal/GMAT/bin/gmat_startup_file.txt
	mv gmat-internal/GMAT/bin/gmat_startup_file.wmatlab.txt \
	    gmat-internal/GMAT/bin/gmat_startup_file.txt

installer-public: gmat.nsi assemble-public
	./genfilelists.sh gmat-public/GMAT
	cp -p gmat-public/GMAT/bin/gmat_startup_file.txt \
	    gmat-public/GMAT/bin/gmat_startup_file.wmatlab.txt
	sed '/libMatlabInterface/s/^\( *PLUGIN\) /#\1/g' \
	    gmat-public/GMAT/bin/gmat_startup_file.wmatlab.txt \
	    | sed '/libFminconOptimizer/s/^\( *PLUGIN\) /#\1/g' \
	    | unix2dos \
	    > gmat-public/GMAT/bin/gmat_startup_file.txt
	cmd /c "makensis /DVERSION=$(VERSION) /DPUBLIC $<"
	rm -f gmat-public/GMAT/bin/gmat_startup_file.txt
	mv gmat-public/GMAT/bin/gmat_startup_file.wmatlab.txt \
	    gmat-public/GMAT/bin/gmat_startup_file.txt

zip: zip-internal zip-public

zip-internal: gmat-internal-win-$(VERSION).zip

zip-public: gmat-win-$(VERSION).zip

#
# Internal targets
#

gmat-internal-win-$(VERSION).zip: gmat-internal/GMAT/bin/GMAT.exe
	rm -f $@
	cd gmat-internal \
	    && zip -r9 ../$@ GMAT

gmat-win-$(VERSION).zip: gmat-public/GMAT/bin/GMAT.exe
	rm -f $@
	cd gmat-public \
	    && zip -r9 ../$@ GMAT

gmat-internal/GMAT/bin/GMAT.exe:
	./assemblegmat.sh -d gmat-internal/GMAT -t full-release

gmat-public/GMAT/bin/GMAT.exe:
	./assemblegmat.sh -d gmat-public/GMAT -t public-release
