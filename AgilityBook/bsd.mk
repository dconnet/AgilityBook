################################################################################
# BSD Make Specific Instructions.                                              #
################################################################################
# Copyright (C) 2005 Jason But, Centre for Advanced Internet Architectures,    #
# Swinburne University                                                         #
#                                                                              #
# This library is free software; you can redistribute it and/or modify it under#
# the terms of the GNU Lesser General Public License as published by the Free  #
# Software Foundation; either version 2.1 of the License, or (at your option)  #
# any later version.                                                           #
#                                                                              #
# This library is distributed in the hope that it will be useful, but WITHOUT  #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS# 
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more#
# details.                                                                     #
#                                                                              #
# You should have received a copy of the GNU Lesser General Public License along 
# along with this library; if not, write to the Free Software Foundation, Inc.,#
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                      #
################################################################################

################################################################################
# Different types of output based on the variable VERBOSE.                     #
#  0           - Super Quiet output                                            #
#  1 (default) - normal/clean output.                                          #
#  2           - verbose output.                                               #
#  other       - Display warning and use default VERBOSE value of 1            #
################################################################################
.ifndef VERBOSE
VERBOSE=1
.endif

.if $(VERBOSE) == 0
.SILENT:
NICE_ECHO = @:

.elif $(VERBOSE) == 2
NICE_ECHO = @:

.else 
.if $(VERBOSE) != 1
.warning "WARNING: Value of ($(VERBOSE)) for VERBOSE is not supported, using default value of 1"
VERBOSE=1
.endif

.SILENT:
NICE_ECHO = @echo -e

.endif

################################################################################
# Linking an executable.                                                       #
# Create variable xxx_OBJS from xxx_SRCS where:                                #
# - xxx is one of the programs listed in the variable PROGRAMS.                #
# - xxx_SRCS is a list of source files, replace the extensions .cpp and .c     #
#   with .o                                                                    #
# Update the value of CLEANFILES, append all determined object files.          #
# Create a dependency for xxx where xxx$(EXEEXT) is dependent on all its       #
# object files.                                                                #
# Update the value of DEPFILES, add all object files with the extension        #
# changed to .d                                                                #
################################################################################
.for prog in $(PROGRAMS)
$(prog)_OBJS = $($(prog)_SRCS:C/.cpp$/.o/:.c=.o) $($(prog)_EXTRAOBJS)
CLEANFILES += $($(prog)_OBJS)
$(prog)$(EXEEXT): $($(prog)_OBJS)
DEPFILES+= $($(prog)_OBJS:C/([^\/]*)$/$(DEPDIR)\/&/:.o=.d)
.endfor

.for ar in $(ARCHIVES)
$(ar)_OBJS = $($(ar)_SRCS:C/.cpp$/.o/:.c=.o) $($(ar)_EXTRAOBJS)
CLEANFILES += $($(ar)_OBJS)
$(ar).a: $($(ar)_OBJS)
DEPFILES+= $($(ar)_OBJS:C/([^\/]*)$/$(DEPDIR)\/&/:.o=.d)
.endfor

################################################################################
# Include the dependency files, do note throw an error if the file does not    #
# exist.  With BSD Make, this is complicated and messy.  We need to include    #
# each file independently, as such we must loop through each file, check if it #
# exists, and only then try to include it.                                     #
################################################################################
.for dep in $(DEPFILES)
.if exists($(dep))

include $(dep)

.endif
.endfor

################################################################################
# Create the variable DEPDIRS as a list of all parent directories of DEPFILES. #
################################################################################
DEPDIRS = $(DEPFILES:H)

################################################################################
# SIGH, BSDMake does not attempt to regenerate the Makefile automatically.  So #
# this is a messy workaround.  The workaround is only when the make target is  #
# not Makefile, if we are directly attempting to remake the Makefile, we just  #
# let the .BEGIN target fall through to regenerating it.  Otherwise we:        #
# - See if the Makefile needs regenerating, if not just fall through and run   #
#   the normal make target.                                                    #
# - If the Makefile needs regenerating, rerun make to generate the Makefile.   #
# - Once the Makefile is regenerated, we echo a warning and fall through to    #
#   make the specified target.  We would like to rerun make on the specified   #
#   target (with the newly created Makefile) and then terminate the current    #
#   make instance (with something like && $(MAKE) $(.TARGETS) && exit 0), but  #
#   the exit command does not cause make to terminate.                         #
################################################################################
.BEGIN:
.ifnmake Makefile
	@$(MAKE) -q Makefile > /dev/null || \
	($(MAKE) Makefile && echo "WARNING: BSD Make, you should re-run make since your project will be made with the original Makefile")
.endif
