# History
# 2020-05-07 Support any wx version (made script more generic)
# (finally put some history in here)

export BUILDDIR
export WXWIN

USAGE="Usage $0 trunk|<wxWidgets-Version> [release|debug]"

if [[ "x$1" = "x" ]]
then
	echo $USAGE
	return
fi

WXWIN=~/devtools/wx/$1
if [[ ! -d "$WXWIN" = "xtrunk" ]]
then
	OLD=$WXWIN
	WXWIN=~/devtools/wx/wxWidgets-$1
	if [[ ! -d "$WXWIN" = "xtrunk" ]]
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
	echo $USAGE
	return
fi

export PATH=$BUILDDIR:$PATH
echo $PATH
