cet_build_plugin(PDHDTPCMonitor art::module
              LIBRARIES
              lardataobj::RawData
              lardata::headers
              nusimdata::SimulationBase
              duneprototypes::Protodune_hd_ChannelMap_PD2HDChannelMapService_service
              dunecore::DuneObj
              art::Framework_Core
              art::Framework_Principal
              art_root_io::tfile_support
              ROOT::Core
              art::Persistency_Provenance
              messagefacility::MF_MessageLogger
              cetlib::cetlib
              ROOT::Core ROOT::Hist ROOT::Tree
              BASENAME_ONLY)

install_fhicl()
install_source()
