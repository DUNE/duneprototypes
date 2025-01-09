///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       PDVDChannelMapService
// Module type: service
// File:        PDVDChannelMapService.h
// Author:      Tom Junk, January 2024
//
// Implementation of hardware-offline channel mapping reading from a file.
// ProtoDUNE-2 Vertical Drift CRP strip electronics to offline channel map
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PDVDChannelMapService_H
#define PDVDChannelMapService_H

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

#include "PDVDChannelMapSP.h"

namespace dune {
  class PDVDChannelMapService;
}

class dune::PDVDChannelMapService {

public:
  PDVDChannelMapService(fhicl::ParameterSet const& pset);
  PDVDChannelMapService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  dune::PDVDChannelMapSP::VDChanInfo_t GetChanInfoFromWIBElements(unsigned int crate,
                                                                  unsigned int slot,
                                                                  unsigned int link,
                                                                  unsigned int wibframechan) const;

  dune::PDVDChannelMapSP::VDChanInfo_t GetChanInfoFromOfflChan(unsigned int offlchan) const;
  unsigned int GetNChannels() { return fVDChanMap.GetNChannels(); };

private:
  dune::PDVDChannelMapSP fVDChanMap;
};

DECLARE_ART_SERVICE(dune::PDVDChannelMapService, LEGACY)

#endif
