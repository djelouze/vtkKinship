FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

# FILTERS PLUGIN --------------------------------------------------
SET( PLUGINS_SRCS
                  ../Filters/vtkImageConvolution.cxx
                  ../Filters/vtkImageLocalConvolution.cxx
                  ../Filters/vtkImageCropVOI.cxx
                  ../Filters/vtkImageMomentKernelSource.cxx
                  ../Filters/vtkIterativePolyDataAlgorithm.cxx
                  ../Filters/vtkDeformableMesh.cxx
                  ../Filters/vtkRegularizedDeformableMesh.cxx
                  ../Filters/vtkThickTubeFilter.cxx
                  
)

  # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
 ADD_PARAVIEW_PLUGIN(
   vtkPVKinship          #<--Name for the plugin
  "1.0"                        #<--Version string
   SERVER_MANAGER_XML ImageConvolution.xml
                      ImageLocalConvolution.xml
                      ImageMomentKernelSource.xml
                      DeformableMesh.xml
                      RegularizedDeformableMesh.xml
                      ThickTubeFilter.xml
   SERVER_MANAGER_SOURCES ${PLUGINS_SRCS}
 )

TARGET_LINK_LIBRARIES( vtkPVKinship vtkKinshipFilters )
#------------------------------------------------------------------


