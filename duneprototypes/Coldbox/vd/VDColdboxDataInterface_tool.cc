#include "VDColdboxDataInterface.h"

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
#include "duneprototypes/Coldbox/vd/ChannelMap/VDColdboxChannelMapService.h"


//The file handle is from the raw::DUNEHDF5FileInfo data product that the source puts into the event. Art's getHandle<type> is usedto retrieve a data product from the event.  
// The idea is hand this function an art event and it will return you APA/CRU info FOR THE VDColdbox.

void readFragmentsForEvent (art::Event &evt)
{

  using namespace dune::HDF5Utils;
  auto infoHandle = evt.getHandle <raw::DUNEHDF5FileInfo> ("daq");
  std::cout << "Got infos? " << infoHandle << std::endl;
  
  const std::string & toplevel_groupname = infoHandle->GetEventGroupName();
  const std::string & file_name = infoHandle->GetFileName();
  hid_t file_id = infoHandle->GetHDF5FileHandle();

  std::cout << "Top-Level Group Name: " << toplevel_groupname << std::endl;
  std::cout << "HDF5 FileName: " << file_name << std::endl;
  
  // now look inside those "Top-Level Group Name" for "Detector type".
  hid_t requestedGroup = getGroupFromPath(file_id, toplevel_groupname);

  std::deque<std::string> detectorTypeNames = getMidLevelGroupNames(requestedGroup);
  
  for (auto& detectorTypeName : detectorTypeNames)
    {
      if (detectorTypeName == "TPC" && detectorTypeName != "TriggerRecordHeader")
	{
	  std::cout << "  Detector type: " << detectorTypeName << std::endl;
	  std::string geoPath = toplevel_groupname + "/" + detectorTypeName;
	  hid_t geoGroup = getGroupFromPath(file_id,geoPath);
	  std::deque<std::string> apaNames = getMidLevelGroupNames(geoGroup);
	  
	  // loop over APAs
	  for (auto& apaName : apaNames)
	    {
	      std::string apaGroupPath = geoPath + "/" + apaName;
	      std::cout << "     Geo path: " << apaGroupPath << std::endl;
	      hid_t linkGroup = getGroupFromPath(file_id,apaGroupPath);
	      std::deque<std::string> linkNames = getMidLevelGroupNames(linkGroup);
	      
	      // loop over Links
	      for (auto& linkName : linkNames)
		{
		  std::string dataSetPath = apaGroupPath + "/" + linkName;
		  std::cout << "      Data Set Path: " << dataSetPath << std::endl;
		  hid_t datasetid = H5Dopen(linkGroup,linkName.data(),H5P_DEFAULT);
		  hsize_t ds_size = H5Dget_storage_size(datasetid);
		  std::cout << "      Data Set Size (bytes): " << ds_size << std::endl;
		  
		  if (ds_size < 80) continue;
		  
		  size_t narray = ds_size / sizeof(char);
		  size_t rdr = ds_size % sizeof(char);
		  if (rdr > 0 || narray == 0) narray++;
		  char *ds_data = new char[narray];
		  herr_t ecode = H5Dread(datasetid, H5T_STD_I8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ds_data);
		  int firstbyte = ds_data[0];
		  firstbyte &= 0xFF;
		  int lastbyte = ds_data[narray-1];
		  lastbyte &= 0xFF;
		  
		  std::cout << std::hex << "      Retrieved data: ecode: " << ecode << "  first byte: " << firstbyte
			    << " last byte: " << lastbyte  << std::dec << std::endl;
		  

		  int magic_word = 0;
		  memcpy(&magic_word,&ds_data[0],4);
		  std::cout << "   Magic word: 0x" << std::hex << magic_word << std::dec << std::endl;
		  
		  int version = 0;
		  memcpy(&version, &ds_data[4],4);
		  std::cout << "   Version: " << std::dec << version << std::dec << std::endl;
		  
		  uint64_t fragsize=0;
		  memcpy(&fragsize, &ds_data[8],8);
		  std::cout << "   Frag Size: " << std::dec << fragsize << std::dec << std::endl;
		  
		  uint64_t trignum=0;
		  memcpy(&trignum, &ds_data[16],8);
		  std::cout << "   Trig Num: " << std::dec << trignum << std::dec << std::endl;
		  
		  uint64_t trig_timestamp=0;
		  memcpy(&trig_timestamp, &ds_data[24],8);
		  std::cout << "   Trig Timestamp: " << std::dec << trig_timestamp << std::dec << std::endl;
		  
		  uint64_t windowbeg=0;
		  memcpy(&windowbeg, &ds_data[32],8);
		  std::cout << "   Window Begin:   " << std::dec << windowbeg << std::dec << std::endl;
		  
		  uint64_t windowend=0;
		  memcpy(&windowend, &ds_data[40],8);
		  std::cout << "   Window End:     " << std::dec << windowend << std::dec << std::endl;
		  
		  int runno=0;
		  memcpy(&runno, &ds_data[48], 4);
		  std::cout << "   Run Number: " << std::dec << runno << std::endl;
		  
		  int errbits=0;
		  memcpy(&errbits, &ds_data[52], 4);
		  std::cout << "   Error bits: " << std::dec << errbits << std::endl;
		  
		  int fragtype=0;
		  memcpy(&fragtype, &ds_data[56], 4);
		  std::cout << "   Fragment type: " << std::dec << fragtype << std::endl;
		  
		  int fragpadding=0;
		  memcpy(&fragtype, &ds_data[60], 4);
		  std::cout << "   Fragment padding: " << std::dec << fragpadding << std::endl;
		  
		  int geoidversion=0;
		  memcpy(&geoidversion, &ds_data[64], 4);
		  std::cout << "   GeoID version: " << std::dec << geoidversion << std::endl;
		  
		  unsigned short geoidtype;
		  memcpy(&geoidtype, &ds_data[70], 1);
		  std::cout << "   GeoID type: " << geoidtype << std::endl;
		  
		  unsigned short geoidregion=0;
		  memcpy(&geoidregion, &ds_data[71], 1);
		  std::cout << "   GeoID region: " << std::dec << geoidregion << std::endl;
		  
		  int geoidelement=0;
		  memcpy(&geoidelement, &ds_data[72], 4);
		  std::cout << "   GeoID element: " << std::dec << geoidelement << std::endl;
		  
		  int geoidpadding=0;
		  memcpy(&geoidpadding, &ds_data[76], 4);
		  std::cout << "   GeoID padding: " << std::dec << geoidpadding << std::endl;
		  
		  delete[] ds_data;  // free up memory

		} 
	    }
	}
    }

}

