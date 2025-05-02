#ifndef DAPHNEChannelMap_H
#define DAPHNEChannelMap_H

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

namespace dune {
  class DAPHNEChannelMap;
}

class dune::DAPHNEChannelMap {

public:
  struct DaphneChanInfo {
    unsigned int slot;        // in gdml and channel sorting convention
    unsigned int link;           // crate number
    unsigned int frame_chan;            // slot number
  };
  struct KeyHasher
  {
    size_t operator()(const DaphneChanInfo& a) const
    {
        return std::hash<int>{}(a.slot*10000+100*a.link+a.frame_chan);
    }
  };
  struct KeyEquals
  {
    bool operator()(const DaphneChanInfo& a, const DaphneChanInfo& b) const
    {
        return (a.slot == b.slot ) && (a.link == b.link) && (a.frame_chan == b.frame_chan);
    } 
  };
  /*struct DAPHNEChanInfo {
    unsigned int offlchan;        // in gdml and channel sorting convention
    unsigned int crate;           // crate number
    unsigned int slot;            // slot number
    std::string APAName;          // Descriptive APA name
    unsigned int link;            // link identifier: 0, 1, 2, 3 
    unsigned int framechan;       // channel index in DAPHNE frame.
  }*/

  DAPHNEChannelMap() {};  // constructor
  DAPHNEChannelMap(bool ignore_links=false) : fIgnoreLinks(ignore_links) {};  // constructor
  void ReadMapFromFile(std::string &fullname);
  void ReadMapFromJson(std::string &fullname);
  unsigned int GetOfflineChannel(unsigned int slot, unsigned int link,
                                 unsigned int frame_chan);

private:
  //Number of channels:
  //    4 Channels/Module x 10 Modules/APA x 4 APAs/PDHD
  const unsigned int fNChans = 4*10*4;

  std::unordered_map<DaphneChanInfo,   // Slot/Endpoint, Link, Frame
    unsigned int, KeyHasher, KeyEquals> fMapToOfflineChannel;

  void check_offline_channel(unsigned int offlineChannel) const {
    if (offlineChannel >= fNChans) {
      throw std::range_error(
        "DAPHNEChannelMap offline Channel out of range");
    }
  };
  bool fIgnoreLinks;

};
#endif
