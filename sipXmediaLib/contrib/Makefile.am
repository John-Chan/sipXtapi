
if AMRNB
AMRNB_DIR = libamrnb
endif

if AMRWB
AMRWB_DIR = libamrwb
endif

SUBDIRS = $(AMRNB_DIR) $(AMRWB_DIR)

if SPEEX_CONTRIB

libspeex/libspeex/.libs/libspeex.so libspeex/libspeex/libspeex.la: libspeex/Makefile
	$(MAKE) -C libspeex

libspeex/libspeex/.libs/libspeexdsp.so libspeex/libspeex/libspeexdsp.la: libspeex/Makefile
	$(MAKE) -C libspeex

# Copy over the speex shared libtool .la files to the codec dir
@DEFAULT_CODECS_PATH@/libspeex.la:  libspeex/libspeex/libspeex.la 
	cp -d libspeex/libspeex/libspeex.la @DEFAULT_CODECS_PATH@/libspeex.la
@DEFAULT_CODECS_PATH@/libspeexdsp.la:  libspeex/libspeex/libspeexdsp.la 
	cp -d libspeex/libspeex/libspeexdsp.la @DEFAULT_CODECS_PATH@/libspeexdsp.la
if CROSS_COMPILING
	$(MAKE) -C libspeex install
endif

# Copy over the speex shared libs to the codec dir
@DEFAULT_CODECS_PATH@/libspeex.so: libspeex/libspeex/.libs/libspeex.so
	cp -d libspeex/libspeex/.libs/libspeex.so* @DEFAULT_CODECS_PATH@/
@DEFAULT_CODECS_PATH@/libspeexdsp.so: libspeex/libspeex/.libs/libspeexdsp.so
	cp -d libspeex/libspeex/.libs/libspeexdsp.so* @DEFAULT_CODECS_PATH@/

speex: @DEFAULT_CODECS_PATH@/libspeex.so @DEFAULT_CODECS_PATH@/libspeex.la @DEFAULT_CODECS_PATH@/libspeexdsp.so @DEFAULT_CODECS_PATH@/libspeexdsp.la

speex_clean: libspeex/Makefile
	$(MAKE) -C libspeex clean

speex_distclean: libspeex/Makefile
	$(MAKE) -C libspeex distclean

ALL_TARGETS := $(ALL_TARGETS) speex

CLEAN_TARGETS := $(CLEAN_TARGETS) speex_clean

DISTCLEAN_TARGETS := $(DISTCLEAN_TARGETS) speex_distclean

.PHONY: speex speex_clean speex_distclean

endif

all-local: $(ALL_TARGETS)

clean-local: $(CLEAN_TARGETS)

distclean-local: $(DISTCLEAN_TARGETS)
