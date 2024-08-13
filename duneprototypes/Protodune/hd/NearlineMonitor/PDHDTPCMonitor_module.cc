///////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Class:       tpc_monitor::PDHDTPCMonitor
// Module type: analyzer
// File:        PDHDTPCMonitor_module.cc
// Author:      Tom Junk -- adapted from the PDSP version August 2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// LArSoft includes

// Data type includes
#include "lardataobj/RawData/raw.h"
#include "lardataobj/RawData/RawDigit.h"
#include "dunecore/DuneObj/RDStatus.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "duneprototypes/Protodune/hd/ChannelMap/PD2HDChannelMapService.h"

// ROOT includes.
#include "TH1.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TMath.h"

// C++ Includes
#include <vector>
#include <map>

namespace tpc_monitor{

  class PDHDTPCMonitor : public art::EDAnalyzer{
  	
  public:
 
    explicit PDHDTPCMonitor(fhicl::ParameterSet const& pset);
    virtual ~PDHDTPCMonitor();

    void beginJob();
    void beginRun(const art::Run& run);
    void reconfigure(fhicl::ParameterSet const& pset);
    void analyze(const art::Event& evt); 
    
  private:
    // Parameters in .fcl file
    std::string fRawDigitLabel;
    std::string fTPCInput;
    std::string fTPCInstance;
    int fRebinX;
    int fRebinY; 
    int fNTicks;
    int fCollPed;
    int fIndPed;
    int fHitThreshColl;
    int fHitThreshInd;
    int fHitDeadtimeColl;
    int fHitDeadtimeInd;
    
    // sampling rate
    float fSampleRate;
    
    // bin width in kHz
    float fBinWidth;

    // mean, rms, occupancy and FFT per APA per plane -- first index is (crate - 1), second is plane: U, V, X1, X2

    std::map<int, std::map< int, TProfile*>>   fChanMean;
    std::map<int, std::map< int, TProfile*>>   fChanRMS;
    std::map<int, std::map< int, TProfile*>>   fChanHitOccupancy;
    std::map<int, std::map< int, TProfile*>>   fChanFFTProfile;
    std::map<int, std::map< int, TH2F*>>       fChanFFT;
    std::map<int, std::map< int, TH1D*>>       fChanADC;
    std::map<int, std::map< int, TH2F*>>       fPersistentFFT;

    // FFT per FEMB -- first index is crate - 1, second is FEMB in the APA  (1:20)

    std::map<int, std::map<int, TProfile*>>    fFEMBFFT;
    
    // 2D histograms of all Mean/RMS by offline channel number
    // Intended as a color map with each bin to represent a single channel
    TProfile2D* fAllChanMean;
    TProfile2D* fAllChanRMS;
    
    TH1F *fNTicksTPC;
    TH1F *fRMSNTicksTPC;
    
    // define functions
    void calculateFFT(TH1D* hist_waveform, TH1D* graph_frequency);
    int FEMBchanToHistogramMap(int FEMBchan, int coord);

  }; // PDHDTPCMonitor

  //-----------------------------------------------------------------------
  // do configuration in constructor
  
  PDHDTPCMonitor::PDHDTPCMonitor(fhicl::ParameterSet const& p)
    : EDAnalyzer(p) {

    fTPCInput        = p.get< std::string >("TPCInputModule","tpcrawdecoder");
    fTPCInstance     = p.get< std::string >("TPCInstanceName","daq");
    fRebinX          = p.get<int>("RebinFactorX",1.0);
    fRebinY          = p.get<int>("RebinFactorY",1.0);
    fNTicks          = p.get<int>("NTicks",5859);
    fHitThreshColl   = p.get<int>("HitThreshColl",140);
    fHitThreshInd    = p.get<int>("HitThreshInd",140);
    fHitDeadtimeColl = p.get<int>("HitDeadtimeColl",100);
    fHitDeadtimeInd  = p.get<int>("HitDeadtimeInd",100);
    fCollPed         = p.get<int>("CollPed",800);
    fIndPed          = p.get<int>("IndPed",8200);
    
    fSampleRate = 1.953125;
    // width of frequencyBin in kHz
    fBinWidth = 1.0/(fNTicks*fSampleRate*1.0e-6);
    std::cout<<"PDHDTPCMonitor FFT Bin Width (kHz)  = " << fBinWidth<<std::endl;
    return;
    
  }

