DOC_MODULE = pygtk
HTML_DIR = ${datadir}/gtk-doc/html
TARGET_DIR = $(HTML_DIR)/$(DOC_MODULE)

REFERENCEXMLFILES = \
	ref/pygtk2-ref.xml \
	ref/pygtk-atk-classes.xml \
	ref/pygtk-classes.xml \
	ref/pygtk-gdk-classes.xml \
	ref/pygtk-gdkatom.xml \
	ref/pygtk-gdkcolormap.xml \
	ref/pygtk-gdkcolor.xml \
	ref/pygtk-gdk-constants.xml \
	ref/pygtk-gdkcursor.xml \
	ref/pygtk-gdkdevice.xml \
	ref/pygtk-gdkdisplay.xml \
	ref/pygtk-gdkdisplaymanager.xml \
	ref/pygtk-gdkdragcontext.xml \
	ref/pygtk-gdkdrawable.xml \
	ref/pygtk-gdkevent.xml \
	ref/pygtk-gdkfont.xml \
	ref/pygtk-gdk-functions.xml \
	ref/pygtk-gdkgc.xml \
	ref/pygtk-gdkimage.xml \
	ref/pygtk-gdkkeymap.xml \
	ref/pygtk-gdkpixbufanimationiter.xml \
	ref/pygtk-gdkpixbufanimation.xml \
	ref/pygtk-gdkpixbufloader.xml \
	ref/pygtk-gdkpixbuf.xml \
	ref/pygtk-gdkpixmap.xml \
	ref/pygtk-gdkrectangle.xml \
	ref/pygtk-gdkscreen.xml \
	ref/pygtk-gdkvisual.xml \
	ref/pygtk-gdkwindow.xml \
	ref/pygtk-gladexml-classes.xml \
	ref/pygtk-gobject-classes.xml \
	ref/pygtk-gobject-constants.xml \
	ref/pygtk-gobject-functions.xml \
	ref/pygtk-gobject-gboxed.xml \
	ref/pygtk-gobject-ginterface.xml \
	ref/pygtk-gobject-gpointer.xml \
	ref/pygtk-gobject-maincontext.xml \
	ref/pygtk-gobject-mainloop.xml \
	ref/pygtk-gobject.xml \
	ref/pygtk-gtkaccelgroup.xml \
	ref/pygtk-gtkaccellabel.xml \
	ref/pygtk-gtkaccessible.xml \
	ref/pygtk-gtkaction.xml \
	ref/pygtk-gtkactiongroup.xml \
	ref/pygtk-gtkadjustment.xml \
	ref/pygtk-gtkalignment.xml \
	ref/pygtk-gtkarrow.xml \
	ref/pygtk-gtkaspectframe.xml \
	ref/pygtk-gtkbin.xml \
	ref/pygtk-gtkborder.xml \
	ref/pygtk-gtkbox.xml \
	ref/pygtk-gtkbutton.xml \
	ref/pygtk-gtkbuttonbox.xml \
	ref/pygtk-gtkcalendar.xml \
	ref/pygtk-gtkcelleditable.xml \
	ref/pygtk-gtkcelllayout.xml \
	ref/pygtk-gtkcellrenderer.xml \
	ref/pygtk-gtkcellrendererpixbuf.xml \
	ref/pygtk-gtkcellrenderertext.xml \
	ref/pygtk-gtkcellrenderertoggle.xml \
	ref/pygtk-gtkcheckbutton.xml \
	ref/pygtk-gtkcheckmenuitem.xml \
	ref/pygtk-gtk-classes.xml \
	ref/pygtk-gtkclipboard.xml \
	ref/pygtk-gtkcolorbutton.xml \
	ref/pygtk-gtkcolorselection.xml \
	ref/pygtk-gtkcolorselectiondialog.xml \
	ref/pygtk-gtkcombo.xml \
	ref/pygtk-gtkcombobox.xml \
	ref/pygtk-gtkcomboboxentry.xml \
	ref/pygtk-gtk-constants.xml \
	ref/pygtk-gtkcontainer.xml \
	ref/pygtk-gtkcurve.xml \
	ref/pygtk-gtkdialog.xml \
	ref/pygtk-gtkdrawingarea.xml \
	ref/pygtk-gtkeditable.xml \
	ref/pygtk-gtkentry.xml \
	ref/pygtk-gtkentrycompletion.xml \
	ref/pygtk-gtkeventbox.xml \
	ref/pygtk-gtkexpander.xml \
	ref/pygtk-gtkfilechooser.xml \
	ref/pygtk-gtkfilechooserdialog.xml \
	ref/pygtk-gtkfilechooserwidget.xml \
	ref/pygtk-gtkfilefilter.xml \
	ref/pygtk-gtkfileselection.xml \
	ref/pygtk-gtkfixed.xml \
	ref/pygtk-gtkfontbutton.xml \
	ref/pygtk-gtkfontselectiondialog.xml \
	ref/pygtk-gtkfontselection.xml \
	ref/pygtk-gtkframe.xml \
	ref/pygtk-gtkgammacurve.xml \
	ref/pygtk-gtkhandlebox.xml \
	ref/pygtk-gtkhbox.xml \
	ref/pygtk-gtkhbuttonbox.xml \
	ref/pygtk-gtkhpaned.xml \
	ref/pygtk-gtkhruler.xml \
	ref/pygtk-gtkhscale.xml \
	ref/pygtk-gtkhscrollbar.xml \
	ref/pygtk-gtkhseparator.xml \
	ref/pygtk-gtkiconfactory.xml \
	ref/pygtk-gtkiconinfo.xml \
	ref/pygtk-gtkiconset.xml \
	ref/pygtk-gtkiconsource.xml \
	ref/pygtk-gtkicontheme.xml \
	ref/pygtk-gtkimage.xml \
	ref/pygtk-gtkimagemenuitem.xml \
	ref/pygtk-gtkimcontextsimple.xml \
	ref/pygtk-gtkimcontext.xml \
	ref/pygtk-gtkimmulticontext.xml \
	ref/pygtk-gtkinputdialog.xml \
	ref/pygtk-gtkinvisible.xml \
	ref/pygtk-gtkitemfactory.xml \
	ref/pygtk-gtkitem.xml \
	ref/pygtk-gtklabel.xml \
	ref/pygtk-gtklayout.xml \
	ref/pygtk-gtkliststore.xml \
	ref/pygtk-gtkmenu.xml \
	ref/pygtk-gtkmenubar.xml \
	ref/pygtk-gtkmenuitem.xml \
	ref/pygtk-gtkmenushell.xml \
	ref/pygtk-gtkmessagedialog.xml \
	ref/pygtk-gtkmisc.xml \
	ref/pygtk-gtknotebook.xml \
	ref/pygtk-gtkobject.xml \
	ref/pygtk-gtkoptionmenu.xml \
	ref/pygtk-gtkpaned.xml \
	ref/pygtk-gtkplug.xml \
	ref/pygtk-gtkprogressbar.xml \
	ref/pygtk-gtkradioaction.xml \
	ref/pygtk-gtkradiobutton.xml \
	ref/pygtk-gtkradiomenuitem.xml \
	ref/pygtk-gtkradiotoolbutton.xml \
	ref/pygtk-gtkrange.xml \
	ref/pygtk-gtkrcstyle.xml \
	ref/pygtk-gtkrequisition.xml \
	ref/pygtk-gtkruler.xml \
	ref/pygtk-gtkscale.xml \
	ref/pygtk-gtkscrollbar.xml \
	ref/pygtk-gtkscrolledwindow.xml \
	ref/pygtk-gtkselectiondata.xml \
	ref/pygtk-gtkseparator.xml \
	ref/pygtk-gtkseparatormenuitem.xml \
	ref/pygtk-gtkseparatortoolitem.xml \
	ref/pygtk-gtksettings.xml \
	ref/pygtk-gtksizegroup.xml \
	ref/pygtk-gtksocket.xml \
	ref/pygtk-gtkspinbutton.xml \
	ref/pygtk-gtkstatusbar.xml \
	ref/pygtk-gtkstyle.xml \
	ref/pygtk-gtktable.xml \
	ref/pygtk-gtktearoffmenuitem.xml \
	ref/pygtk-gtktextattributes.xml \
	ref/pygtk-gtktextbuffer.xml \
	ref/pygtk-gtktextchildanchor.xml \
	ref/pygtk-gtktextiter.xml \
	ref/pygtk-gtktextmark.xml \
	ref/pygtk-gtktexttagtable.xml \
	ref/pygtk-gtktexttag.xml \
	ref/pygtk-gtktextview.xml \
	ref/pygtk-gtktoggleaction.xml \
	ref/pygtk-gtktogglebutton.xml \
	ref/pygtk-gtktoggletoolbutton.xml \
	ref/pygtk-gtktoolbar.xml \
	ref/pygtk-gtktoolbutton.xml \
	ref/pygtk-gtktoolitem.xml \
	ref/pygtk-gtktooltips.xml \
	ref/pygtk-gtktreedragdest.xml \
	ref/pygtk-gtktreedragsource.xml \
	ref/pygtk-gtktreeiter.xml \
	ref/pygtk-gtktreemodel.xml \
	ref/pygtk-gtktreemodelfilter.xml \
	ref/pygtk-gtktreemodelsort.xml \
	ref/pygtk-gtktreerowreference.xml \
	ref/pygtk-gtktreeselection.xml \
	ref/pygtk-gtktreesortable.xml \
	ref/pygtk-gtktreestore.xml \
	ref/pygtk-gtktreeview.xml \
	ref/pygtk-gtktreeviewcolumn.xml \
	ref/pygtk-gtkuimanager.xml \
	ref/pygtk-gtkvbox.xml \
	ref/pygtk-gtkvbuttonbox.xml \
	ref/pygtk-gtkviewport.xml \
	ref/pygtk-gtkvpaned.xml \
	ref/pygtk-gtkvruler.xml \
	ref/pygtk-gtkvscale.xml \
	ref/pygtk-gtkvscrollbar.xml \
	ref/pygtk-gtkvseparator.xml \
	ref/pygtk-gtkwidget.xml \
	ref/pygtk-gtkwindow.xml \
	ref/pygtk-gtkwindowgroup.xml \
	ref/pygtk-gtk-functions.xml \
	ref/pygtk-gtk-stock-items.xml \
	ref/pygtk-hierarchy.xml \
	ref/pygtk-introduction.xml \
	ref/pygtk-pango-classes.xml \
	ref/pygtk-pangoattribute.xml \
	ref/pygtk-pangoattriter.xml \
	ref/pygtk-pangoattrlist.xml \
	ref/pygtk-pangocolor.xml \
	ref/pygtk-pango-constants.xml \
	ref/pygtk-pangocontext.xml \
	ref/pygtk-pangofontdescription.xml \
	ref/pygtk-pangofontface.xml \
	ref/pygtk-pangofontfamily.xml \
	ref/pygtk-pangofontmap.xml \
	ref/pygtk-pangofontmetrics.xml \
	ref/pygtk-pangofontsetsimple.xml \
	ref/pygtk-pangofontset.xml \
	ref/pygtk-pangofont.xml \
	ref/pygtk-pango-functions.xml \
	ref/pygtk-pangoglyphstring.xml \
	ref/pygtk-pangolanguage.xml \
	ref/pygtk-pangolayout.xml \
	ref/pygtk-pangotabarray.xml \
	ref/pygtk-pango-markup.xml \
	ref/pygtk-pygtkgenericcellrenderer.xml \
	ref/pygtk-pygtkgenerictreemodel.xml \
	ref/pygtk-pygtktreemodelrow.xml \
	ref/pygtk-pygtktreemodelrowiter.xml

