# duneprototypes/Protodune/hd/Tool/CMakeLists.txt

# David Adams
# June 2023

# Add plugin for each tool.

cet_build_plugin(PdhdChannelRanges   art::tool LIBRARIES
                 dunecore::ArtSupport
                 art::Utilities canvas::canvas
                 cetlib::cetlib cetlib_except::cetlib_except
                 art::Framework_Services_Registry
                 ROOT::Core ROOT::Hist ROOT::Tree
                )

cet_build_plugin(PdhdChannelGroups   art::tool LIBRARIES
                 dunecore::ArtSupport
                 art::Utilities canvas::canvas
                 cetlib::cetlib cetlib_except::cetlib_except
                 art::Framework_Services_Registry
                 ROOT::Core ROOT::Hist ROOT::Tree
                )

add_subdirectory(test)