  //-----------------------------------------------------------------------


  PDHDTPCMonitor::~PDHDTPCMonitor() {}
   

  //-----------------------------------------------------------------------
  
  void PDHDTPCMonitor::beginJob() {
    art::ServiceHandle<art::TFileService> tfs;

    // for one APA -- can get this from the channel map but this is the PDHD analyzer so hardcode them
 
    std::vector<TString> apalabel = { "APA_P02SU", "APA_P01SU", "APA_P02NL", "APA_P01NL" };
    std::vector<TString> planelabel = { "U", "V", "X1", "X2" };
    std::vector<int> planechancount = { 800, 800, 480, 480 };
    
    for (int iapa=0; iapa<4; ++iapa)
      {
	for (int iplane=0; iplane<4; ++iplane)
	  {
	    TString hnamebase = apalabel.at(iapa) + "_" + planelabel.at(iplane) + "_";
	    fChanMean[iapa][iplane] = tfs->make<TProfile>(hnamebase+"Mean",hnamebase+"Mean;channel;Mean ADC",planechancount.at(iplane),-0.5,planechancount.at(iplane)-0.5);
	    fChanRMS[iapa][iplane] = tfs->make<TProfile>(hnamebase+"RMS",hnamebase+"RMS;channel;RMS ADC",planechancount.at(iplane),-0.5,planechancount.at(iplane)-0.5);
	    fChanHitOccupancy[iapa][iplane] = tfs->make<TProfile>(hnamebase+"HitOccup",hnamebase+"Hit Occupancy;channel;Hits/Trigger Record",planechancount.at(iplane),-0.5,planechancount.at(iplane)-0.5);
	    fChanFFT[iapa][iplane] = tfs->make<TH2F>(hnamebase+"FFT2D",hnamebase+"FFT;channel;Frequency [kHz]",planechancount.at(iplane),-0.5,planechancount.at(iplane)-0.5,fNTicks/2,0,fNTicks/2*fBinWidth);
 	    fChanFFT[iapa][iplane]->Rebin2D(fRebinX, fRebinY);
	    float xlow = fCollPed - 300.5;
	    float xhigh = fCollPed + 299.5;
	    if ( (iplane < 2 && (iapa != 0 || iplane !=1)) || (iapa == 0 && iplane > 1) )
	      {
		xlow = fIndPed - 300.5;
		xhigh = fIndPed + 299.5;
	      }
	    fChanADC[iapa][iplane] = tfs->make<TH1D>(hnamebase+"ADC",hnamebase+"ADC;ADC;Entries",600,xlow,xhigh);
	    fPersistentFFT[iapa][iplane] = tfs->make<TH2F>(hnamebase+"PersistentFFT",hnamebase+"Persistent FFT;Frequency [kHz];Amplitude [dB]",fNTicks/2,0,fNTicks/2*fBinWidth,150,-100.0,50.0);
	    fChanFFTProfile[iapa][iplane] = tfs->make<TProfile>(hnamebase+"FFTProfile",hnamebase+"FFT;Frequency [kHz];Amplitude [dB]",fNTicks/2,0,fNTicks/2*fBinWidth);
	  }
	for (int ifemb=1; ifemb<21; ++ifemb)
	  {
	    TString hname = apalabel.at(iapa) + "_FEMB";
	    hname += ifemb;
	    fFEMBFFT[iapa][ifemb] = tfs->make<TProfile>(hname+"FFTProfile",hname+"FFT;Frequency [kHz];Amplitude [dB]",fNTicks/2,0,fNTicks/2*fBinWidth);
	  }
      }

    //All in one view
    //make the histograms
    fAllChanMean = tfs->make<TProfile2D>("fAllChanMean", "Means for all channels", 240, -0.5, 239.5, 64, -0.5, 63.5);
    fAllChanRMS = tfs->make<TProfile2D>("fAllChanRMS", "RMS for all channels", 240, -0.5, 239.5, 64, -0.5, 63.5);
    //set titles and bin labels
    fAllChanMean->GetXaxis()->SetTitle("APA Number (online)"); fAllChanMean->GetYaxis()->SetTitle("Plane"); fAllChanMean->GetZaxis()->SetTitle("Raw Mean");
    fAllChanRMS->GetXaxis()->SetTitle("APA Number (online)"); fAllChanRMS->GetYaxis()->SetTitle("Plane"); fAllChanRMS->GetZaxis()->SetTitle("Raw RMS");
    fAllChanMean->GetXaxis()->SetLabelSize(.075); fAllChanMean->GetYaxis()->SetLabelSize(.05);
    fAllChanRMS->GetXaxis()->SetLabelSize(.075); fAllChanRMS->GetYaxis()->SetLabelSize(.05);
    fAllChanMean->GetXaxis()->SetBinLabel(40, "3"); fAllChanMean->GetXaxis()->SetBinLabel(120, "2"); fAllChanMean->GetXaxis()->SetBinLabel(200, "1");
    fAllChanRMS->GetXaxis()->SetBinLabel(40, "3"); fAllChanRMS->GetXaxis()->SetBinLabel(120, "2"); fAllChanRMS->GetXaxis()->SetBinLabel(200, "1");
    fAllChanMean->GetYaxis()->SetBinLabel(5, "U"); fAllChanMean->GetYaxis()->SetBinLabel(15, "V"); fAllChanMean->GetYaxis()->SetBinLabel(26, "Z");
    fAllChanMean->GetYaxis()->SetBinLabel(37, "U"); fAllChanMean->GetYaxis()->SetBinLabel(47, "V"); fAllChanMean->GetYaxis()->SetBinLabel(58, "Z");
    fAllChanRMS->GetYaxis()->SetBinLabel(5, "U"); fAllChanRMS->GetYaxis()->SetBinLabel(15, "V"); fAllChanRMS->GetYaxis()->SetBinLabel(26, "Z");
    fAllChanRMS->GetYaxis()->SetBinLabel(37, "U"); fAllChanRMS->GetYaxis()->SetBinLabel(47, "V"); fAllChanRMS->GetYaxis()->SetBinLabel(58, "Z");

    fNTicksTPC = tfs->make<TH1F>("NTicksTPC","NTicks in TPC Channels",100,0,20000);
    fRMSNTicksTPC = tfs->make<TH1F>("RMSNTicksTPC","RMS NTicks in TPC Channels",100,0,100);

  }

