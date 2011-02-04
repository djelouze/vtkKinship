FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

# FILTERS PLUGIN --------------------------------------------------
SET( PLUGINS_SRCS
                  ../Filters/vtkIterativePolyDataAlgorithm.cxx
                  ../Filters/vtkDeformableMesh.cxx
                  ../Filters/vtkPolyDataIterativeWarp.cxx
)

  # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
 ADD_PARAVIEW_PLUGIN(
   vtkPVExplicitDeformableModel          #<--Name for the plugin
  "1.0"                        #<--Version string
   SERVER_MANAGER_XML DeformableMesh.xml
                      PolyDataIterativeWarp.xml
   SERVER_MANAGER_SOURCES ${PLUGINS_SRCS}
   GUI_RESOURCE_FILES ExplicitDeformableModelMenu.xml
 )

TARGET_LINK_LIBRARIES( vtkPVExplicitDeformableModel vtkExplicitDeformableModel )
#------------------------------------------------------------------


