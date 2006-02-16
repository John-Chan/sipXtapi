##
## Libs from SipFoundry
##

## Common C and C++ flags for pingtel related source
AC_DEFUN([SFAC_INIT_FLAGS],
[
    AC_SUBST(CPPUNIT_CFLAGS,  [])
    AC_SUBST(CPPUNIT_LDFLAGS, [])

    ## TODO Remove cpu specifics and use make variables setup for this
    ##
    ## NOTES:
    ##   -D__pingtel_on_posix__   - really used for linux v.s. other
    ##   -D_REENTRANT             - rougewave ?
    ##   -fmessage-length=0       - ?
    ##
    AC_SUBST(SIPX_INCDIR, [${includedir}])
    AC_SUBST(SIPX_LIBDIR, [${libdir}])

    SF_CXX_C_FLAGS="-D__pingtel_on_posix__ -D_linux_ -D_REENTRANT -D_FILE_OFFSET_BITS=64 -fmessage-length=0"

    SF_CXX_WARNINGS="-Wall -Wformat -Wwrite-strings -Wpointer-arith"
    CXXFLAGS="$CXXFLAGS $SF_CXX_C_FLAGS $SF_CXX_WARNINGS"
    CFLAGS="$CFLAGS $SF_CXX_C_FLAGS $SF_CXX_WARNINGS -Wnested-externs -Wmissing-declarations -Wmissing-prototypes"

    ## NOTE: These are not expanded (e.g. contain $(prefix)) and are only
    ## fit for Makefiles. You can however write a Makefile that transforms
    ## *.in to * with the concrete values. 
    ##
    ##  See sipXconfig/Makefile.am for an example.   
    ##  See autoconf manual 4.7.2 Installation Directory Variables for why it's restricted
    ##
    AC_SUBST(SIPX_BINDIR,  [${bindir}])
    AC_SUBST(SIPX_CONFDIR, [${sysconfdir}/sipxpbx])
    AC_SUBST(SIPX_DATADIR, [${datadir}/sipxpbx])
    AC_SUBST(SIPX_LOGDIR,  [${localstatedir}/log/sipxpbx])
    AC_SUBST(SIPX_RUNDIR,  [${localstatedir}/run/sipxpbx])
    AC_SUBST(SIPX_TMPDIR,  [${localstatedir}/sipxdata/tmp])
    AC_SUBST(SIPX_DBDIR,   [${localstatedir}/sipxdata/sipdb])
    AC_SUBST(SIPX_UPGRADEDIR,[${localstatedir}/sipxdata/upgrade])
    AC_SUBST(SIPX_SCHEMADIR, [${datadir}/sipx/schema])
    AC_SUBST(SIPX_DOCDIR,  [${datadir}/doc/sipx])
    AC_SUBST(SIPX_VARDIR,  [${localstatedir}/sipxdata])

    # temporary - see http://track.sipfoundry.org/browse/XPB-33
    AC_SUBST(SIPX_VXMLDATADIR,[${localstatedir}/sipxdata/mediaserver/data])

    AC_SUBST(SIPX_PARKMUSICDIR,[${localstatedir}/sipxdata/parkserver/music])

    # temporary - see http://track.sipfoundry.org/browse/XPB-93
    AC_SUBST(SIPX_BACKUPDIR, [${localstatedir}/sipxdata/backup])
    AC_SUBST(SIPX_CONFIGPHONEDIR, [${localstatedir}/sipxdata/configserver/phone])

    ## Used in a number of different project and subjective where this should really go
    ## INSTALL instruction assume default, otherwise safe to change/override
    AC_ARG_VAR(wwwdir, [Web root for web content, default is ${datadir}/www. \
                        WARNING: Adjust accordingly when following INSTALL instructions])
    test -z $wwwdir && wwwdir='${datadir}/www'

    AC_ARG_VAR(SIPXPBXUSER, [The user that sipXpbx runs under, default is 'sipxchange'])
    test -z $SIPXPBXUSER && SIPXPBXUSER=sipxchange

    AC_SUBST(SIPXPHONECONF, [${sysconfdir}/sipxphone])
    AC_SUBST(SIPXPHONEDATA, [${datadir}/sipxphone])
    AC_SUBST(SIPXPHONELIB, [${datadir}/sipxphone/lib])

    AC_ARG_ENABLE(rpmbuild, 
      [  --enable-rpmbuild       Build an rpm],
      enable_rpmbuild=yes )

    AC_ARG_ENABLE(buildnumber,
                 [  --enable-buildnumber    enable build number as part of RPM name],
                 enable_buildnumber=yes)
])


