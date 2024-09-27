///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       PD2HDChannelMapService
// Module type: service
// File:        PD2HDChannelMapService.h
// Author:      Tom Junk, May 2022
//
// Implementation of hardware-offline channel mapping reading from a file.  
// ProtoDUNE-2 Horizontal Drift APA wire to offline channel map
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PD2HDChannelMapService_H
#define PD2HDChannelMapService_H

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

#include "PD2HDChannelMapSP.h"

namespace dune {
  class PD2HDChannelMapService;
}

class dune::PD2HDChannelMapService {

public:

  PD2HDChannelMapService(fhicl::ParameterSet const& pset);
  PD2HDChannelMapService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  dune::PD2HDChannelMapSP::HDChanInfo_t GetChanInfoFromWIBElements(
   unsigned int crate,
   unsigned int slot,
   unsigned int link,
   unsigned int wibframechan) const;

  dune::PD2HDChannelMapSP::HDChanInfo_t GetChanInfoFromOfflChan(unsigned int offlchan) const;
  unsigned int GetNChannels() {return fHDChanMap.GetNChannels();};

private:

  dune::PD2HDChannelMapSP fHDChanMap;

};

DECLARE_ART_SERVICE(dune::PD2HDChannelMapService, LEGACY)

#endif
