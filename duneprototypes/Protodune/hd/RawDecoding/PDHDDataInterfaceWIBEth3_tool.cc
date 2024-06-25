// very similar input tool to PDHDDataInterfaceWIBEth but uses the HDF5RawFile3Service instead of
// HDF5RawFile2Service.  This is needed because of a data format change on April 23, 2024 when
// moving to the DUNE-DAQ 4.4.0 release

#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <cstring>
#include <string>
#include "TMath.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "dunecore/DuneObj/DUNEHDF5FileInfo2.h"
#include "dunecore/HDF5Utils/HDF5RawFile3Service.h"
#include "detdataformats/wibeth/WIBEthFrame.hpp"
#include "duneprototypes/Protodune/hd/ChannelMap/PD2HDChannelMapService.h"
#include "dunecore/DuneObj/PDSPTPCDataInterfaceParent.h"

class PDHDDataInterfaceWIBEth3 : public PDSPTPCDataInterfaceParent {

private:

  std::map<int,std::vector<std::string>> _input_labels_by_apa;

  //For nicer log syntax
  std::string logname = "PDHDDataInterfaceWIBEth3";
  std::string fFileInfoLabel;

  unsigned int fMaxChan = 1000000;  // no maximum for now
  unsigned int fDefaultCrate = 1;
  int fDebugLevel = 0;   // switch to turn on debugging printout
  std::string fSubDetectorString;  // two values seen in the data:  HD_TPC and VD_Bottom_TPC
  typedef std::vector<raw::RawDigit> RawDigits;
  typedef std::vector<raw::RDTimeStamp> RDTimeStamps;
  typedef std::vector<raw::RDStatus> RDStatuses;

public:

  explicit PDHDDataInterfaceWIBEth3(fhicl::ParameterSet const& p)
    : fFileInfoLabel(p.get<std::string>("FileInfoLabel", "daq")),
      fMaxChan(p.get<int>("MaxChan",1000000)),
      fDefaultCrate(p.get<unsigned int>("DefaultCrate", 1)),
      fDebugLevel(p.get<int>("DebugLevel",0)),
      fSubDetectorString(p.get<std::string>("SubDetectorString","HD_TPC"))
  { }


  // wrapper for backward compatibility.  Return data for all APA's represented 
  // in the fragments on these labels
  int retrieveData(art::Event &evt,
		   std::string inputLabel,
		   std::vector<raw::RawDigit> &raw_digits,
		   std::vector<raw::RDTimeStamp> &rd_timestamps,
		   std::vector<raw::RDStatus> &rdstatuses) override
  {
    return 0;
  }


  int retrieveDataForSpecifiedAPAs(art::Event &evt,
				   std::vector<raw::RawDigit> &raw_digits,
				   std::vector<raw::RDTimeStamp> &rd_timestamps,
				   std::vector<raw::RDStatus> &rdstatuses,
				   std::vector<int> &apalist) override
  {
    auto infoHandle = evt.getHandle<raw::DUNEHDF5FileInfo2>(fFileInfoLabel);
    const std::string & file_name = infoHandle->GetFileName();
    uint32_t runno = infoHandle->GetRun();
    size_t   evtno = infoHandle->GetEvent();
    size_t   seqno = infoHandle->GetSequence();

    dunedaq::hdf5libs::HDF5RawDataFile::record_id_t rid = std::make_pair(evtno, seqno);

    if (fDebugLevel > 0)
      {
	std::cout << logname << " HDF5 FileName: " << file_name << std::endl;
	std::cout << logname << " Run:Event:Seq: " << std::dec << runno << ":" << evtno << ":" << seqno << std::endl;
	std::cout << logname << " : " <<  "Retrieving Data for " << apalist.size() << " APAs " << std::endl;
      }
  
    for (const int & i : apalist)
      {
	int apano = i;
	if (fDebugLevel > 0)
	  {
	    std::cout << logname << " Tool called with requested APA:" << "apano: " << i << std::endl;
	  }

	getFragmentsForEvent(rid, raw_digits, rd_timestamps, apano, rdstatuses);
      }

    return 0;
  }

