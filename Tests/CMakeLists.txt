### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

ADD_EXECUTABLE( testDeformableMesh testDeformableMesh.cxx )
TARGET_LINK_LIBRARIES( testDeformableMesh vtkExplicitDeformableModel vtkCommon vtkGraphics vtkRendering)

ADD_TEST( DeformableMesh ${EXECUTABLE_OUTPUT_PATH}/testDeformableMesh )

