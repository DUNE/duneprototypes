///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       PDVDChannelMapSP
// Module type: standalone algorithm
// File:        PDVDChannelMapSP.cxx
// Author:      Tom Junk, Jan 2025
//
// Implementation of hardware-offline channel mapping reading from a file.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PDVDChannelMapSP.h"

#include <fstream>
#include <iostream>
#include <sstream>

// so far, nothing needs to be done in the constructor

dune::PDVDChannelMapSP::PDVDChannelMapSP() {}

void dune::PDVDChannelMapSP::ReadMapFromFile(std::string& fullname)
{
  std::ifstream inFile(fullname, std::ios::in);
  std::string line;

  while (std::getline(inFile, line)) {
    std::stringstream linestream(line);

    VDChanInfo_t chanInfo;
    linestream >> chanInfo.offlchan >> chanInfo.crate >> chanInfo.CRPName >> chanInfo.wib >>
      chanInfo.link >> chanInfo.femb_on_link >> chanInfo.cebchan >> chanInfo.plane >>
      chanInfo.chan_in_plane >> chanInfo.femb >> chanInfo.asic >> chanInfo.asicchan >>
      chanInfo.wibframechan;

    chanInfo.valid = true;

    // fill maps.

    check_offline_channel(chanInfo.offlchan);

    DetToChanInfo[chanInfo.crate][chanInfo.wib][chanInfo.link][chanInfo.wibframechan] = chanInfo;
    OfflToChanInfo[chanInfo.offlchan] = chanInfo;
  }
  inFile.close();
}

dune::PDVDChannelMapSP::VDChanInfo_t dune::PDVDChannelMapSP::GetChanInfoFromWIBElements(
  unsigned int crate,
  unsigned int slot,
  unsigned int link,
  unsigned int wibframechan) const
{

  unsigned int wib = slot + 1;

  VDChanInfo_t badInfo = {};
  badInfo.valid = false;

  // a hack -- ununderstood crates are mapped to crate 2
  // for use in the Coldbox
  // crate 2 has the lowest-numbered offline channels
  // data with two ununderstood crates, or an ununderstood crate and crate 2,
  // will have duplicate channels.

  auto fm1 = DetToChanInfo.find(crate);
  if (fm1 == DetToChanInfo.end()) {
    unsigned int substituteCrate = 2;
    fm1 = DetToChanInfo.find(substituteCrate);
    if (fm1 == DetToChanInfo.end()) return badInfo;
  }
  auto& m1 = fm1->second;

  auto fm2 = m1.find(wib);
  if (fm2 == m1.end()) return badInfo;
  auto& m2 = fm2->second;

  auto fm3 = m2.find(link);
  if (fm3 == m2.end()) return badInfo;
  auto& m3 = fm3->second;

  auto fm4 = m3.find(wibframechan);
  if (fm4 == m3.end()) return badInfo;
  return fm4->second;
}

dune::PDVDChannelMapSP::VDChanInfo_t dune::PDVDChannelMapSP::GetChanInfoFromOfflChan(
  unsigned int offlineChannel) const
{
  auto ci = OfflToChanInfo.find(offlineChannel);
  if (ci == OfflToChanInfo.end()) {
    VDChanInfo_t badInfo = {};
    badInfo.valid = false;
    return badInfo;
  }
  return ci->second;
}
