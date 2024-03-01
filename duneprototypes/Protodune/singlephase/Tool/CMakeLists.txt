# dunetpc/dune/Protodune/singlephase/Tool/CMakeLists.txt

# David Adams
# June 2018

# Add plugin for each tool.

cet_build_plugin(PdspOnlineChannel   art::tool LIBRARIES
                dunepdlegacy::Services_ChannelMap_PdspChannelMapService_service
                dunecore::DuneInterface_Data
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                art::Framework_Services_Registry
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(IcebergOnlineChannel   art::tool LIBRARIES
                dunepdlegacy::Services_ChannelMap_IcebergChannelMapService_service
                dunecore::DuneInterface_Data
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                art::Framework_Services_Registry
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(ProtoduneOnlineChannel   art::tool LIBRARIES
                art::Utilities canvas::canvas
                dunecore::DuneInterface_Data
                cetlib::cetlib cetlib_except::cetlib_except
                art::Framework_Services_Registry
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(ProtoDuneChannelRanges  art::tool LIBRARIES
                dunecore::ArtSupport
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(ApaChannelRanges  art::tool LIBRARIES
                dunecore::ArtSupport
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(HalfApaChannelRanges  art::tool LIBRARIES
                dunecore::ArtSupport
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(ProtoDuneChannelGroups  art::tool LIBRARIES
                dunecore::ArtSupport
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )

cet_build_plugin(ApaChannelGroups  art::tool LIBRARIES
                dunecore::ArtSupport
                art::Utilities canvas::canvas
                cetlib::cetlib cetlib_except::cetlib_except
                ROOT::Core ROOT::Hist ROOT::Tree
             )

add_subdirectory(test)
