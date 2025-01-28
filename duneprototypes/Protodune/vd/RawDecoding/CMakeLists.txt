include_directories("${dunedaqdataformats_DIR}/../../../include")
include_directories("${dunedetdataformats_DIR}/../../../include")
include_directories("${nlohmann_json_DIR}/../../../include")
include_directories("${HighFive_DIR}/../../../include")


cet_build_plugin(PDVDTPCReader art::module LIBRARIES
                        lardataobj::RawData
			dunecore::HDF5Utils_HDF5RawFile2Service_service
			dunecore::dunedaqhdf5utils2
                        HDF5::HDF5
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
			art::Utilities
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        BASENAME_ONLY
)


cet_build_plugin(PDVDDataInterfaceWIBEth   art::tool LIBRARIES
                        canvas::canvas
                        cetlib::cetlib
                        cetlib_except::cetlib_except
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        duneprototypes_Protodune_vd_ChannelMap_PDVDChannelMapService_service
			art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
			dunecore::HDF5Utils_HDF5RawFile3Service_service
			dunecore::dunedaqhdf5utils3
                        HDF5::HDF5
             )
	   
cet_build_plugin(PDVDTriggerReader3 art::module LIBRARIES
                        lardataobj::RawData
                        dunecore::HDF5Utils_HDF5RawFile3Service_service
                        dunecore::dunedaqhdf5utils2
                        HDF5::HDF5
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
			art::Utilities
                        messagefacility::MF_MessageLogger
                        dunecore::dunedaqhdf5utils3
                        ROOT::Core ROOT::Hist ROOT::Tree
                        BASENAME_ONLY
                )


install_headers()
install_fhicl()
install_source()
install_scripts()
