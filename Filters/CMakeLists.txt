### Find VTK
FIND_PACKAGE( VTK REQUIRED )
IF (VTK_FOUND)
  INCLUDE(${USE_VTK_FILE})
ENDIF (VTK_FOUND)

FILE(GLOB SRC_VTKSDIS_FILTERS *.cxx *.h)

ADD_LIBRARY( vtkSDIS SHARED ${SRC_VTKSDIS_FILTERS} )

TARGET_LINK_LIBRARIES( 
                       vtkSDIS
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )
