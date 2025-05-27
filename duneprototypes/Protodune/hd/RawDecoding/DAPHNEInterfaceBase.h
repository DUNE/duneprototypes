#ifndef DAPHNEInterfaceBase_h
#define DAPHNEInterfaceBase_h
#include "art/Utilities/ToolMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "duneprototypes/Protodune/hd/ChannelMap/DAPHNEChannelMapService.h"

namespace raw {
class OpDetWaveform;
}

namespace daphne {

namespace utils {
class DAPHNETree;
}

class DAPHNEInterfaceBase {
 public:
  virtual void Process(
      art::Event &evt,
      std::string inputlabel,
      std::string subdet_label,
      std::unordered_map<unsigned int, std::vector<raw::OpDetWaveform>> & wf_map,
      utils::DAPHNETree * daphne_tree) = 0;
  virtual void Process2(
      art::Event &evt,
      std::string inputlabel,
      std::vector<std::string> subdet_label,
      std::unordered_map<unsigned int, std::vector<raw::OpDetWaveform>> & wf_map,
      utils::DAPHNETree * daphne_tree) = 0;

  virtual ~DAPHNEInterfaceBase() = default;
 protected:
  art::ServiceHandle<dune::DAPHNEChannelMapService> fChannelMap;
};
}

#endif