  //-----------------------------------------------------------------------
  void PDHDTPCMonitor::beginRun(const art::Run& run) {
    // place to read databases or run independent info
  }
  //-----------------------------------------------------------------------

  void PDHDTPCMonitor::analyze(const art::Event& event) {
    // Get channel map
    art::ServiceHandle<dune::PD2HDChannelMapService> channelMap;

    MF_LOG_INFO("PDHDTPCMonitor")
      << "-------------------- TPC PDHDTPCMonitor -------------------";

    // called once per event

    int Event  = event.id().event(); 
    //fRun    = event.run();
    //fSubRun = event.subRun();
    std::cout << "PDHD TPC Monitor EventNumber = " << Event << std::endl;

    // Get the objects holding raw information: RawDigit for TPC data
    art::InputTag itag1(fTPCInput, fTPCInstance);
    auto  RawTPC = event.getHandle< std::vector<raw::RawDigit> >(itag1);

    // Get RDStatus handle

    auto RDStatusHandle = event.getHandle< std::vector<raw::RDStatus> >(itag1);

    // Fill pointer vectors - more useful form for the raw data
    // a more usable form
    std::vector< art::Ptr<raw::RawDigit> > RawDigits;
    art::fill_ptr_vector(RawDigits, RawTPC);

    //for the large all channel summary histograms these are key points for bin mapping
    //for each offline numbered apa, the left most bin should be at the x value:
    int xEdgeAPA[6] = {0,0,80,80,160,160}; //these numbers may be adjusted to horizontally space out the histogram
    //for each of the apas, the bottom most bin should be at the y value:
    int yEdgeAPA[2] = {0,32}; //these numbers may be adjusted to vertically space out the histograms

    // example of retrieving RDStatus word and flags

    for ( auto const& rdstatus : (*RDStatusHandle) )
      {
	if (rdstatus.GetCorruptDataDroppedFlag())
	  {
	    MF_LOG_INFO("PDHDTPCMonitor_module: ") << "Corrupt Data Dropped Flag set in RDStatus";
	  }
	//std::cout << "RDStatus:  Corrupt Data dropped " << rdstatus.GetCorruptDataDroppedFlag() << std::endl; 
	//std::cout << "RDStatus:  Corrupt Data kept " << rdstatus.GetCorruptDataKeptFlag() << std::endl; 
	//std::cout << "RDStatus:  Status Word " << rdstatus.GetStatWord() << std::endl; 
      }

    // Loop over all RawDigits

    std::vector<float> nticksvec;
    
    for(auto const & dptr : RawDigits) {
      const raw::RawDigit & digit = *dptr;
      
      // Get the channel number for this digit
      uint32_t chan = digit.Channel();
      auto chanInfo = channelMap->GetChanInfoFromOfflChan(chan);
      unsigned int apa = chanInfo.crate - 1;  // 0:3	  
      unsigned int plane = chanInfo.plane;  // 0:2
      if ( (chan % 2560) > 2079) ++plane;   // call X1 and X2 different planes
      int tchan = chan % 2560;          // count channels within each plane starting from zero.
      int planechan = tchan;
      if (planechan > 799) planechan -= 800;
      if (planechan > 799) planechan -= 800;
      if (tchan > 1599 && planechan > 479) planechan -= 480;
      
      // number of samples in uncompressed ADC
      int nSamples = digit.Samples();
      fNTicksTPC->Fill(nSamples);
      nticksvec.push_back(nSamples);
      
      int pedestal = (int)digit.GetPedestal();
      //int pedestal = 0;  

      std::vector<short> uncompressed(nSamples);
      // with pedestal	  
      raw::Uncompress(digit.ADCs(), uncompressed, pedestal, digit.Compression());

      // subtract pedestals
      std::vector<short> uncompPed(nSamples);

      auto adchistopointer = fChanADC[apa][plane];
      for (int i=0; i<nSamples; i++) 
	{ 
	  auto adc=uncompressed.at(i);
	  uncompPed.at(i) = adc - pedestal;
	  adchistopointer->Fill(adc);
        }

      // number of ADC uncompressed without pedestal
      size_t nADC_uncompPed=uncompPed.size();	 
      
      // wavefrom histogram   
      TH1D* histwav=new TH1D(Form("wav%d",(int)chan),Form("wav%d",(int)chan),nSamples,0,nSamples); 
      for(int k=0;k<(int)nADC_uncompPed;k++) {
	histwav->SetBinContent(k+1, uncompPed.at(k));
      }
	    
      // Do FFT for single waveforms
      TH1D* histfft=new TH1D(Form("fft%d",(int)chan),Form("fft%d",(int)chan),nSamples,0,nSamples*fBinWidth); 
      calculateFFT(histwav, histfft);

      auto fftptr = fChanFFT[apa][plane];
      auto fftprofileptr = fChanFFTProfile[apa][plane];
      auto fftpersistentptr = fPersistentFFT[apa][plane];
      auto fftfembptr = fFEMBFFT[apa][chanInfo.femb];
      
      for(int k=0;k<(int)nADC_uncompPed/2;k++) {
	double bc = histfft->GetBinContent(k+1);
	if (bc > -1E6 && bc < 1E6)
	  {
	    fftptr->Fill(planechan,(k+0.5)*fBinWidth, bc);
	    fftprofileptr->Fill((k+0.5)*fBinWidth, bc);
	    fftpersistentptr->Fill((k+0.5)*fBinWidth, bc);
	    fftfembptr->Fill((k+0.5)*fBinWidth, bc);
	  }
      }

      // Mean and RMS
      float mean = digit.GetPedestal();
      float rms = digit.GetSigma();
      fChanMean[apa][plane]->Fill(planechan,mean);
      fChanRMS[apa][plane]->Fill(planechan,rms);

      // fill the summary plots

      int FEMBchan = chanInfo.cebchan;
      int iFEMB = chanInfo.femb - 1;
      //Get the location of any FEMBchan in the histogram
      //put as a function for cleanliness.
      int xBin = ((FEMBchanToHistogramMap(FEMBchan,0))+(iFEMB*4)+xEdgeAPA[apa]); // (fembchan location on histogram) + shift from mobo + shift from apa
      int yBin = ((FEMBchanToHistogramMap(FEMBchan,1))+yEdgeAPA[(apa%2)]); //(fembchan location on histogram) + shift from apa 

      fAllChanMean->Fill(xBin,yBin,mean); //histogram the mean
      fAllChanRMS->Fill(xBin,yBin,rms); //histogram the rms
      
      histwav->Delete(); 
      histfft->Delete();                                                                                                                    

      int nhits = 0;
      int lasthittick = -10000;
      int thresh = fHitThreshColl;
      if (plane < 2) thresh = fHitThreshInd;
      int ndeadlimit = fHitDeadtimeColl;
      if (plane < 2) ndeadlimit = fHitDeadtimeInd;
      for (int itick=0; itick< (int) uncompPed.size(); ++itick)
	{
	  if (TMath::Abs(uncompPed.at(itick)) > thresh && itick > lasthittick + ndeadlimit)
	    {
	      ++nhits;
	      lasthittick = itick;
	    }
	}
      fChanHitOccupancy[apa][plane]->Fill(planechan,nhits);
      
    } // RawDigits

    auto ntrms = TMath::RMS(nticksvec.size(),nticksvec.data());
    fRMSNTicksTPC->Fill(ntrms);
    
    return;
  }
  
