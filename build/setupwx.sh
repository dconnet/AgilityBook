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

USAGE="Usage $0 trunk|3.1.5|3.1.4|3.1.3|3.1.2 [debug|release]"

if test "x$1" = "xtrunk"; then
	WXWIN=~/devtools/wx/trunk
	VERSION="--disable-compat28 --disable-compat30"
	MAC_CONFIG_PARAMS=" --disable-nativedvc"
	MAC_MIN_OS=10.10
elif test "x$1" = "x3.1.5"; then
	WXWIN=~/devtools/wx/wxWidgets-3.1.5
	VERSION="--disable-compat28 --disable-compat30"
	MAC_CONFIG_PARAMS=" --disable-nativedvc"
	MAC_MIN_OS=10.10
elif test "x$1" = "x3.1.4"; then
	WXWIN=~/devtools/wx/wxWidgets-3.1.4
	VERSION="--disable-compat28 --disable-compat30"
	MAC_CONFIG_PARAMS=" --disable-nativedvc"
	MAC_MIN_OS=10.10
elif test "x$1" = "x3.1.3"; then
	WXWIN=~/devtools/wx/wxWidgets-3.1.3
	VERSION="--disable-compat28 --disable-compat30"
	MAC_MIN_OS=10.7
elif test "x$1" = "x3.1.2"; then
	WXWIN=~/devtools/wx/wxWidgets-3.1.2
	VERSION="--disable-compat28 --disable-compat30"
	MAC_MIN_OS=10.7
else
	echo $USAGE
	exit
fi
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

rm -rf $BUILDDIR
mkdir $BUILDDIR
cd $BUILDDIR

case `uname` in
Darwin*)
	export CXXFLAGS="-std=c++14 -stdlib=libc++"
	export OBJCXXFLAGS="-std=c++14 -stdlib=libc++"
	export LDFLAGS="-stdlib=libc++"
	export LIBS="-lc++"
	TARGETARCH="--enable-macosx_arch=x86_64"

	if test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk; then
		echo "Using 10.11 SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk
		MIN_OS=$MAC_MIN_OS

	elif test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk; then
		echo "Using 10.12 SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk
		MIN_OS=$MAC_MIN_OS

	elif test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk; then
		echo "Using 10.13 SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk
		MIN_OS=$MAC_MIN_OS

	elif test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk; then
		echo "Using 10.14 SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk
		MIN_OS=$MAC_MIN_OS

	elif test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk; then
		echo "Using 10.15 SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk
		MIN_OS=$MAC_MIN_OS

	elif test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk; then
		echo "Using 11.0 SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk
		MIN_OS=$MAC_MIN_OS

	elif test -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.1.sdk; then
		echo "Using 11.1 SDK"
		TARGETSDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.1.sdk
		MIN_OS=$MAC_MIN_OS

	fi

	if test "x$TARGETSDK" = "x"; then
		echo "ERROR: Can't find an SDK - 10.11 or newer is required"
		exit
	fi

	CONFIG_PARAMS+=" --with-macosx-version-min=$MIN_OS --with-macosx-sdk=$TARGETSDK"
	;;

Linux)
	if test "x$2" = "xdebug"; then
		export CFLAGS="-g"
		export CXXFLAGS="-g -std=c++14"
		export OBJCXXFLAGS="-g -std=c++14"
	else
		export CXXFLAGS="-std=c++14"
		export OBJCXXFLAGS="-std=c++14"
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

LIBRARIES+=" --with-cxx=14 --with-expat=builtin --with-regex=builtin --with-zlib=builtin --without-libiconv --without-liblzma"

echo "../configure $COMPILERS $DEBUG $VERSION $LIBRARIES $CONFIG_PARAMS"

../configure $COMPILERS $DEBUG $VERSION $LIBRARIES $CONFIG_PARAMS
