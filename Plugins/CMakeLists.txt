FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

# FILTERS PLUGIN --------------------------------------------------
SET( PLUGINS_SRCS
                  ../Filters/vtkImageConvolution.cxx
                  ../Filters/vtkImageLocalConvolution.cxx
                  ../Filters/vtkDeformableMesh.cxx
                  
)

  # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
 ADD_PARAVIEW_PLUGIN(
   vtkPVKinship          #<--Name for the plugin
  "1.0"                        #<--Version string
   SERVER_MANAGER_XML ConvolutionFilters.xml DeformableMesh.xml  #<-- server manager xml
   SERVER_MANAGER_SOURCES ${PLUGINS_SRCS}
 )

TARGET_LINK_LIBRARIES( vtkPVKinship vtkKinshipFilters )
#------------------------------------------------------------------


