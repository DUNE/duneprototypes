///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       PDVDChannelMapSP
// Module type: algorithm
// File:        PDVDChannelMapSP.h
// Author:      Tom Junk, Jan 2025
//
// Implementation of hardware-offline channel mapping reading from a file.
// art-independent class
// ProtoDUNE-2 Vertical Drift CRP strip electronics identifier to offline channel map
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PDVDChannelMapSP_H
#define PDVDChannelMapSP_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace dune {
  class PDVDChannelMapSP;
}

class dune::PDVDChannelMapSP {

public:
  typedef struct VDChanInfo {
    unsigned int offlchan;     // in gdml and channel sorting convention
    unsigned int crate;        // crate number
    std::string  CRPName;      // Descriptive CRP name
    unsigned int wib;          // 1, 2, 3, 4 or 5  (slot number +1?)
    unsigned int link;         // link identifier: 0 or 1
    unsigned int femb_on_link; // which of two FEMBs in the WIB frame this FEMB is:  0 or 1
    unsigned int cebchan;      // cold electronics channel on FEMB:  0 to 127
    unsigned int plane;        // 0: U,  1: V,  2: X
    unsigned int
      chan_in_plane; // which channel this is in the plane in the FEMB:  0:39 for U and V, 0:47 for X
    unsigned int femb;     // which FEMB on a CRP
    unsigned int asic;     // ASIC:   1 to 8
    unsigned int asicchan; // ASIC channel:  0 to 15
    unsigned int
      wibframechan; // channel index in WIB frame (used with get_adc in detdataformats/WIB2Frame.hh).  0:255
    bool valid; // true if valid, false if not
  } VDChanInfo_t;

  PDVDChannelMapSP(); // constructor

  // initialize:  read map from file

  void ReadMapFromFile(std::string& fullname);

  // TPC channel map accessors

  // Map instrumentation numbers (crate:slot:link:FEMB:plane) to offline channel number.  FEMB is 0 or 1 and indexes the FEMB in the WIB frame.
  // plane = 0 for U, 1 for V and 2 for X

  VDChanInfo_t GetChanInfoFromWIBElements(unsigned int crate,
                                          unsigned int slot,
                                          unsigned int link,
                                          unsigned int wibframechan) const;

  VDChanInfo_t GetChanInfoFromOfflChan(unsigned int offlchan) const;

  unsigned int GetNChannels() { return fNChans; };

private:
  const unsigned int fNChans = 3072 * 4; // top + bottom

  // map of crate, slot, link, femb_on_link, plane, chan to channel info struct

  std::unordered_map<
    unsigned int,                                                          // crate
    std::unordered_map<unsigned int,                                       // wib
                       std::unordered_map<unsigned int,                    // link
                                          std::unordered_map<unsigned int, // wibframechan
                                                             VDChanInfo_t>>>>
    DetToChanInfo;

  // map of chan info indexed by offline channel number

  std::unordered_map<unsigned int, VDChanInfo_t> OfflToChanInfo;

  //-----------------------------------------------

  void check_offline_channel(unsigned int offlineChannel) const
  {
    if (offlineChannel >= fNChans) {
      throw std::range_error("PDVDChannelMapSP offline Channel out of range");
    }
  };
};

#endif
