set(name "AHCALProducer")
IF(NOT IGNORE_KNOWN_ISSUES AND WIN32) # issues compiling under WINDOWS only
  IF(NOT BUILD_allproducer) # only warn if not configuring all producers at the same time (for testing)
    MESSAGE(WARNING "There are known issues compiling ${name} on your platform. Set the IGNORE_KNOWN_ISSUES option to configure this producer to be build anyway.")
  ENDIF(NOT BUILD_allproducer)
ELSE(NOT IGNORE_KNOWN_ISSUES AND WIN32)
  set(sourcefiles src/AHCALProducer.cc src/ScReader.cc)
  set(ext_libraries "" )
  set(ext_lib_paths "" )
  set(ext_inc_paths "./include" )

  ADD_DEFINITIONS()
  LINK_DIRECTORIES( ${ext_lib_paths} )
  INCLUDE_DIRECTORIES( ${ext_inc_paths})
  ADD_EXECUTABLE(${name} ${sourcefiles})

  TARGET_LINK_LIBRARIES(${name} EUDAQ ${ext_libraries})

  INSTALL(TARGETS ${name}
    RUNTIME DESTINATION bin COMPONENT AHCALPRODUCER
    LIBRARY DESTINATION lib COMPONENT AHCALPRODUCER
    ARCHIVE DESTINATION lib COMPONENT AHCALPRODUCER )
ENDIF(NOT IGNORE_KNOWN_ISSUES AND WIN32)
