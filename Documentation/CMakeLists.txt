
# Build the documentation
FIND_PACKAGE(Doxygen)
EXECUTE_PROCESS( COMMAND ${DOXYGEN_EXECUTABLE} "${vtkKinship_SOURCE_DIR}/Documentation/Doxyfile"
                 WORKING_DIRECTORY "${vtkKinship_BINARY_DIR}/Documentation"
                 OUTPUT_QUIET
                 ERROR_QUIET
               )
