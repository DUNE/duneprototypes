#include "DAPHNEChannelMapService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

dune::DAPHNEChannelMapService::DAPHNEChannelMapService(fhicl::ParameterSet const& pset)
  : fChannelMap(pset.get<bool>("IgnoreLinks", false)) {

  std::string channelMapFile = pset.get<std::string>("FileName");

  std::string fullname;
  cet::search_path sp("FW_SEARCH_PATH");
  sp.find_file(channelMapFile, fullname);

  if (fullname.empty()) {
    std::cout << "Input file " << channelMapFile << " not found" << std::endl;
    throw cet::exception("File not found");
  }
  else
    std::cout << "DAPHNE Channel Map: Building DAPHNE channel map from file " << channelMapFile << std::endl;

  if(fullname.find(".json") != std::string::npos)
     fChannelMap.ReadMapFromJson(fullname);
  else 
     fChannelMap.ReadMapFromFile(fullname);
}

dune::DAPHNEChannelMapService::DAPHNEChannelMapService(fhicl::ParameterSet const& pset, art::ActivityRegistry&) : DAPHNEChannelMapService(pset) {}

unsigned int dune::DAPHNEChannelMapService::GetOfflineChannel(
    unsigned int slot,
    unsigned int link,
    unsigned int daphne_channel) {

  return fChannelMap.GetOfflineChannel(slot, link, daphne_channel);
}

DEFINE_ART_SERVICE(dune::DAPHNEChannelMapService)
