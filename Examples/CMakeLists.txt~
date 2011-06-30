### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

ADD_EXECUTABLE( IterativeWarp IterativeWarp.cxx )
TARGET_LINK_LIBRARIES( IterativeWarp vtkKinshipFilters vtkCommon vtkGraphics vtkRendering)

ADD_EXECUTABLE( DeformableMesh DeformableMesh.cxx )
TARGET_LINK_LIBRARIES( DeformableMesh vtkKinshipFilters vtkCommon vtkGraphics vtkRendering)

ADD_EXECUTABLE( StraightenedCurvedPlanarReformation StraightenedCurvedPlanarReformation.cxx )
TARGET_LINK_LIBRARIES( StraightenedCurvedPlanarReformation vtkKinshipFilters vtkCommon vtkIO )

ADD_EXECUTABLE( SplineDrivenImageReslice SplineDrivenImageReslice.cxx )
TARGET_LINK_LIBRARIES( SplineDrivenImageReslice vtkKinshipFilters vtkCommon vtkIO )
