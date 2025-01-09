///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       PDVDChannelMapService
// Module type: service
// File:        PDVDChannelMapService_service.cc
// Author:      Tom Junk, January 2025
//
// Implementation of hardware-offline channel mapping reading from a file.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PDVDChannelMapService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

dune::PDVDChannelMapService::PDVDChannelMapService(fhicl::ParameterSet const& pset)
{

  std::string wireReadoutFile = pset.get<std::string>("FileName");

  std::string fullname;
  cet::search_path sp("FW_SEARCH_PATH");
  sp.find_file(wireReadoutFile, fullname);

  if (fullname.empty()) {
    std::cout << "Input file " << wireReadoutFile << " not found" << std::endl;
    throw cet::exception("File not found");
  }
  else
    std::cout << "PDVD Channel Map: Building TPC wiremap from file " << wireReadoutFile
              << std::endl;

  fVDChanMap.ReadMapFromFile(fullname);
}

dune::PDVDChannelMapService::PDVDChannelMapService(fhicl::ParameterSet const& pset,
                                                   art::ActivityRegistry&)
  : PDVDChannelMapService(pset)
{}

dune::PDVDChannelMapSP::VDChanInfo_t dune::PDVDChannelMapService::GetChanInfoFromWIBElements(
  unsigned int crate,
  unsigned int slot,
  unsigned int link,
  unsigned int wibframechan) const
{

  return fVDChanMap.GetChanInfoFromWIBElements(crate, slot, link, wibframechan);
}

dune::PDVDChannelMapSP::VDChanInfo_t dune::PDVDChannelMapService::GetChanInfoFromOfflChan(
  unsigned int offlineChannel) const
{

  return fVDChanMap.GetChanInfoFromOfflChan(offlineChannel);
}

DEFINE_ART_SERVICE(dune::PDVDChannelMapService)
