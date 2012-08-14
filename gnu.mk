################################################################################
# GNU Make Specific Instructions.                                              #
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
ifeq ($(VERBOSE),0)
.SILENT:
NICE_ECHO = @:

else
ifeq ($(VERBOSE),1)
.SILENT:
NICE_ECHO = @echo -e

else
ifeq ($(VERBOSE),2)
NICE_ECHO = @:

else
ifdef VERBOSE
$(warning "WARNING: Value of ($(VERBOSE)) for VERBOSE is not supported, using default value of 1")
endif

.SILENT:
NICE_ECHO = @echo -e

endif
endif
endif

################################################################################
# Build rules.                                                                 #
# - Create a function BUILD_VARS(target) which:                                #
#   o Creates the variable target_OBJS from target_SRCS, replacing extensions  #
#     .cpp and .c with .o                                                      #
#   o Updates the value of CLEANFILES, appending target_OBJS.                  #
#   o Updates the values of DEPFILES to include during a build.                #
# - Create a function PROG_DEPS(target,extension) which:                       #
#   o Creates a dependency rule for (target)(extenstion) for each object file  #
#     in the variable target_OBJS.  We have to do this one dependency at a     #
#     time due to the bug in GNU Makes implementation of eval causing memory   #
#     exhausted errors.                                                        #
################################################################################
define BUILD_VARS
 $(1)_OBJS = $(patsubst %.cpp,%.o,$($(1)_SRCS:.c=.o)) $($(1)_EXTRAOBJS)
 CLEANFILES += $$($(1)_OBJS)
 DEPFILES += $$(join $$(dir $$($(1)_OBJS)),$$(addprefix $(DEPDIR)/,$$(notdir $$($(1)_OBJS:.o=.d))))
endef

define PROG_DEPS
 $(foreach dep,$($(1)_OBJS),$(eval $(1:=$(2)): $(dep)))
endef

################################################################################
# The BUILD_VARS() and PROG_DEPS() functions are called with the appropriate   #
# elements of the PROGRAMS and ARCHIVES variables and corresponding extensions #
################################################################################
$(foreach target,$(PROGRAMS),$(eval $(call BUILD_VARS,$(target))))
$(foreach target,$(ARCHIVES),$(eval $(call BUILD_VARS,$(target))))
$(foreach target,$(SHAREDLIBS),$(eval $(call BUILD_VARS,$(target))))

$(foreach target,$(PROGRAMS),$(eval $(call PROG_DEPS,$(target),$(EXEEXT))))
$(foreach target,$(ARCHIVES),$(eval $(call PROG_DEPS,$(target),.a)))
$(foreach target,$(SHAREDLIBS),$(eval $(call PROG_DEPS,$(target),.$(SHAREDLIBEXT))))

################################################################################
# Include the dependency files, do note throw an error if the file does not    #
# exist.  With GNU Make, this is done with the (-include) command.  The        #
# dependency file is located in the $(DEP_DIR) directory from where the source #
# code file lives.                                                             #
################################################################################
ifdef DEPFILES
-include $(DEPFILES)
endif

################################################################################
# Create the variable DEPDIRS as a list of all parent directories of DEPFILES. #
################################################################################
DEPDIRS = $(dir $(DEPFILES))
