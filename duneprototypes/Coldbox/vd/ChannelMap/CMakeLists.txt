
# Tom Junk 2021

install_fhicl()
install_headers()
install_source()

file(GLOB channel_map_file *chanmap*.txt)
install_fw( LIST ${channel_map_file} )

file(GLOB channel_map_file *ChannelMap*.txt)
install_fw( LIST ${channel_map_file} )

# Add plugin for each service

if( DEFINED ENV{CANVAS_DIR} )
  set( persistency_lib art::Utilities canvas::canvas)
else()
  set( persistency_lib art::Utilities)
endif()

simple_plugin(VDColdboxChannelMapService "service"
              ${ART_PERSISTENCY_ROOTDB}
              art::Framework_Services_Registry
              ${persistency_lib}
              SQLite::SQLite3
              messagefacility::MF_MessageLogger
              ${CETLIB_LIBS}
              ROOT::Core ROOT::Hist ROOT::Tree
)

simple_plugin(VDColdboxTDEChannelMapService "service"
              ${ART_PERSISTENCY_ROOTDB}
              art::Framework_Services_Registry
              ${persistency_lib}
              SQLite::SQLite3
              messagefacility::MF_MessageLogger
              ${CETLIB_LIBS}
              ROOT::Core ROOT::Hist ROOT::Tree
)
