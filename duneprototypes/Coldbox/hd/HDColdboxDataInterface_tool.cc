#include "HDColdboxDataInterface.h"

#include <hdf5.h>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <cstring>
#include <string>
#include "TMath.h"
#include "TString.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "dunecore/DuneObj/DUNEHDF5FileInfo.h"
#include "dunecore/HDF5Utils/HDF5Utils.h"
#include "detdataformats/wib/WIBFrame.hpp"
#include "dunepdlegacy/Services/ChannelMap/PdspChannelMapService.h"



HDColdboxDataInterface::HDColdboxDataInterface(fhicl::ParameterSet const& p)
  : fForceOpen(p.get<bool>("ForceOpen", false)),
    fFileInfoLabel(p.get<std::string>("FileInfoLabel", "daq")),
    fMaxChan(p.get<int>("MaxChan",1000000)), 
    fDefaultCrate(p.get<unsigned int>("DefaultCrate", 3)),
    fDebugLevel(p.get<int>("DebugLevel",0))
{
}


int HDColdboxDataInterface::retrieveData(art::Event &evt, 
					 std::string inputLabel, 
					 std::vector<raw::RawDigit> &raw_digits, 
					 std::vector<raw::RDTimeStamp> &rd_timestamps,
					 std::vector<raw::RDStatus> &rdstatuses) {
  return 0;
}


int HDColdboxDataInterface::retrieveDataForSpecifiedAPAs(art::Event &evt,
                                                         std::vector<raw::RawDigit> &raw_digits,
                                                         std::vector<raw::RDTimeStamp> &rd_timestamps,
                                                         std::vector<raw::RDStatus> &rdstatuses,
                                                         std::vector<int> &apalist)
{
  using namespace dune::HDF5Utils;
  auto infoHandle = evt.getHandle<raw::DUNEHDF5FileInfo>(fFileInfoLabel);
  const std::string & toplevel_groupname = infoHandle->GetEventGroupName();
  const std::string & file_name = infoHandle->GetFileName();
  hid_t file_id = infoHandle->GetHDF5FileHandle();
  
  if (fDebugLevel > 0)
    {
      std::cout << "HDColdboxDataInterface : " << "HDF5 FileName: " << file_name << std::endl;
      std::cout << "HDColdboxDataInterface :" << "Top-Level Group Name: " << toplevel_groupname << std::endl;
    }

  // If the fcl file said to force open the file (i.e. because one is just running DataPrep), then open
  // but only if we are on a new file -- identified by if the handle stored in the event is different.
  if (fForceOpen && (file_id != fPrevStoredHandle))
    {
      fHDFFile = H5Fopen(file_name.data(), H5F_ACC_RDONLY, H5P_DEFAULT);
    } // If the handle is the same, fHDFFile won't change
  else if (!fForceOpen)
    {
      fHDFFile = file_id;
    }
  fPrevStoredHandle = file_id;
  
  hid_t the_group = getGroupFromPath(fHDFFile, toplevel_groupname);
  
  if (fDebugLevel > 0)
    {
      std::cout << "HDColdboxDataInterface : " <<  "Retrieving Data for " << apalist.size() << " APAs " << std::endl;
    }

  // NOTE: The "apalist" that DataPrep hands to the method is always of size 1.
  // Also "apalist" should technically hand you the current APA No. we are looking at but there is exception.
  // CAUTION: This is only and only for HDColdBox.The reason is VDColdBox has only one APA/CRU.
  for (const int & i : apalist)
    {
      int apano = i;
      if (fDebugLevel > 0)
	{
	  std::cout << "HDColdboxDataInterface :" << "apano: " << i << std::endl;
	}
 
      getFragmentsForEvent(the_group, raw_digits, rd_timestamps, apano, fMaxChan);
      
      //Currently putting in dummy values for the RD Statuses
      rdstatuses.clear();
      rdstatuses.emplace_back(false, false, 0);
    }
  
  return 0;
}


