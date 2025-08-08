// very similar input tool to PDHDDataInterfaceWIBEth3 -- copies the same input algorithms for BDE
// TDE decoder code added in 2025.

#include "TMath.h"
#include <cstring>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <string>

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "detdataformats/tde/TDEEthFrame.hpp"
#include "detdataformats/wibeth/WIBEthFrame.hpp"
#include "dunecore/DuneObj/DUNEHDF5FileInfo2.h"
#include "dunecore/DuneObj/PDSPTPCDataInterfaceParent.h"
#include "dunecore/HDF5Utils/HDF5RawFile3Service.h"
#include "dunecore/ChannelMap/TPCChannelMapService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

class PDVDDataInterfaceWIBEth : public PDSPTPCDataInterfaceParent {

private:
  //For nicer log syntax
  std::string logname = "PDVDDataInterfaceWIBEth";
  std::string fFileInfoLabel;

  unsigned int fMaxChan = 1000000; // no maximum for now
  unsigned int fBDEDefaultCrate = 11;
  unsigned int fTDEDefaultCrate = 1;
  int fDebugLevel = 0;                  // switch to turn on debugging printout
  std::string fSubDetectorStringBottom; // VD_Bottom_TPC
  std::string fSubDetectorStringTop;    // VD_Top_TPC
  typedef std::vector<raw::RawDigit> RawDigits;
  typedef std::vector<raw::RDTimeStamp> RDTimeStamps;
  typedef std::vector<raw::RDStatus> RDStatuses;

public:
  explicit PDVDDataInterfaceWIBEth(fhicl::ParameterSet const& p)
    : fFileInfoLabel(p.get<std::string>("FileInfoLabel", "daq"))
    , fMaxChan(p.get<int>("MaxChan", 1000000))
    , fBDEDefaultCrate(p.get<unsigned int>("BDEDefaultCrate", 11))
    , fTDEDefaultCrate(p.get<unsigned int>("TDEDefaultCrate", 1))
    , fDebugLevel(p.get<int>("DebugLevel", 0))
    , fSubDetectorStringBottom(p.get<std::string>("SubDetectorStringBottom", "VD_Bottom_TPC"))
    , fSubDetectorStringTop(p.get<std::string>("SubDetectorStringTop", "VD_Top_TPC"))
  {}

  // wrapper for backward compatibility.  Return data for all crates represented
  // in the fragments on these labels
  int retrieveData(art::Event& evt,
                   std::string inputLabel,
                   std::vector<raw::RawDigit>& raw_digits,
                   std::vector<raw::RDTimeStamp>& rd_timestamps,
                   std::vector<raw::RDStatus>& rdstatuses) override
  {
    return 0;
  }

  // the method name says APAs, but really it's one crate at a time.  HD detectors have a correspondence between APAs and crates, but
  // the VD detectors have CRPs per crate.  The name stuck because of the tool parent class
  // if one doesn't specify a detector type, use BDE, just for backward compatibility with the tool interface for PDSP.
  
  int retrieveDataForSpecifiedAPAs(art::Event& evt,
                                   std::vector<raw::RawDigit>& raw_digits,
                                   std::vector<raw::RDTimeStamp>& rd_timestamps,
                                   std::vector<raw::RDStatus>& rdstatuses,
                                   std::vector<int>& cratelist) override
  {
    return retrieveDataForSpecifiedAPAs(evt, raw_digits, rd_timestamps, rdstatuses, cratelist, "BDE");
  }

  // new tool interface function that respects the detector type string
  
  int retrieveDataForSpecifiedAPAs(art::Event& evt,
                                   std::vector<raw::RawDigit>& raw_digits,
                                   std::vector<raw::RDTimeStamp>& rd_timestamps,
                                   std::vector<raw::RDStatus>& rdstatuses,
                                   std::vector<int>& cratelist,
				   std::string detectortype) override  
    
