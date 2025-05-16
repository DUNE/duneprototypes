#include "DAPHNEChannelMap.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

void dune::DAPHNEChannelMap::ReadMapFromFile(std::string &fullname) {
  std::ifstream inFile(fullname, std::ios::in);
  std::string line;

  while (std::getline(inFile,line))
  {
    std::stringstream linestream(line);

    unsigned int slot, link, daphne_channel, offline_channel;
    linestream 
      >> slot 
      >> link 
      >> daphne_channel 
      >> offline_channel;

    // fill maps.
    if (fIgnoreLinks) link = 0;
    check_offline_channel(offline_channel);
    fMapToOfflineChannel[DaphneChanInfo({slot,link,daphne_channel})] = offline_channel;
    //OfflToChanInfo[chanInfo.offlchan] = chanInfo;

  }
  inFile.close();
}

void dune::DAPHNEChannelMap::ReadMapFromJson(std::string &fullname)
{

    std::ifstream inFile(fullname, std::ios::in);
    nlohmann::json PDmap;
    inFile >> PDmap;
    
    for (const auto& ch : PDmap)
    {
        for (const auto& hw : ch["HardwareChannel"])
        {
            unsigned int slot = hw["Slot"];
            unsigned int link = hw["Link"];
            unsigned int daphne_channel = hw["DaphneChannel"];
            unsigned int offline_channel = hw["OfflineChannel"];
            
            // fill maps.
            if (fIgnoreLinks) link = 0;
            fMapToOfflineChannel[DaphneChanInfo({slot,link,daphne_channel})] = offline_channel;
        }
    }
    
    inFile.close();
}





unsigned int dune::DAPHNEChannelMap::GetOfflineChannel(
    unsigned int slot, unsigned int link, unsigned int daphne_channel) {

  if (fIgnoreLinks) link = 0;
  if (fMapToOfflineChannel.find({slot,link,daphne_channel}) == fMapToOfflineChannel.end()) {
    std::string err = "DAPHNEChannelMap -- Could not find ";
    err += "slot " + std::to_string(slot);
    err += "link " + std::to_string(link);
    err += "daphne_channel " + std::to_string(daphne_channel);
    throw std::range_error(err);
  }
  return fMapToOfflineChannel[DaphneChanInfo({slot,link,daphne_channel})];
}

