#ifndef CrpChannelHelper_H
#define CrpChannelHelper_H

// Interpret a CRP detector name.

#include "dunecore/DuneCommon/Utility/StringManipulator.h"

#include <string>
#include <vector>
#include <iostream>

class CrpChannelHelper {

public:

  using Index = unsigned int;
  using IndexVector = std::vector<Index>;
  using Name = std::string;
  using NameVector = std::vector<Name>;

  // Number of strips in each plane
  const Index nsu = 952;
  const Index nsv = 952;
  const Index nsz = 1168;
  IndexVector nPlaneStrips = {nsu, nsv, nsz};

  // Number of strips in a CRU.
  const Index nsc = nsu + nsv + nsz;

  // Number of FEMBs in a CRU.
  const Index nfc = 24;

  // Number of strips in the detector.
  Index nsdet = 0;

  // Number of CRUs.
  Index ncru = 0;

  // Number of TPC volumes.
  // For DUNE and ProtoDUNE, each has both top and bottom readout.
  Index nvol = 0;

  // Number of planes in CRU.
  Index npla = 3;

  // Names and labels.
  Name detname;
  NameVector plaLabs = {"u", "v", "z"};
  NameVector volumeNames;
  //NameVector cruLabs;

  // Flag indicating if FEMB ranges are defined.
  bool usefembs =false;

  // Flag indicating if FEMB adapters are defined.
  bool useadaps =false;


  // Ctor from detector name.
  CrpChannelHelper(Name sdet);

  // Report if this is a valid detector name.
  bool isValid() const;

  // Name of the end (top or bottom) of a CRU.
  Name cruEndName(Index icru) const {
    if ( icru >= ncru ) return "";
    Name scr = "cru";
    if ( ncru > 1 ) scr = icru < nvol ? "crb" : "crt";
    return scr;
  }

  // Name of the volume for a CRU: C, A, B, 01, 02, ...
  Name cruVolumeName(Index icru) const {
    if ( icru >= ncru ) return "";
    return volumeNames[icru%nvol];
  }

  // Name of a CRU.
  Name cruName(Index icru) const {
    return cruEndName(icru) + cruVolumeName(icru);
  }

  // Label for a CRU.
  Name cruLabel(Index icru) const {
    Name enam = cruEndName(icru);
    for ( char& c : enam ) c = std::toupper(c);
    return enam + "-" + cruVolumeName(icru);
  }

  // Name of a CRU plane.
  Name cruPlaneName(Index icru, Index ipla) const {
    return cruName(icru) + plaLabs[ipla];
  }

  // Label for a CRU plane.
  Name cruPlaneLabel(Index icru, Index ipla) const {
    return cruLabel(icru) + plaLabs[ipla];
  }

  // Return if a CRU has FEMBs.
  // For two-ended detectors, this is the first half of the range.
  bool cruHasFembs(Index icru) const {
    if ( ! usefembs ) return false;
    if ( ncru == 1 ) return true;
    return icru < ncru/2;
  }

  // Name of a CRU FEMB.
  Name fembName(Index icru, Index ifmb) const {
    Name sfmb = std::to_string(ifmb);
    if ( sfmb.size() < 2 ) sfmb = "0" + sfmb;
    return "femb" + cruVolumeName(icru) + sfmb;
  }

  // Label for a CRU FEMB.
  Name fembLabel(Index icru, Index ifmb) const {
    Name slab = fembName(icru, ifmb);
    slab.insert(4, "-");
    return slab;
  }

  // Name of a CRU FEMB-view.
  Name fembPlaneName(Index icru, Index ifmb, Index ipla) const {
    return fembName(icru, ifmb) + plaLabs[ipla];
  }

  // Name of a CRU FEMB-view.
  Name fembPlaneLabel(Index icru, Index ifmb, Index ipla) const {
    return fembLabel(icru, ifmb) + plaLabs[ipla];
  }

};

inline CrpChannelHelper::CrpChannelHelper(Name sdet) {
   Name myname = "CrpChannelHelper::ctor: ";
  // Split detector name and options.
  NameVector vals = StringManipulator(sdet).split(":");
  detname = vals[0];
  // Build the labels and set default fo usefembs.
  usefembs = false;
  if ( detname == "cb2022" ) {
    ncru = 1;
    nvol = 1;
    volumeNames.push_back("C");
  } else if ( detname == "pdvd" ) {
    usefembs = true;
    ncru = 4;
    nvol = 2;
    volumeNames.push_back("A");
    volumeNames.push_back("B");
  } else {
    std::cout << myname << "ERROR: Invalid detector name: " << detname << std::endl;
  }
  nsdet = nsc*ncru;
  // Override usefembs.
  for ( Index ival=1; ival<vals.size(); ++ival ) {
    Name val = vals[ival];
    if      ( val == "fembs" )   usefembs = true;
    else if ( val == "nofembs" ) usefembs = false;
    else {
      std::cout << myname << "WARNING: Ignoring invalid detector option " << val << std::endl;
    }
  }
}

inline bool CrpChannelHelper::isValid() const {
  return ncru != 0;
}

#endif
