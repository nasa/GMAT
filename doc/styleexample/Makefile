# Makefile for the GMAT Style Example.
# Must be run in an MSYS environment on Windows.

# Common variables
contribdir = ../build/contrib
JAVA = java

# Jing
JING = $(JAVA) -jar $(contribdir)/jing/bin/jing.jar

# Xalan
XALAN = $(JAVA) $(XALANDEFS) -cp $(XALANCP) org.apache.xalan.xslt.Process
XALANCP = $(contribdir)/xalan-j/serializer.jar:$(contribdir)/xalan-j/xalan.jar:$(contribdir)/xalan-j/xercesImpl.jar:$(contribdir)/xml-apis.jar:$(contribdir)/docbook-xsl-ns/extensions/xalan27.jar
XALANDEFS=-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=org.apache.xerces.parsers.XIncludeParserConfiguration

# FOP
FOP = java -Dfop.home=$(contribdir)/fop -jar $(contribdir)/fop/build/fop.jar -c $(FOPCONF)
FOPCONF = config/fop.xconf

# Rules
all: pdf

pdf: GmatStyleExampleA4.pdf

clean:
	rm -f *.fo GmatStyleExampleA4.pdf

validate:
	$(JING) $(contribdir)/docbook/docbookxi.rng src/*.xml

GmatStyleExampleA4.pdf: GmatStyleExampleA4.fo $(FOPCONF)
	$(FOP) $< $@

GmatStyleExampleA4.fo: validate src/*.xml xform/fo.xsl
	$(XALAN) \
	    -PARAM paper.type A4 \
	    -PARAM img.src.path src/ \
	    -PARAM use.extensions 1 \
	    -PARAM fop1.extensions 1 \
	    -PARAM graphicsize.extension 1 \
	    -IN src/GmatStyleExample.xml \
	    -XSL xform/fo.xsl \
	    -OUT $@

