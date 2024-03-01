
cet_build_plugin(PDSPTPCDataInterface   art::tool LIBRARIES
				     canvas::canvas
				     cetlib::cetlib
				     cetlib_except::cetlib_except
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
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


cet_build_plugin(TimingRawDecoder art::module LIBRARIES
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
			artdaq_core::artdaq-core_Utilities
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        BASENAME_ONLY
)

cet_build_plugin(SSPRawDecoder art::module LIBRARIES
                        lardataobj::RawData
                        lardataobj::RecoBase
                        lardataalg::DetectorInfo
                        larcore::headers
                        lardata::headers
                        dunepdlegacy::Overlays
                        dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        BASENAME_ONLY
)

#[[
cet_build_plugin(PDWaveform art::module LIBRARIES
                         lardataobj::RawData
                         lardataobj::RecoBase
                         dunepdlegacy::Overlays
                         dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
 			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
                         art::Framework_Core
                         art::Framework_Principal
                         art::Framework_Services_Registry
                         art_root_io::tfile_support
                         ROOT::Core
                         art_root_io::TFileService_service
                         messagefacility::MF_MessageLogger
			 dunecore::Geometry
                         ROOT::Core ROOT::Hist ROOT::Tree
			 ROOT_SPECTRUM
                         BASENAME_ONLY
)
]]


cet_build_plugin(RceRawDecoder art::module LIBRARIES
                        lardataobj::RawData
                        dunepdlegacy::Overlays
			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
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
                        BASENAME_ONLY
)

cet_build_plugin(FelixRawDecoder art::module LIBRARIES
                        lardataobj::RawData
                        dunepdlegacy::Overlays
			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
			dunecore::Geometry
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

cet_build_plugin(FelixIntegrityTest art::module LIBRARIES
                        lardataobj::RawData
                        dunepdlegacy::Overlays
			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
			dunecore::Geometry
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

cet_build_plugin(PDSPTPCRawDecoder art::module LIBRARIES
                        lardataobj::RawData
                        dunepdlegacy::Overlays
                        dunecore::DuneObj
			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
                        dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
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

cet_build_plugin(CRTRawDecoder art::module LIBRARIES
                        dunepdlegacy::Overlays
                        dunecore::Geometry
			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
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
                        BASENAME_ONLY
)

cet_build_plugin(PDSPCTBRawDecoder art::module LIBRARIES
                        dunepdlegacy::Overlays
			artdaq_core::artdaq-core_Data
			artdaq_core::artdaq-core_Utilities
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        messagefacility::MF_MessageLogger
                        ROOT::Core ROOT::Hist ROOT::Tree
                        dunepdlegacy::rce_dataaccess
                        BASENAME_ONLY
)

art_make_library( LIBRARY_NAME dunetpc_protodune_RawDecodingUtils
                  SOURCE
                  RawDecodingUtils.cc
                  LIBRARIES
                  lardataobj::RawData
                  dunepdlegacy::Overlays
                  artdaq_core::artdaq-core_Data
		  artdaq_core::artdaq-core_Utilities
                  art::Framework_Core
                  art::Framework_Principal
                  art::Framework_Services_Registry
                  art_root_io::tfile_support
                  ROOT::Core
                  art_root_io::TFileService_service
 #                 art::Persistency_Common canvas_Persistency_Common
 #                 art::Persistency_Provenance canvas_Persistency_Provenance
 #                 art::Utilities canvas_Utilities
                  messagefacility::MF_MessageLogger
                  ROOT::Core ROOT::Hist ROOT::Tree
                  dunepdlegacy::rce_dataaccess
)


install_headers()
install_fhicl()
install_source()
install_scripts()
