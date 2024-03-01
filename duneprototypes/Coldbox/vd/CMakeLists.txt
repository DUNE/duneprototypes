include_directories("${dunedaqdataformats_DIR}/../../../include")
include_directories("${dunedetdataformats_DIR}/../../../include")
include_directories("${nlohmann_json_DIR}/../../../include")
include_directories("${HighFive_DIR}/../../../include")

cet_build_plugin(VDColdboxDataInterface   art::tool LIBRARIES
                                     canvas::canvas
                                     cetlib::cetlib
                                     cetlib_except::cetlib_except
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
                        artdaq_core::artdaq-core_Data
                        artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
                        dunecore::HDF5Utils
                        HDF5::HDF5
                        duneprototypes_Coldbox_vd_ChannelMap_VDColdboxChannelMapService_service
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


simple_plugin(VDColdboxTDERawInput "source"
  duneprototypes_Coldbox_vd_ChannelMap_VDColdboxTDEChannelMapService_service
  dunecore::DuneObj
  art::Framework_Services_Registry
  canvas::canvas
  cetlib::cetlib
  cetlib_except::cetlib_except
  lardataobj::RawData
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art_root_io::tfile_support
  ROOT::Core
  
  art::Persistency_Provenance
  messagefacility::MF_MessageLogger
  ROOT::Core ROOT::Hist ROOT::Tree
)


cet_build_plugin(VDColdboxPDSDecoder art::module LIBRARIES
                 lardataobj::RawData
                 lardataobj::RecoBase
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
                 BASENAME_ONLY
)



add_subdirectory(ChannelMap)
add_subdirectory(fcl)
add_subdirectory(Tools)

install_headers()
install_fhicl()
install_source()
install_scripts()
