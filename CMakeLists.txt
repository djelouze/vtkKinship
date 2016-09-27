cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project (vtkKinship)

set(LIBRARY_OUTPUT_PATH ${vtkKinship_BINARY_DIR}/bin
     CACHE 
     PATH
     "Single output directory for building all libraries.")

set(EXECUTABLE_OUTPUT_PATH ${vtkKinship_BINARY_DIR}/bin
     CACHE
     PATH
     "Single output directory for building all executables.")
     
INCLUDE_DIRECTORIES (${LIBRARY_OUTPUT_PATH})
INCLUDE_DIRECTORIES (${EXECUTABLE_OUTPUT_PATH})
INCLUDE_DIRECTORIES (${vtkKinship_SOURCE_DIR}/Filters)
INCLUDE_DIRECTORIES (Utils/yaap)

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
   ENABLE_TESTING()
   INCLUDE(Dart)
   SUBDIRS(Tests)
ENDIF( BUILD_TESTS )

IF( BUILD_EXAMPLES )
   SUBDIRS(Examples)
ENDIF( BUILD_EXAMPLES )

