all: amberfish.pdf

amberfish.pdf: amberfish.texi version.texi
	texi2dvi --pdf amberfish.texi

amberfish.html: amberfish.texi version.texi
	makeinfo --force --html -I. amberfish.texi -o amberfish.html

version.texi: ../VERSION
	echo "@set VERSION `cat ../VERSION`" > version.texi

clean:
	rm -f amberfish.{pdf,aux,cp,cps,fn,fns,ky,log,pg,tmp,toc,tp,vr,vrs} \
		version.texi

distclean:
	make clean
	rm -f *~ *#
