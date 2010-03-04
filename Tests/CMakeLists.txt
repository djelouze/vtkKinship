
### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)


ADD_EXECUTABLE( testImageConvolution testImageConvolution.cxx )
TARGET_LINK_LIBRARIES( 
                       testImageConvolution
                       vtkKinshipFilters
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )
ADD_EXECUTABLE( testPolyDataNeighbourhood testPolyDataNeighbourhood.cxx )
TARGET_LINK_LIBRARIES( 
                       testPolyDataNeighbourhood
                       vtkKinshipFilters
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )
