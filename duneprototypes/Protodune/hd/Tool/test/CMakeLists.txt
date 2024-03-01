# dunetpc/dune/Protodune/singlephase/Tool/test/CMakeLists.txt

# David Adams
# June 2021

# Build test for each tool.

cet_test(test_PdhdChannelRanges SOURCE test_PdhdChannelRanges.cxx
  LIBRARIES
    dunecore::ArtSupport
    art::Utilities
    canvas::canvas
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
    ROOT::Core ROOT::Hist ROOT::Tree
    SQLite::SQLite3
    Boost::filesystem
)

cet_test(test_PdhdChannelGroups SOURCE test_PdhdChannelGroups.cxx
  LIBRARIES
    dunecore::ArtSupport
    art::Utilities
    canvas::canvas
    fhiclcpp::fhiclcpp
    cetlib::cetlib 
    cetlib_except::cetlib_except
    ROOT::Core ROOT::Hist ROOT::Tree
    SQLite::SQLite3
    Boost::filesystem
)
include(CetTest)
