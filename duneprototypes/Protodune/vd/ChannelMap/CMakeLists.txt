include_directories($ENV{NLOHMANN_JSON_INC})

install_fhicl()
install_headers()
install_source()

file(GLOB channel_map_file *ChannelMap*.txt)
install_fw( LIST ${channel_map_file} )

art_make( ALLOW_UNDERSCORES
          SERVICE_LIBRARIES 
                           art::Framework_Services_Registry
                           art::Framework_Principal
                           art::Framework_Core
                           art::Persistency_Provenance
                           messagefacility::MF_MessageLogger
                           ROOT::Core
 	                   duneprototypes::Protodune_vd_ChannelMap
)

# install PDVD_PDS_Mapping.json with mapping of the photon detectors
install_fw(LIST PDVD_PDS_Mapping_v04152025.json)
