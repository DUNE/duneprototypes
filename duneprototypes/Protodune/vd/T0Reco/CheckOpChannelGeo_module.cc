////////////////////////////////////////////////////////////////////////
// Class:       CheckOpChannelGeo
// Plugin Type: analyzer (Unknown Unknown)
// File:        CheckOpChannelGeo_module.cc
//
// Generated at Wed Jul 24 11:05:37 2024 by Yoann Kermaidic using cetskelgen
// from cetlib version 3.18.02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "larcorealg/Geometry/GeometryCore.h"
#include "larcore/Geometry/Geometry.h"
#include "lardataobj/RecoBase/OpHit.h"

#include "art_root_io/TFileService.h"

#include "TH2.h"
#include "TGraph.h"

using std::vector;
using std::string;

namespace pdvdana {
  class CheckOpChannelGeo;
}


class pdvdana::CheckOpChannelGeo : public art::EDAnalyzer {
public:
  explicit CheckOpChannelGeo(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  CheckOpChannelGeo(CheckOpChannelGeo const&) = delete;
  CheckOpChannelGeo(CheckOpChannelGeo&&) = delete;
  CheckOpChannelGeo& operator=(CheckOpChannelGeo const&) = delete;
  CheckOpChannelGeo& operator=(CheckOpChannelGeo&&) = delete;

  // Required functions.
  void analyze(art::Event const& e) override;

  // Selected optional functions.
  void beginJob() override;
  void endJob() override;

private:

  TH2F* h2_yz;
  TGraph* gFrame;

  // detector geometry
  const geo::Geometry* geom;
     
};


pdvdana::CheckOpChannelGeo::CheckOpChannelGeo(fhicl::ParameterSet const& p)
  : EDAnalyzer{p}  // ,
{
  geom = &*art::ServiceHandle<geo::Geometry>();
}

void pdvdana::CheckOpChannelGeo::analyze(art::Event const& e)
{

   int fEventNum = e.id().event();
    
   std::cout << "CheckOpChannelGeo: Start analysing event " << fEventNum << " ..." << std::endl;
   std::cout << "CheckOpChannelGeo: Detector name: " << geom->DetectorName()  << std::endl;
   std::cout << "CheckOpChannelGeo: NOpDets: " << geom->NOpDets() << " / NOpChannels: " << geom->NOpChannels() << std::endl;

   std::ostringstream sstr;
   geom->Print(sstr,"   ");
   //std::cout << sstr.str() << std::endl;
   
   vector<bool> isopdetset(geom->NOpDets(),false);

   auto ophits = e.getValidHandle<vector<recob::OpHit>>("ophit");
   size_t nophits = ophits->size();

   for(size_t ihits=0;ihits<nophits; ++ihits){
     const recob::OpHit& hit = ophits->at(ihits); 

     auto const xyz = geom->OpDetGeoFromOpChannel(hit.OpChannel()).GetCenter();
//     auto const fid = geom->OpDetGeoFromOpChannel(hit.OpChannel()).ID(); 
     int opdet      = geom->OpDetFromOpChannel(hit.OpChannel());
     int hwdet      = geom->HardwareChannelFromOpChannel(hit.OpChannel());

     //std::cout << "   " << ihits << " " << hit.OpChannel() << " " << hit.PeakTime() << " " << hit.Amplitude() << " " << hit.Frame() << " " << xyz.X() << " " << xyz.Y() << " " << xyz.Z() << std::endl;
 
     for(int biny=1;biny<=h2_yz->GetNbinsY();biny++){
       for(int binx=1;binx<=h2_yz->GetNbinsX();binx++){
         float ylow  = h2_yz->GetXaxis()->GetBinLowEdge(binx);
         float yhigh = ylow + h2_yz->GetXaxis()->GetBinWidth(binx);

         float zlow  = h2_yz->GetYaxis()->GetBinLowEdge(biny);
         float zhigh = zlow + h2_yz->GetYaxis()->GetBinWidth(biny);
        
         if(xyz.Y() < ylow || xyz.Y() > yhigh) continue; 
         if(xyz.Z() < zlow || xyz.Z() > zhigh) continue; 


         if(!isopdetset[opdet]){
           isopdetset[opdet] = true;
           h2_yz->Fill(xyz.Y(),xyz.Z(),hit.OpChannel());
	   
           std::cout << "     channel: "    << hit.OpChannel()  << " opdet: " << opdet << " hwdet: " << hwdet
                     << " geo id: "         << geom->Cryostat().OpDet(hit.OpChannel()).ID().toString()
		     << " ("                << binx    << "," << biny 
                     << ") -> ["            << ylow    << "," << zlow 
                     << "] < ["             << xyz.Y() << "," << xyz.Z() 
                     << "] < ["             << yhigh   << "," << zhigh 
                     << "]"                 << std::endl;
         }
       }
     }
   }
}

void pdvdana::CheckOpChannelGeo::beginJob()
{
   std::cout << "CheckOpChannelGeo: begin job" << std::endl;

   art::ServiceHandle<art::TFileService> tfs;

   // VD cathode layout
   int   ny   = 5,      nz   = 4;
   float ymin = -168.5, zmin = 0;
   float dy   = 84.25,  dz   = 75;
  

   h2_yz     = tfs->make<TH2F>("h2_yz"    ,";Y (cm);Z (cm)",2*ny,ymin,ymin+dy*ny,nz,zmin,zmin+dz*nz);

   for(int binx=1;binx<=h2_yz->GetNbinsX();binx++)
     for(int biny=1;biny<=h2_yz->GetNbinsY();biny++){
       h2_yz->SetBinContent(binx,biny,0);
     }
   
   vector<float> ysub(ny+1), zsub(nz+1);

   for(int i=0;i<=ny;i++) ysub[i] = ymin + i*dy;
   for(int i=0;i<=nz;i++) zsub[i] = zmin + i*dz;

   double xframe[21] = {ysub[0],ysub[0], ysub[1], ysub[1],ysub[2],ysub[2], ysub[3], ysub[3],ysub[4],ysub[4], ysub[ny],ysub[ny],ysub[0],ysub[0],ysub[ny],ysub[ny],ysub[0],ysub[0],ysub[ny],ysub[ny],ysub[0] };
   double yframe[21] = {zsub[0],zsub[nz],zsub[nz],zsub[0],zsub[0],zsub[nz],zsub[nz],zsub[0],zsub[0],zsub[nz],zsub[nz],zsub[0], zsub[0],zsub[1],zsub[1], zsub[2], zsub[2],zsub[3],zsub[3], zsub[nz],zsub[nz]};
  
   gFrame = tfs->make<TGraph>(21,xframe,yframe);
}

void pdvdana::CheckOpChannelGeo::endJob()
{
   std::cout << "CheckOpChannelGeo: end job" << std::endl;

   h2_yz->Write();
   gFrame->Write();
}

DEFINE_ART_MODULE(pdvdana::CheckOpChannelGeo)
