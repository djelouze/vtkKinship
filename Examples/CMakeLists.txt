### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

ADD_EXECUTABLE( StraightenedCurvedPlanarReformation StraightenedCurvedPlanarReformation.cxx )
TARGET_LINK_LIBRARIES( StraightenedCurvedPlanarReformation vtkSDIS vtkCommon vtkIO )

ADD_EXECUTABLE( SplineDrivenImageSlicer SplineDrivenImageSlicer.cxx )
TARGET_LINK_LIBRARIES( SplineDrivenImageSlicer vtkSDIS vtkCommon vtkIO )