  //-----------------------------------------------------------------------  
  //calculate FFT
  void PDHDTPCMonitor::calculateFFT(TH1D* hist_waveform, TH1D* hist_frequency) {
  
    int n_bins = hist_waveform->GetNbinsX();
    TH1* hist_transform = 0;

    // Create hist_transform from the input hist_waveform
    hist_transform = hist_waveform->FFT(hist_transform, "MAG");
    hist_transform -> Scale (1.0 / float(n_bins));
    int nFFT=hist_transform->GetNbinsX();
  
    double frequency;
    double amplitude;
    double amplitudeLog;
  
    // Loop on the hist_transform to fill the hist_transform_frequency                                                                                        
    for (int k = 0; k < nFFT/2; k++){

      frequency =  (k+0.5)*fBinWidth; // kHz
      amplitude = hist_transform->GetBinContent(k+1); 
      amplitudeLog = 20*log10(amplitude); // dB
      hist_frequency->Fill(frequency, amplitudeLog);
    }

    hist_transform->Delete();  
  }

  //----------------------------------------------------------------------
  //define the mapping of FEMBchans to the histogram.
  int PDHDTPCMonitor::FEMBchanToHistogramMap(int FEMBchan, int coord){
    //to see the reason for this channel mapping, check DocDB 4064 Table 5
    //for one FEMB, this dictates the coordinates on the histogram as a 4X32 block.
    int FEMBchanToHistogram[128][2] = { {0,0},{0,1},{0,2},{0,3},{0,4},//for U
                                        {0,10},{0,11},{0,12},{0,13},{0,14},//for V
                                        {0,20},{0,21},{0,22},{0,23},{0,24},{0,25},//for Z
                                        {0,5},{0,6},{0,7},{0,8},{0,9},//for U
                                        {0,15},{0,16},{0,17},{0,18},{0,19},//for V
                                        {0,26},{0,27},{0,28},{0,29},{0,30},{0,31},//for Z
                                        {1,20},{1,21},{1,22},{1,23},{1,24},{1,25},//for Z
                                        {1,10},{1,11},{1,12},{1,13},{1,14},//for V
                                        {1,0},{1,1},{1,2},{1,3},{1,4},//for U
                                        {1,26},{1,27},{1,28},{1,29},{1,30},{1,31},//for Z
                                        {1,15},{1,16},{1,17},{1,18},{1,19},//for V
                                        {1,5},{1,6},{1,7},{1,8},{1,9},//for U
                                        {2,0},{2,1},{2,2},{2,3},{2,4},//for U
                                        {2,10},{2,11},{2,12},{2,13},{2,14},//for V
                                        {2,20},{2,21},{2,22},{2,23},{2,24},{2,25},//for Z
                                        {2,5},{2,6},{2,7},{2,8},{2,9},//for U
                                        {2,15},{2,16},{2,17},{2,18},{2,19},//for V
                                        {2,26},{2,27},{2,28},{2,29},{2,30},{2,31},//for Z
                                        {3,20},{3,21},{3,22},{3,23},{3,24},{3,25},//for Z
                                        {3,10},{3,11},{3,12},{3,13},{3,14},//for V
                                        {3,0},{3,1},{3,2},{3,3},{3,4},//for U
                                        {3,26},{3,27},{3,28},{3,29},{3,30},{3,31},//for Z
                                        {3,15},{3,16},{3,17},{3,18},{3,19},//for V
                                        {3,5},{3,6},{3,7},{3,8},{3,9} };//for U
    return FEMBchanToHistogram[FEMBchan][coord];
  }
}


DEFINE_ART_MODULE(tpc_monitor::PDHDTPCMonitor)
