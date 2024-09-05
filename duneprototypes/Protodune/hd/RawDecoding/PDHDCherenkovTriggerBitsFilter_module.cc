#include <iostream>
#include <utility>
#include <set>

#include "art/Framework/Core/EDAnalyzer.h" 
#include "art/Framework/Core/EDFilter.h" 
#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Principal/Event.h" 
#include "art_root_io/TFileService.h"

#include "detdataformats/hsi/HSIFrame.hpp"
#include "lardataobj/RawData/RDTimeStamp.h"

#include <set>


namespace pdhd {

class PDHDCherenkovTriggerBitsFilter : public art::EDFilter {

    static constexpr unsigned int const LLT_LPCHKV_MASK = 1 << 3; //LLT_3
    static constexpr unsigned int const LLT_HPCHKV_MASK = 1 << 2; //LLT_2

    const std::set<std::string> ALLOWED_CHERENKOV_TRIGGERS = { "HL","HLx","HxL","HxLx" };

 public:
    explicit PDHDCherenkovTriggerBitsFilter(fhicl::ParameterSet const & pset);
    virtual ~PDHDCherenkovTriggerBitsFilter() {};
    virtual bool filter(art::Event& e);

 private:
    std::string fTriggerTimestampLabel;
    std::string fLLTLabel;

    std::set<std::string> fCherenkovTriggers;
    int fTimeToleranceDTSTicks;

    int fDebugLevel;

};

PDHDCherenkovTriggerBitsFilter::PDHDCherenkovTriggerBitsFilter::PDHDCherenkovTriggerBitsFilter(fhicl::ParameterSet const & pset)
  : EDFilter(pset)
  , fTriggerTimestampLabel(pset.get<std::string>("TriggerTimestampLabel"))
  , fLLTLabel(pset.get<std::string>("LLTLabel"))
  , fTimeToleranceDTSTicks(pset.get<int>("TimeToleranceDTSTicks",5))
  , fDebugLevel(pset.get<int>("DebugLevel",0))
{
    std::vector<std::string> input_cts = pset.get< std::vector<std::string> >("CherenkovTriggers");
    for(auto const& ct : input_cts) {
        if (ALLOWED_CHERENKOV_TRIGGERS.count(ct) == 0)
            throw cet::exception("PDHDCherenkovTriggerBitsFilter")
                    << "UNKNOWN CHERENKOV TRIGGER TYPE " << ct << ". Must be 'HL', 'HLx', 'HxL', or 'HxLx'.";
        fCherenkovTriggers.insert(ct);
    }
}


bool PDHDCherenkovTriggerBitsFilter::filter(art::Event & evt) {

    auto &trigger_timestamps = *evt.getValidHandle<std::vector<raw::RDTimeStamp>>(fTriggerTimestampLabel);
    if(trigger_timestamps.size()!=1) {
        throw cet::exception("PDHDCherenkovTriggerBitsFilter") <<
            "ERROR: WRONG NUMBER OF TRIGGER TIMESTAMPS (" << trigger_timestamps.size() << ") FOUND";
    }

    long long int trigger_timestamp = trigger_timestamps.at(0).GetTimeStamp();

    if(fDebugLevel>0){
        std::cout << "Trigger timestamp= " << trigger_timestamp << std::endl;
    }


    auto &llt_hsi_frames = *evt.getValidHandle<std::vector<dunedaq::detdataformats::HSIFrame>>(fLLTLabel);
    if(llt_hsi_frames.size()==0) {
        std::cout << "WARNING: NO LLT WORDS FOUND!" << std::endl;
        return false;
    }

    if(fDebugLevel>0){
        std::cout << "Found " << llt_hsi_frames.size() << " LLT HSI frames." << std::endl;
    }

    bool found_lpchkv_hit = false;
    bool found_hpchkv_hit = false;
    for(auto const& llt_frame : llt_hsi_frames){

        if(fDebugLevel>1){
            std::cout << "\t\tLLT -- Timestamp=" << llt_frame.get_timestamp()
                      << "  TriggerBits=" << llt_frame.trigger << std::endl;
        }

        long long int llt_timestamp = llt_frame.get_timestamp();
        if(std::abs(llt_timestamp-trigger_timestamp)>fTimeToleranceDTSTicks) continue;

        if(fDebugLevel>0){
            std::cout << "\t\tMatching LLT (tdiff=" << std::abs(llt_timestamp-trigger_timestamp) << ")."
                      << " LP hit? " << (llt_frame.trigger & LLT_LPCHKV_MASK)
                      << " HP hit? " << (llt_frame.trigger & LLT_HPCHKV_MASK)
                      << std::endl;
        }

        if(llt_frame.trigger & LLT_LPCHKV_MASK) found_lpchkv_hit = true;
        if(llt_frame.trigger & LLT_HPCHKV_MASK) found_hpchkv_hit = true;

    }

    std::string ct="";
    if(found_hpchkv_hit&&found_lpchkv_hit) ct="HL";
    else if(found_hpchkv_hit&&!found_lpchkv_hit) ct="HLx";
    else if(!found_hpchkv_hit&&found_lpchkv_hit) ct="HxL";
    else ct="HxLx";

    if(fDebugLevel>0){
        std::cout << "Trigger type " << ct << std::endl;
    }

    if(fCherenkovTriggers.count(ct)==1) {
        if(fDebugLevel>0){
            std::cout << "Filter pass" << std::endl;
        }
        return true;
    }
    else {
        if(fDebugLevel>0){
            std::cout << "Filter fail" << std::endl;
        }
        return false;
    }
}



DEFINE_ART_MODULE(PDHDCherenkovTriggerBitsFilter)

}
