////////////////////////////////////////////////////////////////////////
//// File:        PDVDPDMapAlg.hh
//// Authors: Kaixin Zhu (based on the SBND mapping algorithm)
////
//// Updates: 2025-02, v08_45_00. Kaixin Zhu kzhu04@colostate.edu
////          Added properties to PDS MAP and functions to access these.
////
//// This class stores the ProtoDUNE VD PDS Map and channel's properties;
//// also implements functions to access these.
////
//// As of version v09_24_01 each entry of the PDS Map
//// has the following characteristics:
////
//// channel: 0 to 39
//// pd_type: Membrane, Cathode, PMT TPB, PMT PEN, PMT PEN + Q, Membrane + Q, PMT CLEAR
//// sens_Ar: true or false
//// sens_Xe: true or false
//// eff: 0 to 1
//// HardwareChannel: Slot, Link, DaphneChannel, OfflineChannel
//////////////////////////////////////////////////////////////////////////
//

#ifndef PDVDPDMAPALG_HH
#define PDVDPDMAPALG_HH

#include "PDMapAlg.h"
//#include "art/Utilities/ToolMacros.h"
//#include "art/Utilities/make_tool.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <string>

//#include "art_root_io/TFileService.h"

#include <nlohmann/json.hpp>

namespace opdet {

  class PDVDPDMapAlg : public PDMapAlg{

  public:
    //Default constructor
    explicit PDVDPDMapAlg(const fhicl::ParameterSet& pset);
    PDVDPDMapAlg() : PDVDPDMapAlg(fhicl::ParameterSet()) {}
    //Default destructor
    ~PDVDPDMapAlg();

    nlohmann::json getCollectionWithProperty(std::string property);
    template<typename T> nlohmann::json getCollectionWithProperty(std::string property, T property_value);
    template<typename Func> nlohmann::json getCollectionFromCondition(Func condition);

    // struct Config {};

    //  PDVDPDMapAlg(Config const&) {}

    // void setup() {}

    bool isValidHardwareChannel(int hwch) const;
    bool isValidOpDetChannel(size_t opDet) const;

    bool isPDType(size_t opDet, std::string pdname) const override;
    bool isSensitiveToAr(size_t opDet) const;
    bool isSensitiveToXe(size_t opDet) const;
    std::string pdType(size_t opDet) const override;
    double Efficiency(size_t opDet) const;

    bool isPDTypeHardwareChannel(int hdch, std::string pdname) const;
    bool isSensitiveToArHardwareChannel(int hdch) const;
    bool isSensitiveToXeHardwareChannel(int hdch) const;
    std::string pdTypeHardwareChannel(int hdch) const;
    double EfficiencyHardwareChannel(int hdch) const;


    struct HardwareChannelEntry
      {
        int slot;
        int link;
        int daphne_channel;
        int offline_channel;
      };
    std::vector<HardwareChannelEntry> hardwareChannel(size_t opDet) const;

    std::vector<int> getChannelsOfType(std::string pdname) const;
    auto getOpDetChannelEntry(size_t opDet) const;
    auto getOpDetChannelPerHardwareChannel(size_t hwch) const;
    std::map<int,int> MapHardwareChannelToOpDetChannel;
      
    size_t size() const;

  private:
    nlohmann::json PDmap;
  }; // class PDVDPDMapAlg

  template<typename T>
  nlohmann::json PDVDPDMapAlg::getCollectionWithProperty(std::string property, T property_value)
  {
    nlohmann::json subSetPDmap;
    std::copy_if (PDmap.begin(), PDmap.end(), std::back_inserter(subSetPDmap),
                  [property, property_value](auto const& e)->bool
                    {return e[property] == property_value;} );
    return subSetPDmap;
  }

  template<typename Func>
  nlohmann::json PDVDPDMapAlg::getCollectionFromCondition(Func condition)
  {
    nlohmann::json subSetPDmap;
    std::copy_if (PDmap.begin(), PDmap.end(), std::back_inserter(subSetPDmap),
                  condition);
    return subSetPDmap;
  }

} // namespace

#endif // PDVDPDMapAlg_HH

