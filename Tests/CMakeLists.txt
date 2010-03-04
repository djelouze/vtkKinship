INCLUDE_DIRECTORIES (${VTK_ALGO_SRCS_PATH})

### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

### Listing of the files used in VesselExtraction
FILE(GLOB SRC_TESTCONVOLUTIONS *.cxx *.h)

### Defines VesselExtractionVTK lib an links
IF( WIN32 )
   ADD_LIBRARY( vtkConvolutions STATIC ${SRC_TESTCONVOLUTIONS} )
ELSE( WIN32 )
   ADD_EXECUTABLE( testConvolutions ${SRC_TESTCONVOLUTIONS} )
ENDIF( WIN32)

TARGET_LINK_LIBRARIES( 
                       testConvolutions
                       vtkConvolutions
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )
