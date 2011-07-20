### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

ADD_EXECUTABLE( testFrenetSerretFrame testFrenetSerretFrame.cxx
              )
TARGET_LINK_LIBRARIES( 
                       testFrenetSerretFrame
                       vtkSDIS
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )

ADD_TEST( testFrenetSerretFrame ${EXECUTABLE_OUTPUT_PATH}/testFrenetSerretFrame )

ADD_EXECUTABLE( testSplineDriveImageSlicer testSplineDriveImageSlicer.cxx
              )
TARGET_LINK_LIBRARIES( 
                       testSplineDriveImageSlicer
                       vtkSDIS
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )

ADD_TEST( testSplineDriveImageSlicer ${EXECUTABLE_OUTPUT_PATH}/testSplineDriveImageSlicer )


