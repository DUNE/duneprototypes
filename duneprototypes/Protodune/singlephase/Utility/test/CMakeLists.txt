# dunetpc/dune/DataPrep/Utility/test/CMakeLists.txt

# David Adams
# June 2018

# Build test for each utility.

include(CetTest)

cet_test(test_ProtoduneChannelHelper SOURCES test_ProtoduneChannelHelper.cxx
  LIBRARIES
    duneprototypes_Protodune_singlephase_Utility
    canvas::canvas
    fhiclcpp
    cetlib::cetlib cetlib_except
    cetlib_except::cetlib_except
    SQLITE3
    Boost::filesystem
    ROOT_BASIC_LIB_LIST
)

