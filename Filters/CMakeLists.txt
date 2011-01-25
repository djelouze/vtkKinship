### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

### Listing of the files used in VesselExtraction
FILE(GLOB SRC_EXPLICITDEFORMABLEMODEL *.cxx *.h)
ADD_LIBRARY( vtkExplicitDeformableModel SHARED ${SRC_EXPLICITDEFORMABLEMODEL} )

TARGET_LINK_LIBRARIES( vtkExplicitDeformableModel
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )
