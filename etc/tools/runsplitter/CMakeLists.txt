SET(name "EURunsplitter.exe")

SET(sourcefiles src/Runsplitter.cc)

ADD_EXECUTABLE(${name} ${sourcefiles})

target_link_libraries(${name} EUDAQ )

INSTALL(TARGETS ${name}
  RUNTIME DESTINATION bin COMPONENT MAIN_EXE
  LIBRARY DESTINATION lib COMPONENT MAIN_EXE
  ARCHIVE DESTINATION lib COMPONENT MAIN_EXE)
