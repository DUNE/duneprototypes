///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       PD2HDChannelMapService
// Module type: service
// File:        PD2HDChannelMapService_service.cc
// Author:      Tom Junk, May 2022
//
// Implementation of hardware-offline channel mapping reading from a file.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PD2HDChannelMapService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

dune::PD2HDChannelMapService::PD2HDChannelMapService(fhicl::ParameterSet const& pset) {

  std::string wireReadoutFile = pset.get<std::string>("FileName");

  std::string fullname;
  cet::search_path sp("FW_SEARCH_PATH");
  sp.find_file(wireReadoutFile, fullname);

  if (fullname.empty()) {
    std::cout << "Input file " << wireReadoutFile << " not found" << std::endl;
    throw cet::exception("File not found");
  }
  else
    std::cout << "PD2HD Channel Map: Building TPC wiremap from file " << wireReadoutFile << std::endl;

  fHDChanMap.ReadMapFromFile(fullname);
}

dune::PD2HDChannelMapService::PD2HDChannelMapService(fhicl::ParameterSet const& pset, art::ActivityRegistry&) : PD2HDChannelMapService(pset) {
}

dune::PD2HDChannelMapSP::HDChanInfo_t dune::PD2HDChannelMapService::GetChanInfoFromWIBElements(
    unsigned int crate,
    unsigned int slot,
    unsigned int link,
    unsigned int wibframechan ) const {

  return fHDChanMap.GetChanInfoFromWIBElements(crate,slot,link,wibframechan);
}


dune::PD2HDChannelMapSP::HDChanInfo_t dune::PD2HDChannelMapService::GetChanInfoFromOfflChan(unsigned int offlineChannel) const {

  return fHDChanMap.GetChanInfoFromOfflChan(offlineChannel);

}


DEFINE_ART_SERVICE(dune::PD2HDChannelMapService)
