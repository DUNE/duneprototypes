#ifndef DAPHNEUtils_h
#define DAPHNEUtils_h


#include "detdataformats/daphne/DAPHNEFrame.hpp"
#include "detdataformats/daphne/DAPHNEStreamFrame.hpp"
#include "detdataformats/daphne/DAPHNEFrame2.hpp"
#include "detdataformats/daphne/DAPHNEStreamFrame2.hpp"

#include "duneprototypes/Protodune/hd/ChannelMap/DAPHNEChannelMapService.h"
#include "lardataobj/RawData/OpDetWaveform.h"

#include "TTree.h"

namespace daphne {

  using WaveformVector = std::vector<raw::OpDetWaveform>;
namespace utils {

  class DAPHNETree {
   public:
    int fRun, fEvent, fTriggerNumber, fNFrames, fSlot, fCrate, fDaphneChannel,
        fOfflineChannel, fTriggerSampleValue, fThreshold, fBaseline;
    long fTimestamp, fWindowEnd, fWindowBegin, fFrameTimestamp;
    short fADCValue[1024];


    DAPHNETree(TTree * tree);
    DAPHNETree();
    void Fill();
    void SetBranches();
   private:
    TTree * fTree = nullptr;
  };

  raw::OpDetWaveform & MakeWaveform(
    unsigned int offline_chan,
    size_t n_adcs,
    raw::TimeStamp_t timestamp,
    std::unordered_map<unsigned int, WaveformVector> & wf_map,
    bool is_stream = false);
    bool CheckSubdet(size_t geo_id, std::string subdet_label);
    bool CheckSubdet(size_t geo_id, std::vector<std::string> &subdet_label);
}
}

#endif
