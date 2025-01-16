// visualize wires attached to one FEMB for PDVDBDE
// get the wire dump file protodunevd_v4_driftx_wiredump.txt from DUNE-doc-32587-v2
// run as a ROOT macro

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <TGraph2D.h>
#include <TString.h>

typedef struct HDChanInfo {
  unsigned int offlchan;        // in gdml and channel sorting convention
  unsigned int crate;           // crate number
  std::string CRPName;          // Descriptive APA name
  unsigned int wib;             // 1, 2, 3, 4 or 5  (slot number +1?)
  unsigned int link;            // link identifier: 0 or 1
  unsigned int femb_on_link;    // which of two FEMBs in the WIB frame this FEMB is:  0 or 1
  unsigned int cebchan;         // cold electronics channel on FEMB:  0 to 127
  unsigned int plane;           // 0: U,  1: V,  2: X
  unsigned int chan_in_plane;   // which channel this is in the plane in the FEMB:  0:39 for U and V, 0:47 for X
  unsigned int femb;            // which FEMB on an APA -- 1 to 20
  unsigned int asic;            // ASIC:   1 to 8
  unsigned int asicchan;        // ASIC channel:  0 to 15
  unsigned int wibframechan;    // channel index in WIB frame (used with get_adc in detdataformats/WIB2Frame.hh).  0:255
  bool valid;          // true if valid, false if not
} ChanInfo_t;

typedef struct winfo {
  unsigned int offlchan;
  unsigned int cryo;
  unsigned int tpc;
  unsigned int plane;
  unsigned int wire;
  double x1;
  double y1;
  double z1;
  double x2;
  double y2;
  double z2;
} winfo_t;
  
void fvis(int crateplot=11, int fplot=1)
{

  std::vector<ChanInfo_t> cvec;
  
  std::string fullname("PD2VDBottomTPCChannelMap_v1.txt");
  std::ifstream inFile(fullname, std::ios::in);
  std::string line;

  while (std::getline(inFile,line)) {
    std::stringstream linestream(line);

    ChanInfo_t chanInfo;
    linestream 
      >> chanInfo.offlchan 
      >> chanInfo.crate 
      >> chanInfo.CRPName
      >> chanInfo.wib 
      >> chanInfo.link 
      >> chanInfo.femb_on_link 
      >> chanInfo.cebchan 
      >> chanInfo.plane 
      >> chanInfo.chan_in_plane 
      >> chanInfo.femb 
      >> chanInfo.asic 
      >> chanInfo.asicchan
      >> chanInfo.wibframechan; 

    chanInfo.valid = true;
    cvec.push_back(chanInfo);
  }
  inFile.close();

  std::vector<winfo_t> wvec;
  std::string fullname2("protodunevd_v4_driftx_wiredump.txt");
  std::ifstream inFile2(fullname2, std::ios::in);
  while (std::getline(inFile2,line)) {
    std::stringstream linestream(line);

    winfo_t w;
    linestream 
      >> w.offlchan 
      >> w.cryo 
      >> w.tpc
      >> w.plane 
      >> w.wire
      >> w.x1
      >> w.y1
      >> w.z1
      >> w.x2
      >> w.y2
      >> w.z2;
      

    wvec.push_back(w);
    //std::cout << w.offlchan << " " << w.x1 << std::endl; 
  }
  inFile2.close();

  {
    double x[2],y[2],z[2];
    x[0] = -370;
    x[1] = 370;
    y[0] = -370;
    y[1] = 370;
    z[0] = -10;
    z[1] = 370;
    auto dt = new TGraph2D(2,x,y,z);
    dt->SetLineColor(0);
    dt->Draw("LINE");
  }

  for (const auto ci : cvec)
    {
      if (ci.crate == crateplot && ci.femb == fplot)
	{
	  for (const auto &w : wvec)
	    {
	      if (w.offlchan == ci.offlchan)
		{
		  double x[2];
		  x[0] = w.x1;
		  x[1] = w.x2;	  
		  double y[2];
		  y[0] = w.y1;
		  y[1] = w.y2;
		  double z[2];
		  z[0] = w.z1;
		  z[1] = w.z2;
		  auto dt = new TGraph2D(2,x,y,z);
		  TString gt;
		  gt = ci.offlchan;
		  dt->SetTitle(gt);
		  dt->SetName(gt);
		  dt->Print();
		  dt->SetLineColor(1);
		  dt->Draw("LINE,SAME");
		}
	    }
	}
    }  
}

