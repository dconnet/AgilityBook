# History
# 2023-03-11 Allow building non-fat binaries on Mac
# 2020-05-03 Fix directory test
# 2020-05-07 Support any wx version (made script more generic)
# (finally put some history in here)

export BUILDDIR
export WXWIN

# Note: This is a kludge to pass the arch type to my configure.in
# wx-config does not have a way of getting "enable-macosx_arch" flags back
# TODO: Modify my configure.in to pass which wx-config to use, then it will
# know what arch to set. And this script will go away.
export WX_ARCH

USAGE="Usage setwxpath.sh <directory>|wxWidgets-<version> [release|debug][ x64|arm|fat]\nNote: x64/arm arguments are only used on MacOS\nex: setwxpath.sh trunk release\nex: setwxpath.sh 3.1.5 release"

if [[ "x$1" = "x" ]]
then
	echo "$USAGE"
	return
fi

WXWIN=~/devtools/wx/$1
if [[ ! -d "$WXWIN" ]]
then
	OLD=$WXWIN
	WXWIN=~/devtools/wx/wxWidgets-$1
	if [[ ! -d "$WXWIN" ]]
	then
		echo Neither $OLD or $WXWIN exist!
		return
	fi
fi

BUILDDIR="$WXWIN/build"

if [[ "x$2" = "xdebug" ]]
then
	BUILDDIR+="-debug"
elif [[ "x$2" = "xrelease" ]]
then
	BUILDDIR+="-release"
else
	echo "$USAGE"
	return
fi

case `uname` in
Darwin*)
	case "$3" in
	x64)
		BUILDDIR+="-x64"
		WX_ARCH="-arch x86_64"
		;;
	arm)
		BUILDDIR+="-arm"
		WX_ARCH="-arch arm64"
		;;
	fat)
		BUILDDIR+="-fat"
		WX_ARCH="-arch x86_64 -arch arm64"
		;;
	*)
		echo "$USAGE"
		return
		;;
	esac
	;;
esac

export PATH=$BUILDDIR:$PATH
echo $PATH
