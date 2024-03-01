# dunetpc/dune/Protodune/singlephase/Tool/test/CMakeLists.txt

# David Adams
# June 2018

# Build test for each tool.

include(CetTest)

cet_test(test_PdspOnlineChannel SOURCE test_PdspOnlineChannel.cxx
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

cet_test(test_IcebergOnlineChannel SOURCE test_IcebergOnlineChannel.cxx
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

cet_test(test_ProtoduneOnlineChannel SOURCE test_ProtoduneOnlineChannel.cxx
  LIBRARIES
    dunecore::ArtSupport
    duneprototypes_Protodune_singlephase_Utility
    art::Utilities
    canvas::canvas
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
    ROOT::Core ROOT::Hist ROOT::Tree
    SQLite::SQLite3
    Boost::filesystem
)

cet_test(test_ProtoDuneChannelRanges SOURCE test_ProtoDuneChannelRanges.cxx
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

cet_test(test_ApaChannelRanges SOURCE test_ApaChannelRanges.cxx
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

cet_test(test_HalfApaChannelRanges SOURCE test_HalfApaChannelRanges.cxx
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

cet_test(test_ProtoDuneChannelGroups SOURCE test_ProtoDuneChannelGroups.cxx
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

cet_test(test_ApaChannelGroups SOURCE test_ApaChannelGroups.cxx
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

