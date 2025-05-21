#include "DAPHNEUtils.h"
#include "detdataformats/DetID.hpp"

namespace daphne::utils {

DAPHNETree::DAPHNETree() : fTree(nullptr) {}
DAPHNETree::DAPHNETree(TTree * tree) : fTree(tree) {
  SetBranches();
};

void DAPHNETree::Fill() {
  if (fTree != nullptr) 
    fTree->Fill();
}

void DAPHNETree::SetBranches() {
  if (fTree == nullptr) return;

  fTree->Branch("Run", &fRun, "Run/I");
  fTree->Branch("Event", &fEvent, "Event/I");
  fTree->Branch("TriggerNumber", &fTriggerNumber, "TriggerNumber/I");
  fTree->Branch("TimeStamp", &fTimestamp, "TimeStamp/l");
  fTree->Branch("Window_begin", &fWindowBegin, "Window_begin/l");
  fTree->Branch("Window_end", &fWindowEnd, "Window_end/l");

  fTree->Branch("Slot", &fSlot, "Slot/I");
  fTree->Branch("Crate", &fCrate, "Crate/I");
  fTree->Branch("DaphneChannel",& fDaphneChannel, "DaphneChannel/I");
  fTree->Branch("OfflineChannel", &fOfflineChannel, "OfflineChannel/I");
  fTree->Branch("FrameTimestamp",& fFrameTimestamp, "FrameTimestamp/l");
  fTree->Branch("adc_channel", fADCValue, "adc_value[1024]/S");

  fTree->Branch("TriggerSampleValue", &fTriggerSampleValue,
                "TriggerSampleValue/I"); //only for self-trigger
  fTree->Branch("Threshold", &fThreshold,
                "Threshold/I"); //only for self-trigger
  fTree->Branch("Baseline", &fBaseline,
                "Baseline/I"); //only for self-trigger

}

raw::OpDetWaveform & MakeWaveform(
  unsigned int offline_chan,
  size_t n_adcs,
  raw::TimeStamp_t timestamp,
  std::unordered_map<unsigned int, std::vector<raw::OpDetWaveform>> & wf_map,
  bool is_stream) {

  //If needed, make a new element in the map
  if (wf_map.find(offline_chan) == wf_map.end()) {
    wf_map.emplace(offline_chan, std::vector<raw::OpDetWaveform>());
  }

  //If is_stream, we just want to change the waveform
  //If not, or if this is the first time hitting this channel,
  //add a new waveform to the vector
  if (wf_map.at(offline_chan).size() == 0 || !is_stream) {
    wf_map.at(offline_chan).emplace_back(
        raw::OpDetWaveform(timestamp, offline_chan));
  }


  auto & waveform = wf_map.at(offline_chan).back();
  //Reserve more adcs at once for efficiency
  waveform.reserve(waveform.size() + n_adcs);
  return waveform;

}

bool CheckSubdet(size_t geo_id, std::string subdet_label) {
  dunedaq::detdataformats::DetID::Subdetector det_idenum
      = static_cast<dunedaq::detdataformats::DetID::Subdetector>(
          0xffff & geo_id);

  //Check that it's photon detectors
  auto subdetector_string
      = dunedaq::detdataformats::DetID::subdetector_to_string(det_idenum);
  return (subdetector_string == subdet_label);

}


bool CheckSubdet(size_t geo_id, std::vector<std::string> &subdet_label) {
  dunedaq::detdataformats::DetID::Subdetector det_idenum
      = static_cast<dunedaq::detdataformats::DetID::Subdetector>(
          0xffff & geo_id);

  //Check that it's photon detectors
  auto subdetector_string
      = dunedaq::detdataformats::DetID::subdetector_to_string(det_idenum);
  return !(std::find(subdet_label.begin(), subdet_label.end(), subdetector_string) == subdet_label.end());

}

}
