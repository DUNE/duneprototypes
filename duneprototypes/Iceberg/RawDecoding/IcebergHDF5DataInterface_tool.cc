// IcebergHDF5DataInterface_tool.cc

#include "IcebergHDF5DataInterface.h"
#include "TMath.h"
#include "TString.h"
#include <iostream>
#include <set>

#include "art/Framework/Services/Registry/ServiceHandle.h"

// dunecore and dunepdlegacy includes
#include "dunepdlegacy/Services/ChannelMap/IcebergChannelMapService.h"
#include "detdataformats/wib2/WIB2Frame.hpp"
#include "dunecore/DuneObj/DUNEHDF5FileInfo.h"
#include "dunecore/HDF5Utils/HDF5Utils.h"

IcebergHDF5DataInterface::IcebergHDF5DataInterface(fhicl::ParameterSet const& p)
{
  _min_offline_channel = p.get<long int>("MinOfflineChannel",-1);
  _max_offline_channel = p.get<long int>("MaxOfflineChannel",-1);
  _FileInfoLabel = p.get<std::string>("FileInfoLabel", "daq"),
    _debugprint = p.get<bool>("DebugPrint",false);
}

// wrapper for backward compatibility.  Return data for all APA's represented in the fragments on these labels

int IcebergHDF5DataInterface::retrieveData(art::Event &evt, 
                                           std::string inputLabel, 
                                           std::vector<raw::RawDigit> &raw_digits, 
                                           std::vector<raw::RDTimeStamp> &rd_timestamps,
                                           std::vector<raw::RDStatus> &rdstatuses)
{
  std::vector<int> apalist;
  apalist.push_back(-1);
  int retcode = retrieveDataAPAListWithLabels(evt, inputLabel, raw_digits, rd_timestamps, rdstatuses, apalist );
  _collectRDStatus(rdstatuses);
  return retcode;
}

// get data for specified APAs, without specifying an input label.  Since there are no input labels for HDF5 iceberg
// data, this method is just kept for interface compatibility


int IcebergHDF5DataInterface::retrieveDataForSpecifiedAPAs(art::Event &evt, 
                                                           std::vector<raw::RawDigit> &raw_digits, 
                                                           std::vector<raw::RDTimeStamp> &rd_timestamps,
                                                           std::vector<raw::RDStatus> &rdstatuses, 
                                                           std::vector<int> &apalist)
{
  std::string inputlabel = "dummy label";
  int totretcode = IcebergHDF5DataInterface::retrieveDataAPAListWithLabels(evt, 
                                                                           inputlabel, 
                                                                           raw_digits, 
                                                                           rd_timestamps,
                                                                           rdstatuses, 
                                                                           apalist);

  _collectRDStatus(rdstatuses);
  return totretcode;
}

// get data for a specific label, but only return those raw digits that correspond to APA's on the list
// input label is ignored.

int IcebergHDF5DataInterface::retrieveDataAPAListWithLabels(art::Event &evt, 
                                                            std::string, 
                                                            std::vector<raw::RawDigit> &raw_digits, 
                                                            std::vector<raw::RDTimeStamp> &rd_timestamps,
                                                            std::vector<raw::RDStatus> &rdstatuses, 
                                                            std::vector<int> &apalist)
{

  using namespace dune::HDF5Utils;
  auto infoHandle = evt.getHandle<raw::DUNEHDF5FileInfo>(_FileInfoLabel);
  const std::string & toplevel_groupname = infoHandle->GetEventGroupName();
  //const std::string & file_name = infoHandle->GetFileName();
  hid_t file_id = infoHandle->GetHDF5FileHandle();
  hid_t the_group = getGroupFromPath(file_id, toplevel_groupname);
  
  if (_debugprint)
    {
      std::cout << "Requested Data for " << apalist.size() << " APAs " << std::endl;
      std::cout << "Top level group name: " << toplevel_groupname << "  the_group " << the_group << std::endl;
    }

  // NOTE: The "apalist" that DataPrep hands to the method is always of size 1.
  // Also "apalist" should technically hand you the current APA No. we are looking at but there is exception.
  // CAUTION: This is only and only for ICEBERG, which has only one APA

  int apano = 0;

  getIcebergHDF5Data(the_group, raw_digits, rd_timestamps, apano);
      
  //Currently putting in dummy values for the RD Statuses
  rdstatuses.clear();
  rdstatuses.emplace_back(false, false, 0);

  _collectRDStatus(rdstatuses);
  
  return 0;
}


void IcebergHDF5DataInterface::_collectRDStatus(std::vector<raw::RDStatus> &rdstatuses)
{
  if (rdstatuses.size() < 2) return; 
  unsigned int statword=0;
  bool dcflag = false;
  bool kcflag = false;
  for (size_t i=0; i<rdstatuses.size(); ++i)
    {
      statword |= rdstatuses.at(i).GetStatWord();
      dcflag |= rdstatuses.at(i).GetCorruptDataDroppedFlag();
      kcflag |= rdstatuses.at(i).GetCorruptDataKeptFlag();
    }
  rdstatuses.clear();
  rdstatuses.emplace_back(dcflag,kcflag,statword);
}


// just one APA in Iceberg, so ignore the APA list.

