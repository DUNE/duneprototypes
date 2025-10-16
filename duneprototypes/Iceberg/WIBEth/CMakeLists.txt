file(GLOB channel_map_file *chanmap*.txt)
install_fw( LIST ${channel_map_file} )
install_fhicl()

include_directories("${dunedaqdataformats_DIR}/../../../include")
include_directories("${dunedetdataformats_DIR}/../../../include")

art_make(MODULE_LIBRARIES
                        lardataobj::RawData
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        messagefacility::MF_MessageLogger
			HDF5::HDF5
			duneprototypes::Protodune_hd_ChannelMap_PD2HDChannelMapService_service
                        BASENAME_ONLY
)
