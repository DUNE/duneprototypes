#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

typedef struct VDChanInfo {
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

void printinfo(ChanInfo_t info);

int main(int argc, char **argv)
{

  std::vector<ChanInfo_t> cvec;
  
  std::string fullname("vdcbce_chanmap_v4.txt");
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

  // write out CRP 5, with the same map as V4 CB BDE
  
  for (const auto& ci_old : cvec)
    {
      ChanInfo_t ci = ci_old;
      ci.CRPName = "BottomCRP5";
      ci.crate = 11;
      printinfo(ci);
    }

  // write out CRP 4, rotated 180 degrees

  for (const auto& ci_old : cvec)
    {
      ChanInfo_t ci = ci_old;
      ci.CRPName = "BottomCRP4";
      ci.crate = 10;
      int nc = 0;  // new offline channel
      const int oc = ci.offlchan; // old offline channel
      if (oc < 952)
	{
	  nc = 951 - oc;
	}
      else if (oc < 1904)
	{
	  nc = 2855 - oc; 
	}
      else if (oc < 3072)
	{
	  nc = 4975 - oc;
	}
      else
	{
	  std::cout << "ununderstood channel: " << oc << std::endl;
	  return 1;
	}
      ci.offlchan = nc + 3072;
      printinfo(ci);
    }
  return 0;
}

void printinfo(ChanInfo_t info)
{
  std::cout << info.offlchan 
	    << " " << info.crate 
	    << " " << info.CRPName
	    << " " << info.wib 
	    << " " << info.link 
	    << " " << info.femb_on_link 
	    << " " << info.cebchan 
	    << " " << info.plane 
	    << " " << info.chan_in_plane 
	    << " " << info.femb 
	    << " " << info.asic 
	    << " " << info.asicchan
	    << " " << info.wibframechan << std::endl; 
}