// get data for a specific label, but only return those raw digits that correspond to APA's on the list
int HDColdboxDataInterface::retrieveDataAPAListWithLabels( art::Event &evt, 
                                                           std::string inputLabel, 
                                                           std::vector<raw::RawDigit> &raw_digits, 
                                                           std::vector<raw::RDTimeStamp> &rd_timestamps,
                                                           std::vector<raw::RDStatus> &rdstatuses, 
                                                           std::vector<int> &apalist) 
{
  return 0;
}


// This is designed to read 1APA/CRU, only for VDColdBox data. The function uses "apano", handed by DataPrep,
// as an argument.
void HDColdboxDataInterface::getFragmentsForEvent(hid_t the_group, RawDigits& raw_digits, RDTimeStamps &timestamps,
						  int apano, unsigned int maxchan) 
{
  using namespace dune::HDF5Utils;
  using dunedaq::fddetdataformats::WIBFrame;
  using dunedaq::fddetdataformats::WIBHeader;
  
  art::ServiceHandle<dune::PdspChannelMapService> wireReadout;

  std::deque<std::string> det_types
    = getMidLevelGroupNames(the_group);

  for (const auto & det : det_types)
    {
      if (det != "TPC") continue;
      if (fDebugLevel > 0)
	{
	  std::cout << "HDColdboxDataInterface :"  << "Detector type:  " << det << std::endl;
	}
      hid_t geoGroup = getGroupFromPath(the_group, det);
      std::deque<std::string> apaNames
        = getMidLevelGroupNames(geoGroup);
      
      if (fDebugLevel > 0)
	{
	  std::cout << "HDColdboxDataInterface :" << "Size of apaNames: " << apaNames.size() << std::endl;
	  std::cout << "HDColdboxDataInterface :" << "apaNames[apano]: "  << apaNames[apano-1] << std::endl;
	}
      // apaNames is a vector whose elements start at [0].
      hid_t linkGroup = getGroupFromPath(geoGroup, apaNames[apano-1]);
      std::deque<std::string> linkNames = getMidLevelGroupNames(linkGroup);

      for (const auto & t : linkNames)
        {
          hid_t dataset = H5Dopen(linkGroup, t.data(), H5P_DEFAULT);
          hsize_t ds_size = H5Dget_storage_size(dataset);
          if (ds_size <= sizeof(FragmentHeader)) continue; //Too small
          
          std::vector<char> ds_data(ds_size);
          H5Dread(dataset, H5T_STD_I8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ds_data.data());
          H5Dclose(dataset);
          
          //Each fragment is a collection of WIB Frames
          Fragment frag(&ds_data[0], Fragment::BufferAdoptionMode::kReadOnlyMode);
          size_t n_frames = (ds_size - sizeof(FragmentHeader))/sizeof(WIBFrame);
	  if (fDebugLevel > 0)
	    {
	      std::cout << "HDColdboxDataInterface :" << "n_frames : " << n_frames << std::endl;
	    }
	  std::vector<raw::RawDigit::ADCvector_t> adc_vectors(256);
	  unsigned int slot = 0, fiber = 0;

          for (size_t i = 0; i < n_frames; ++i)
            {
	      auto frame = reinterpret_cast<WIBFrame*>(static_cast<uint8_t*>(frag.get_data()) + i*sizeof(WIBFrame));
	      if (fDebugLevel > 0)
		{
		  std::cout << "HDColdboxDataInterface :" << "frame : " << frame << std::endl;
		}
              for (size_t j = 0; j < adc_vectors.size(); ++j)
                {
                  adc_vectors[j].push_back(frame->get_channel(j));
                }
      	
              if (i == 0)
                {
                  slot = frame->get_wib_header()->slot_no;
                  fiber = frame->get_wib_header()->fiber_no;
                }
            }
	  if (fDebugLevel > 0)
	    {
	      std::cout << "HDColdboxDataInterface :" << "slot, fiber: "  << slot << ", " << fiber << std::endl;
	    }
          for (size_t iChan = 0; iChan < 256; ++iChan)
            {
              const raw::RawDigit::ADCvector_t & v_adc = adc_vectors[iChan];
	      if (fDebugLevel > 0)
		{
		  std::cout << "HDColdboxDataInterface : " << "Channel: " << iChan << " N ticks: " << v_adc.size() << " Timestamp: " << frag.get_trigger_timestamp() << std::endl;
		}
	      // handle 256 channels on two fibers -- use the channel map that assumes 128 chans per fiber (=FEMB)
	      // Channels 0-127 are on "fiberloc" 1 and channels 128-255 are on fiberloc 2.
	      // Use separate variables, for example, "fiberloc" and "chloc" to keep track of the actual channel and fiber and to accommodate future needs.
	      unsigned int fiberloc = 0;
	      if (fiber == 1) 
		{
		  fiberloc = 1;
		}

	      else if (fiber == 2)
		{
		  fiberloc = 3;
		}
	      else
		{
		  MF_LOG_WARNING("_process_FELIX_AUX:") << " Fiber number " << (int) fiber << " is expected to be 1 or 2 -- revisit logic";
		  fiberloc = 1;
		}

	      unsigned int chloc = iChan;
	      if (chloc > 127)
		{
		  chloc -= 128;
		  fiberloc++;
		}

	      //In the channel map call, the crate number is ill-defined for the HD coldbox, as there is only one crate, and the dataprep and event display have room for six.  Pick Default Crate number 3 to send in to the call
	      unsigned int offline_chan = wireReadout->GetOfflineNumberFromDetectorElements(fDefaultCrate, slot, fiberloc, chloc, dune::PdspChannelMapService::kFELIX);
	      if (fDebugLevel > 0)
		{
		  std::cout << "HDColdboxDataInterface : " << "iChan : " << iChan << std::endl;
		  std::cout << "HDColdboxDataInterface : " << "offline_chan  : " << offline_chan << std::endl;
		}
              if (offline_chan < 0) continue;
	      if (offline_chan > maxchan) continue;
	      raw::RDTimeStamp rd_ts(frag.get_trigger_timestamp(), offline_chan);
              timestamps.push_back(rd_ts);
	      
              float median = 0., sigma = 0.;
              getMedianSigma(v_adc, median, sigma);
	      raw::RawDigit rd(offline_chan, v_adc.size(), v_adc);
              rd.SetPedestal(median, sigma);
              raw_digits.push_back(rd);
            }
          
        }
      H5Gclose(linkGroup);
    }
  
}