REFERENCELINKS = \
	html/images \
	html/icons \
	html/cursors

XSL_FILES = \
	devhelp.xsl \
	common.xsl \
	html.xsl \
	html-style.xsl

EXTRA_DIST = \
	$(REFERENCEXMLFILES) \
	$(XSL_FILES)

REF_VERSION = 2.4

html: ${XSLFILES} ${XSL_FILES}
	xsltproc --nonet --xinclude -o html/ \
		--stringparam gtkdoc.bookname pygtk \
		--stringparam gtkdoc.version $(REF_VERSION) \
		html-style.xsl ref/pygtk2-ref.xml

install-data-local: 
	$(mkinstalldirs) $(DESTDIR)$(TARGET_DIR) 
	(installfiles=`echo ./html/*.html`; \
	if test "$$installfiles" = './html/*.html'; \
	  then echo '-- Nothing to install' ; \
	else \
	  for i in $$installfiles; do \
	    echo '-- Installing '$$i ; \
	    $(INSTALL_DATA) $$i $(DESTDIR)$(TARGET_DIR); \
	  done; \
	  pngfiles=`echo ./html/*.png`; \
	  if test "$$pngfiles" != './html/*.png'; then \
	    for i in $$pngfiles; do \
	      echo '-- Installing '$$i ; \
	      $(INSTALL_DATA) $$i $(DESTDIR)$(TARGET_DIR); \
	    done; \
	  fi; \
	  echo '-- Installing $(srcdir)/html/$(DOC_MODULE).devhelp' ; \
	  $(INSTALL_DATA) $(srcdir)/html/$(DOC_MODULE).devhelp \
	    $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE).devhelp; \
	fi) 
uninstall-local:
	(installfiles=`echo ./html/*.html`; \
	if test "$$installfiles" = './html/*.html'; \
	then echo '-- Nothing to uninstall' ; \
	else \
	  for i in $$installfiles; do \
	    rmfile=`basename $$i` ; \
	    echo '-- Uninstalling $(DESTDIR)$(TARGET_DIR)/'$$rmfile ; \
	    rm -f $(DESTDIR)$(TARGET_DIR)/$$rmfile; \
	  done; \
	  pngfiles=`echo ./html/*.png`; \
	  if test "$$pngfiles" != './html/*.png'; then \
	    for i in $$pngfiles; do \
	      rmfile=`basename $$i` ; \
	      echo '-- Uninstalling $(DESTDIR)$(TARGET_DIR)/'$$rmfile ; \
	      rm -f $(DESTDIR)$(TARGET_DIR)/$$rmfile; \
	    done; \
	  fi; \
	  echo '-- Uninstalling $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE).devhelp' ; \
	  rm -f $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE).devhelp; \
	fi) 
	if test -d $(DESTDIR)$(TARGET_DIR); then rmdir -p --ignore-fail-on-non-empty $(DESTDIR)$(TARGET_DIR); fi


.PHONY: html
