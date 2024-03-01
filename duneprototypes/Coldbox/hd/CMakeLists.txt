include_directories("${dunedaqdataformats_DIR}/../../../include")
include_directories("${dunedetdataformats_DIR}/../../../include")
include_directories("${nlohmann_json_DIR}/../../../include")
include_directories("${HighFive_DIR}/../../../include")

cet_build_plugin(HDColdboxDataInterface   art::tool LIBRARIES
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
                        dunecore::HDF5Utils
                        HDF5::HDF5
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        cetlib::cetlib
                        ROOT::Core ROOT::Hist ROOT::Tree
             )


cet_build_plugin(HDColdboxDataInterfaceWIB3   art::tool LIBRARIES
                        canvas::canvas
                        cetlib::cetlib
                        cetlib_except::cetlib_except
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        duneprototypes_Protodune_hd_ChannelMap_PD2HDChannelMapService_service
			art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        dunecore::HDF5Utils
                        HDF5::HDF5
             )

cet_build_plugin(HDColdboxDAQWriter art::module LIBRARIES
                        lardataobj::RawData
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        cetlib::cetlib
                        ROOT::Core ROOT::Hist ROOT::Tree
			HDF5::HDF5
                        dunecore::HDF5Utils
                        duneprototypes_Protodune_hd_ChannelMap_PD2HDChannelMapService_service
                        BASENAME_ONLY
)


add_subdirectory(fcl)
add_subdirectory(ChannelMap)

install_headers()
install_fhicl()
install_source()
install_scripts()
