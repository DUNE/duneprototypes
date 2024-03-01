# duneprototypes/Coldbox/hd/fcl/test/CMakeLists.txt

# David Adams
# June 2023

# Build test for each tool.

include(CetTest)

cet_test(test_fcl_pdhdChannelFemb SOURCE test_fcl_pdhdChannelFemb.cxx
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