// Keep this here for now. This needs scrutiny regarding whether the input labels are fetching right values.
VDColdboxDataInterface::VDColdboxDataInterface(fhicl::ParameterSet const& p)
  : fForceOpen(p.get<bool>("ForceOpen", false)),
    fFileInfoLabel(p.get<std::string>("FileInfoLabel", "daq")),
    fMaxChan(p.get<int>("MaxChan",1000000)) {
}


int VDColdboxDataInterface::retrieveData(art::Event &evt, 
					 std::string inputLabel, 
					 std::vector<raw::RawDigit> &raw_digits, 
					 std::vector<raw::RDTimeStamp> &rd_timestamps,
					 std::vector<raw::RDStatus> &rdstatuses) {
  return 0;
}


int VDColdboxDataInterface::retrieveDataForSpecifiedAPAs(art::Event &evt,
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
  
  std::cout << "HDF5 FileName: " << file_name << std::endl;
  std::cout << "Top-Level Group Name: " << toplevel_groupname << std::endl;
  
  //If the fcl file said to force open the file
  //(i.e. because one is just running DataPrep), then open
  //but only if we are on a new file -- identified by if the handle
  //stored in the event is different
  if (fForceOpen && (file_id != fPrevStoredHandle))
    {
      std::cout << "Opening" << std::endl;
      fHDFFile = H5Fopen(file_name.data(), H5F_ACC_RDONLY, H5P_DEFAULT);
    }//If the handle is the same, fHDFFile won't change
  else if (!fForceOpen)
    {
      fHDFFile = file_id;
    }
  fPrevStoredHandle = file_id;
  
  hid_t the_group = getGroupFromPath(fHDFFile, toplevel_groupname);
  
  std::cout << "Retrieving Data for " << apalist.size() << " APA " << std::endl;
  
  // NOTE: The "apalist" that DataPrep hands to the method is always of size 1.
  // Also "apalist" should technically hand you the current APA No. we are looking at but there is exception.
  // CAUTION: This is only and only for VDColdBox.The reason is VDColdBox has only one APA/CRU.
  for (const int & i : apalist)
    {
      int apano = i;
      std::cout << "apano: " << i << std::endl;

      getFragmentsForEvent(the_group, raw_digits, rd_timestamps, apano, fMaxChan);
      
      //Currently putting in dummy values for the RD Statuses
      rdstatuses.clear();
      rdstatuses.emplace_back(false, false, 0);
    }
  
  return 0;
}


