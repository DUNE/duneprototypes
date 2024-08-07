////////////////////////////////////////////////////////////////////////
// Class:       PDHDTimingRawDecoder
// Plugin Type: producer (Unknown Unknown)
// File:        PDHDTimingRawDecoder_module.cc
//
// Generated by Jacob Calcutt using cetskelgen
// from  version .
//
// Functionality integrated by Alex Oranday
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
#include "art/Utilities/make_tool.h"
#include "lardataobj/RawData/RDTimeStamp.h"

#include "dunecore/DuneObj/DUNEHDF5FileInfo2.h"
#include "dunecore/HDF5Utils/HDF5RawFile3Service.h"

#include "TTree.h"
#include "art_root_io/TFileService.h"

#include <memory>
namespace pdhd {

class PDHDTimingRawDecoder;

class PDHDTimingRawDecoder : public art::EDProducer {
public:
  explicit PDHDTimingRawDecoder(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  PDHDTimingRawDecoder(PDHDTimingRawDecoder const&) = delete;
  PDHDTimingRawDecoder(PDHDTimingRawDecoder&&) = delete;
  PDHDTimingRawDecoder& operator=(PDHDTimingRawDecoder const&) = delete;
  PDHDTimingRawDecoder& operator=(PDHDTimingRawDecoder&&) = delete;

  // Required functions.
  void produce(art::Event& event) override;

private:
  std::string fInputLabel, fOutputLabel;
};
}

pdhd::PDHDTimingRawDecoder::PDHDTimingRawDecoder(fhicl::ParameterSet const& p)
  : EDProducer{p},
    fInputLabel(p.get<std::string>("InputLabel", "daq")),
    fOutputLabel(p.get<std::string>("OutputLabel")) {
  produces<std::vector<raw::RDTimeStamp>> (fOutputLabel);
}

void pdhd::PDHDTimingRawDecoder::produce(art::Event& event) {
  // Get the HDF5 file.
  art::ServiceHandle<dune::HDF5RawFile3Service> rawFileService;
  auto raw_file = rawFileService->GetPtr();

  //Get the HDF5 file to be opened
  //TODO -- will have to chunk out this functionality for any file
  //format evolution
  //But I'm doing it here for now
  auto infoHandle = event.getHandle<raw::DUNEHDF5FileInfo2>(fInputLabel);
  dunedaq::hdf5libs::HDF5RawDataFile::record_id_t record_id
      = std::make_pair(infoHandle->GetEvent(), infoHandle->GetSequence());


  //Get the trigger record header from this record
  auto trh_ptr = raw_file->get_trh_ptr(record_id);
  //Grab the timestamp from the trigger and store it in a vector
  std::vector<raw::RDTimeStamp> timestamps = {
      raw::RDTimeStamp(trh_ptr->get_trigger_timestamp())
  };

  //Put it in the art event
  event.put(std::make_unique<std::vector<raw::RDTimeStamp>>(
      std::move(timestamps)), fOutputLabel);
}

DEFINE_ART_MODULE(pdhd::PDHDTimingRawDecoder)
