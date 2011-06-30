FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

# FILTERS PLUGIN --------------------------------------------------
SET( PLUGINS_SRCS
                  ../Filters/vtkFrenetSerretFrame.cxx
                  ../Filters/vtkSplineDrivenImageSlicer.cxx
)

  # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
 ADD_PARAVIEW_PLUGIN(
   pvSDIS          #<--Name for the plugin
  "1.0"                        #<--Version string
   SERVER_MANAGER_XML FrenetSerretFrame.xml
                      SplineDrivenImageSlicer.xml
   SERVER_MANAGER_SOURCES ${PLUGINS_SRCS}
 )

TARGET_LINK_LIBRARIES( pvSDIS vtkSDIS )

#------------------------------------------------------------------


