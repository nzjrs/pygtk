
xml:
	$(PYTHON) build-docs.py
	perl -pi -e 's/<\!>/<\!-- -->/' xml/*.xml
html:
	-mkdir html
	(cd html && xsltproc --xinclude ../pygtk-docs.xsl ../pygtk-docs.xml)
	cp $(srcdir)/images/*.png html/

devhelp:
	(rm -rf dhbook && \
         mkdir dhbook && \
	 mkdir dhbook/book && \
	 ln html/*.html dhbook/book/ && \
	 ln html/*.png dhbook/book/ && \
	 ln html/pygtk-2.0.devhelp dhbook/book.devhelp)
	(cd dhbook && tar czf ../pygtk-2.0.tar.gz book.devhelp book)
	rm -rf dhbook

EXTRA_DIST = \
  build-docs.py \
  gobject.defs \
  pygtk-docs.xml \
  pygtk-docs.xsl \
  devhelp.xsl \
  images/home.png \
  images/left.png \
  images/right.png \
  images/up.png

.PHONY: xml html devhelp
