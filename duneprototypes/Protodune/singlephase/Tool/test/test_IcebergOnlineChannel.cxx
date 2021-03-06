// test_IcebergOnlineChannel.cxx
//
// David Adams
// May 2018
//
// Test IcebergOnlineChannel.

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "dunecore/ArtSupport/DuneToolManager.h"
#include "dunecore/DuneInterface/Tool/IndexMapTool.h"
#include "dunecore/ArtSupport/ArtServiceHelper.h"
#include "TH1F.h"

#undef NDEBUG
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using std::ofstream;
using std::istringstream;
using fhicl::ParameterSet;
using Index = IndexMapTool::Index;
using IndexVector = std::vector<Index>;

//**********************************************************************

int test_IcebergOnlineChannel(bool useExistingFcl =false) {
  const string myname = "test_IcebergOnlineChannel: ";
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";

  cout << myname << line << endl;
  string fclfile = "test_IcebergOnlineChannel.fcl";
  if (useExistingFcl) {
    cout << myname << "Using existing top-level FCL." << endl;
  } else {
    cout << myname << "Creating top-level FCL." << endl;
    ofstream fout(fclfile.c_str());
    fout << "#include \"IcebergChannelMapService.fcl\"" << endl;
    fout << "services: { IcebergChannelMapService: @local::icebergchannelmap }" << endl;
    fout << "tools: {" << endl;
    fout << "  mytool: {" << endl;
    fout << "    tool_type: IcebergOnlineChannel" << endl;
    fout << "     LogLevel: 2" << endl;
    fout << "     Ordering: \"FEMB\"" << endl;
    fout << "  }" << endl;
    fout << "  reftool: {" << endl;
    fout << "    tool_type: ProtoduneOnlineChannel" << endl;
    fout << "     LogLevel: 1" << endl;
    fout << "  }" << endl;
    fout << "}" << endl;
    fout.close();
  }

  cout << myname << line << endl;
  cout << myname << "Fetching tool manager." << endl;
  DuneToolManager* ptm = DuneToolManager::instance(fclfile);
  assert ( ptm != nullptr );
  DuneToolManager& tm = *ptm;
  tm.print();
  assert( tm.toolNames().size() >= 1 );

  std::ifstream config{fclfile};
  ArtServiceHelper::load_services(config);

  cout << myname << line << endl;
  cout << myname << "Fetching tool." << endl;
  auto cma = tm.getPrivate<IndexMapTool>("mytool");
  assert( cma != nullptr );

  Index badIndex = IndexMapTool::badIndex();

  cout << myname << line << endl;
  cout << myname << "Check some good values." << endl;
  Index ichaOff = 0;
  while ( ichaOff < 1240 ) {
    Index ichaOn = cma->get(ichaOff);
    cout << myname << ichaOff << " --> " << ichaOn << endl;
    assert( ichaOff != badIndex );
    ++ichaOff;
    ichaOn = cma->get(ichaOff);
    cout << myname << ichaOff << " --> " << ichaOn << endl;
    assert( ichaOff != badIndex );
    ichaOff += (ichaOff < 800 ? 40 : 48) - 1;
  }

  cout << myname << line << endl;
  cout << myname << "Check some bad values." << endl;
  cout << myname << "Bad index: " << badIndex << endl;
  for ( Index ichaOff : { -1, 1280, 20000 } ) {
    Index ichaOn = cma->get(ichaOff);
    cout << myname << ichaOff << " --> " << ichaOn << endl;
    assert( ichaOn == badIndex );
  }

  cout << myname << line << endl;
  cout << myname << "Check each online index appears exactly once." << endl;
  const Index ncha = 1280;
  IndexVector onlineCounts(ncha);
  IndexVector offlineChannel(ncha, badIndex);
  Index nshow = 64;
  for ( Index ichaOff=0; ichaOff<ncha; ++ichaOff ) {
    Index ichaOn = cma->get(ichaOff);
    if ( nshow*(ichaOff/nshow) == ichaOff || ichaOn >= ncha )
      cout <<  myname << "  "  << ichaOff << " --> " << ichaOn << endl;
    assert( ichaOn < ncha );
    if ( offlineChannel[ichaOn] != badIndex ) {
      cout << myname << "ERROR: Online channel " << ichaOn
           << " is mapped to two offline channels:" << endl;
      cout << "  " << offlineChannel[ichaOn] << endl;
      cout << "  " << ichaOff << endl;
      assert( false );
    }
    assert( onlineCounts[ichaOn] == 0 );
    onlineCounts[ichaOn] += 1;
    offlineChannel[ichaOn] = ichaOff;
  }
  for ( Index ichaOn=0; ichaOn<ncha; ++ichaOn ) {
    assert( onlineCounts[ichaOn] == 1 );
    assert( offlineChannel[ichaOn] != badIndex );
  }

  cout << myname << line << endl;
  cout << myname << "Done." << endl;
  return 0;
}

//**********************************************************************

int main(int argc, char* argv[]) {
  bool useExistingFcl = false;
  if ( argc > 1 ) {
    string sarg(argv[1]);
    if ( sarg == "-h" ) {
      cout << "Usage: " << argv[0] << " [keepFCL] [RUN]" << endl;
      cout << "  If keepFCL = true, existing FCL file is used." << endl;
      cout << "  If RUN is nonzero, the data for that run are displayed." << endl;
      return 0;
    }
    useExistingFcl = sarg == "true" || sarg == "1";
  }
  return test_IcebergOnlineChannel(useExistingFcl);
}

//**********************************************************************
