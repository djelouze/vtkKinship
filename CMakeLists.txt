PROJECT (vtkExplicitDeformableModel)

### The following are needed to avoid warnings. See CMake doc. ######
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
#####################################################################

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)

SET (LIBRARY_OUTPUT_PATH ${vtkExplicitDeformableModel_BINARY_DIR}/bin
     CACHE 
     PATH
     "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${vtkExplicitDeformableModel_BINARY_DIR}/bin
     CACHE
     PATH
     "Single output directory for building all executables.")
     
INCLUDE_DIRECTORIES (${LIBRARY_OUTPUT_PATH})
INCLUDE_DIRECTORIES (${EXECUTABLE_OUTPUT_PATH})
INCLUDE_DIRECTORIES (${vtkExplicitDeformableModel_SOURCE_DIR}/Filters)

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
        ON )

OPTION( BUILD_EXAMPLES
        "Build examples"
        OFF )


IF( BUILD_DOCUMENTATION )
   SUBDIRS(Documentation)
ENDIF( BUILD_DOCUMENTATION )

IF( BUILD_PARAVIEW_PLUGINS)
   SUBDIRS(Plugins)
ENDIF( BUILD_PARAVIEW_PLUGINS)

IF( BUILD_TESTS)
   SUBDIRS(Tests)
ENDIF( BUILD_TESTS )

IF( BUILD_EXAMPLES )
   SUBDIRS(Examples)
ENDIF( BUILD_EXAMPLES )

