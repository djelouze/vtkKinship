# Copyright (c) 2010, Jérôme Velut
# All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the University of California, Berkeley nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

PROJECT (vtkKinship)

### The following are needed to avoid warnings. See CMake doc. ######
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
#####################################################################

SET (LIBRARY_OUTPUT_PATH ${vtkKinship_BINARY_DIR}/bin
     CACHE 
     PATH
     "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${vtkKinship_BINARY_DIR}/bin
     CACHE
     PATH
     "Single output directory for building all executables.")
     
INCLUDE_DIRECTORIES (${LIBRARY_OUTPUT_PATH})
INCLUDE_DIRECTORIES (${EXECUTABLE_OUTPUT_PATH})
INCLUDE_DIRECTORIES (${vtkKinship_SOURCE_DIR}/Filters)

MARK_AS_ADVANCED(
   LIBRARY_OUTPUT_PATH
   EXECUTABLE_OUTPUT_PATH
)

SUBDIRS(
  Filters 
)

OPTION( BUILD_DOCUMENTATION 
        "Build Doxygen documentation"
        OFF )

OPTION( BUILD_PARAVIEW_PLUGINS
        "Build paraview plugin (filters and sources)"
        OFF )

OPTION( BUILD_TESTS
        "Build tests (filters and sources)"
        OFF )

IF( BUILD_DOCUMENTATION )
   SUBDIRS(Doc)
ENDIF( BUILD_DOCUMENTATION )

IF( BUILD_PARAVIEW_PLUGINS)
   SUBDIRS(Plugins)
ENDIF( BUILD_PARAVIEW_PLUGINS)

IF( BUILD_TESTS)
   SUBDIRS(Tests)
ENDIF( BUILD_TESTS )