  {
    auto infoHandle = evt.getHandle<raw::DUNEHDF5FileInfo2>(fFileInfoLabel);
    const std::string& file_name = infoHandle->GetFileName();
    uint32_t runno = infoHandle->GetRun();
    size_t evtno = infoHandle->GetEvent();
    size_t seqno = infoHandle->GetSequence();

    dunedaq::hdf5libs::HDF5RawDataFile::record_id_t rid = std::make_pair(evtno, seqno);

    if (fDebugLevel > 0) {
      std::cout << logname << " HDF5 FileName: " << file_name << std::endl;
      std::cout << logname << " Run:Event:Seq: " << std::dec << runno << ":" << evtno << ":"
                << seqno << std::endl;
      std::cout << logname << " : "
                << "Retrieving Data for " << cratelist.size() << " crates in a detector of type " << detectortype << std::endl;
    }

    for (const int& i : cratelist) {
      int crateno = i;
      if (fDebugLevel > 0) {
        std::cout << logname << " Tool called with requested Crate:"
                  << "crateno: " << i << std::endl;
      }

      getFragmentsForEvent(rid, raw_digits, rd_timestamps, crateno, rdstatuses, detectortype);
    }

    return 0;
  }

  // get data for APAs on the list.  Retrieve the HDF5 raw file pointer from the HDF5RawFile2Service
  // legacy -- not used. This was an old method in which data had to be retrieved from an artROOT file with different
  // labels for different APAs.  HDF5 files do not need this.

  int retrieveDataAPAListWithLabels(art::Event& evt,
                                    std::string inputLabel,
                                    std::vector<raw::RawDigit>& raw_digits,
                                    std::vector<raw::RDTimeStamp>& rd_timestamps,
                                    std::vector<raw::RDStatus>& rdstatuses,
                                    std::vector<int>& apalist) override
  {
    return 0;
  }

  // This is designed to get data from one crate.
  // detectortype is a string that can be "TDE", "BDE" or "ALL"
  
