# duneprototypes/Coldbox/hd/fcl/test/CMakeLists.txt

# David Adams
# June 2023

# Build test for each tool.

include(CetTest)

cet_test(test_fcl_pdhdChannelFemb SOURCES test_fcl_pdhdChannelFemb.cxx
  LIBRARIES
    dunecore_ArtSupport
    art::Utilities
    canvas::canvas
    fhiclcpp
    cetlib::cetlib cetlib_except
    cetlib_except::cetlib_except
    ROOT_BASIC_LIB_LIST
    SQLITE3
    Boost::filesystem
)
