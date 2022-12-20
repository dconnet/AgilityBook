# A script I use on the Mac to help manage multiple build environments.
# This allows working with the released version and also testing using
# the current SourceForge trunk. (Use svn to manage that)
#
# This will clean and configure the build directory for a wxWidget build.
# (I put this script in my home dir)
#
# Note: Found old OSX SDKs at https://github.com/phracker/MacOSX-SDKs/releases
#
# History
# 2022-12-20 Simplied SDKs (assumes we have at least 12.0) so we no longer need
#            to update this and all the configure.in files each time xcode
#            updates the SDK.
# 2022-12-15 Added SDK13.1
# 2022-11-07 Min OSX10.13, Added SDK13.0
# 2022-07-05 Change arg parsing to allow passing any wx version
# 2022-04-04 Upgraded 3.1.5 to 3.1.6
# 2022-01-22 Changed to C++17. Min OSX10.12
# 2021-12-20 Added SDK12.1
# 2021-11-30 Added SDK12.0, arm64.
#            Change -with...builtin options to --disable-sys-libs
# 2021-04-14 Upgraded 3.1.4 to 3.1.5
# 2021-01-20 Added SDK11.1
# 2020-11-22 Added SDK11.0 (reordered to make older SDKs prefered)
# 2020-11-05 Min OSX SDK for 3.1.4 is 10.11.
# 2020-07-23 Upgraded 3.1.3 to 3.1.4
# 2019-10-28 Upgraded 3.1.2 to 3.1.3
# 2019-08-24 Make gtk3 the default.
# 2019-08-17 Remove carbon
# 2019-07-26 Add support for Ubuntu (18.04), changed default build dir name
# 2018-12-11 Upgraded 3.1.1 to 3.1.2
# 2018-02-19 Upgraded 3.1.0 to 3.1.1
# 2016-11-29 Removed wx3.0 support
# 2016-02-29 Added wx3.1 support
# 2015-11-10 Removed wx2.8/osx10.4 support
# 2015-11-25 Strip 10.5/10.8 sdk (my build machine is 10.11 now)

export BUILDDIR=
export DEBUG=
export LIBRARIES=
export MAC_CONFIG_PARAMS=
export MAC_MIN_OS=
export TARGETARCH=
export TARGETSDK=
export VERSION=
export WXWIN=

USAGE="Usage $0 <directory>|wxWidgets-<version> [debug|release] [test]\nex: setupwx.sh trunk debug\nex: setupwx.sh 3.1.7 release"

if [[ "x$1" = "x" ]]
then
	echo "$USAGE"
	exit
fi

WXWIN=~/devtools/wx/$1
if [[ ! -d "$WXWIN" ]]
then
	OLD=$WXWIN
	WXWIN=~/devtools/wx/wxWidgets-$1
	if [[ ! -d "$WXWIN" ]]
	then
		echo Neither $OLD or $WXWIN exist!
		exit
	fi
fi

# Assume we're targetting 3.1.6 or newer.
VERSION="--disable-compat28 --disable-compat30"
MAC_CONFIG_PARAMS=" --disable-nativedvc"
MAC_MIN_OS=10.13

BUILDDIR="$WXWIN/build"

CONFIG_PARAMS=" --disable-mediactrl --disable-shared --enable-unicode"

case `uname` in
Darwin*)
	# There are known bugs with the native wxDVC under Cocoa, use generic
	# Disable curl on Mac - use NSURLSession (since I'm targetting a lower OS,
	# I get a warning that curl is built for a newer OS - so just avoid it).
	CONFIG_PARAMS+=" --without-libcurl --with-osx"$MAC_CONFIG_PARAMS
	;;
Linux)
	CONFIG_PARAMS+=" --with-gtk=3"
	;;
esac

if test "x$2" = "xdebug"; then
	DEBUG="--enable-debug --enable-debug_info --enable-debug_gdb"
	BUILDDIR+="-debug"
elif test "x$2" = "xrelease"; then
	DEBUG="--disable-debug_flag"
	BUILDDIR+="-release"
else
	$0 $1 debug
	$0 $1 release
	exit
fi

TEST_ONLY=0
if test "x$3" = "xtest"; then
	TEST_ONLY=1
fi

if test $TEST_ONLY = 0; then
	rm -rf $BUILDDIR
	mkdir $BUILDDIR
	cd $BUILDDIR
fi

case `uname` in
Darwin*)
	export CXXFLAGS="-std=c++17 -stdlib=libc++"
	export OBJCXXFLAGS="-std=c++17 -stdlib=libc++"
	export LDFLAGS="-stdlib=libc++"
	export LIBS="-lc++"
	TARGETARCH="--enable-macosx_arch=x86_64"

	# Leaving this in for a couple compilers for an example.
	# Current min deployment is 10.13 (as set via wx-config, via setupwx.sh)
	if test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk; then
		echo "Using 10.13 SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk
		MIN_OS=$MAC_MIN_OS

	# Note 12.0 added arm64 support
	# That's where this originally changed to setting options as below.

	# Assume the current SDK is at least 12.0
	elif test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk; then
		echo "Using Current SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
		TARGETARCH="--enable-macosx_arch=x86_64,arm64"
		MIN_OS=$MAC_MIN_OS

	fi

	if test "x$TARGETSDK" = "x"; then
		echo "ERROR: Can't find an SDK - 10.13 or newer is required"
		echo ls -CF /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs
		ls -CF /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs
		exit
	fi

	CONFIG_PARAMS+=" --with-macosx-version-min=$MIN_OS --with-macosx-sdk=$TARGETSDK"
	;;

Linux)
	if test "x$2" = "xdebug"; then
		export CFLAGS="-g"
		export CXXFLAGS="-g -std=c++17"
		export OBJCXXFLAGS="-g -std=c++17"
	else
		export CXXFLAGS="-std=c++17"
		export OBJCXXFLAGS="-std=c++17"
	fi
	#export LDFLAGS="-stdlib=libc++"
	#export LIBS="-lc++"
	;;

*)
	echo "ERROR: No idea what you're on..."
	exit
	;;
esac

CONFIG_PARAMS+=" $TARGETARCH"

LIBRARIES+=" --with-cxx=17 --disable-sys-libs --without-libiconv --without-liblzma"

echo "../configure $COMPILERS $DEBUG $VERSION $LIBRARIES $CONFIG_PARAMS"

if test $TEST_ONLY = 0; then
	../configure $COMPILERS $DEBUG $VERSION $LIBRARIES $CONFIG_PARAMS
fi
