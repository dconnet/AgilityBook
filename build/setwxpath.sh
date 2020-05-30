# History
# 2020-05-03 Fix directory test
# 2020-05-07 Support any wx version (made script more generic)
# (finally put some history in here)

export BUILDDIR
export WXWIN

USAGE=$'Usage setwxpath.sh <directory>|wxWidgets-<version> [release|debug]\nex: setwxpath.sh trunk release\nex: setwxpath.sh 3.1.3 release'

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
	DUMMY=0
else
	echo "$USAGE"
	return
fi

export PATH=$BUILDDIR:$PATH
echo $PATH
