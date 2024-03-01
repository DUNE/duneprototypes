include_directories("${dunedaqdataformats_DIR}/../../../include")
include_directories("${dunedetdataformats_DIR}/../../../include")
include_directories("${nlohmann_json_DIR}/../../../include")
include_directories("${HighFive_DIR}/../../../include")

#include_directories( "$ENV{DUNEPDSPRCE_INC}" ) 
#cet_find_library( RCEDAMLIB NAMES protodune-dam PATHS ENV DUNEPDSPRCE_LIB NO_DEFAULT_PATH )

cet_build_plugin(IcebergTPCRawDecoder art::module LIBRARIES
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_IcebergChannelMapService_service
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        dunepdlegacy::rce_dataaccess
                        z
                        BASENAME_ONLY
)


cet_build_plugin(IcebergFELIXBufferDecoderMarch2021 art::module LIBRARIES
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_IcebergChannelMapService_service
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        dunepdlegacy::rce_dataaccess
                        z
                        cetlib::cetlib
                        BASENAME_ONLY
)

cet_build_plugin(IcebergDataInterface   art::tool LIBRARIES
                                     canvas::canvas
                                     cetlib::cetlib
                                     cetlib_except::cetlib_except
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_IcebergChannelMapService_service
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        dunepdlegacy::rce_dataaccess
                        z
             )


cet_build_plugin(IcebergDataInterfaceFELIXBufferMarch2021   art::tool LIBRARIES
                                     canvas::canvas
                                     cetlib::cetlib
                                     cetlib_except::cetlib_except
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_IcebergChannelMapService_service
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        z
             )


cet_build_plugin(IcebergHDF5DataInterface   art::tool LIBRARIES
                        canvas::canvas
                        cetlib::cetlib
                        cetlib_except::cetlib_except
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_IcebergChannelMapService_service
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


install_headers()
install_fhicl()
install_source()
install_scripts()
