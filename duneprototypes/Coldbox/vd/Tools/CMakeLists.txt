cet_build_plugin(VDColdboxChannelRanges  art::tool LIBRARIES
                dunecore::ArtSupport
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(VDColdboxChannelGroups  art::tool LIBRARIES
                dunecore::ArtSupport
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )


cet_build_plugin(VDColdboxOnlineChannel  art::tool LIBRARIES
                duneprototypes_Coldbox_vd_ChannelMap_VDColdboxChannelMapService_service
                dunecore::ArtSupport
                art::Framework_Services_Registry
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(CrpChannelRanges  art::tool LIBRARIES
                dunecore::ArtSupport
                dunecore::DuneCommon_Utility
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(CrpChannelGroups  art::tool LIBRARIES
                dunecore::ArtSupport
                dunecore::DuneCommon_Utility
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )


install_fhicl()

add_subdirectory(test)
