#include "ifbeam.h"
//#include "ifdh_art/IFBeamService/IFBeam_service.h"
#include <memory>
#include <string>
#include <iostream>
#include <strings.h>

int main(int argc, char ** argv) {

  long double timestamp = 107428980173512887;
  for (int iArg = 1; iArg < argc; iArg++) {
    if (!strcasecmp(argv[iArg],"-t")) {
      timestamp = std::atof(argv[++iArg]);
    }
    if (!strcasecmp(argv[iArg],"-h")) {
      std::cout << "Usage: runEventCounter -c fclfile.fcl -o output_file [-n nthreads]" << std::endl;
      return 1;
    }
  }

  std::string url = "https://ifb-data.fnal.gov:8104/ifbeam";
  std::string bundle_name = "DUNE_CERN_SEP2018";
  double time_width = 60.;
  std::unique_ptr<BeamFolder> bfp = std::make_unique<BeamFolder>(
      bundle_name, url, time_width);
    
  std::cout << "Filling cache" << std::endl;
  bfp->FillCache(timestamp);
  std::cout << "Done" << std::endl;
  
}
