## Process this file with automake to produce Makefile.in
include $(top_srcdir)/config/sipXcommon.am.inc

EXTRA_DIST = \
    config/sipX-config.in \
    config/sipX-buildstamp.cpp.in \
    config/sipX-buildstamp.h.in \
    config/sipXprojtop.mak \
    config/sipXcommon.am.inc \
    CONTRIBUTORS

unconditional_SUBDIRS = . src include
# bin is not really needed now that we have a top-level build system
#unconditional_SUBDIRS += bin

include $(top_srcdir)/config/sipXprojtop.mak

SUBDIRS = $(unconditional_SUBDIRS) $(doc_SUBDIRS)
RELEASE = 1

.PHONY : rpm
# Where rpmbuild will do its work.
RPMBUILD_TOPDIR = $(shell rpm --eval '%{_topdir}')
# Get the revision number
if USE_BLDNO
BUILDPARM=--define="buildno 0.$(shell cat $(srcdir)/SVN-VERSION)"
else
BUILDPARM=--define="buildno $(RELEASE)"
endif

rpm : dist
	rpmbuild -ta $(BUILDPARM) $(PACKAGE)-$(VERSION).tar.gz
	mv -f $(RPMBUILD_TOPDIR)/SRPMS/$(PACKAGE)-$(VERSION)-*.rpm .
	mv -f $(RPMBUILD_TOPDIR)/RPMS/*/$(PACKAGE)*-$(VERSION)-*.rpm .
	md5sum $(PACKAGE)-$(VERSION).tar.gz >$(PACKAGE)-$(VERSION).tar.gz.md5

# RPM Spec file
sipxmedialib.spec : sipxmedialib.spec.in
	$(LocalizeSipXconfig) $(srcdir)/sipxmedialib.spec.in > sipxmedialib.spec

# 'rpmbuild -ta' searches root of tarball for first *.spec file to build
# RPM from
#dist-hook: sipxmedialib.spec
#	cp sipxmedialib.spec $(distdir)
