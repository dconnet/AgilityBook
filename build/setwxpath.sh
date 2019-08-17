export BUILDDIR
export WXWIN

USAGE="Usage $0 trunk|3.1 [release|debug]"

if test "x$1" = "xtrunk"; then
	WXWIN=~/devtools/wx/trunk
elif test "x$1" = "x3.1"; then
	WXWIN=~/devtools/wx/wxWidgets-3.1.2
else
	echo $USAGE
	return
fi
BUILDDIR="$WXWIN/build"

if test "x$2" = "xdebug"; then
	BUILDDIR+="-debug"
elif test "x$2" = "xrelease"; then
	BUILDDIR+="-release"
	DUMMY=0
else
	echo $USAGE
	return
fi

export PATH=$BUILDDIR:$PATH
echo $PATH
