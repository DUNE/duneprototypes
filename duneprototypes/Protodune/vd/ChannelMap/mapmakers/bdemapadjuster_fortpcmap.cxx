#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

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


typedef struct TPCChanInfo {
  unsigned int offlchan;      // in gdml and channel sorting convention
  unsigned int detid;         // from detdatformats/DetID.hpp.   Map key
  unsigned int crate;         // crate number   Map key
  unsigned int slot;          // slot in crate (WIB for BDE and HD, card for TDE)   Map key
  unsigned int stream;        // Hermes stream for BDE and HD, 0 for TDE   Map key
  unsigned int streamchan;    // channel in the stream.  Usually 0:63   Map key
  unsigned int plane;         // 0: U,  1: V,  2: X      For info only
  unsigned int chan_in_plane; // which channel this is in the plane   For info only
  unsigned int femb;          // which FEMB      For info only
  unsigned int asic;          // ASIC            For info only
  unsigned int asicchan;      // ASIC channel    For info only
  bool valid;                 // true if valid, false if not  -- if map lookup fails, this is false
} TPCChanInfo_t;

void printinfo_orig(ChanInfo_t info);

void printinfo(TPCChanInfo_t info);

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

  // write out CRP 4, with the same map as V4 CB BDE but shifted by 3072 channels
  
  for (const auto& ci_old : cvec)
    {
      TPCChanInfo_t ci;
      // ci.CRPName = "BottomCRP4";

      ci.offlchan = ci_old.offlchan;
      ci.detid = 10;   //  kVD_BottomTPC from detdataformats/DetID.hpp
      ci.crate = 10;   //  CRP 4
      ci.slot = ci_old.wib - 1;
      ci.stream = 64*ci_old.link + ci_old.wibframechan/64;
      ci.streamchan = ci_old.wibframechan % 64;
      ci.plane = ci_old.plane;
      ci.chan_in_plane = ci_old.chan_in_plane;
      ci.femb = ci_old.femb;
      ci.asic = ci_old.asic;
      ci.asicchan = ci_old.asicchan;
      ci.valid = true;
      
      ci.offlchan = ci.offlchan + 3072;
      // adjust it
      const int oc = ci.offlchan;
      int nc = 0;
      if (oc < 3548)  // plane 0
	{
	  nc = oc + 476;
	}
      else if (oc < 4024) // plane 0
	{
	  nc = oc - 476;
	}
      else if ( oc < 4500 ) // plane 1
	{
	  nc = oc + 476;
	}
      else if ( oc < 4976) // plane 1
	{
	  nc = oc - 476;
	}
      else if (oc < 5268) // plane 2
	{
	  nc = 10827 - oc;
	}
      else if (oc < 5560) // plane 2
	{
	  nc = 11411 - oc;
	}
      else if (oc < 5852) // plane 2
	{
	  nc = 10827 - oc;
	}
      else if (oc < 6144) // plane 2
	{
	  nc = 11411 - oc;
	}
      else
	{
	  std::cout << "ununderstood channel: " << oc << std::endl;
	  return 1;
	}
      ci.offlchan = nc;      
      printinfo(ci);
    }

  // write out CRP 5, rotated 180 degrees

  for (const auto& ci_old : cvec)
    {

      TPCChanInfo_t ci;
      // ci.CRPName = "BottomCRP5";

      ci.offlchan = ci_old.offlchan;
      ci.detid = 10;   //  kVD_BottomTPC from detdataformats/DetID.hpp
      ci.crate = 11;   //  CRP 5
      ci.slot = ci_old.wib - 1;
      ci.stream = 64*ci_old.link + ci_old.wibframechan/64;
      ci.streamchan = ci_old.wibframechan % 64;
      ci.plane = ci_old.plane;
      ci.chan_in_plane = ci_old.chan_in_plane;
      ci.femb = ci_old.femb;
      ci.asic = ci_old.asic;
      ci.asicchan = ci_old.asicchan;
      ci.valid = true;

      int nc = 0;  // new offline channel
      const int oc = ci.offlchan; // old offline channel
      if (oc < 476)      // plane 0
	{
	  nc = 475 - oc;
	}
      else if (oc < 952) // plane 0
	{
	  nc = 1427 - oc;
	}
      else if (oc < 1428) // plane 1
	{
	  nc = 2379 - oc; 
	}
      else if (oc < 1904) // plane 1
	{
	  nc = 3331 - oc;
	}
      else if (oc < 2196) // plane 2
	{
	  nc = oc + 292;
	}
      else if (oc < 2488) // plane 2
	{
	  nc = oc - 292;
	}
      else if (oc < 2780) // plane 2
	{
	  nc = oc + 292;
	}
      else if (oc < 3072) // plane 2
	{
	  nc = oc - 292;
	}
      else
	{
	  std::cout << "ununderstood channel: " << oc << std::endl;
	  return 1;
	}
      ci.offlchan = nc;
      printinfo(ci);
    }
  return 0;
}

// coldbox chaninfo struct printer

void printinfo_orig(ChanInfo_t info)
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

// new unifed chaninfo struct printer

void printinfo(TPCChanInfo_t info)
{
  std::cout << info.offlchan 
	    << " " << info.detid
	    << " " << info.crate 
	    << " " << info.slot
	    << " " << info.stream
	    << " " << info.streamchan
	    << " " << info.plane 
	    << " " << info.chan_in_plane 
	    << " " << info.femb 
	    << " " << info.asic 
	    << " " << info.asicchan << std::endl; 
}
