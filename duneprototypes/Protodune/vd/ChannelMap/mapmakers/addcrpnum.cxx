#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

typedef struct TPCChanInfo {
  unsigned int offlchan;      // in gdml and channel sorting convention
  unsigned int detid;         // from detdatformats/DetID.hpp.   Map key
  unsigned int detelement;    // CRP or APA number
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


void printinfo(TPCChanInfo_t info);

int main(int argc, char **argv)
{

  while (true) {
    TPCChanInfo_t info; std::cin >> info.offlchan 
				 >> info.detid
				 >> info.crate
				 >> info.slot
				 >> info.stream
				 >> info.streamchan
				 >> info.plane 
				 >> info.chan_in_plane 
				 >> info.femb 
				 >> info.asic 
				 >> info.asicchan;
    info.valid = true;
    if (info.offlchan < 3072 )
      {
	info.detelement = 5;
      }
    else if (info.offlchan < 6144 )
      {
	info.detelement = 4;
      }
    else if (info.offlchan < 9216 )
      {
	info.detelement = 2;
      }
    else if (info.offlchan < 12288 )
      {
	info.detelement = 3;
      }
    else
      {
	std::cout << "offlchan not understood: " << info.offlchan << std::endl;
	info.detelement = 0;
        info.valid = false; // doesn't get in the output but we'll say this here anyway
      }
    if (std::cin.eof()) break;
    
    printinfo(info);
  }

  return 0;
}

// new unifed chaninfo struct printer

void printinfo(TPCChanInfo_t info)
{
  std::cout << info.offlchan 
	    << " " << info.detid
            << " " << info.detelement
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
