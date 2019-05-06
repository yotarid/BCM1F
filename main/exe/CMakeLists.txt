add_executable(ClusterExtractor.exe     src/ClusterExtractor.cxx  )
add_executable(Converter.exe            src/Converter.cxx         )
add_executable(ExampleProducer.exe      src/ExampleProducer.cxx   )
add_executable(ExampleSlowProducer.exe  src/ExampleSlowProducer.cxx   )
add_executable(ExampleReader.exe        src/ExampleReader.cxx     )
add_executable(FileChecker.exe          src/FileChecker.cxx       )
add_executable(IPHCConverter.exe        src/IPHCConverter.cxx     )
add_executable(MagicLogBook.exe         src/MagicLogBook.cxx      )
add_executable(OptionExample.exe        src/OptionExample.cxx     )
add_executable(RunListener.exe          src/RunListener.cxx       )
add_executable(euDataCollector.exe        src/euDataCollector.cxx )
add_executable(TestLogCollector.exe     src/TestLogCollector.cxx  )
add_executable(TestMonitor.exe          src/TestMonitor.cxx       )
add_executable(TestProducer.exe         src/TestProducer.cxx      )
add_executable(TestReader.exe           src/TestReader.cxx        )
add_executable(TestRunControl.exe       src/TestRunControl.cxx    )

# ${ADDITIONAL_LIBRARIES} is only set if e.g. the native reader processor is built (EUTelescope/LCIO)
target_link_libraries(ClusterExtractor.exe   EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(Converter.exe          EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(ExampleProducer.exe    EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(ExampleSlowProducer.exe    EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(ExampleReader.exe      EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(FileChecker.exe        EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(IPHCConverter.exe      EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(MagicLogBook.exe       EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(OptionExample.exe      EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(RunListener.exe        EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(euDataCollector.exe      EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(TestLogCollector.exe   EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(TestMonitor.exe        EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(TestProducer.exe       EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(TestReader.exe         EUDAQ ${ADDITIONAL_LIBRARIES})
target_link_libraries(TestRunControl.exe     EUDAQ ${ADDITIONAL_LIBRARIES})



##########################################################################
############################# ubcm Producer ##############################
##########################################################################

# add user include directory for ubcmProducer
INCLUDE_DIRECTORIES(/afs/cern.ch/user/y/yotarid/BCM1F/ubcm/sw/include)
INCLUDE_DIRECTORIES(/opt/cactus/include)
INCLUDE_DIRECTORIES(/usr/include/root)
# add user lib directory for ubcmProducer
LINK_DIRECTORIES(/afs/cern.ch/user/y/yotarid/BCM1F/ubcm/sw/lib)
LINK_DIRECTORIES(/opt/cactus/lib)
LINK_DIRECTORIES(/usr/lib64/root)
LINK_DIRECTORIES(/usr/lib64)
# compile ubcmProducer
add_executable(ubcmProducer.exe src/ubcmProducer.cxx)
# link against user lib
target_link_libraries(ubcmProducer.exe EUDAQ ${ADDITIONAL_LIBRARIES} ubcm pthread ncurses boost_filesystem boost_regex boost_system boost_thread pugixml cactus_uhal_log cactus_uhal_grammars cactus_uhal_uhal)

##########################################################################
############################### ubcm Reader ##############################
##########################################################################
add_executable(ubcmReader.exe  src/ubcmReader.cxx )
target_link_libraries(ubcmReader.exe EUDAQ ${ADDITIONAL_LIBRARIES})


###############################################################################
############################### VME Producer ##################################
###############################################################################

# add include directories for VMEProducer
INCLUDE_DIRECTORIES(/afs/cern.ch/user/y/yotarid/BCM1F/VME/ADC_DAQ/operateBCM1F_multiBoard/include)
INCLUDE_DIRECTORIES(/opt/xdaq/include)
#INCLUDE_DIRECTORIES(/d1/brmpro/sw/include)
INCLUDE_DIRECTORIES(/usr/include/root)

# add lib directories fro VMEProducer
LINK_DIRECTORIES(/afs/cern.ch/user/y/yotarid/BCM1F/VME/ADC_DAQ/operateBCM1F_multiBoard/lib)
LINK_DIRECTORIES(/opt/xdaq/lib)
#LINK_DIRECTORIES(/d1/brmpro/sw/lib)
LINK_DIRECTORIES(/usr/lib64/root)
LINK_DIRECTORIES(/usr/lib64)

# compile VMEProducer
add_executable(VMEProducer.exe src/VMEProducer.cxx)
# link against user lib
target_link_libraries( VMEProducer.exe EUDAQ ${ADDITIONAL_LIBRARIES} VME CAENVME pthread dl rt )

###############################################################################
############################### VME Reader ##################################
###############################################################################
add_executable(VMEReader.exe src/VMEReader.cxx)
target_link_libraries(VMEReader.exe EUDAQ ${ADDITIONAL_LIBRARIES})



INSTALL(TARGETS ubcmProducer.exe ubcmReader.exe VMEProducer.exe VMEReader.exe ClusterExtractor.exe Converter.exe ExampleProducer.exe ExampleSlowProducer.exe ExampleReader.exe FileChecker.exe IPHCConverter.exe MagicLogBook.exe OptionExample.exe RunListener.exe euDataCollector.exe TestLogCollector.exe TestMonitor.exe TestProducer.exe TestReader.exe TestRunControl.exe
  EXPORT ${PROJECT_NAME}
  RUNTIME DESTINATION bin COMPONENT MAIN_EXE
  LIBRARY DESTINATION lib COMPONENT MAIN_EXE
  ARCHIVE DESTINATION lib COMPONENT MAIN_EXE)


