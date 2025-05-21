////////////////////////////////////////////////////////////////////////
// Class:       DAPHNEReaderPDVD
// Plugin Type: producer (Unknown Unknown)
// File:        DAPHNEReaderPDVD_module.cc
// Adapted to use DaphneInterface3 (deal with several fSubDetString)
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "duneprototypes/Protodune/hd/ChannelMap/DAPHNEChannelMapService.h"
#include "art/Utilities/make_tool.h" 

#include "duneprototypes/Protodune/hd/RawDecoding/DAPHNEInterfaceBase.h"
#include "duneprototypes/Protodune/hd/RawDecoding/DAPHNEUtils.h"

#include "lardataobj/RawData/OpDetWaveform.h"
#include "TTree.h"
#include "art_root_io/TFileService.h"

#include <memory>
namespace pdhd {

//For brevity 
using WaveformVector = std::vector<raw::OpDetWaveform>;

class DAPHNEReaderPDVD;

  // clang complained -- commenting out
  //const Int_t kMaxFrames = 50;

class DAPHNEReaderPDVD : public art::EDProducer {
public:
  explicit DAPHNEReaderPDVD(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  DAPHNEReaderPDVD(DAPHNEReaderPDVD const&) = delete;
  DAPHNEReaderPDVD(DAPHNEReaderPDVD&&) = delete;
  DAPHNEReaderPDVD& operator=(DAPHNEReaderPDVD const&) = delete;
  DAPHNEReaderPDVD& operator=(DAPHNEReaderPDVD&&) = delete;

  // Required functions.
  void produce(art::Event& e) override;
  void beginJob() override;
  void endJob() override;

private:

  std::unique_ptr<daphne::DAPHNEInterfaceBase> fDAPHNETool;
  daphne::utils::DAPHNETree * fDAPHNETree = nullptr;
  std::string fInputLabel, fOutputLabel, fFileInfoLabel ;
  std::vector<std::string> fSubDetString;
  TTree * fWaveformTree;

  bool fExportWaveformTree;
  //vars per event
  //int _Run;
  // clang complained -- commenting out
  //int _SubRun;
  //int _Event;
  //int _TriggerNumber;
  //long int _TimeStamp;
  //long int _Window_end;
  //long int _Window_begin;
  //int _NFrames;
  //int _Slot;
  //int _Crate;
  //int _DaphneChannel;
  //int _OfflineChannel;
  //long int _FrameTimestamp;
  //short _adc_value[1024];
  //int _TriggerSampleValue;
  //int _Threshold;
  //int _Baseline;
};
}

void pdhd::DAPHNEReaderPDVD::beginJob() {
  art::ServiceHandle<art::TFileService> tfs;

  if (fExportWaveformTree) {
    fWaveformTree = tfs->make<TTree>("WaveformTree","Waveforms Tree");

    fDAPHNETree = new daphne::utils::DAPHNETree(fWaveformTree);
  }
}

pdhd::DAPHNEReaderPDVD::DAPHNEReaderPDVD(fhicl::ParameterSet const& p)
  : EDProducer{p}, 
    fDAPHNETool{
        art::make_tool<daphne::DAPHNEInterfaceBase>(
            p.get<fhicl::ParameterSet>("DAPHNEInterface"))},
    fInputLabel(p.get<std::string>("InputLabel", "daq")),
    fOutputLabel(p.get<std::string>("OutputLabel", "daq")),
    fFileInfoLabel(p.get<std::string>("FileInfoLabel", "daq")),
    fSubDetString(p.get<std::vector<std::string>>("SubDetString",{"VD_Membrane_PDS","VD_Cathode_PDS","VD_PMT_PDS"})),
    fExportWaveformTree(p.get<bool>("ExportWaveformTree",true)) {
  produces<std::vector<raw::OpDetWaveform>> (fOutputLabel);
}

void pdhd::DAPHNEReaderPDVD::produce(art::Event& evt) {

//std::cout << "RUNNIN NEW EVENT ==================" << std::endl;

  WaveformVector opdet_waveforms;
  std::unordered_map<unsigned int, WaveformVector> wf_map;

  //Process the event
  fDAPHNETool->Process2(evt, fFileInfoLabel, fSubDetString, wf_map, fDAPHNETree);

  //Convert map to vector for output
  for (auto & chan_wf_vector : wf_map) {//Loop over channels
    //std::cout << "Inserting " << chan_wf_vector.first << " " << chan_wf_vector.second.size() << std::endl;
    opdet_waveforms.insert(opdet_waveforms.end(),
                           chan_wf_vector.second.begin(),
                           chan_wf_vector.second.end());
    //Remove elements from wf_map to save memory
    chan_wf_vector.second.clear();
  }
  //std::cout << "EventNumber " << _Event << std::endl;

  evt.put(
      std::make_unique<decltype(opdet_waveforms)>(std::move(opdet_waveforms)),
      fOutputLabel
  );

}

void pdhd::DAPHNEReaderPDVD::endJob() {
  if (fDAPHNETree != nullptr) delete fDAPHNETree;
}

DEFINE_ART_MODULE(pdhd::DAPHNEReaderPDVD)