## Check to see that we are using the minimum required version of automake
AC_DEFUN([SFAC_AUTOMAKE_VERSION],[
   AC_MSG_CHECKING(for automake version >= $1)
   sf_am_version=`automake --version | head -n 1 | awk '/^automake/ {print $NF}'`
   AX_COMPARE_VERSION( [$1], [le], [$sf_am_version], AC_MSG_RESULT( $sf_am_version is ok), AC_MSG_ERROR( found $sf_am_version - you must upgrade automake ))
])


## sipXportLib 
# SFAC_LIB_PORT attempts to find the sf portability library and include
# files by looking in /usr/[lib|include], /usr/local/[lib|include], and
# relative paths.
#
# If not found, the configure is aborted.  Otherwise, variables are defined
# for both the INC and LIB paths 
# AND the paths are added to the CFLAGS and CXXFLAGS
AC_DEFUN([SFAC_LIB_PORT],
[
    AC_REQUIRE([SFAC_INIT_FLAGS])
    AC_REQUIRE([CHECK_PCRE])
    AC_REQUIRE([CHECK_SSL])

    SFAC_ARG_WITH_INCLUDE([os/OsDefs.h],
            [sipxportinc],
            [ --with-sipxportinc=<dir> portability include path ],
            [sipXportLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('os/OsDefs.h' not found)
    fi
    SIPXPORTINC=$foundpath
    AC_SUBST(SIPXPORTINC)

    CFLAGS="-I$SIPXPORTINC $PCRE_CFLAGS $CPPUNIT_CFLAGS $CFLAGS"
    CXXFLAGS="-I$SIPXPORTINC $PCRE_CXXFLAGS $CPPUNIT_CFLAGS $CXXFLAGS"

    foundpath=""

    SFAC_ARG_WITH_INCLUDE([sipxunit/TestUtilities.h],
            [sipxportinc],
            [ --with-sipxportinc=<dir> portability include path ],
            [sipXportLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('sipxunit/TestUtilities.h' not found)
    fi
    SIPXUNITINC=$foundpath
    AC_SUBST(SIPXUNITINC)

    CFLAGS="-I$SIPXUNITINC $CFLAGS"
    CXXFLAGS="-I$SIPXUNITINC $CXXFLAGS"

    foundpath=""

    SFAC_ARG_WITH_LIB([libsipXport.la],
            [sipxportlib],
            [ --with-sipxportlib=<dir> portability library path ],
            [sipXportLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
        AC_SUBST(SIPXPORT_LIBS,    "$foundpath/libsipXport.la")
        AC_SUBST(SIPXPORT_LDFLAGS, "-L$foundpath")
    else
        AC_MSG_ERROR('libsipXport.la' not found)
    fi

    foundpath=""

    SFAC_ARG_WITH_LIB([libsipXunit.la],
            [sipxportlib],
            [ --with-sipxportlib=<dir> portability library path ],
            [sipXportLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
        # sipXunit unitesting support
        AC_SUBST(SIPXUNIT_LDFLAGS, "-L$foundpath")
        AC_SUBST(SIPXUNIT_LIBS,    "$foundpath/libsipXunit.la")
    else
        AC_MSG_ERROR('libsipXunit.la' not found)
    fi
]) # SFAC_LIB_PORT


## sipXtackLib 
# SFAC_LIB_STACK attempts to find the sf networking library and include
# files by looking in /usr/[lib|include], /usr/local/[lib|include], and
# relative paths.
#
# If not found, the configure is aborted.  Otherwise, variables are defined
# for both the INC and LIB paths AND the paths are added to the CFLAGS, 
# CXXFLAGS, LDFLAGS, and LIBS.
AC_DEFUN([SFAC_LIB_STACK],
[
    AC_REQUIRE([SFAC_LIB_PORT])

    SFAC_ARG_WITH_INCLUDE([net/SipUserAgent.h],
            [sipxtackinc],
            [ --with-sipxtackinc=<dir> sip stack include path ],
            [sipXtackLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('net/SipUserAgent.h' not found)
    fi
    SIPXTACKINC=$foundpath
    AC_SUBST(SIPXTACKINC)

    if test "$SIPXTACKINC" != "$SIPXPORTINC"
    then
        CFLAGS="-I$SIPXTACKINC $CFLAGS"
        CXXFLAGS="-I$SIPXTACKINC $CXXFLAGS"
    fi

    SFAC_ARG_WITH_LIB([libsipXtack.la],
            [sipxtacklib],
            [ --with-sipxtacklib=<dir> sip stack library path ],
            [sipXtackLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('libsipXtack.la' not found)
    fi

    SIPXTACKLIB=$foundpath

    AC_SUBST(SIPXTACK_LIBS,["$SIPXTACKLIB/libsipXtack.la"])
    AC_SUBST(SIPXTACK_LDFLAGS,["-L$SIPXTACKLIB"])

]) # SFAC_LIB_STACK


## sipXmediaLib 
# SFAC_LIB_MEDIA attempts to find the sf media library and include
# files by looking in /usr/[lib|include], /usr/local/[lib|include], and
# relative paths.
#
# If not found, the configure is aborted.  Otherwise, variables are defined
# for both the INC and LIB paths AND the paths are added to the CFLAGS, 
# CXXFLAGS, LDFLAGS, and LIBS.
AC_DEFUN([SFAC_LIB_MEDIA],
[
    AC_REQUIRE([SFAC_LIB_STACK])

    SFAC_ARG_WITH_INCLUDE([mp/MpMediaTask.h],
            [sipxmediainc],
            [ --with-sipxmediainc=<dir> media library include path ],
            [sipXmediaLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('mp/MpMediaTask.h' not found)
    fi
    SIPXMEDIAINC=$foundpath
    AC_SUBST(SIPXMEDIAINC)

    if test "$SIPXMEDIAINC" != "$SIPXPORTINC"
    then
        CFLAGS="-I$SIPXMEDIAINC $CFLAGS"
        CXXFLAGS="-I$SIPXMEDIAINC $CXXFLAGS"
    fi
    
    SFAC_ARG_WITH_LIB([libsipXmedia.la],
            [sipxmedialib],
            [ --with-sipxmedialib=<dir> media library path ],
            [sipXmediaLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('libsipXmedia.la' not found)
    fi
    SIPXMEDIALIB=$foundpath

    AC_SUBST(SIPXMEDIA_LIBS, ["$SIPXMEDIALIB/libsipXmedia.la"])
    AC_SUBST(SIPXMEDIA_LDFLAGS, ["-L$SIPXMEDIALIB"])
]) # SFAC_LIB_MEDIA



## sipXmediaAdapterLib 
# SFAC_LIB_MEDIAADAPTER attempts to find the sf media iadapter library and include
# files by looking in /usr/[lib|include], /usr/local/[lib|include], and
# relative paths.
#
# If not found, the configure is aborted.  Otherwise, variables are defined
# for both the INC and LIB paths AND the paths are added to the CFLAGS, 
# CXXFLAGS, LDFLAGS, and LIBS.
AC_DEFUN([SFAC_LIB_MEDIAADAPTER],
[
    AC_REQUIRE([SFAC_LIB_MEDIA])

    SFAC_ARG_WITH_INCLUDE([mi/CpMediaInterface.h],
            [sipxmediaadapterinc],
            [ --with-sipxmediaadapterinc=<dir> media adapter library include path ],
            [sipXmediaAdapterLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('mi/CpMediaInterface.h' not found)
    fi
    SIPXMEDIAADAPTERINC=$foundpath
    AC_SUBST(SIPXMEDIAADAPTERINC)

    if test "$SIPXMEDIAADAPTERINC" != "$SIPXPORTINC"
    then
        CFLAGS="-I$SIPXMEDIAADAPTERINC $CFLAGS"
        CXXFLAGS="-I$SIPXMEDIAADAPTERINC $CXXFLAGS"
    fi
    
    SFAC_ARG_WITH_LIB([libsipXmediaProcessing.la],
            [sipxmediaadapterlib],
            [ --with-sipxmediaadapterlib=<dir> media adapter library path ],
            [sipXmediaAdapterLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('libsipXmediaProcessing.la' not found)
    fi
    SIPXMEDIAADAPTERLIB=$foundpath

    AC_SUBST(SIPXMEDIAADAPTER_LIBS, ["$SIPXMEDIAADAPTERLIB/libsipXmediaProcessing.la"])
    AC_SUBST(SIPXMEDIAADAPTER_LDFLAGS, ["-L$SIPXMEDIAADAPTERLIB"])
]) # SFAC_LIB_MEDIAADAPTER


## Optionally compile in the GIPS library in the media subsystem
# (sipXmediaLib project) and executables that link it in
# Conditionally use the GIPS audio libraries
AC_DEFUN([CHECK_GIPSNEQ],
[
   AC_ARG_WITH(gipsneq,
      [  --with-gipsneq       Compile the media subsystem with the GIPS audio libraries
],
      compile_with_gips=yes)

   AC_MSG_CHECKING(if link in with gips NetEQ)

   if test x$compile_with_gips = xyes
   then
      AC_MSG_RESULT(yes)
      
      SFAC_SRCDIR_EXPAND

      AC_MSG_CHECKING(for gips includes)
      # Define HAVE_GIPS for c pre-processor
      GIPS_CPPFLAGS=-DHAVE_GIPS
      if test -e $withval/include/GIPS/Vendor_gips_typedefs.h
      then
         gips_dir=$withval
      elif test -e $abs_srcdir/../sipXbuild/vendors/gips/include/GIPS/Vendor_gips_typedefs.h
      then
         gips_dir=$abs_srcdir/../sipXbuild/vendors/gips
      else
         AC_MSG_ERROR(GIPS/Vendor_gips_typedefs.h not found)
      fi

      AC_MSG_RESULT($gips_dir)

      # Add GIPS include path
      GIPSINC=$gips_dir/include
      # Add GIPS objects to be linked in
      GIPS_NEQ_OBJS=$gips_dir/GIPS_SuSE_i386.a
      CPPFLAGS="$CPPFLAGS $GIPS_CPPFLAGS -I$GIPSINC"
      # GIPS needs to be at the end of the list
      #LIBS="$LIBS $GIPS_OBJS"

   else
      AC_MSG_RESULT(no)
   fi

   AC_SUBST(GIPSINC)
   AC_SUBST(GIPS_NEQ_OBJS)
   AC_SUBST(GIPS_CPPFLAGS)

   AC_SUBST(SIPXMEDIA_MP_LIBS, ["$SIPXMEDIALIB/libsipXmediaProcessing.la"])
]) # CHECK_GIPSNEQ


AC_DEFUN([CHECK_GIPSVE],
[
   AC_ARG_WITH(gipsve,
      [  --with-gipsve       Link to GIPS voice engine if --with-gipsve is set],
      link_with_gipsve=yes)

   AC_MSG_CHECKING(if linking to gips voice engine)

   if test x$link_with_gipsve = xyes
   then
      AC_MSG_RESULT(yes)
      
      SFAC_SRCDIR_EXPAND

      AC_MSG_CHECKING(for gips includes)

      if test -e $abs_srcdir/../sipXbuild/vendors/gips/VoiceEngine/interface/GipsVoiceEngineLib.h
      then
         gips_dir=$abs_srcdir/../sipXbuild/vendors/gips
      else
         AC_MSG_ERROR(sipXbuild/vendors/gips/VoiceEngine/interface/GipsVoiceEngineLib.h not found)
      fi

      AC_MSG_RESULT($gips_dir)

      # Add GIPS include path
      GIPSINC=$gips_dir/VoiceEngine/interface
      CPPFLAGS="$CPPFLAGS -I$gips_dir/include -I$GIPSINC"
      # Add GIPS objects to be linked in
      GIPS_VE_OBJS="$gips_dir/VoiceEngine/libraries/VoiceEngine_Linux_gcc.a"

   else
      AC_MSG_RESULT(no)
   fi

   AC_SUBST(GIPSINC)
   AC_SUBST(GIPS_VE_OBJS)

   AC_SUBST(SIPXMEDIA_VE_LIBS, ["$SIPXMEDIALIB/libsipXvoiceEngine.la"])

   AM_CONDITIONAL(BUILDVE, test x$link_with_gipsve = xyes)

]) # CHECK_GIPSVE

AC_DEFUN([CHECK_GIPSCE],
[
   AC_ARG_WITH(gipsce,
      [  --with-gipsce       Link to GIPS conference engine if --with-gipsce is set],
      link_with_gipsce=yes)

   AC_MSG_CHECKING(if linking to gips conference engine)

   if test x$link_with_gipsce = xyes
   then
      AC_MSG_RESULT(yes)
      
      SFAC_SRCDIR_EXPAND

      AC_MSG_CHECKING(for gips includes)

      if test -e $abs_srcdir/../sipXbuild/vendors/gips/ConferenceEngine/interface/ConferenceEngine.h
      then
         gips_dir=$abs_srcdir/../sipXbuild/vendors/gips
      else
         AC_MSG_ERROR(sipXbuild/vendors/gips/ConferenceEngine/interface/ConferenceEngine.h not found)
      fi

      AC_MSG_RESULT($gips_dir)

      # Add GIPS include path
      GIPSINC=$gips_dir/ConferenceEngine/interface
      CPPFLAGS="$CPPFLAGS -I$gips_dir/include -I$GIPSINC"
      # Add GIPS objects to be linked in
      GIPS_CE_OBJS="$gips_dir/ConferenceEngine/libraries/ConferenceEngine_Linux_gcc.a"

   else
      AC_MSG_RESULT(no)
   fi

   AC_SUBST(GIPSINC)
   AC_SUBST(GIPS_CE_OBJS)

   AC_SUBST(SIPXMEDIA_CE_LIBS, ["$SIPXMEDIALIB/libsipXconferenceEngine.la"])
   AM_CONDITIONAL(BUILDCE, test x$link_with_gipsce = xyes)

]) # CHECK_GIPSCE


## sipXcallLib
# SFAC_LIB_CALL attempts to find the sf call processing library and include
# files by looking in /usr/[lib|include], /usr/local/[lib|include], and
# relative paths.
#
# If not found, the configure is aborted.  Otherwise, variables are defined
# for both the INC and LIB paths AND the paths are added to the CFLAGS,
# CXXFLAGS, LDFLAGS, and LIBS.
AC_DEFUN([SFAC_LIB_CALL],
[
    AC_REQUIRE([SFAC_LIB_MEDIA])

    SFAC_ARG_WITH_INCLUDE([cp/CallManager.h],
            [sipxcallinc],
            [ --with-sipxcallinc=<dir> call processing library include path ],
            [sipXcallLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('cp/CallManager.h' not found)
    fi
    SIPXCALLINC=$foundpath
    AC_SUBST(SIPXCALLINC)

    if test "$SIPXCALLINC" != "$SIPXPORTINC"
    then
        CFLAGS="-I$SIPXCALLINC $CFLAGS"
        CXXFLAGS="-I$SIPXCALLINC $CXXFLAGS"
    fi

    SFAC_ARG_WITH_LIB([libsipXcall.la],
            [sipxcalllib],
            [ --with-sipxcalllib=<dir> call processing library path ],
            [sipXcallLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('libsipXcall.la' not found)
    fi
    SIPXCALLLIB=$foundpath

    AC_SUBST(SIPXCALL_LIBS,   ["$SIPXCALLLIB/libsipXcall.la"])
    AC_SUBST(SIPXCALL_LDFLAGS,["-L$SIPXCALLLIB"])
]) # SFAC_LIB_CALL


## sipXcommserverLib

# SFAC_LIB_CALL attempts to find the sf communication server library and 
# include files by looking in /usr/[lib|include], /usr/local/[lib|include], 
# and relative paths.
#
# If not found, the configure is aborted.  Otherwise, variables are defined
# for both the INC and LIB paths AND the paths are added to the CFLAGS,
# CXXFLAGS, LDFLAGS, and LIBS.
AC_DEFUN([SFAC_LIB_COMMSERVER],
[
    AC_REQUIRE([SFAC_LIB_STACK])

    SFAC_ARG_WITH_INCLUDE([sipdb/SIPDBManager.h],
            [sipxcommserverinc],
            [ --with-sipxcommserverinc=<dir> call processing library include path ],
            [sipXcommserverLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('sipdb/SIPDBManager.h' not found)
    fi
    SIPXCOMMSERVERINC=$foundpath
    if test "$SIPXCOMMSERVERINC" != "$SIPXPORTINC"
    then
        CFLAGS="-I$SIPXCOMMSERVERINC $CFLAGS"
        CXXFLAGS="-I$SIPXCOMMSERVERINC $CXXFLAGS"
    fi
    AC_SUBST(SIPXCOMMSERVERINC)

    SFAC_ARG_WITH_LIB([libsipXcommserver.la],
            [sipxcommserverlib],
            [ --with-sipxcommserverlib=<dir> call processing library path ],
            [sipXcommserverLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_ERROR('libsipXcommserver.la' not found)
    fi
    SIPXCOMMSERVERLIB=$foundpath

    AC_SUBST(SIPXCOMMSERVER_LIBS,   ["$SIPXCOMMSERVERLIB/libsipXcommserver.la"])
    AC_SUBST(SIPXCOMMSERVER_LDFLAGS,["-L$SIPXCOMMSERVERLIB"])

]) # SFAC_LIB_COMMSERVER


## resiprocate
# CHECK_RESIPROCATE attempts to find the resiprocate project tree
# 
# If not found, the configure is aborted.  Otherwise, variables are defined for:
# RESIPROCATE_PATH     - the top of the resiprocate tree
# RESIPROCATE_CFLAGS   
# RESIPROCATE_CXXFLAGS
# RESIPROCATE_LIBS
# RESIPROCATE_LDFLAGS
AC_DEFUN([CHECK_RESIPROCATE],
[
    AC_REQUIRE([SFAC_INIT_FLAGS])
    
    AC_ARG_WITH([resiprocate],
        [--with-resiprocate specifies the path to the top of a resiprocate project tree],
        [resiprocate_path=$withval],
        [resiprocate_path="[$abs_top_srcdir]/../../resiprocate $prefix/include /usr /usr/local" ]
    )

    AC_ARG_WITH([resiplib],
        [--with-resiplib specifies the directory where resiprocate libraries are installed],
        [resiplib_path=$withval],
        [resiplib_path="/usr/lib/resiprocate /usr/local/lib/resiprocate" ]
    )

    AC_MSG_CHECKING([for resiprocate sources])
    foundpath=NO
    for dir in $resiprocate_path ; do
        if test -d "$dir/resip";
        then
            foundpath=$dir;
            break;
        fi;
    done
    if test x_$foundpath = x_NO
    then
       AC_MSG_ERROR([not found; searched $resiprocate_path])
    else
       AC_MSG_RESULT($foundpath)

       RESIPROCATE_PATH=$foundpath

       RESIPROCATE_CFLAGS="-I$RESIPROCATE_PATH"
       RESIPROCATE_CXXFLAGS="-I$RESIPROCATE_PATH"
    fi

    AC_MSG_CHECKING([for resiprocate libraries])
    foundpath=NO
    for dir in $resiplib_path ; do
        if test -f "$dir/libresip.so";
        then
            foundpath=$dir;
            break;
        fi;
    done
    if test x_$foundpath = x_NO
    then
       AC_MSG_ERROR([not found; searched $resiplib_path])
    else
       AC_MSG_RESULT($foundpath)

       RESIPROCATE_LIB_PATH="$foundpath"
       RESIPROCATE_LDFLAGS=" -L$foundpath"
       RESIPROCATE_LIBS=" -ldum -lresip -lrutil -lares"
    fi

    AC_SUBST(RESIPROCATE_PATH)
    AC_SUBST(RESIPROCATE_CFLAGS)
    AC_SUBST(RESIPROCATE_CXXFLAGS)

    AC_SUBST(RESIPROCATE_LIB_PATH)
    AC_SUBST(RESIPROCATE_LIBS)
    AC_SUBST(RESIPROCATE_LDFLAGS)

]) # CHECK_RESIPROCATE

## repro
# CHECK_REPRO attempts to find the repro project tree
# 
# If not found, the configure is aborted.  Otherwise, variables are defined for:
# REPRO_PATH     - the top of the repro tree
# REPRO_CFLAGS   
# REPRO_CXXFLAGS
# REPRO_LIBS
# REPRO_LDFLAGS
AC_DEFUN([CHECK_REPRO],
[
    AC_REQUIRE([CHECK_RESIPROCATE])
    
    AC_ARG_WITH([reprolib],
        [--with-reprolib specifies the directory where repro libraries are installed],
        [reprolib_path=$withval],
        [reprolib_path="$RESIPROCATE_LIB_PATH /usr/lib/resiprocate /usr/local/lib/resiprocate" ]
    )

    AC_MSG_CHECKING([for repro libraries])
    foundpath=NO
    for dir in $resiplib_path ; do
        if test -f "$dir/librepro.so";
        then
            if test -f "$dir/libtfm.so";
            then
                foundpath=$dir;
                break;
            else
                AC_MSG_ERROR([found librepro.so in $foundpath, but libtrf.so not found])
            fi
        fi
    done
    if test x_$foundpath = x_NO
    then
       AC_MSG_ERROR([not found; searched $reprolib_path])
    else
       AC_MSG_RESULT($foundpath)

       REPRO_LIB_PATH="$foundpath"
       REPRO_LDFLAGS=" -L$foundpath"
       REPRO_LIBS=" -lrepro -ltfm -lNetxx -lpopt -ldb_cxx"
    fi

    AC_SUBST(REPRO_LIB_PATH)
    AC_SUBST(REPRO_LIBS)
    AC_SUBST(REPRO_LDFLAGS)

]) # CHECK_REPRO



##  Generic find of an include
#   Fed from AC_DEFUN([SFAC_INCLUDE_{module name here}],
#
# $1 - sample include file
# $2 - variable name (for overridding with --with-$2
# $3 - help text
# $4 - directory name (assumed parallel with this script)
AC_DEFUN([SFAC_ARG_WITH_INCLUDE],
[
    SFAC_SRCDIR_EXPAND()

    AC_MSG_CHECKING(for [$4] [($1)] includes)
    AC_ARG_WITH( [$2],
        [ [$3] ],
        [ include_path=$withval ],
        [ include_path="$includedir $prefix/include /usr/include /usr/local/include [$abs_srcdir]/../[$4]/include [$abs_srcdir]/../[$4]/interface [$abs_srcdir]/../[$4]/src/test"]
    )

    for dir in $include_path ; do
        if test -f "$dir/[$1]";
        then
            foundpath=$dir;
            break;
        fi;
    done
    if test x_$foundpath = x_; then
       AC_MSG_ERROR("'$1' not found; searched $include_path")
    fi
        

]) # SFAC_ARG_WITH_INCLUDE


##  Generic find of a library
#   Fed from AC_DEFUN([SFAC_LIB_{module name here}],
#
# $1 - sample lib file
# $2 - variable name (for overridding with --with-$2
# $3 - help text
# $4 - directory name (assumed parallel with this script)
AC_DEFUN([SFAC_ARG_WITH_LIB],
[
    SFAC_SRCDIR_EXPAND()

    AC_MSG_CHECKING(for [$4] [($1)] libraries)
    AC_ARG_WITH( [$2],
        [ [$3] ],
        [ lib_path=$withval ],
        [ lib_path="$libdir $prefix/lib /usr/lib /usr/local/lib `pwd`/../[$4]/src `pwd`/../[$4]/sipXmediaMediaProcessing/src `pwd`/../[$4]/src/test/sipxunit" ]
    )
    foundpath=""
    for dir in $lib_path ; do
        if test -f "$dir/[$1]";
        then
            foundpath=$dir;
            break;
        fi;
    done
    if test x_$foundpath = x_; then
       AC_MSG_ERROR("'$1' not found; searched $lib_path")
    fi
]) # SFAC_ARG_WITH_LIB


AC_DEFUN([SFAC_SRCDIR_EXPAND], 
[
    abs_srcdir=`cd $srcdir && pwd`
])


AC_DEFUN([SFAC_FEATURE_SIP_TLS],
[
   AC_ARG_ENABLE(sip-tls, 
                 [  --enable-sip-tls        enable support for sips: and transport=tls (no)],
                 [], [enable_sip_tls=no])
   AC_MSG_CHECKING([Support for SIP over TLS])
   AC_MSG_RESULT(${enable_sip_tls})

   if test "${enable_sip_tls}" = "yes"
   then
      CFLAGS="-DSIP_TLS $CFLAGS"
      CXXFLAGS="-DSIP_TLS $CXXFLAGS"
   fi
])


AC_DEFUN([SFAC_FEATURE_SIPX_EZPHONE],
[
   AC_REQUIRE([CHECK_WXWIDGETS])

   AC_ARG_ENABLE(sipx-ezphone, 
                 [  --enable-sipx-ezphone    build the sipXezPhone example application (no)],
                 [], [enable_sipx_ezphone=no])
   AC_MSG_CHECKING([Building sipXezPhone])

   # If sipx-ezphone is requested, check for its prerequisites.
   if test x$enable_sipx_ezphone = xyes
   then
       if test x$enable_wxwidgets != xyes
       then
	      AC_MSG_ERROR([wxWidgets is required for sipXezPhone])
	      enable_sipx_ezphone=no
       fi
   fi

   AM_CONDITIONAL(BUILDEZPHONE, test x$enable_sipx_ezphone = xyes)

   AC_MSG_RESULT(${enable_sipx_ezphone})
])
