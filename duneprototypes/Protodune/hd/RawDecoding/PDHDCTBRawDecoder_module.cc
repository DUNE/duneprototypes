#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/make_tool.h" 
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "canvas/Persistency/Common/Assns.h"
#include "art/Persistency/Common/PtrMaker.h"

#include "dunecore/DuneObj/DUNEHDF5FileInfo2.h"
#include "dunecore/HDF5Utils/HDF5RawFile3Service.h"
#include "detdataformats/hsi/HSIFrame.hpp"

#include <memory>
#include <iostream>

class PDHDCTBRawDecoder;

class PDHDCTBRawDecoder : public art::EDProducer {
public:
    explicit PDHDCTBRawDecoder(fhicl::ParameterSet const& p);
    // The compiler-generated destructor is fine for non-base
    // classes without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    PDHDCTBRawDecoder(PDHDCTBRawDecoder const&) = delete;
    PDHDCTBRawDecoder(PDHDCTBRawDecoder&&) = delete;
    PDHDCTBRawDecoder& operator=(PDHDCTBRawDecoder const&) = delete;
    PDHDCTBRawDecoder& operator=(PDHDCTBRawDecoder&&) = delete;

    // Required functions.
    void produce(art::Event& e) override;

private:

    std::string fInputLabel;

    bool fProcessLLTs;
    bool fProcessHLTs;
    std::string fOutputInstance_LLT;
    std::string fOutputInstance_HLT;
    int fDebugLevel;
};


PDHDCTBRawDecoder::PDHDCTBRawDecoder(fhicl::ParameterSet const& p)
    : EDProducer{p}
    , fInputLabel(p.get<std::string>("InputLabel","daq"))
    , fProcessLLTs(p.get<bool>("ProcessLLTs",true))
    , fProcessHLTs(p.get<bool>("ProcessHLTs",true))
    , fOutputInstance_LLT(p.get<std::string>("OutputInstance_LLT","daqLLT"))
    , fOutputInstance_HLT(p.get<std::string>("OutputInstance_HLT","daqHLT"))
    , fDebugLevel(p.get<int>("DebugLevel",0))
{
    if(fProcessLLTs) produces<std::vector<dunedaq::detdataformats::HSIFrame>>(fOutputInstance_LLT);
    if(fProcessHLTs) produces<std::vector<dunedaq::detdataformats::HSIFrame>>(fOutputInstance_HLT);

    consumes<raw::DUNEHDF5FileInfo2>(fInputLabel);  // the tool actually does the consuming of this product
}



void PDHDCTBRawDecoder::produce(art::Event& e) {

    std::vector<dunedaq::detdataformats::HSIFrame> hsi_llt_col, hsi_hlt_col;

    auto infoHandle = e.getHandle<raw::DUNEHDF5FileInfo2>(fInputLabel);

    const std::string & file_name = infoHandle->GetFileName();
    uint32_t runno = infoHandle->GetRun();
    size_t   evtno = infoHandle->GetEvent();
    size_t   seqno = infoHandle->GetSequence();

    dunedaq::hdf5libs::HDF5RawDataFile::record_id_t rid = std::make_pair(evtno, seqno);
  
    if (fDebugLevel > 0){
        std::cout << "PDHDDataInterface HDF5 FileName: " << file_name << std::endl;
        std::cout << "PDHDDataInterface Run:Event:Seq: " << runno << ":" << evtno << ":" << seqno << std::endl;
    }

    // Grab the source ids for fragments with HSI frames
    art::ServiceHandle<dune::HDF5RawFile3Service> rawFileService;
    auto rf = rawFileService->GetPtr();
    auto trh_timestamp = rf->get_trh_ptr(rid)->get_trigger_timestamp();

    if (fDebugLevel > 0){
        std::cout << "PDHDDataInterface Trigger Timestamp: " << trh_timestamp << std::endl;
    }


    auto hsi_sourceids = rf->get_source_ids_for_fragment_type(rid,dunedaq::daqdataformats::FragmentType::kHardwareSignal);

    // Loop over SourceIDs, Calculates the number of TriggerPrimitive objects in the individual Fragment Payload
    if (fDebugLevel > 0){
        std::cout << "\t N_HSI_SourceIDs: " << hsi_sourceids.size() << std::endl;
    }
 
    for (auto const& source_id : hsi_sourceids){
        // Perform a check to make sure we are only grabbing information from the HwSignalsInterface
        if (source_id.subsystem != dunedaq::daqdataformats::SourceID::Subsystem::kHwSignalsInterface) continue;

        //get the fragment and number of contained HSI frames
        auto frag_ptr = rf->get_frag_ptr(rid, source_id);
        size_t n_frames = frag_ptr->get_data_size() / sizeof(dunedaq::detdataformats::HSIFrame);
        auto frag_data_ptr = (char*)frag_ptr->get_data();

        //loop over the HSI frames
        for(size_t iframe=0; iframe<n_frames; ++iframe){
            auto hsi_frame = *(reinterpret_cast<dunedaq::detdataformats::HSIFrame*>(frag_data_ptr+iframe*sizeof(dunedaq::detdataformats::HSIFrame)));

            //check if LLT or HLT, and push back onto output collections
            auto link_no = hsi_frame.link;
            if(fProcessLLTs && link_no==0)
                hsi_llt_col.push_back(hsi_frame);
            if(fProcessHLTs && link_no==1)
                hsi_hlt_col.push_back(hsi_frame);

            //debug print statement
            if(fDebugLevel>1){
                if(link_no==0) std::cout << "\t\tLLT: ";
                else if(link_no==1) std::cout << "\t\tHLT: ";
                else std::cout << "\t\t???: ";

                std::cout << "TS=" << hsi_frame.get_timestamp();

                size_t position=0;
                std::cout << " Trigger=";
                auto val = hsi_frame.trigger;
                while(val>0) {
                    if (val & 1) std::cout << position << ",";
                    val = val >> 1;
                    position++;
                }

                position=0;
                std::cout << " Inputs=";
                val = hsi_frame.input_low;
                while(val>0) {
                    if (val & 1) std::cout << position << ",";
                    val = val >> 1;
                    position++;
                }
                position=0;
                val = hsi_frame.input_high;
                while(val>0) {
                    if (val & 1) std::cout << position+32 << ",";
                    val = val >> 1;
                    position++;
                }
                std::cout << std::endl;

            }
        }
    }

    if(fProcessLLTs)
        e.put(std::make_unique<std::vector<dunedaq::detdataformats::HSIFrame>>(std::move(hsi_llt_col)),fOutputInstance_LLT);
    if(fProcessHLTs)
        e.put(std::make_unique<std::vector<dunedaq::detdataformats::HSIFrame>>(std::move(hsi_hlt_col)),fOutputInstance_HLT);

}

DEFINE_ART_MODULE(PDHDCTBRawDecoder)