void HDColdboxDataInterface::getMedianSigma(
					    const raw::RawDigit::ADCvector_t &v_adc, float &median,
					    float &sigma) {
  size_t asiz = v_adc.size();
  int imed=0;
  if (asiz == 0) {
    median = 0;
    sigma = 0;
  }
  else {
    // the RMS includes tails from bad samples and signals and may not be the best RMS calc.

    imed = TMath::Median(asiz,v_adc.data()) + 0.01;  // add an offset to make sure the floor gets the right integer
    median = imed;
    sigma = TMath::RMS(asiz,v_adc.data());

    // add in a correction suggested by David Adams, May 6, 2019

    size_t s1 = 0;
    size_t sm = 0;
    for (size_t i = 0; i < asiz; ++i) {
      if (v_adc.at(i) < imed) s1++;
      if (v_adc.at(i) == imed) sm++;
    }
    if (sm > 0) {
      float mcorr = (-0.5 + (0.5*(float) asiz - (float) s1)/ ((float) sm) );
      if (fDebugLevel > 0)
	{
	  if (std::abs(mcorr)>1.0) std::cout << "mcorr: " << mcorr << std::endl;
	}
      median += mcorr;
    }
  }
  
}

DEFINE_ART_CLASS_TOOL(HDColdboxDataInterface)
