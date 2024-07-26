#include <iostream>
#include <utility>
#include <set>

#include "art/Framework/Core/EDAnalyzer.h" 
#include "art/Framework/Core/EDFilter.h" 
#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Principal/Event.h" 
#include "art_root_io/TFileService.h"

#include "lardataobj/RawData/RawDigit.h"
#include "dunecore/DuneObj/RDStatus.h"
#include "duneprototypes/Protodune/hd/ChannelMap/PD2HDChannelMapService.h"

#include <bitset>



namespace pdhd {

using RawDigitVector = std::vector<raw::RawDigit>;
using RDStatusVector = std::vector<raw::RDStatus>;

class PDHDFEMBFilter : public art::EDFilter {
 public:
  explicit PDHDFEMBFilter(fhicl::ParameterSet const & pset);
  virtual ~PDHDFEMBFilter() {};
  virtual bool filter(art::Event& e);
  void beginJob();

 private:
  unsigned int fLogLevel;
  std::string fRawDigitLabel;
  bool fRequireSameSize; //Whether we want to filter out events
                         //with inconsistently-sized TPC readouts

  bool fRequireAllChannels; //Make sure all channels are available
                            //Determined from the num of channels
                            //in the channel map

  bool fCheckStatWords; //Check the StatWords from RDStatuses
  std::vector<size_t> fAllowedStatBits; //Which will we allow?
  bool fSkipDigitCheck; //Only look at RDStatus objects

  std::bitset<32> fStatusMask;
};

PDHDFEMBFilter::PDHDFEMBFilter::PDHDFEMBFilter(fhicl::ParameterSet const & pset):
  EDFilter(pset), fLogLevel(pset.get<unsigned int>("LogLevel")),
  fRawDigitLabel(pset.get<std::string>("RawDigitLabel")),
  fRequireSameSize(pset.get<bool>("RequireSameSize")),
  fRequireAllChannels(pset.get<bool>("RequireAllChannels")),
  fCheckStatWords(pset.get<bool>("CheckStatWords")),
  fAllowedStatBits(pset.get<std::vector<size_t>>("AllowedStatBits")),
  fSkipDigitCheck(pset.get<bool>("SkipDigitCheck")) {}


bool PDHDFEMBFilter::filter(art::Event & evt) {

  if (!evt.isRealData()) {
    //Filter is designed for Data only. Don't want to filter on MC
    return true;
  }

  //const std::string myname = "PDHDFEMBFilter::filter: ";
  art::ServiceHandle<dune::PD2HDChannelMapService> channelMap;   
 
  //We'll drop raw digits after a while, so allow for that
  if (!fSkipDigitCheck) {
    auto digits = evt.getValidHandle<RawDigitVector>(fRawDigitLabel);

    if (fRequireAllChannels && (digits->size() != channelMap->GetNChannels())) {
      if (fLogLevel > 0)
        std::cout << "Have " << digits->size() << " digits but expected " <<
                     channelMap->GetNChannels() << std::endl;
      return false;
    }

    if (fRequireSameSize) {
      bool first = true;
      size_t size = 0;
      for (const auto & d : (*digits)) {
        //Get the size from first digit vector
        if (first) {
          size = d.Samples();
          first = false;
          continue;
        }

        //If any subsequent vectors are of different size return false;
        if (d.Samples() != size) {
          if (fLogLevel > 0)
            std::cout << "Found at least 2 different-sized Raw Digit vectors " <<
                         size << " " << d.Samples() << std::endl;
          return false;
        }
      }
    }
  }

  //Check the statuses
  auto statuses = evt.getValidHandle<RDStatusVector>(fRawDigitLabel);

  //Check that we have all channels for these
  if (fRequireAllChannels && (statuses->size() != channelMap->GetNChannels())) {
    if (fLogLevel > 0)
      std::cout << "Have " << statuses->size() << " statuses but expected " <<
                   channelMap->GetNChannels() << std::endl;
    return false;
  }

  //Check that none of the stat word digits are unallowed
  if (fCheckStatWords) {
    if (fLogLevel > 0)
      std::cout << "Checking stat words" << std::endl;
    for (const auto & status : (*statuses)) {
      if (fLogLevel > 0)
        std::cout << "Checking against stat word: " << status.GetStatWord() <<
                     std::endl;

      bool bad = (std::bitset<32>(status.GetStatWord()) & fStatusMask).any();
      if (bad) {
        if (fLogLevel > 0)
          std::cout << "Failed status word check" << std::endl;
        return false;
      }
    }
  }
  
  return true;
}

void PDHDFEMBFilter::beginJob() {
  //Make a mask from the allowed words and invert it
  for (const auto & i : fAllowedStatBits) {
    if (fLogLevel > 0) std::cout << "AllowedStatBit: " << i << std::endl;
    fStatusMask[i] = 1;
  }

  fStatusMask.flip();
  if (fLogLevel > 0)
    std::cout << "Status Mask: " << fStatusMask.to_string() << std::endl;
}

DEFINE_ART_MODULE(PDHDFEMBFilter)

}
