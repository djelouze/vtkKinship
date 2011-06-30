### Find VTK
FIND_PACKAGE( VTK REQUIRED )
IF (VTK_FOUND)
  INCLUDE(${USE_VTK_FILE})
ENDIF (VTK_FOUND)

FILE(GLOB SRC_VTKSDIR_FILTERS *.cxx *.h)

ADD_LIBRARY( vtkKinshipFilters SHARED ${SRC_VTKSDIR_FILTERS} )

TARGET_LINK_LIBRARIES( 
                       vtkSDIR
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )
