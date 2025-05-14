#include "PDVD_PDMapAlg.hh"
#include "art/Utilities/ToolMacros.h"
#include "art/Utilities/make_tool.h"

    //------------------------------------------------------------------------------
    //--- opdet::PDVD_PDMapAlg implementation
    //------------------------------------------------------------------------------

    namespace opdet {

     PDVD_PDMapAlg::PDVD_PDMapAlg(const fhicl::ParameterSet& pset)
      {
        std::string fname;
        cet::search_path sp("FW_SEARCH_PATH");
        std::string filename = pset.get<std::string>("MappingFile", "PDVD_PDS_Mapping_v04152025.json");
        sp.find_file(filename, fname);
        std::ifstream i(fname, std::ifstream::in);
        i >> PDmap;
        i.close();
      }

    PDVD_PDMapAlg::~PDVD_PDMapAlg()
      { }
      
      bool PDVD_PDMapAlg::isValid(size_t ch) const
      {
        return ch < PDmap.size() && PDmap.at(ch)["channel"] == ch;
      }     

      bool PDVD_PDMapAlg::isPDType(size_t ch, std::string pdname) const
      {
        if(PDmap.at(ch)["pd_type"] == std::string(pdname)) return true;
        return false;
      }

      bool PDVD_PDMapAlg::isSensitiveToAr(size_t ch) const
      {
        return PDmap.at(ch)["sens_Ar"].get<bool>();
      }

      bool PDVD_PDMapAlg::isSensitiveToXe(size_t ch) const
      {
        return PDmap.at(ch)["sens_Xe"].get<bool>();
      }

      std::string PDVD_PDMapAlg::pdType(size_t ch) const
      {
        return PDmap.at(ch)["pd_type"];
      }

      double PDVD_PDMapAlg::Efficiency(size_t ch) const
      {
      return PDmap.at(ch)["eff"];
      }

      std::vector<PDVD_PDMapAlg::HardwareChannelEntry> PDVD_PDMapAlg::hardwareChannel(size_t ch) const
      {
        std::vector<HardwareChannelEntry> channels;

        if (PDmap.at(ch).contains("HardwareChannel"))
        {
          const auto& jsonArray = PDmap.at(ch).at("HardwareChannel");

          for (const auto& item : jsonArray)
          {
            HardwareChannelEntry entry;
            entry.slot = item["Slot"].get<int>();
            entry.link = item["Link"].get<int>();
            entry.daphne_channel = item["DaphneChannel"].get<int>();
            entry.offline_channel = item["OfflineChannel"].get<int>();

            channels.push_back(entry);
          }
        }

            return channels;
      }



      std::vector<int> PDVD_PDMapAlg::getChannelsOfType(std::string pdname) const
      {
        std::vector<int> out_ch_v;
        for (size_t ch = 0; ch < PDmap.size(); ch++) {
          if (PDmap.at(ch)["pd_type"] == pdname) out_ch_v.push_back(ch);
        }
        return out_ch_v;
      }

      auto PDVD_PDMapAlg::getChannelEntry(size_t ch) const
      {
        return PDmap.at(ch);
      }

      size_t PDVD_PDMapAlg::size() const
      {
        return PDmap.size();
      }

      // template<>
      nlohmann::json PDVD_PDMapAlg::getCollectionWithProperty(std::string property)
      {
        nlohmann::json subSetPDmap;
        std::copy_if (PDmap.begin(), PDmap.end(), std::back_inserter(subSetPDmap),
                      [property](const nlohmann::json e)->bool
                        {return e[property];} );
        return subSetPDmap;
      }





    DEFINE_ART_CLASS_TOOL(PDVD_PDMapAlg)
    }