// This is designed to read 1APA/CRU, only for VDColdBox data. The function uses "apano", handed by DataPrep,
// as an argument.
void IcebergHDF5DataInterface::getIcebergHDF5Data(
                                                  hid_t the_group, RawDigits& raw_digits, RDTimeStamps &timestamps,
                                                  int ) {
  using namespace dune::HDF5Utils;
  using dunedaq::fddetdataformats::WIB2Frame;
  //using dunedaq::detdataformats::wib2::Header;

  art::ServiceHandle<dune::IcebergChannelMapService> wireReadout;
  
  std::deque<std::string> det_types
    = getMidLevelGroupNames(the_group);

  for (const auto & det : det_types)
    {
      if (det != "TPC") continue;
      //std::cout << "  Detector type:  " << det << std::endl;
      hid_t geoGroup = getGroupFromPath(the_group, det);
      std::deque<std::string> apaNames
        = getMidLevelGroupNames(geoGroup);
      
      if (_debugprint)
        {
          std::cout << "Size of apaNames: " << apaNames.size() << std::endl;
          std::cout << "apaNames[0]: "  << apaNames[0] << std::endl;
        }

      // apaNames is a vector whose elements start at [0].
      hid_t linkGroup = getGroupFromPath(geoGroup, apaNames[0]);
      std::deque<std::string> linkNames
        = getMidLevelGroupNames(linkGroup);
      for (const auto & t : linkNames)
        {
          hid_t dataset = H5Dopen(linkGroup, t.data(), H5P_DEFAULT);
          hsize_t ds_size = H5Dget_storage_size(dataset);
          if (ds_size <= sizeof(FragmentHeader)) continue; //Too small
          
          std::vector<char> ds_data(ds_size);
          H5Dread(dataset, H5T_STD_I8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                  ds_data.data());
          H5Dclose(dataset);
          
          //Each fragment is a collection of WIB Frames
          Fragment frag(&ds_data[0], Fragment::BufferAdoptionMode::kReadOnlyMode);
          size_t n_frames = (ds_size - sizeof(FragmentHeader))/sizeof(WIB2Frame);
          if (_debugprint)
            {
              std::cout << "N_Frames calc: " << ds_size << " " << sizeof(FragmentHeader) << " " << sizeof(WIB2Frame) << " " << n_frames << std::endl;
            }
          std::vector<raw::RawDigit::ADCvector_t> adc_vectors(256);
          uint32_t slot = 0, fiber = 0, crate = 0;
          for (size_t i = 0; i < n_frames; ++i)
            {
              auto frame = reinterpret_cast<WIB2Frame*>(static_cast<uint8_t*>(frag.get_data()) + i*sizeof(WIB2Frame));
              for (size_t j = 0; j < adc_vectors.size(); ++j)
                {
		  adc_vectors[j].push_back(frame->get_adc(j));
                }
        
              if (i == 0)
                {
                  crate = frame->header.crate;
                  slot = frame->header.slot;
                  fiber = frame->header.link;
                }
            }
          if (_debugprint)
            {
              std::cout << "IcebergHDF5DataInterfaceTool: crate, slot, fiber: "  << crate << ", " << slot << ", " << fiber << std::endl;
            }
          for (size_t iChan = 0; iChan < 256; ++iChan)
            {
              const raw::RawDigit::ADCvector_t & v_adc = adc_vectors[iChan];
              //std::cout << "Channel: " << iChan << " N ticks: " << v_adc.size() << " Timestamp: " << frag.get_trigger_timestamp() << std::endl;

              uint32_t fiberloc = 0;
              if (fiber == 1) 
                {
                  fiberloc = 1;
                }
              else if (fiber == 2)
                {
                  fiberloc = 3;
                }
              size_t chloc = iChan;
              if (chloc > 127)
                {
                  chloc -= 128;
                  fiberloc++;
                }
              uint32_t crateloc = 0;
              uint32_t slotloc = slot;

              int offline_chan = wireReadout->GetOfflineNumberFromDetectorElements(crateloc, slotloc, fiberloc, chloc, dune::IcebergChannelMapService::kFELIX); 
              if (offline_chan < _min_offline_channel) continue;
              if (_max_offline_channel >= 0 && offline_chan > _max_offline_channel) continue;
              raw::RDTimeStamp rd_ts(frag.get_trigger_timestamp(), offline_chan);
              timestamps.push_back(rd_ts);
        
              float median = 0., sigma = 0.;
              computeMedianSigma(v_adc, median, sigma);
              raw::RawDigit rd(offline_chan, v_adc.size(), v_adc);
              rd.SetPedestal(median, sigma);
              raw_digits.push_back(rd);
            }
          
        }
      H5Gclose(linkGroup);
    }
}


// compute median and sigma.  

void IcebergHDF5DataInterface::computeMedianSigma(const raw::RawDigit::ADCvector_t &v_adc, float &median, float &sigma)
{
  size_t asiz = v_adc.size();
  int imed=0;
  if (asiz == 0)
    {
      median = 0;
      sigma = 0;
    }
  else
    {
      // the RMS includes tails from bad samples and signals and may not be the best RMS calc.

      imed = TMath::Median(asiz,v_adc.data()) + 0.01;  // add an offset to make sure the floor gets the right integer
      median = imed;
      sigma = TMath::RMS(asiz,v_adc.data());

      // add in a correction suggested by David Adams, May 6, 2019

      size_t s1 = 0;
      size_t sm = 0;
      for (size_t i=0; i<asiz; ++i)
        {
          if (v_adc[i] < imed) s1++;
          if (v_adc[i] == imed) sm++;
        }
      if (sm > 0)
        {
          float mcorr = (-0.5 + (0.5*(float) asiz - (float) s1)/ ((float) sm) );
          //if (std::abs(mcorr)>1.0) std::cout << "mcorr: " << mcorr << std::endl;
          median += mcorr;
        }
    }
}

DEFINE_ART_CLASS_TOOL(IcebergHDF5DataInterface)
