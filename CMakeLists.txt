PROJECT (VTKSDIS)

### The following are needed to avoid warnings. See CMake doc. ######
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
#####################################################################

SET (LIBRARY_OUTPUT_PATH ${VTKSDIS_BINARY_DIR}/bin
     CACHE 
     PATH
     "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${VTKSDIS_BINARY_DIR}/bin
     CACHE
     PATH
     "Single output directory for building all executables.")
     
INCLUDE_DIRECTORIES (${LIBRARY_OUTPUT_PATH})
INCLUDE_DIRECTORIES (${EXECUTABLE_OUTPUT_PATH})
INCLUDE_DIRECTORIES (${VTKSDIS_SOURCE_DIR}/Filters)

MARK_AS_ADVANCED(
   LIBRARY_OUTPUT_PATH
   EXECUTABLE_OUTPUT_PATH
)

ADD_SUBDIRECTORY(
  Filters 
)

OPTION( BUILD_DOCUMENTATION 
        "Build Doxygen documentation"
        OFF )

OPTION( BUILD_PARAVIEW_PLUGINS
        "Build paraview plugin"
        OFF )

OPTION( BUILD_TESTS
        "Build tests (filters and sources)"
        OFF )

OPTION( BUILD_EXAMPLES
        "Build examples"
        OFF )


IF( BUILD_DOCUMENTATION )
   ADD_SUBDIRECTORY(Documentation)
ENDIF( BUILD_DOCUMENTATION )

IF( BUILD_PARAVIEW_PLUGINS)
   ADD_SUBDIRECTORY(Plugins)
ENDIF( BUILD_PARAVIEW_PLUGINS)

IF( BUILD_TESTS)
   ENABLE_TESTING()
   INCLUDE(Dart)
   ADD_SUBDIRECTORY(Tests)
ENDIF( BUILD_TESTS )

IF( BUILD_EXAMPLES )
   ADD_SUBDIRECTORY(Examples)
ENDIF( BUILD_EXAMPLES )

