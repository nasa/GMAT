# Written in accordance with the GNU Makefile Conventions:
# http://www.gnu.org/prep/standards/html_node/Makefile-Conventions.html

# boilerplate initializations
SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .o

all: GMATMathSpec.pdf

clean:
	rm -f *.aux \
	    *.bbl \
	    *.dvi \
	    *.idx \
	    *.ilg \
	    *.ind \
	    *.lof \
	    *.lot \
	    *.out \
	    *.blg \
	    *.ps \
	    *.log \
	    *.toc

GMATMathSpec.dvi: *.tex *.sty *.dtx *.bib Images/*.eps
	latex GMATMathSpec
	bibtex GMATMathSpec
	makeindex GMATMathSpec
	latex GMATMathSpec
	latex GMATMathSpec	# can't tell if all these are needed

# implicit rules
%.pdf: %.ps
	ps2pdf $<

%.ps: %.dvi
	dvips $<