// get data for a specific label, but only return those raw digits that correspond to APA's on the list
int VDColdboxDataInterface::retrieveDataAPAListWithLabels(
    art::Event &evt, 
    std::string inputLabel, 
    std::vector<raw::RawDigit> &raw_digits, 
    std::vector<raw::RDTimeStamp> &rd_timestamps,
    std::vector<raw::RDStatus> &rdstatuses, 
    std::vector<int> &apalist) {
  return 0;
}


// This is designed to read 1APA/CRU, only for VDColdBox data. The function uses "apano", handed by DataPrep,
// as an argument.
void VDColdboxDataInterface::getFragmentsForEvent(
    hid_t the_group, RawDigits& raw_digits, RDTimeStamps &timestamps,
    int apano, int maxchan) {

  using namespace dune::HDF5Utils;
  using dunedaq::fddetdataformats::WIBFrame;
  using dunedaq::fddetdataformats::WIBHeader;

  art::ServiceHandle<dune::VDColdboxChannelMapService> wireReadout;
  
  std::deque<std::string> det_types
    = getMidLevelGroupNames(the_group);

  for (const auto & det : det_types)
    {
      if (det != "TPC") continue;
      //std::cout << "  Detector type:  " << det << std::endl;
      hid_t geoGroup = getGroupFromPath(the_group, det);
      std::deque<std::string> apaNames
        = getMidLevelGroupNames(geoGroup);
      
      std::cout << "Size of apaNames: " << apaNames.size() << std::endl;
      std::cout << "apaNames[apano]: "  << apaNames[apano-1] << std::endl;
      
      // apaNames is a vector whose elements start at [0].
      hid_t linkGroup = getGroupFromPath(geoGroup, apaNames[apano-1]);
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
          size_t n_frames = (ds_size - sizeof(FragmentHeader))/sizeof(WIBFrame);
          std::vector<raw::RawDigit::ADCvector_t> adc_vectors(256);
          uint32_t slot = 0, fiber = 0;
          for (size_t i = 0; i < n_frames; ++i)
            {
              auto frame = reinterpret_cast<WIBFrame*>(static_cast<uint8_t*>(frag.get_data()) + i*sizeof(WIBFrame));
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
          //std::cout << "slot, fiber: "  << slot << ", " << fiber << std::endl;
          for (size_t iChan = 0; iChan < 256; ++iChan)
            {
              const raw::RawDigit::ADCvector_t & v_adc = adc_vectors[iChan];
      	//std::cout << "Channel: " << iChan << " N ticks: " << v_adc.size() << " Timestamp: " << frag.get_trigger_timestamp() << std::endl;

              int offline_chan = wireReadout->getOfflChanFromSlotFiberChan(slot, fiber, iChan);
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


void VDColdboxDataInterface::getMedianSigma(
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
      //if (std::abs(mcorr)>1.0) std::cout << "mcorr: " << mcorr << std::endl;
      median += mcorr;
    }
  }
  
}

DEFINE_ART_CLASS_TOOL(VDColdboxDataInterface)
