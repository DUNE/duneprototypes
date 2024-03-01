# dunetpc/dune/Protodune/singlephase/Exe
#
# David Adams
# October 2018
#
# Instructions to build and install fcldump.

cet_make_exec(NAME pdChannelRange
  SOURCE pdChannelRange.cxx
  LIBRARIES
    dunecore::ArtSupport
    art::Framework_Core
    art::Utilities
    art::Persistency_Common
    fhiclcpp::fhiclcpp
    canvas::canvas
    cetlib::cetlib 
    cetlib_except::cetlib_except
)

install_source()