  void getFragmentsForEvent(dunedaq::hdf5libs::HDF5RawDataFile::record_id_t& rid,
                            RawDigits& raw_digits,
                            RDTimeStamps& timestamps,
                            int crateno,
                            RDStatuses& rdstatuses,
			    std::string detectortype)
  {
    using dunedaq::fddetdataformats::WIBEthFrame;  // BDE
    using dunedaq::fddetdataformats::TDEEthFrame;  // TDE

    art::ServiceHandle<dune::TPCChannelMapService> channelMap;
    art::ServiceHandle<dune::HDF5RawFile3Service> rawFileService;
    auto rf = rawFileService->GetPtr();
    auto sourceids = rf->get_source_ids(rid);
    for (const auto& source_id : sourceids) {
      // only want detector readout data (i.e. not trigger info)
      if (source_id.subsystem != dunedaq::daqdataformats::SourceID::Subsystem::kDetectorReadout)
        continue;

      // look through the geo IDs and see if we are in the right crate
      bool has_desired_bde_crate = false;
      bool has_desired_tde_crate = false;

      auto gids = rf->get_geo_ids_for_source_id(rid, source_id);
      for (const auto& gid : gids) {
        if (fDebugLevel > 1) {
          std::cout << logname << " Tool Geoid: " << std::hex << gid << std::dec << std::endl;
        }
        uint16_t detid = 0xffff & gid;
        dunedaq::detdataformats::DetID::Subdetector detidenum =
          static_cast<dunedaq::detdataformats::DetID::Subdetector>(detid);
        auto subdetector_string = dunedaq::detdataformats::DetID::subdetector_to_string(detidenum);
        if (fDebugLevel > 1) {
          std::cout << logname << " Found subdetector string: " << subdetector_string << std::endl;
          std::cout << logname << " Tool looking for subdets: " << fSubDetectorStringBottom << " and " << fSubDetectorStringTop << std::endl;
	  std::cout << logname << " Currently decoding detectors of type " << detectortype << std::endl;
        }

        if (subdetector_string == fSubDetectorStringBottom && (detectortype == "BDE" || detectortype == "ALL") ) {
          uint16_t crate_from_geo = 0xffff & (gid >> 16);
          if (fDebugLevel > 1) {
            std::cout << "crate from geo: " << crate_from_geo << std::endl;
            uint16_t slot_from_geo = 0xffff & (gid >> 32);
            std::cout << "slot from geo: " << slot_from_geo << std::endl;
            uint16_t stream_from_geo = 0xffff & (gid >> 48);
            std::cout << "stream from geo: " << stream_from_geo << std::endl;
          }

          if (-1 == crateno) {
            has_desired_bde_crate = true;
            if (fDebugLevel > 1) {
              std::cout << "assume we desire all crates (-1)" << std::endl;
            }
            break;
          }

          if (crate_from_geo == crateno) {
            has_desired_bde_crate = true;
            if (fDebugLevel > 1) { std::cout << "found desired BDE crate" << std::endl; }
            break;
          }
        }

        if (subdetector_string == fSubDetectorStringTop && (detectortype == "TDE" || detectortype == "ALL")) {
          uint16_t crate_from_geo = 0xffff & (gid >> 16);
          if (fDebugLevel > 1) {
            std::cout << "crate from geo: " << crate_from_geo << std::endl;
            uint16_t slot_from_geo = 0xffff & (gid >> 32);
            std::cout << "slot from geo: " << slot_from_geo << std::endl;
            uint16_t stream_from_geo = 0xffff & (gid >> 48);
            std::cout << "stream from geo: " << stream_from_geo << std::endl;
          }

          if (-1 == crateno) {
            has_desired_tde_crate = true;
            if (fDebugLevel > 1) {
              std::cout << "assume we desire all crates (-1)" << std::endl;
            }
            break;
          }

          if (crate_from_geo == crateno) {
            has_desired_tde_crate = true;
            if (fDebugLevel > 1) { std::cout << "found desired TDE crate" << std::endl; }
            break;
          }
        }	
      } // end loop over geo IDs for this source ID

      //-------------------------------------------------------------
      
      if (has_desired_bde_crate) {
        // this reads the relevant dataset and returns a std::unique_ptr.  Memory is released when
        // it goes out of scope.

        auto frag = rf->get_frag_ptr(rid, source_id);
        auto frag_size = frag->get_size();
        auto frag_timestamp = frag->get_trigger_timestamp();
        auto frag_window_begin = frag->get_window_begin();
        auto frag_window_end = frag->get_window_end();

        auto total_wib_ticks = std::lround((frag_window_end - frag_window_begin) * 16. / 512.);

        size_t fhs = sizeof(dunedaq::daqdataformats::FragmentHeader);
        if (frag_size <= fhs) continue; // Too small to even have a header
        size_t n_frames = (frag_size - fhs) / sizeof(WIBEthFrame);
        if (fDebugLevel > 0) {
          std::cout << "n_frames calc.: " << frag_size << " " << fhs << " " << sizeof(WIBEthFrame)
                    << " " << n_frames << std::endl;
        }

        std::vector<raw::RawDigit::ADCvector_t> adc_vectors(64); // 64 channels per WIBEth frame
        unsigned int detid = 0, crate = 0, slot = 0, stream = 0;

        //We expect to have extra wib ticks, so figure out how many
        //in total we cut out.
        auto leftover_wib_ticks = n_frames * 64 - total_wib_ticks;
        uint64_t latest_time = 0;

        //For bookkeeping if we need to reorder
        std::vector<std::pair<uint64_t, size_t>> timestamp_indices;

        //This will track if we see any problems
        bool any_bad = false;

        //For reordering
        std::vector<std::vector<raw::RawDigit::ADCvector_t>> temp_adcs;
        //Tracks whether a given frame has hit the end
        bool reached_end = false;

        //need to keep track of the timestamp of our first sample
        uint64_t first_sample_timestamp = 0;

        for (size_t i = 0; i < n_frames; ++i) {
          //Makes a 64-channel wide vector
          temp_adcs.emplace_back(64);

          std::bitset<8> condition;
          if (fDebugLevel > 2) {
            // dump WIB frames in binary
            std::cout << "Frame number: " << i << std::endl;
            size_t wfs32 = sizeof(WIBEthFrame) / 4;
            uint32_t* fdp = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(frag->get_data()) +
                                                        i * sizeof(WIBEthFrame));
            std::cout << std::dec;
            for (size_t iwdt = 0; iwdt < std::min(wfs32, (size_t)4);
                 iwdt++) // dumps just the first 4 words.  use wfs32 if you want them all
	      {
		std::cout << iwdt << " : 10987654321098765432109876543210" << std::endl;
		std::cout << iwdt << " : " << std::bitset<32>{fdp[iwdt]} << std::endl;
	      }
            std::cout << std::dec;
          }

          auto frame = reinterpret_cast<WIBEthFrame*>(static_cast<uint8_t*>(frag->get_data()) +
                                                      i * sizeof(WIBEthFrame));

          //Get the timestamps from the WIB Frames.
          //Best practice is to ensure that the
          //timestamps are consistent with the Trigger timestamp

          //Jake Calcutt -- per Roger Huang on Slack:
          //"It would be a good check to put in. If they ever don't match,
          //the recommended action is to mark the data as bad"
          auto link0_timestamp = frame->header.colddata_timestamp_0;
          auto link1_timestamp = frame->header.colddata_timestamp_1;
          auto frame_timestamp = frame->get_timestamp();
          auto frame_size = 64 * 512 / 16;

          if (fDebugLevel > 0) {
            std::cout << "Frame " << i << " timestamps:"
                      << "\n\tlink0: " << link0_timestamp << "\n\tlink1: " << link1_timestamp
                      << "\n\tmaster:" << frame_timestamp << "\n\tw_begin: " << frag_window_begin
                      << "\n\ttrigger: " << frag_timestamp << "\n\tw_end: " << frag_window_end
                      << std::endl;
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
          auto frame_end = frame_timestamp + frame_size;

          frame_good &= (frame_end > frag_window_begin);
          condition[3] = !(frame_end > frag_window_begin);

          frame_good &= (frame_timestamp < frag_window_end);
          condition[4] = !(frame_timestamp < frag_window_end);

          //Check if any frame has hit the end
          reached_end |= ((frame_end >= frag_window_end) && (frame_timestamp < frag_window_end) &&
                          (frame_timestamp >= frag_window_begin));

          //If one frame's bad, make note
          any_bad |= !frame_good;

          //Should also check that none of the frames come out of order
          //TODO -- figure out a way to order them if not good
          if (frame_timestamp < latest_time) {
            std::cout << "Frame " << i << " is earlier than the so-far latest time " << latest_time
                      << std::endl;
          }
          else if (frame_timestamp == latest_time) {
            std::cout << "Frame " << i << " is same as the so-far latest time " << latest_time
                      << std::endl;
          }
          else {
            latest_time = frame_timestamp;
          }

          //Store the frame_timestamp and the index
          timestamp_indices.emplace_back(frame_timestamp, i);

          //Determine if we're in the first frame
          bool first_frame = (frag_window_begin >= frame_timestamp);
          int start_tick = 0;
          if (first_frame) {
            //Turn these into doubles so we can go negative
            start_tick =
              std::lround((frag_window_begin * 16. / 512 - frame_timestamp * 16. / 512.));
            if (fDebugLevel > 0)
              std::cout << "\tFirst frame. Start tick:" << start_tick << std::endl;

            if (i != 0) {
              std::cout << "PDVDDataInterfaceWIBEth WARNING. FIRST FRAME BY TIME, BUT NOT BY ITERATION" << std::endl;
            }
            leftover_wib_ticks -= start_tick;
          }

          if (i == 0) { first_sample_timestamp = frame_timestamp + start_tick * 512 / 16; }

          int adcvs = adc_vectors.size(); // convert to int
          int last_tick = 64;
          //if the readout time is past the frame, don't change anything
          //if frame is past readout time, determine where to stop
          if (frame_timestamp + 512. * 64 / 16 > frag_window_end) {
            //Account for the ticks at the front
            last_tick -= leftover_wib_ticks;
            if (fDebugLevel > 0) std::cout << "Last frame. last tick: " << last_tick << std::endl;
          }

          for (int jChan = 0; jChan < adcvs; ++jChan) // these are ints because get_adc wants ints.
	    {
	      for (int kSample = start_tick; kSample < last_tick; ++kSample) {
		adc_vectors[jChan].push_back(frame->get_adc(jChan, kSample));
		temp_adcs.back()[jChan].push_back(frame->get_adc(jChan, kSample));
	      }
	    }

          if (i == 0) {
	    detid = 10;  // hardwire this because early data from np02 doesn't get this right
            crate = frame->daq_header.crate_id;
            slot = frame->daq_header.slot_id;
            stream = frame->daq_header.stream_id;
            //stream goes from  0:3 and 64:67
          }
        }
        if (fDebugLevel > 0) {
          std::cout << "PDVDDataInterfaceToolWIBEth: detid, crate, slot, stream: " << detid << " " << crate << ", " << slot
                    << ", " << stream << std::endl;
        }

        //Copy the vector to see if it was reordered
        auto unordered = timestamp_indices;

        //Sort the indices according to the timestamp
        std::sort(timestamp_indices.begin(),
                  timestamp_indices.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });

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
            const auto& ti = timestamp_indices[i];
            const auto& u = unordered[i];
            std::cout << "\t" << ti.first << " " << ti.second << " " << u.first << " " << u.second
                      << std::endl;

            //Get the next frame
            auto& this_adcs = temp_adcs[ti.second];
            if (this_adcs.empty()) {
              throw cet::exception("PDVDDataInterfaceWIBEth3_tool.cc")
                << "Somehow the reordering vector is empty at index " << ti.second;
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

        //Check that no frames are dropped,they should be 2048 DTS ticks apart
        //64 WIB tick * 512 ns/WIB tick / (16 ns/DTS tick) = 2048 DTS ticks
        auto prev_timestamp = timestamp_indices[0].first;
        bool skipped_frames = false;
        for (size_t i = 1; i < timestamp_indices.size(); ++i) {
          auto this_timestamp = timestamp_indices[i].first;
          auto delta = this_timestamp - prev_timestamp;
          if (fDebugLevel > 0) std::cout << i << " " << this_timestamp << " " << delta << std::endl;
          prev_timestamp = this_timestamp;

          //For now, set this if the difference isn't 2048
          skipped_frames |= (delta != 2048);

          if (delta != 2048)
            std::cout << "PDVDDataInterfaceWIBEth WARNING. APPARENT SKIPPED BDE FRAME " << i << " timestamp delta: " << delta
                      << std::endl;
          //TODO -- implement the patching,
          //but wait until we have bad data to work with
          //so we can properly test
        }

        for (size_t iChan = 0; iChan < 64; ++iChan) {
          const raw::RawDigit::ADCvector_t& v_adc = adc_vectors[iChan];

	  unsigned int locchan = iChan;

          auto vdchaninfo =
            channelMap->GetChanInfoFromElectronicsIDs(detid, crate, slot, stream, locchan);
          if (fDebugLevel > 2) {
            std::cout << "PDVDDataInterfaceToolWIBEth: wibframechan, valid: " << locchan << " "
                      << vdchaninfo.valid << std::endl;
          }
          if (!vdchaninfo.valid) continue;

          unsigned int offline_chan = vdchaninfo.offlchan;
          if (offline_chan > fMaxChan) continue;

          raw::RDTimeStamp rd_ts(first_sample_timestamp, offline_chan);
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
          //For now: any bad, set first bit
          //         if it was be reodered, set second bit
          //         if any frames appeared to be skipped, set third bit
          //         if the frames did not reach the end of the readout window
          //              set that the fourth bit
          std::bitset<4> statword;
          statword[0] = (any_bad ? 1 : 0);
          statword[1] = (reordered ? 1 : 0);
          statword[2] = (skipped_frames ? 1 : 0);
          statword[3] = (reached_end ? 0 : 1); //Considered good (0) if we hit the end
          rdstatuses.emplace_back(false, statword.any(), statword.to_ulong());
        } // loop over channels
      }  // if has_desired_bde_crate


      //-------------------------------------------------------------
      
      if (has_desired_tde_crate)
	{

	  // this reads the relevant dataset and returns a std::unique_ptr.  Memory is released when
	  // it goes out of scope.

	  auto frag = rf->get_frag_ptr(rid, source_id);
	  auto frag_size = frag->get_size();
	  auto frag_timestamp = frag->get_trigger_timestamp();
	  auto frag_window_begin = frag->get_window_begin();
	  auto frag_window_end = frag->get_window_end();

	  // n.b. TDE sampling frequency is slightly higher than the BDE
	  
	  auto total_tde_ticks = std::lround((frag_window_end - frag_window_begin) * 16. / 500.);

	  size_t fhs = sizeof(dunedaq::daqdataformats::FragmentHeader);
	  if (frag_size <= fhs) continue; // Too small to even have a header
	  size_t n_frames = (frag_size - fhs) / sizeof(TDEEthFrame);
	  if (fDebugLevel > 0) {
	    std::cout << "n_frames calc.: " << frag_size << " " << fhs << " " << sizeof(TDEEthFrame)
		      << " " << n_frames << std::endl;
	  }

	  std::vector<raw::RawDigit::ADCvector_t> adc_vectors(64); // 64 channels per TDEEth frame
	  unsigned int detid = 0, crate = 0, slot = 0, stream = 0;

	  //We expect to have extra ADC sampling ticks, so figure out how many
	  //in total we cut out.
	  auto leftover_tde_ticks = n_frames * 64 - total_tde_ticks;
	  uint64_t latest_time = 0;

	  //For bookkeeping if we need to reorder
	  std::vector<std::pair<uint64_t, size_t>> timestamp_indices;

	  //This will track if we see any problems
	  bool any_bad = false;

	  //For reordering
	  std::vector<std::vector<raw::RawDigit::ADCvector_t>> temp_adcs;
	  //Tracks whether a given frame has hit the end
	  bool reached_end = false;

	  //need to keep track of the timestamp of our first sample
	  uint64_t first_sample_timestamp = 0;

	  for (size_t i = 0; i < n_frames; ++i) {
	    //Makes a 64-channel wide vector
	    temp_adcs.emplace_back(64);

	    std::bitset<8> condition;
	    if (fDebugLevel > 2) {
	      // dump TDEETH frames in binary
	      std::cout << "Frame number: " << i << std::endl;
	      size_t wfs32 = sizeof(TDEEthFrame) / 4;
	      uint32_t* fdp = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(frag->get_data()) +
							  i * sizeof(TDEEthFrame));
	      std::cout << std::dec;
	      for (size_t iwdt = 0; iwdt < std::min(wfs32, (size_t)4);
		   iwdt++) // dumps just the first 4 words.  use wfs32 if you want them all
		{
		  std::cout << iwdt << " : 10987654321098765432109876543210" << std::endl;
		  std::cout << iwdt << " : " << std::bitset<32>{fdp[iwdt]} << std::endl;
		}
	      std::cout << std::dec;
	    }

	    auto frame = reinterpret_cast<TDEEthFrame*>(static_cast<uint8_t*>(frag->get_data()) +
							i * sizeof(TDEEthFrame));

	    //Get the timestamps from the TDEEth Frames.
	    //Best practice is to ensure that the
	    //timestamps are consistent with the Trigger timestamp

	    //Jake Calcutt -- per Roger Huang on Slack:
	    //"It would be a good check to put in. If they ever don't match,
	    //the recommended action is to mark the data as bad"
	    //auto link0_timestamp = frame->header.colddata_timestamp_0;
	    //auto link1_timestamp = frame->header.colddata_timestamp_1;
	    auto frame_timestamp = frame->header.TAItime;
	    auto frame_size = 64 * 500 / 16;

	    if (fDebugLevel > 0) {
	      std::cout << "Frame " << i << " timestamps:"
			<< "\n\tmaster:" << frame_timestamp << "\n\tw_begin: " << frag_window_begin
			<< "\n\ttrigger: " << frag_timestamp << "\n\tw_end: " << frag_window_end
			<< std::endl;
	    }

	    //If this is non-zero, mark bad
	    bool frame_good = (frame->header.tde_errors == 0);
	    condition[0] = !(frame->header.tde_errors == 0);

	    //These should be self-consistent
	    // frame_good &= (link0_timestamp == link1_timestamp);
	    condition[1] = false; // !(link0_timestamp == link1_timestamp);
	    //Lower 15 bits of the timestamps should match the "master" timestamPp
	    //frame_good &= (link0_timestamp == (frame_timestamp & 0x7FFF));
	    condition[2] = false; // !(link0_timestamp == (frame_timestamp & 0x7FFF));
	    //We shouldn't have a frame that is entirely outside of the readout window
	    //(64 ticks x 500 ns per tick)/16ns ticks before the fragment window
	    auto frame_end = frame_timestamp + frame_size;

	    frame_good &= (frame_end > frag_window_begin);
	    condition[3] = !(frame_end > frag_window_begin);

	    frame_good &= (frame_timestamp < frag_window_end);
	    condition[4] = !(frame_timestamp < frag_window_end);

	    //Check if any frame has hit the end
	    reached_end |= ((frame_end >= frag_window_end) && (frame_timestamp < frag_window_end) &&
			    (frame_timestamp >= frag_window_begin));

	    //If one frame's bad, make note
	    any_bad |= !frame_good;

	    //Should also check that none of the frames come out of order
	    //TODO -- figure out a way to order them if not good
	    if (frame_timestamp < latest_time) {
	      std::cout << "Frame " << i << " is earlier than the so-far latest time " << latest_time
			<< std::endl;
	    }
	    else if (frame_timestamp == latest_time) {
	      std::cout << "Frame " << i << " is same as the so-far latest time " << latest_time
			<< std::endl;
	    }
	    else {
	      latest_time = frame_timestamp;
	    }

	    //Store the frame_timestamp and the index
	    timestamp_indices.emplace_back(frame_timestamp, i);

	    //Determine if we're in the first frame
	    bool first_frame = (frag_window_begin >= frame_timestamp);
	    int start_tick = 0;
	    if (first_frame) {
	      //Turn these into doubles so we can go negative
	      start_tick =
		std::lround((frag_window_begin * 16. / 500 - frame_timestamp * 16. / 500.));
	      if (fDebugLevel > 0)
		std::cout << "\tFirst frame. Start tick:" << start_tick << std::endl;

	      if (i != 0) {
		std::cout << "PDVDDataInterfaceWIBEth WARNING. FIRST FRAME BY TIME, BUT NOT BY ITERATION" << std::endl;
	      }
	      leftover_tde_ticks -= start_tick;
	    }

	    if (i == 0) { first_sample_timestamp = frame_timestamp + start_tick * 500 / 16; }

	    int adcvs = adc_vectors.size(); // convert to int
	    int last_tick = 64;
	    //if the readout time is past the frame, don't change anything
	    //if frame is past readout time, determine where to stop
	    if (frame_timestamp + 500. * 64 / 16 > frag_window_end && i==n_frames-1) {
	      //Account for the ticks at the front
	      last_tick -= leftover_tde_ticks;
	      if (fDebugLevel > 0) std::cout << "Last frame. last tick: " << last_tick << std::endl;
	    }

	    for (int jChan = 0; jChan < adcvs; ++jChan) // these are ints because get_adc wants ints.
	      {
		for (int kSample = start_tick; kSample < last_tick; ++kSample) {
		  adc_vectors[jChan].push_back(frame->get_adc(jChan, kSample));
		  temp_adcs.back()[jChan].push_back(frame->get_adc(jChan, kSample));
		}
	      }

	    if (i == 0) {
	      detid = 11;  // hardwire this because early data from np02 doesn't get this right
	      crate = frame->daq_header.crate_id;
	      slot = frame->daq_header.slot_id;
	      stream = frame->daq_header.stream_id;
	      //stream goes from  0:3 and 64:67
	    }
	  }
	  if (fDebugLevel > 0) {
	    std::cout << "PDVDDataInterfaceToolWIBEth: detid, crate, slot, stream: " << detid << " " << crate << ", " << slot
		      << ", " << stream << std::endl;
	  }

	  //Copy the vector to see if it was reordered
	  auto unordered = timestamp_indices;

	  //Sort the indices according to the timestamp
	  std::sort(timestamp_indices.begin(),
		    timestamp_indices.end(),
		    [](const auto& a, const auto& b) { return a.first < b.first; });

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
	      const auto& ti = timestamp_indices[i];
	      const auto& u = unordered[i];
	      std::cout << "\t" << ti.first << " " << ti.second << " " << u.first << " " << u.second
			<< std::endl;

	      //Get the next frame
	      auto& this_adcs = temp_adcs[ti.second];
	      if (this_adcs.empty()) {
		throw cet::exception("PDVDDataInterfaceWIBEth3_tool.cc")
		  << "Somehow the reordering vector is empty at index " << ti.second;
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

	  //Check that no frames are dropped,they should be 2048 DTS ticks apart
	  //64 TDE tick * 500 ns/TDE tick / (16 ns/DTS tick) = 2000 DTS ticks
	  auto prev_timestamp = timestamp_indices[0].first;
	  bool skipped_frames = false;
	  for (size_t i = 1; i < timestamp_indices.size(); ++i) {
	    auto this_timestamp = timestamp_indices[i].first;
	    auto delta = this_timestamp - prev_timestamp;
	    if (fDebugLevel > 0) std::cout << i << " " << this_timestamp << " " << delta << std::endl;
	    prev_timestamp = this_timestamp;

	    //For now, set this if the difference isn't 32000
	    skipped_frames |= (delta != 32000);

	    if (delta != 32000)
	      std::cout << "PDVDDataInterfaceWIBEth WARNING. APPARENT SKIPPED TDE FRAME " << i << " timestamp delta: " << delta
			<< std::endl;
	    //TODO -- implement the patching,
	    //but wait until we have bad data to work with
	    //so we can properly test
	  }

	  for (size_t iChan = 0; iChan < 64; ++iChan) {
	    const raw::RawDigit::ADCvector_t& v_adc = adc_vectors[iChan];

	    unsigned int locchan = iChan;

	    auto vdchaninfo =
	      channelMap->GetChanInfoFromElectronicsIDs(detid, crate, slot, stream, locchan);
	    if (fDebugLevel > 2) {
	      std::cout << "PDVDDataInterfaceToolWIBEth: wibframechan, valid: " << locchan << " "
			<< vdchaninfo.valid << std::endl;
	    }
	    if (!vdchaninfo.valid) continue;

	    unsigned int offline_chan = vdchaninfo.offlchan;
	    if (offline_chan > fMaxChan) continue;

	    raw::RDTimeStamp rd_ts(first_sample_timestamp, offline_chan);
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
	    //For now: any bad, set first bit
	    //         if it was be reodered, set second bit
	    //         if any frames appeared to be skipped, set third bit
	    //         if the frames did not reach the end of the readout window
	    //              set that the fourth bit
	    std::bitset<4> statword;
	    statword[0] = (any_bad ? 1 : 0);
	    statword[1] = (reordered ? 1 : 0);
	    statword[2] = (skipped_frames ? 1 : 0);
	    statword[3] = (reached_end ? 0 : 1); //Considered good (0) if we hit the end
	    rdstatuses.emplace_back(false, statword.any(), statword.to_ulong());
	  } // loop over channels
	}
      
    }  // loop over source IDs
    if (fDebugLevel > 0) {
      std::cout << "PDVDDataInterfaceToolWIBEth: number of raw digits found: " << raw_digits.size()
                << std::endl;
    }
  }

  void getMedianSigma(const raw::RawDigit::ADCvector_t& v_adc, float& median, float& sigma)
  {
    size_t asiz = v_adc.size();
    int imed = 0;
    if (asiz == 0) {
      median = 0;
      sigma = 0;
    }
    else {
      // the RMS includes tails from bad samples and signals and may not be the best RMS calc.

      imed = TMath::Median(asiz, v_adc.data()) +
	0.01; // add an offset to make sure the floor gets the right integer
      median = imed;
      sigma = TMath::RMS(asiz, v_adc.data());

      // add in a correction suggested by David Adams, May 6, 2019

      size_t s1 = 0;
      size_t sm = 0;
      for (size_t i = 0; i < asiz; ++i) {
        if (v_adc.at(i) < imed) s1++;
        if (v_adc.at(i) == imed) sm++;
      }
      if (sm > 0) {
        float mcorr = (-0.5 + (0.5 * (float)asiz - (float)s1) / ((float)sm));
        if (fDebugLevel > 0) {
          if (std::abs(mcorr) > 1.0) std::cout << "mcorr: " << mcorr << std::endl;
        }
        median += mcorr;
      }
    }
  }
};

DEFINE_ART_CLASS_TOOL(PDVDDataInterfaceWIBEth)
