### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

ADD_EXECUTABLE( IterativeWarp IterativeWarp.cxx )
TARGET_LINK_LIBRARIES( IterativeWarp vtkExplicitDeformableModel vtkCommon vtkGraphics vtkRendering)

ADD_EXECUTABLE( DeformableMesh DeformableMesh.cxx )
TARGET_LINK_LIBRARIES( DeformableMesh vtkExplicitDeformableModel vtkCommon vtkGraphics vtkRendering)