
// Class:       SSPRawDecoder
// Module Type: producer
// File:        SSPRawDecoder_module.cc
//
// Generated at Thu Jul  6 18:31:48 2017 by Antonino Sergi,32 1-A14,+41227678738, using artmod
// from cetpkgsupport v1_11_00.
//
//   
////////////////////////////////////////////////////////////////////////

// art includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "dune-raw-data/Services/ChannelMap/PdspChannelMapService.h"

// artdaq and dune-raw-data includes
#include "dune-raw-data/Overlays/SSPFragment.hh"
#include "artdaq-core/Data/ContainerFragment.hh"
// larsoft includes
#include "lardataobj/RawData/OpDetWaveform.h"
#include "lardataobj/RecoBase/OpHit.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"

// ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

// C++ Includes
#include <memory>
#include <map>

namespace dune {
  class SSPRawDecoder;
}

class dune::SSPRawDecoder : public art::EDProducer {
public:
  explicit SSPRawDecoder(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  SSPRawDecoder(SSPRawDecoder const &) = delete;
  SSPRawDecoder(SSPRawDecoder &&) = delete;
  SSPRawDecoder & operator = (SSPRawDecoder const &) = delete;
  SSPRawDecoder & operator = (SSPRawDecoder &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;
  void reconfigure(const fhicl::ParameterSet &pset);
  void printParameterSet();
  
  struct trig_variables {
    unsigned int header;
    unsigned short length;
    unsigned short type;
    unsigned short status_flags;
    unsigned short header_type;
    unsigned short trig_id;
    unsigned short module_id;
    unsigned short channel_id;
    unsigned int timestamp_sync_delay;
    unsigned int timestamp_sync_count;
    unsigned long timestamp_nova;
    uint32_t peaksum;
    unsigned short peaktime;
    unsigned int prerise;
    unsigned int intsum;
    unsigned short baseline;
    unsigned long baselinesum;
    unsigned short cfd_interpol[4];
    unsigned short internal_interpol;
    uint64_t internal_timestamp;
  };
  void readHeader(const SSPDAQ::EventHeader* daqHeader, struct trig_variables* tv);

  void getFragments(art::Event &evt,std::vector<artdaq::Fragment>* fragments);
  void beginJob() override;
  void endJob() override;
  void beginEvent(art::EventNumber_t eventNumber);
  void endEvent  (art::EventNumber_t eventNumber);

  void setRootObjects();

  recob::OpHit ConstructOpHit(trig_variables &trig, unsigned int channel);

private:

  // Declare member data here.
  std::string fRawDataLabel;
  std::string fOutputDataLabel;
  bool fUseChannelMap;
  bool fDebug;
  raw::Compress_t        fCompression;      ///< compression type to use
  unsigned int           fZeroThreshold;    ///< Zero suppression threshold

  uint32_t n_adc_counter_;  //counter of total number of ALL adc values in an event
  uint64_t adc_cumulative_; //cumulative total of ALL adc values in an event

  uint32_t         verb_adcs_;
  bool             verb_meta_;
  bool _expect_container_fragments;

  TH1D * n_event_packets_;
  TH1D * frag_sizes_;

  // m1, i1, i2
  double m1,i1,i2;

  double NOvAClockFrequency;
  double SPESize;

  //Graphs and vectors                                                                                                                                      

  int number_of_packets = 12;  // 12 channels per SSP

  std::map<size_t,TH1D*> trigger_type_; // internal vs. external  (16 internal, 48 external)

  // more parameters from the FCL file
  int fragment;

  std::vector<raw::OpDetWaveform> waveforms;
  std::vector<recob::OpHit> hits;

};


dune::SSPRawDecoder::SSPRawDecoder(fhicl::ParameterSet const & pset)
// :
{
  reconfigure(pset);
  produces< std::vector<raw::OpDetWaveform> > (fOutputDataLabel);
  produces< std::vector<recob::OpHit> > (fOutputDataLabel);
}

void dune::SSPRawDecoder::reconfigure(fhicl::ParameterSet const& pset) {

  fRawDataLabel = pset.get<std::string>("RawDataLabel");
  fOutputDataLabel = pset.get<std::string>("OutputDataLabel");
  fUseChannelMap = pset.get<bool>("UseChannelMap");
  number_of_packets=pset.get<int>("number_of_packets");
  fDebug = pset.get<bool>("Debug");
  _expect_container_fragments = pset.get<bool>("ExpectContainerFragments",true);
  fZeroThreshold=0;
  fCompression=raw::kNone;

  if(fDebug) printParameterSet();

  verb_adcs_=pset.get<uint32_t>        ("verbose_adcs", 10000); 
  verb_meta_=pset.get<bool>            ("verbose_metadata", true); 
  n_adc_counter_=0; 
  adc_cumulative_=0; 
  
  // m1, i1, i2
  m1=pset.get<int>("SSP_m1"); 
  i1=pset.get<int>("SSP_i1"); 
  i2=pset.get<int>("SSP_i2"); 
  NOvAClockFrequency=pset.get<double>("NOvAClockFrequency"); // in MHz
  SPESize=pset.get<double>("SPESize");
                                                       
  std::cout << "Parameters from the fcl file" << std::endl;
  std::cout << "m1: " << m1 << std::endl;
  std::cout << "i1: " << i1 << std::endl;
  std::cout << "i2: " << i2 << std::endl;
  std::cout << "Fragment: " << fragment << std::endl;
  std::cout << "NOvAClockFrequency: " << NOvAClockFrequency << std::endl; 
  std::cout << "SPESize: " << SPESize << std::endl;
  std::cout << std::endl;

  //startTime=0;

}

void dune::SSPRawDecoder::printParameterSet(){

  for(int i=0;i<20;i++) std::cout << "=";
  std::cout << std::endl;
  std::cout << "Parameter Set" << std::endl;
  for(int i=0;i<20;i++) std::cout << "=";
  std::cout << std::endl;

  std::cout << "fRawDataLabel: " << fRawDataLabel << std::endl;
  std::cout << "fOutputDataLabel: " << fOutputDataLabel << std::endl;
  std::cout << "fDebug: ";
  if(fDebug) std::cout << "true" << std::endl;
  else std::cout << "false" << std::endl;

  for(int i=0;i<20;i++) std::cout << "=";
  std::cout << std::endl;    
}

void dune::SSPRawDecoder::setRootObjects(){
  art::ServiceHandle<art::TFileService> tFileService;

  n_event_packets_ = tFileService->make<TH1D>("ssp_n_event_packets","SSP: n_event_packets",960,-0.5,959.5);  
  frag_sizes_ = tFileService->make<TH1D>("ssp_frag_sizes","SSP: frag_sizes",960,0,2e6);  

}

void dune::SSPRawDecoder::readHeader(const SSPDAQ::EventHeader* daqHeader, struct trig_variables* tv){

  
  tv->header = daqHeader->header;                          // the 'start of header word' (should always be 0xAAAAAAAA)
  tv->length = daqHeader->length;                          // the length of the packet in unsigned ints (including header)
  tv->type = ((daqHeader->group1 & 0xFF00) >> 8);          // packet type
  tv->status_flags = ((daqHeader->group1 & 0x00F0) >> 4);  // status flags
  tv->header_type = ((daqHeader->group1 & 0x000F) >> 0);   // header type
  tv->trig_id = daqHeader->triggerID;                      // the packet ID
  tv->module_id = ((daqHeader->group2 & 0xFFF0) >> 4);     // module ID
  tv->channel_id = ((daqHeader->group2 & 0x000F) >> 0);    // channel ID

                                                           // external timestamp sync delay (FP mode)
  tv->timestamp_sync_delay = ((unsigned int)(daqHeader->timestamp[1]) << 16) + (unsigned int)(daqHeader->timestamp[0]);
                                                           // external timestamp sync count (FP mode)
  tv->timestamp_sync_count = ((unsigned int)(daqHeader->timestamp[3]) << 16) + (unsigned int)(daqHeader->timestamp[2]);
                                                           // get the external timestamp (NOvA mode)
  tv->timestamp_nova = ((unsigned long)daqHeader->timestamp[3] << 48) + ((unsigned long)daqHeader->timestamp[2] << 32)
    + ((unsigned long)daqHeader->timestamp[1] << 16) + ((unsigned long)daqHeader->timestamp[0] << 0);
 

  tv->peaksum = ((daqHeader->group3 & 0x00FF) >> 16) + daqHeader->peakSumLow;  // peak sum
  if(tv->peaksum & 0x00800000) {
    tv->peaksum |= 0xFF000000;
  }

  tv->peaktime = ((daqHeader->group3 & 0xFF00) >> 8);                                  // peak time
  tv->prerise = ((daqHeader->group4 & 0x00FF) << 16) + daqHeader->preriseLow;          // prerise
  tv->intsum = ((unsigned int)(daqHeader->intSumHigh) << 8) + (((unsigned int)(daqHeader->group4) & 0xFF00) >> 8);  // integrated sum
  tv->baseline = daqHeader->baseline;                                                  // baseline
  tv->baselinesum = ((daqHeader->group4 & 0x00FF) << 16) + daqHeader->preriseLow;      // baselinesum
  for(unsigned int i_cfdi = 0; i_cfdi < 4; i_cfdi++)                                   // CFD timestamp interpolation points
    tv->cfd_interpol[i_cfdi] = daqHeader->cfdPoint[i_cfdi];

  tv->internal_interpol = daqHeader->intTimestamp[0];                                  // internal interpolation point
                                                                                       // internal timestamp
  tv->internal_timestamp = ((uint64_t)((uint64_t)daqHeader->intTimestamp[3] << 32)) + ((uint64_t)((uint64_t)daqHeader->intTimestamp[2]) << 16) + ((uint64_t)((uint64_t)daqHeader->intTimestamp[1]));
  
}

void dune::SSPRawDecoder::getFragments(art::Event &evt, std::vector<artdaq::Fragment> *fragments){

  art::EventNumber_t eventNumber = evt.event();

  art::Handle<artdaq::Fragments> rawFragments;
  art::Handle<artdaq::Fragments> containerFragments;

  if (_expect_container_fragments) {
    /// Container Fragments:
    evt.getByLabel(fRawDataLabel, "ContainerPHOTON", containerFragments);
    // Check if there is SSP data in this event
    // Don't crash code if not present, just don't save anything    
    try { containerFragments->size(); }
    catch(std::exception e)  {
      std::cout << "WARNING: Container SSP data not found in event " << eventNumber << std::endl;
      return;
    }
    //Check that the data is valid
    if(!containerFragments.isValid()){
      std::cerr << "Run: " << evt.run()
                << ", SubRun: " << evt.subRun()
                << ", Event: " << eventNumber
                << " is NOT VALID" << std::endl;
      throw cet::exception("containerFragments NOT VALID");
    }

    for (auto cont : *containerFragments)
      {
        std::cout << "container fragment type: " << (unsigned)cont.type() << std::endl;
        artdaq::ContainerFragment contf(cont);
        for (size_t ii = 0; ii < contf.block_count(); ++ii)
          {
            size_t fragSize = contf.fragSize(ii);
            frag_sizes_->Fill(fragSize);
            //artdaq::Fragment thisfrag;
            //thisfrag.resizeBytes(fragSize);
	    
            //memcpy(thisfrag.headerAddress(), contf.at(ii), fragSize);
            fragments->emplace_back(*contf[ii]);
          }
      }
  }
  else {
    /// Raw Fragments:
    evt.getByLabel(fRawDataLabel, "PHOTON", rawFragments);
    
    // Check if there is SSP data in this event
    // Don't crash code if not present, just don't save anything
    try { rawFragments->size(); }
    catch(std::exception e) {
      std::cout << "WARNING: Raw SSP data not found in event " << eventNumber << std::endl;
      return;
    }

    //Check that the data is valid
    if(!rawFragments.isValid()){
      std::cerr << "Run: " << evt.run()
	        << ", SubRun: " << evt.subRun()
	        << ", Event: " << eventNumber
	        << " is NOT VALID" << std::endl;
      throw cet::exception("rawFragments NOT VALID");
    }
    for(auto const& rawfrag: *rawFragments){
      fragments->emplace_back( rawfrag );
    }
  }
  
}

void dune::SSPRawDecoder::beginJob(){
  
  setRootObjects();
  
}

void dune::SSPRawDecoder::beginEvent(art::EventNumber_t /*eventNumber*/)
{
  n_adc_counter_  = 0;
  adc_cumulative_ = 0;
}

void dune::SSPRawDecoder::endEvent(art::EventNumber_t eventNumber)
{
  //write the ADC histogram for the given event
  //if(n_adc_counter_)
  //  adc_values_->Write(Form("adc_values:event_%d", eventNumber));

}
 
void dune::SSPRawDecoder::endJob(){

}

void dune::SSPRawDecoder::produce(art::Event & evt){

  art::ServiceHandle<art::TFileService> tFileService;
  art::ServiceHandle<dune::PdspChannelMapService> channelMap;

  //LOG_INFO("SSPRawDecoder") << "-------------------- SSP RawDecoder -------------------";
  // Implementation of required member function here.

  art::EventNumber_t eventNumber = evt.event();  
  
  /// Get the fragments (Container or Raw)
  std::vector<artdaq::Fragment> fragments;
  getFragments(evt,&fragments);

  unsigned int allPacketsProcessed = 0;
  unsigned int waveform_counter = 0;
  
  std::map<int, int> packets_per_fragment;
  
  /// Process all packets:
  
  for(auto const& frag: fragments){
    if((unsigned)frag.type() != 3) continue;
    // print raw fragment header information
    std::cout << "   SequenceID = " << frag.sequenceID()
	      << "   fragmentID = " << frag.fragmentID()
	      << "   fragmentType = " << (unsigned)frag.type()
	      << "   Timestamp =  " << std::dec << frag.timestamp() << std::endl;
    
    ///> Create a SSPFragment from the generic artdaq fragment
    dune::SSPFragment sspf(frag);
    
    ///> get the size of the event in units of dune::SSPFragment::Header::data_t
    dune::SSPFragment::Header::event_size_t event_size = sspf.hdr_event_size();
    
    ///> get the size of the header in units of dune::SSPFragment::Header::data_t
    std::size_t header_size = sspf.hdr_run_number();
    
    ///> get the event run number
    dune::SSPFragment::Header::run_number_t run_number = sspf.hdr_run_number();
    
    ///> get the number of ADC values describing data beyond the header
    std::size_t n_adc_values = sspf.total_adc_values();
    
    std::cout << std::endl;
    std::cout << "SSP fragment "     << frag.fragmentID() 
	      << " has total size: " << event_size << " SSPFragment::Header::data_t words"
	      << " (of which " << header_size << " is header)"
	      << " and run number: " << run_number
	      << " with " << n_adc_values << " total ADC values"
	      << std::endl;
    std::cout << std::endl;
    
    unsigned int n_packets = 0;
    
    const SSPDAQ::MillisliceHeader* meta=0;
    ///> get the information from the header
    if(frag.hasMetadata())
      {
	///> get the metadata
	meta = &(frag.metadata<SSPFragment::Metadata>()->sliceHeader);
	
	///> get the start and end times for the millislice
	unsigned long start_time = meta->startTime;
	unsigned long end_time   = meta->endTime;
	
	///> get the length of the millislice in unsigned ints (including header)
	unsigned int milli_length = meta->length;
	
	///> get the number of packets in the millislice
	n_packets = meta->nTriggers;
	
	std::cout << "Event number: " << eventNumber << ", packets: " << n_packets << std::endl;
	
	std::cout
	  <<"===Slice metadata:"<<std::endl
	  <<"Start time         "<< start_time   <<std::endl
	  <<"End time           "<< end_time     <<std::endl
	  <<"Packet length      "<< milli_length <<std::endl
	  <<"Number of packets "<< n_packets   <<std::endl <<std::endl;
      }
    else
      {
	std::cout << "SSP fragment has no metadata associated with it." << std::endl;
      }
    
    ///> get a pointer to the first packet in the millislice
    const unsigned int* dataPointer = sspf.dataBegin();
    
    ///> loop over the packets in the millislice
    unsigned int packetsProcessed=0;
    while(( meta==0 || packetsProcessed<meta->nTriggers) && dataPointer<sspf.dataEnd() ){
      
      ///> get the packet header
      const SSPDAQ::EventHeader* daqHeader=reinterpret_cast<const SSPDAQ::EventHeader*>(dataPointer);
      
      /// read the header to provide the trigger variables structure	
      struct trig_variables trig;
      readHeader(daqHeader, &trig);
      
      /// time
      double time = trig.internal_timestamp/150*1E-6;
      
      /// channel (0..number_of_packets*number_of_fragments)
      unsigned int channel = frag.fragmentID()*number_of_packets + trig.channel_id;
      
      // pedestal, area and peak (according to the Register table, the  SSP User Manual has i1 and i2 inverted)
      double pedestal = trig.baseline / ((double)i1);    
      double area = trig.intsum  - pedestal * ((double)i2);
      double peak = trig.peaksum / ((double)m1) - pedestal;
      
      if(verb_meta_) {
	std::cout
	  << "Channel:                            " << channel                   << std::endl
	  << "Header:                             " << trig.header               << std::endl
	  << "Length:                             " << trig.length               << std::endl
	  << "Trigger type:                       " << trig.type                 << std::endl
	  << "Status flags:                       " << trig.status_flags         << std::endl
	  << "Header type:                        " << trig.header_type          << std::endl
	  << "Trigger ID:                         " << trig.trig_id              << std::endl
	  << "Module ID:                          " << trig.module_id            << std::endl
	  << "Channel ID:                         " << trig.channel_id           << std::endl
	  << "External timestamp (F mode):        "                              << std::endl
	  << "  Sync delay:                       " << trig.timestamp_sync_delay << std::endl
	  << "  Sync count:                       " << trig.timestamp_sync_count << std::endl
	  << "External timestamp (NOvA mode):     " << trig.timestamp_nova       << std::endl
	  << "Peak sum:                           " << trig.peaksum              << std::endl
	  << "Peak time:                          " << trig.peaktime             << std::endl
	  << "Prerise:                            " << trig.prerise              << std::endl
	  << "Integrated sum:                     " << trig.intsum               << std::endl
	  << "Baseline sum:                       " << trig.baseline             << std::endl
	  << "CFD Timestamp interpolation points: " << trig.cfd_interpol[0]      << " " << trig.cfd_interpol[1] << " " << trig.cfd_interpol[2]   << " " << trig.cfd_interpol[3] << std::endl
	  << "Internal interpolation point:       " << trig.internal_interpol    << std::endl
	  << "Internal timestamp:                 " << trig.internal_timestamp   << std::endl
	  << std::endl
	  << "Pedestal                            " << pedestal                  << std::endl
	  << "Area                                " << area                      << std::endl
	  << "Peak heigth                         " << peak                      << std::endl
	  << std::endl;
      }
      
      ///> Trigger type histogram
      if (trigger_type_.find(channel) == trigger_type_.end())
	{
	  TH1D* tth = tFileService->make<TH1D>(Form("trigger_type_channel_%d",channel),Form("trigger_type_channel_%d",channel),4,0,3);
          tth->SetTitle(Form("Trigger type - Channel %d",channel));
          tth->GetXaxis()->SetTitle("Trigger type");
          tth->GetXaxis()->SetBinLabel(2,"Internal (16)");
          tth->GetXaxis()->SetBinLabel(3,"External (48)");
	  trigger_type_[channel] = tth;
	}

      if ( trig.type == 16 ) trigger_type_[channel]->Fill(1);
      if ( trig.type == 48 ) trigger_type_[channel]->Fill(2);
      
      ///> increment the data pointer past the packet header
      dataPointer+=sizeof(SSPDAQ::EventHeader)/sizeof(unsigned int);
      
      //>get the information from the data
      bool verb_values = false;
      
      ///> get the number of ADC values in the packet
      unsigned int nADC=(trig.length-sizeof(SSPDAQ::EventHeader)/sizeof(unsigned int))*2;
      
      ///> get a pointer to the first ADC value
      const unsigned short* adcPointer=reinterpret_cast<const unsigned short*>(dataPointer);
      
      char histname[100];
      sprintf(histname,"evt%i_frag%d_wav%d",eventNumber, frag.fragmentID(), packetsProcessed);
      
      TH1D* hist=new TH1D("hist","hist",nADC,0,nADC);
      
      // map the channel number to offline if requested

      unsigned int mappedchannel = channel;
      if (fUseChannelMap) mappedchannel = channelMap->SSPOfflineChannelFromOnlineChannel(channel);

      // Get information from the header, //added by Jingbo
            
      unsigned short     OpChannel   =  (unsigned short) mappedchannel;   ///< Derived Optical channel
      raw::OpDetWaveform Waveform(time, OpChannel, nADC);
      
      ///> copy the waveforms

      for(size_t idata = 0; idata < nADC; idata++) {
	///> get the 'idata-th' ADC value
	const unsigned short* adc = adcPointer + idata;
	
	Waveform.push_back(*adc); //added by Jingbo
	waveform_counter++;
	
	n_adc_counter_++;
	adc_cumulative_ += (uint64_t)(*adc);
	
	///> Waveform 
	hist->SetBinContent(idata+1,*adc);
	// hist.at(packetsProcessed)->SetBinContent(idata+1, *adc); 
	// std::cout << idata+1 << ":" << *adc << std::endl;

	if (idata >= verb_adcs_) verb_values = false;
	verb_values = false; //don't print adc. Added by J.Wang
	if(verb_values) {
	  if(idata == 0&&verb_adcs_>0) std::cout << "Printing the " << nADC << " ADC values saved with the packet:" << std::endl;
	  std::cout << *adc << " ";
	}

      }// idata
      
      ///> increment the data pointer to the end of the current packet (to the start of the next packet header, if available)
      dataPointer+=nADC/2;
      
      // fill waveforms, added by Jingbo 
      waveforms.emplace_back( Waveform );
      
      // fill the ophit and put it in hits
      hits.emplace_back( ConstructOpHit(trig, mappedchannel) );
      
      hist->Delete();
      
      ++packetsProcessed;
      //std::cout<<std::endl<<"Packets processed: "<<packetsProcessed<<std::endl<<std::endl;
    }// packets
    
    packets_per_fragment[frag.fragmentID()] = packetsProcessed;
    allPacketsProcessed += packetsProcessed;
    
  }// frag: fragments
  
  n_event_packets_->Fill(allPacketsProcessed);
  std::cout << "Event " << eventNumber << " has " << allPacketsProcessed << " total packets";
  for(std::map<int, int>::iterator i_packets_per_fragment = packets_per_fragment.begin(); i_packets_per_fragment != packets_per_fragment.end(); i_packets_per_fragment++)
    std::cout << " " << i_packets_per_fragment->first << ":" << i_packets_per_fragment->second;
  std::cout << std::endl;
  std::cout << std::endl
	    << "ADC total is (from counter):           " << (double)adc_cumulative_
            << std::endl
	    << "Event ADC average is (from counter):   " << ((n_adc_counter_ == 0) ? 0 : (double)adc_cumulative_/(double)n_adc_counter_)
	    << std::endl;
  endEvent(eventNumber);
  
  evt.put(std::make_unique<decltype(waveforms)>(std::move(waveforms)), fOutputDataLabel);
  evt.put(std::make_unique<decltype(hits)>(std::move(hits)), fOutputDataLabel);
}

recob::OpHit dune::SSPRawDecoder::ConstructOpHit(trig_variables &trig, unsigned int channel)
{
  // Get basic information from the header
  unsigned short     OpChannel   = channel;         ///< Derived Optical channel
  unsigned long      FirstSample = trig.timestamp_nova;
  double             TimeStamp   = ((double)FirstSample)/NOvAClockFrequency; ///< first sample time in microseconds

  auto const* ts = lar::providerFrom<detinfo::DetectorClocksService>();
  double peakTime = ((double) trig.peaktime) * ts->OpticalClock().TickPeriod(); // microseconds
  double width = ((double)i1) * ts->OpticalClock().TickPeriod(); // microseconds
  
  double pedestal = ( (double) trig.baselinesum ) / ( (double) i1 );
  double area =     ( (double) trig.intsum      ) - pedestal * ( (double) i2 );
  double peak =     ( (double) trig.peaksum     ) / ( (double) m1 ) - pedestal;
  
  
  recob::OpHit ophit(OpChannel,
                     TimeStamp+peakTime,  // Relative Time
                     TimeStamp+peakTime,  // Absolute time
                     0,          // Frame, not used by DUNE
                     width,
                     area,
                     peak,
                     area / SPESize, // PE
                     0.);
  
  return ophit;
}


DEFINE_ART_MODULE(dune::SSPRawDecoder)