  // get data for APAs on the list.  Retrieve the HDF5 raw file pointer from the HDF5RawFile2Service

  int retrieveDataAPAListWithLabels( art::Event &evt,
				     std::string inputLabel,
				     std::vector<raw::RawDigit> &raw_digits,
				     std::vector<raw::RDTimeStamp> &rd_timestamps,
				     std::vector<raw::RDStatus> &rdstatuses,
				     std::vector<int> &apalist) override
  {
    return 0;
  }


  // This is designed to get data from one APA. 
  void getFragmentsForEvent(dunedaq::hdf5libs::HDF5RawDataFile::record_id_t &rid,
                            RawDigits& raw_digits,
                            RDTimeStamps &timestamps,
                            int apano,
                            RDStatuses & rdstatuses)
  {
    using dunedaq::fddetdataformats::WIBEthFrame;
    art::ServiceHandle<dune::PD2HDChannelMapService> channelMap;
    art::ServiceHandle<dune::HDF5RawFile3Service> rawFileService;
    auto rf = rawFileService->GetPtr();
    auto sourceids = rf->get_source_ids(rid);
    for (const auto &source_id : sourceids)  
      {
	// only want detector readout data (i.e. not trigger info)
	if (source_id.subsystem != dunedaq::daqdataformats::SourceID::Subsystem::kDetectorReadout) continue;

	// look through the geo IDs and see if we are in the right crate
	bool has_desired_apa = false;
	auto gids = rf->get_geo_ids_for_source_id(rid, source_id);
	for (const auto &gid : gids)
	  {
	    if (fDebugLevel > 1)
	      {
		std::cout << logname << " Tool Geoid: " << std::hex << gid << std::dec << std::endl;
	      }
	    uint16_t detid = 0xffff & gid;
	    dunedaq::detdataformats::DetID::Subdetector detidenum = static_cast<dunedaq::detdataformats::DetID::Subdetector>(detid);
	    auto subdetector_string = dunedaq::detdataformats::DetID::subdetector_to_string(detidenum);
	    if (fDebugLevel > 1)
	      {
		std::cout << logname << " Tool subdetector string: " << subdetector_string << std::endl;
		std::cout << logname << " Tool looking for subdet: " << fSubDetectorString << std::endl;
	      }
	  
	    if (subdetector_string == fSubDetectorString)
	      {
		uint16_t crate_from_geo = 0xffff & (gid >> 16);
		if (fDebugLevel > 1)
		  {
		    std::cout << "crate from geo: " << crate_from_geo << std::endl;
		    uint16_t slot_from_geo = 0xffff & (gid >> 32);
		    std::cout << "slot from geo: " << slot_from_geo << std::endl;
		    uint16_t stream_from_geo = 0xffff & (gid >> 48);
		    std::cout << "stream from geo: " << stream_from_geo << std::endl;
		  }
		  

		if (-1 == apano)
		  {
		    has_desired_apa = true;
		    if (fDebugLevel > 1)
		      {
			std::cout << "assume desired APA, please use with caution" << std::endl;
		      }
		    break;
		  }

		if (crate_from_geo == apano)
		  {
		    has_desired_apa = true;
		    if (fDebugLevel > 1)
		      {
			std::cout << "found desired APA" << std::endl;
		      }
		    break;
		  }
	      }
	  }
	if (has_desired_apa)
	  {
	    // this reads the relevant dataset and returns a std::unique_ptr.  Memory is released when 
	    // it goes out of scope.
 
	    auto frag = rf->get_frag_ptr(rid, source_id);
	    auto frag_size = frag->get_size();
            auto frag_timestamp = frag->get_trigger_timestamp();
            auto frag_window_begin = frag->get_window_begin();
            auto frag_window_end = frag->get_window_end();

            auto total_wib_ticks = std::lround(
                (frag_window_end - frag_window_begin)*16./512.
            );

	    size_t fhs = sizeof(dunedaq::daqdataformats::FragmentHeader);
	    if (frag_size <= fhs) continue; // Too small to even have a header
	    size_t n_frames = (frag_size - fhs)/sizeof(WIBEthFrame);
	    if (fDebugLevel > 0)
	      {
		std::cout << "n_frames calc.: " << frag_size << " " << fhs << " " << sizeof(WIBEthFrame) << " " << n_frames << std::endl;
	      }

	    std::vector<raw::RawDigit::ADCvector_t> adc_vectors(64);   // 64 channels per WIBEth frame
	    unsigned int slot = 0, link = 0, crate = 0, stream = 0, locstream = 0;
          
            //We expect to have extra wib ticks, so figure out how many
            //in total we cut out.
            auto leftover_wib_ticks = n_frames*64 - total_wib_ticks;
            uint64_t latest_time = 0;

            //For bookkeeping if we need to reorder
            std::vector<std::pair<uint64_t, size_t>> timestamp_indices;

            //This will track if we see any problems
            bool any_bad = false;

            //For reordering
            std::vector<std::vector<raw::RawDigit::ADCvector_t>> temp_adcs;
	    for (size_t i = 0; i < n_frames; ++i)
	      {
                //Makes a 64-channel wide vector
                temp_adcs.emplace_back(64);

                std::bitset<8> condition;
		if (fDebugLevel > 2)
		  {
		    // dump WIB frames in binary
		    std::cout << "Frame number: " << i << std::endl;
		    size_t wfs32 = sizeof(WIBEthFrame)/4;
		    uint32_t *fdp = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(frag->get_data()) + i*sizeof(WIBEthFrame));
		    std::cout << std::dec;
		    for (size_t iwdt = 0; iwdt < std::min(wfs32, (size_t) 4); iwdt++)  // dumps just the first 4 words.  use wfs32 if you want them all
		      {
			std::cout << iwdt << " : 10987654321098765432109876543210" << std::endl;
			std::cout << iwdt << " : " << std::bitset<32>{fdp[iwdt]} << std::endl;
		      }
		    std::cout << std::dec;
		  }

		auto frame = reinterpret_cast<WIBEthFrame*>(static_cast<uint8_t*>(frag->get_data()) + i*sizeof(WIBEthFrame));

                //Get the timestamps from the WIB Frames.
                //Best practice is to ensure that the
                //timestamps are consistent with the Trigger timestamp

                //Jake Calcutt -- per Roger Huang on Slack:
                //"It would be a good check to put in. If they ever don't match,
                //the recommended action is to mark the data as bad"
                auto link0_timestamp = frame->header.colddata_timestamp_0;
                auto link1_timestamp = frame->header.colddata_timestamp_1;
                auto frame_timestamp = frame->get_timestamp();
                auto frame_size = 64*512/16;

                if (fDebugLevel > 0) {
                  std::cout << "Frame " << i << " timestamps:" <<
                               "\n\tlink0: " << link0_timestamp <<
                               "\n\tlink1: " << link1_timestamp <<
                               "\n\tmaster:" << frame_timestamp <<
                               "\n\tw_begin: " << frag_window_begin <<
                               "\n\ttrigger: " << frag_timestamp <<
                               "\n\tw_end: " << frag_window_end << std::endl;
                }

                //If this is non-zero, mark bad 
                bool frame_good = (frame->header.crc_err == 0);
                condition[0] = !(frame->header.crc_err == 0);

                //These should be self-consistent
                frame_good &= (link0_timestamp == link1_timestamp);
                condition[1] = !(link0_timestamp == link1_timestamp);
                //Lower 15 bits of the timestamps should match the "master" timestamPp
                frame_good &= (link0_timestamp == (frame_timestamp & 0x7FFF));
                condition[2] = !(link0_timestamp == (frame_timestamp & 0x7FFF));
                //We shouldn't have a frame that is entirely outside of the readout window
                //(64 ticks x 512 ns per tick)/16ns ticks before the fragment window
                frame_good &= ((frame_timestamp + frame_size) > frag_window_begin);
                condition[3] = !((frame_timestamp + frame_size) > frag_window_begin);
                frame_good &= (frame_timestamp < frag_window_end);
                condition[4] = !(frame_timestamp < frag_window_end);

                //If one frame's bad, make note
                any_bad |= !frame_good;

                //Should also check that none of the frames come out of order
                //TODO -- figure out a way to order them if not good
                if (frame_timestamp < latest_time) {
                  std::cout << "Frame " << i <<
                               " is earlier than the so-far latest time " <<
                               latest_time << std::endl;
                }
                else if (frame_timestamp == latest_time) {
                  std::cout << "Frame " << i <<
                               " is same as the so-far latest time " <<
                               latest_time << std::endl;
                }
                else {
                  latest_time = frame_timestamp;
                }

                //Store the frame_timestamp and the index
                timestamp_indices.emplace_back(frame_timestamp, i);

                //Determine if we're in the first frame
                bool first_frame = (frag_window_begin > frame_timestamp);
                int start_tick = 0;
                if (first_frame) {
                  //Turn these into doubles so we can go negative
                  start_tick = std::lround(
                      (frag_window_begin*16./512 - frame_timestamp*16./512.)
                  );
                  if (fDebugLevel > 0)
                    std::cout << "\tFirst frame. Start tick:" << start_tick << std::endl;

                  if (i != 0) {
                    std::cout << "WARNING. FIRST FRAME BY TIME, BUT NOT BY ITERATION" << std::endl;
                  }
                  leftover_wib_ticks -= start_tick;
                }

		int adcvs = adc_vectors.size();  // convert to int
                int last_tick = 64;
                //if the readout time is past the frame, don't change anything
                //if frame is past readout time, determine where to stop
                if (frame_timestamp + 512.*64/16 > frag_window_end) {
                  //Account for the ticks at the front
                  last_tick -= leftover_wib_ticks;
                  if (fDebugLevel > 0)
                    std::cout << "Last frame. last tick: " << last_tick << std::endl;
                }

		for (int jChan = 0; jChan < adcvs; ++jChan)   // these are ints because get_adc wants ints.
		  {
		    for (int kSample = start_tick; kSample < last_tick; ++kSample)
		      {
			adc_vectors[jChan].push_back(frame->get_adc(jChan,kSample));
                        temp_adcs.back()[jChan].push_back(frame->get_adc(jChan,kSample));
		      }
		  }
              
		if (i == 0)
		  {
		    crate = frame->daq_header.crate_id;
		    slot = frame->daq_header.slot_id;
		    stream = frame->daq_header.stream_id;

		    // local copy of the stream number -- change 0:3 & 64:67 to a single 0:3 number locstream
		    // and set the link number
		    // to be zero for stream from 0:3 and 1 for streams 64:67
		    // n.b. locstream goes from 0 to 3 twice
		  
		    locstream = stream & 0x3;
		    link = (stream >> 6) & 1;

		  }
	      }
	    if (fDebugLevel > 0)
	      {
		std::cout << "PDHDDataInterfaceToolWIBEth: crate, slot, link: "  << crate << ", " << slot << ", " << link << std::endl;
		std::cout << "PDHDDataInterfaceToolWIBEth: stream, locstream: " << stream << ", " << locstream << std::endl;
	      }

            
            //Copy the vector to see if it was reordered
            auto unordered = timestamp_indices;

            //Sort the indices according to the timestamp
            std::sort(timestamp_indices.begin(), timestamp_indices.end(),
                      [](const auto & a, const auto & b)
                          {return a.first < b.first;});

            //Check if any value is different
            bool reordered = false;
            for (size_t i = 0; i < timestamp_indices.size(); ++i) {
              reordered |= (timestamp_indices[i] != unordered[i]);
            }

            //If we need to reorder, go through and correct the adcs
            if (reordered) {
              std::cout << "Sorted: " << std::endl;

              //Use this to move through full adc vectors in increments of
              //the frame sizes
              size_t sample_start = 0;

              //Loop over frame in correct order
              for (size_t i = 0; i < timestamp_indices.size(); ++i) {
                const auto & ti = timestamp_indices[i];
                const auto & u = unordered[i];
                std::cout << "\t" << ti.first << " " << ti.second <<
                             " " << u.first << " " << u.second << std::endl;

                //Get the next frame
                auto & this_adcs = temp_adcs[ti.second];
                if (this_adcs.empty()) {
                  throw cet::exception("PDHDDataInterfaceWIBEth3_tool.cc") <<
                      "Somehow the reordering vector is empty at index " <<
                      ti.second;
                }

                //Use first one -- should be safe because of above exception
                size_t frame_samples = this_adcs[0].size();
                //Go over channels in this frame
                for (size_t jChan = 0; jChan < this_adcs.size(); ++jChan) {
                  //For this channel, look over the samples
                  for (size_t kSample = 0; kSample < frame_samples; ++kSample) {
                    //And set the corresponding one in the output vector
                    adc_vectors[jChan][kSample + sample_start] = this_adcs[jChan][kSample];
                  }
                }
                //Move forward in the output vector by the length of this frame
                sample_start += frame_samples;
              }
            }

	    for (size_t iChan = 0; iChan < 64; ++iChan)
	      {
		const raw::RawDigit::ADCvector_t & v_adc = adc_vectors[iChan];

		uint32_t slotloc = slot;
		slotloc &= 0x7;

		size_t wibframechan = iChan + 64*locstream; 

		auto hdchaninfo = channelMap->GetChanInfoFromWIBElements (crate, slotloc, link, wibframechan);
		if (fDebugLevel > 2)
		  {
		    std::cout << "PDHDDataInterfaceToolWIBEth: wibframechan, valid: " << wibframechan << " " << hdchaninfo.valid << std::endl;
		  }
		if (!hdchaninfo.valid) continue;

		unsigned int offline_chan = hdchaninfo.offlchan;
		if (offline_chan > fMaxChan) continue;

		raw::RDTimeStamp rd_ts(frag->get_trigger_timestamp(), offline_chan);
		timestamps.push_back(rd_ts);

		float median = 0., sigma = 0.;
		getMedianSigma(v_adc, median, sigma);
		raw::RawDigit rd(offline_chan, v_adc.size(), v_adc);
		rd.SetPedestal(median, sigma);
		raw_digits.push_back(rd);

                //Add a status so we can tell if it's bad or not
                //
                //Constructor is (corrupt_data_dropped, corrupt_data_kept, statword)
                //We're not dropping, so first is false
                //If any frame is NOT GOOD or are out of order
                //then make the corrupt_data_kep flag true
                //
                //Finally make a statword to describe what happened.
                //For now: any bad, set first (binary) digit 
                //         if needs to be reodered, set second digit
                std::bitset<2> statword;
                statword[0] = (any_bad ? 1 : 0);
                statword[1] = (reordered ? 1 : 0);
                rdstatuses.emplace_back(false,
                                        statword.any(),
                                        statword.to_ulong());
	      }
	  }
      }
    if (fDebugLevel > 0)
      {
	std::cout << "PDHDDataInterfaceToolWIBEth: number of raw digits found: "  << raw_digits.size() << std::endl;
      }
  }

  void getMedianSigma(const raw::RawDigit::ADCvector_t &v_adc, float &median,
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
};

  DEFINE_ART_CLASS_TOOL(PDHDDataInterfaceWIBEth3)
