cet_build_plugin(IcebergTpcMonitor art::module LIBRARIES
              larcorealg::Geometry
              larcore::Geometry_Geometry_service
              lardataalg::DetectorInfo
              lardataobj::RawData
              lardata::headers
              nusimdata::SimulationBase
              dunepdlegacy::Services_ChannelMap_IcebergChannelMapService_service
              dunecore::DuneObj
              art::Framework_Core
              art::Framework_Principal
              art::Framework_Services_Registry
              art_root_io::tfile_support
              ROOT::Core
              
              art::Persistency_Provenance
              messagefacility::MF_MessageLogger
              cetlib::cetlib
              ROOT::Core ROOT::Hist ROOT::Tree
              BASENAME_ONLY)
install_headers()
install_fhicl()
install_source()
install_scripts()
