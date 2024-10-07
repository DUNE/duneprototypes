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
#include "TH1F.h"
#include "TH2F.h"


namespace pdhd {

using RawDigitVector = std::vector<raw::RawDigit>;

class PDHDGroundShakeFilter : public art::EDFilter {
 public:
  explicit PDHDGroundShakeFilter(fhicl::ParameterSet const & pset);
  virtual ~PDHDGroundShakeFilter() {};
  virtual bool filter(art::Event& e);

 private:
  std::string fRawDigitLabel;


};

PDHDGroundShakeFilter::PDHDGroundShakeFilter::PDHDGroundShakeFilter(fhicl::ParameterSet const & pset):
  EDFilter(pset),
  fRawDigitLabel(pset.get<std::string>("RawDigitLabel")){}


bool PDHDGroundShakeFilter::filter(art::Event & evt) {


  // auto const &rawdigits = *evt.getValidHandle<vector<raw::RawDigit>>(rawdigits_tag);
  auto &rawdigits = *evt.getValidHandle<RawDigitVector>(fRawDigitLabel);
  if (rawdigits.empty())
        {
            std::cout << "WARNING: no RawDigit found." << std::endl;
            return false;
        }  

  const int nticks = rawdigits[0].Samples();
  const int nchans = rawdigits.size();
  // int Event_ = evt.id().event();
  // std::cout<<"evt = "<<m_Event<<std::endl;
  // std::cout<<"nticks = "<<nticks<<std::endl;
  // std::cout<<"nchans = "<<nchans<<std::endl;
  TH2F *h_orig = new TH2F("h_orig", "RawDigits", nchans, -0.5, nchans - 0.5, nticks, 0, nticks);

  for (auto rd : rawdigits){
            int channel = rd.Channel();
            // int nSamples = rd.Samples();
            for (int j = 0; j < nticks; j++)
            {
                h_orig->SetBinContent(channel + 1, j + 1, rd.ADC(j) - rd.GetPedestal());
            }
  } // end of rawdigits

  // Project all channels to 1D.
  TH1F *hhh = new TH1F("hhh","hhh",nticks,0,nticks);
  for(int j=0;j<nticks;j++){
      TH1F *h1 = (TH1F *)h_orig->ProjectionX("proj_x", j+1, j+1);
      h1->SetDirectory(0);
      int val = h1->Integral();
      hhh->SetBinContent(j+1,val);
      delete h1;
  }
  double diff = hhh->GetMaximum()-hhh->GetMinimum();
  //cout<<"diff = "<<hhh->GetMaximum()-hhh->GetMinimum()<<endl;
  delete hhh;
  delete h_orig;
  if(diff>1e7){
        std::cout<<"found! Groud shake noise at Evt "<<evt.id().event()<<std::endl;
        return false;
  }
  // std::cout<<"Groud shake noise filter applied"<<std::endl;
  return true;
}



DEFINE_ART_MODULE(PDHDGroundShakeFilter)

}
