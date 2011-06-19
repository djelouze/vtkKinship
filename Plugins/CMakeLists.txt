FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

# FILTERS PLUGIN --------------------------------------------------
SET( PLUGINS_SRCS
                  ../Filters/vtkImageConvolution.cxx
                  ../Filters/vtkImageLocalConvolution.cxx
                  ../Filters/vtkImageCropVOI.cxx
                  ../Filters/vtkImageMomentKernelSource.cxx
                  ../Filters/vtkIterativePolyDataAlgorithm.cxx
                  ../Filters/vtkPolyDataIterativeWarp.cxx
                  ../Filters/vtkDeformableMesh.cxx
                  ../Filters/vtkRegularizedDeformableMesh.cxx
                  ../Filters/vtkThickTubeFilter.cxx
                  ../Filters/vtkSmoothPolyDataVectors.cxx
                  ../Filters/vtkFrenetSerretFrame.cxx
                  ../Filters/vtkSplineDrivenImageReslice.cxx
                  ../Filters/vtkPolyDataToBinaryImage.cxx
                  ../Filters/vtkImageEigenElements.cxx
                  ../Filters/vtkSymmetricRecursivePolyDataFilter.cxx
                  ../Filters/vtkUVSphereSource.cxx
                  ../Filters/vtkICPPolyDataFilter.cxx
                  
)

  # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
 ADD_PARAVIEW_PLUGIN(
   vtkPVKinship          #<--Name for the plugin
  "1.0"                        #<--Version string
   SERVER_MANAGER_XML ImageConvolution.xml
                      ImageLocalConvolution.xml
                      ImageMomentKernelSource.xml
                      DeformableMesh.xml
                      PolyDataIterativeWarp.xml
                      RegularizedDeformableMesh.xml
                      ThickTubeFilter.xml
                      SmoothPolyDataVectors.xml
                      FrenetSerretFrame.xml
                      SplineDrivenImageReslice.xml
                      PolyDataToBinaryImage.xml
                      ImageEigenElements.xml
                      UVSphereSource.xml
                      ICPPolyDataFilter.xml
   SERVER_MANAGER_SOURCES ${PLUGINS_SRCS}
   GUI_RESOURCE_FILES vtkKinshipFilterMenu.xml
 )

TARGET_LINK_LIBRARIES( vtkPVKinship vtkKinshipFilters )

  # GIPL Image File Reader
 ADD_PARAVIEW_PLUGIN(
   OFFReader          #<--Name for the plugin
  "1.0"                        #<--Version string
   SERVER_MANAGER_SOURCES ../Filters/vtkOFFReader.cxx
   SERVER_MANAGER_XML OFFReader.xml  #<-- server manager xml
   GUI_RESOURCE_FILES OFFReaderGUI.xml
 )
TARGET_LINK_LIBRARIES( OFFReader vtkKinshipFilters )
#------------------------------------------------------------------


