#include "ifbeam.h"
//#include "ifdh_art/IFBeamService/IFBeam_service.h"
#include <memory>
#include <string>
#include <iostream>
#include <strings.h>

int main(int argc, char ** argv) {


  ifbeam_ns::BeamFolder::_debug = 1;
  long long timestamp = 107428980173512887;
  std::string bundle_name = "DUNE_CERN_SEP2018";
  for (int iArg = 1; iArg < argc; iArg++) {
    if (!strcasecmp(argv[iArg],"-t")) {
      timestamp = std::atof(argv[++iArg]);
    }
    if (!strcasecmp(argv[iArg],"-b")) {
        bundle_name = argv[++iArg];
    }
    if (!strcasecmp(argv[iArg],"-h")) {
      std::cout << "Usage: test_ifbeam [-t timestamp] [-b bundle]" << std::endl;
      return 1;
    }
  }

  std::string url = "https://ifb-data.fnal.gov:8104/ifbeam";
  std::cout << "timestamp: " << timestamp << std::endl;
  std::cout << "url: " << url << std::endl;
  std::cout << "bundle: " << bundle_name << std::endl;

  double time_width = 60.;
  std::unique_ptr<BeamFolder> bfp = std::make_unique<BeamFolder>(
      bundle_name, url, time_width);

  for (auto & dev : bfp->GetDeviceList()) {
    std::cout << dev << std::endl;
  }

  bfp->set_epsilon(6.);
    
  std::cout << "Filling cache" << std::endl;
  bfp->FillCache(timestamp);
  std::cout << "Done" << std::endl;
  
}
