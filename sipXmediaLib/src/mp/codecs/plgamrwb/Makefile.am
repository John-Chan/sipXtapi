## Process this file with automake to produce Makefile.in

LIBAMRWB_SUBDIR = ../../../../contrib/libamrwb
LIBAMRWB_LIB = @AMRWB_LIB_ROOT@/.libs/libamrwb.la

INCLUDES = -I$(top_srcdir)/sipXmediaLib/include -I$(top_srcdir)/sipXportLib/include @AMRWB_INCLUDE@

codeclib_LTLIBRARIES = codec_amrwb.la

codec_amrwb_la_SOURCES = PlgAmrWB.c
codec_amrwb_la_LIBADD = ${LIBAMRWB_LIB}
codec_amrwb_la_LDFLAGS = -module -avoid-version
codec_amrwb_la_DEPENDENCIES = ${LIBAMRWB_LIB}

${LIBAMRWB_LIB}: ${LIBAMRWB_SUBDIR}/Makefile 
	make -C @AMRWB_LIB_ROOT@

${LIBAMRWB_SUBDIR}/Makefile: ${LIBAMRWB_SUBDIR}/configure
	( cd ${LIBAMRWB_SUBDIR}; ./configure --host=${host_triplet} --build=${build_triplet} )

clean-local:
	if [ -f ${LIBAMRWB_SUBDIR}/Makefile ]; then make -C  @AMRWB_LIB_ROOT@ clean; fi
distclean-local:
	if [ -f ${LIBAMRWB_SUBDIR}/Makefile ]; then make -C @AMRWB_LIB_ROOT@ distclean; fi

