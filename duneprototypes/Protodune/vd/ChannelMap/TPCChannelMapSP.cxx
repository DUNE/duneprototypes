///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       TPCChannelMapSP
// Module type: standalone algorithm
// File:        TPCChannelMapSP.cxx
// Author:      Tom Junk, Jan 2025
//
// Implementation of hardware-offline channel mapping reading from a file.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TPCChannelMapSP.h"

#include <fstream>
#include <iostream>
#include <sstream>

dune::TPCChannelMapSP::TPCChannelMapSP() {
  fSubstituteCrate = 1;
  fNChans = 0;
}

void dune::TPCChannelMapSP::ReadMapFromFile(std::string& fullname)
{
  fNChans = 0;
  
  std::ifstream inFile(fullname, std::ios::in);
  std::string line;

  while (std::getline(inFile, line)) {
    std::stringstream linestream(line);

    TPCChanInfo_t chanInfo;
    linestream >>
      chanInfo.offlchan >>
      chanInfo.detid >>
      chanInfo.crate >>
      chanInfo.slot >>
      chanInfo.stream >>
      chanInfo.streamchan >>
      chanInfo.plane >>
      chanInfo.chan_in_plane >>
      chanInfo.femb >>
      chanInfo.asic >> 
      chanInfo.asicchan;

    chanInfo.valid = true;
    ++fNChans;

    // fill maps.

    check_offline_channel(chanInfo.offlchan);

    DetToChanInfo[chanInfo.detid][chanInfo.crate][chanInfo.slot][chanInfo.stream][chanInfo.streamchan] = chanInfo;
    OfflToChanInfo[chanInfo.offlchan] = chanInfo;
  }
  inFile.close();
}

dune::TPCChannelMapSP::TPCChanInfo_t dune::TPCChannelMapSP::GetChanInfoFromElectronicsIDs(
                                                                                          unsigned int detid,
                                                                                          unsigned int crate,
                                                                                          unsigned int slot,
                                                                                          unsigned int stream,
                                                                                          unsigned int streamchan) const
{

  TPCChanInfo_t badInfo = {};
  badInfo.valid = false;

  auto fm1 = DetToChanInfo.find(detid);
  if (fm1 == DetToChanInfo.end()) return badInfo;
  auto& m1 = fm1->second;
  
  auto fm2 = m1.find(crate);
  if (fm2 == m1.end()) {
    fm1 = DetToChanInfo.find(fSubstituteCrate);
    if (fm2 == m1.end()) return badInfo;
  }
  auto& m2 = fm2->second;

  auto fm3 = m2.find(slot);
  if (fm3 == m2.end()) return badInfo;
  auto& m3 = fm3->second;

  auto fm4 = m3.find(stream);
  if (fm4 == m3.end()) return badInfo;
  auto& m4 = fm4->second;

  auto fm5 = m4.find(streamchan);
  if (fm5 == m4.end()) return badInfo;
  return fm5->second;
}

dune::TPCChannelMapSP::TPCChanInfo_t dune::TPCChannelMapSP::GetChanInfoFromOfflChan(
                                                                                    unsigned int offlineChannel) const
{
  auto ci = OfflToChanInfo.find(offlineChannel);
  if (ci == OfflToChanInfo.end()) {
    TPCChanInfo_t badInfo = {};
    badInfo.valid = false;
    return badInfo;
  }
  return ci->second;
}
