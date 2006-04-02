EXTRA_DIST = $(tests) common.py runtests.py testmodule.py leak.glade

tests =  \
	test_glade.py \
	test_conversion.py \
	test_dialog.py \
	test_enum.py \
	test_gdk.py \
	test_radiobutton.py \
	test_textview.py

check-local: $(top_srcdir)/gtk/__init__.py
	@if test "$(top_builddir)" != "$(top_srcdir)"; then \
	   cp $(top_srcdir)/gtk/__init__.py $(top_builddir)/gtk/__init__.py; \
	   cp $(top_srcdir)/gtk/keysyms.py $(top_builddir)/gtk/keysyms.py; \
	fi
	@$(PYTHON) $(srcdir)/runtests.py $(top_builddir) $(top_srcdir)
	@if test "$(top_builddir)" != "$(top_srcdir)"; then \
	   rm -f $(top_builddir)/gtk/__init__.py*; \
	   rm -f $(top_builddir)/gtk/keysyms.py*; \
	fi
	@rm -fr *.pyc 
