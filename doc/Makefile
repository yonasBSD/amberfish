INCLUDE = version.texi

all: amberfish.pdf amberfish.html

amberfish.pdf: amberfish.texi ${INCLUDE}
	texi2dvi --pdf amberfish.texi

amberfish.html: amberfish.texi ${INCLUDE}
	makeinfo --force --html -I. amberfish.texi -o amberfish.html

version.texi: ../VERSION
	echo "@set VERSION `cat ../VERSION`" > version.texi

clean:
	rm -f *.aux *.cp *.cps *.fn *.fns *.ky *.log *.pg *.tmp *.toc *.tp *.vr *.vrs

distclean:
	make clean
	rm -f *~ *#
