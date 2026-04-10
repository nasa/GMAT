# Makefile for the GMAT User Guide and Training Manual.
# On Windows, requires an MSYS environment.
# This file generally follows the rules of the GNU Makefile Conventions.

# portability boilerplate
SHELL = /bin/sh
.SUFFIXES:

# directories
vpath %.xml $(srcdir)
srcdir = ./src
configdir = ./config
contribdir = ../build/contrib
docbookxsldir = $(contribdir)/docbook-xsl-ns-1.78.1
fopdir = $(contribdir)/fop-1.1

# tools
JAVAFLAGS = -Xmx1024M
FOP = $(JAVA) -cp $(contribdir)/offo-hyphenation-binary/fop-hyph.jar -Dfop.home=$(fopdir) $(JAVAFLAGS) -jar $(fopdir)/build/fop.jar -c $(configdir)/fop.xconf
JAVA = java
JING = $(JAVA) $(ALL_JAVAFLAGS) -jar $(contribdir)/jing/bin/jing.jar
XALAN = $(JAVA) -cp $(contribdir)/xalan-j/serializer.jar:$(contribdir)/xalan-j/xalan.jar:$(contribdir)/xalan-j/xercesImpl.jar:$(contribdir)/xml-apis.jar:$(docbookxsldir)/extensions/xalan27.jar $(ALL_JAVAFLAGS) org.apache.xalan.xslt.Process

# flags
ALL_JAVAFLAGS = -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=org.apache.xerces.parsers.XIncludeParserConfiguration $(JAVAFLAGS)
ALL_XALANFLAGS = -PARAM use.extensions 1 \
	  -PARAM fop1.extensions 1 \
	  -PARAM graphicsize.extension 1 \
	  -PARAM tablecolumns.extension 1 \
	  -PARAM appendix.autolabel 0 \
	  -PARAM part.autolabel 0 \
	  -PARAM refentry.generate.name 0 \
	  -PARAM refentry.generate.title 1 \
	  $(XALANFLAGS)

# source files
helpsrc = $(srcdir)/help.xml
srcfiles = $(srcdir)/*.xml
fostyle = ./xform/fo.xsl

# Rules
all: help

clean:
	rm -rf *.chw *.fo *.html *.pdf *.xml *.xsl files html

distclean: clean
	rm -rf help
mostlyclean: clean
maintainer-clean: clean

validate-pre: $(helpsrc)
	$(JING) $(contribdir)/docbook/docbookxi.rng $(helpsrc)

dist: all
	mkdir help
	cp -pR \
	    help-a4.pdf \
	    help-letter.pdf \
	    help.html \
	    html \
	    files \
	    help

help: help-pdf \
    help-html-single \
    help-html-multiple

html: help-html-single \
    help-html-multiple

pdf: help-pdf

help-pdf: help-pdf-letter \
    help-pdf-a4

help-pdf-letter: help-letter.pdf

help-pdf-a4: help-a4.pdf

help-html-single: help.html

help-html-multiple: html/index.html

help-pre.xml: validate-pre $(helpsrc) $(srcfiles)
	$(XALAN) \
	    -PARAM profile.condition ug \
	    -IN $(helpsrc) \
	    -XSL $(docbookxsldir)/profiling/profile.xsl \
	    -OUT $@
	$(JING) $(contribdir)/docbook/docbookxi.rng $@

training-pre.xml: validate-pre $(helpsrc) $(srcfiles)
	$(XALAN) \
	    -PARAM profile.condition tm \
	    -IN $(helpsrc) \
	    -XSL $(docbookxsldir)/profiling/profile.xsl \
	    -OUT $@
	$(JING) $(contribdir)/docbook/docbookxi.rng $@

help-letter.pdf: help-letter.fo $(fopconf)
	$(FOP) $< $@


help-a4.pdf: help-a4.fo $(fopconf)
	$(FOP) $< $@

help-letter.fo: help-pre.xml fo.titlepage.templates.xsl $(fostyle)
	$(XALAN) \
	    -PARAM admon.graphics.path $(docbookxsldir)/images/ \
	    -PARAM img.src.path src/ \
	    -PARAM paper.type USletter \
	    -IN help-pre.xml \
	    -XSL $(fostyle) \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)

help-a4.fo: help-pre.xml fo.titlepage.templates.xsl $(fostyle)
	$(XALAN) \
	    -PARAM admon.graphics.path $(docbookxsldir)/images/ \
	    -PARAM img.src.path src/ \
	    -PARAM paper.type A4 \
	    -IN help-pre.xml \
	    -XSL $(fostyle) \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)

help.html: help-pre.xml files/style.css
	$(XALAN) \
	    -PARAM html.stylesheet files/style.css \
	    -PARAM make.valid.html 1 \
	    -PARAM use.id.as.filename 1 \
	    -PARAM variablelist.as.table 1 \
	    -IN help-pre.xml \
	    -XSL $(docbookxsldir)/html/docbook.xsl \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)

html/index.html: help-pre.xml files/style.css
	$(XALAN) \
	    -PARAM base.dir html/ \
	    -PARAM img.src.path ../ \
	    -PARAM html.stylesheet ../files/style.css \
	    -PARAM chunk.quietly 1 \
	    -PARAM make.valid.html 1 \
	    -PARAM use.id.as.filename 1 \
	    -PARAM variablelist.as.table 1 \
	    -IN help-pre.xml \
	    -XSL $(docbookxsldir)/html/chunkfast.xsl \
	    $(ALL_XALANFLAGS)

files/style.css: $(srcdir)/files/style.css
	mkdir -p files
	cp -av src/files/* files


htmlhelp.titlepage.templates.xsl: xform/htmlhelp.titlepage.templates.xml
	$(XALAN) \
	-IN $< \
	-XSL $(docbookxsldir)/template/titlepage.xsl \
	-OUT $@

fo.titlepage.templates.xsl: xform/fo.titlepage.templates.xml
	$(XALAN) \
	-IN $< \
	-XSL $(docbookxsldir)/template/titlepage.xsl \
	-OUT $@

%.pdf: %.fo
	$(FOP) $< $@

%.fo: %.xml
	$(XALAN) \
	    -PARAM admon.graphics.path $(docbookxsldir)/images/ \
	    -PARAM img.src.path src/ \
	    -PARAM paper.type USletter \
	    -IN $< \
	    -XSL $(fostyle) \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)
	
