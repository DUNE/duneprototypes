art_make( 
  LIB_LIBRARIES 
  lardataobj::RecoBase
  larana::OpticalDetector_OpHitFinder
  larsim::MCSTReco
  lardataobj::MCBase
  larcorealg::Geometry
  larcore::Geometry_Geometry_service
  larsim::PhotonPropagation_PhotonVisibilityService_service
  larana::OpticalDetector_OpDigiProperties_service
  lardata::Utilities
  nusimdata::SimulationBase
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art_root_io::tfile_support
  ROOT::Core
  art::Persistency_Common
  art::Utilities
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib::cetlib cetlib_except::cetlib_except
  ROOT::Core ROOT::Hist ROOT::Tree

  MODULE_LIBRARIES larana::OpticalDetector_OpDigiProperties_service
  larana::OpticalDetector_OpHitFinder
  larana::OpticalDetector
  larsim::PhotonPropagation_PhotonVisibilityService_service
  larsim::MCCheater_BackTrackerService_service
  larsim::MCCheater_ParticleInventoryService_service
  larevt::Filters
  lardata::RecoObjects
  lardataalg::DetectorInfo
  lardataobj::RecoBase
  lardataobj::MCBase
  larreco::RecoAlg
  larcorealg::Geometry
  larcore::Geometry_Geometry_service
  larsim::Simulation nug4::ParticleNavigation lardataobj::Simulation
  lardata::Utilities
  lardataobj::RawData
  larreco::Calorimetry
  lardataobj::AnalysisBase
  nurandom::RandomUtils_NuRandomService_service
  nusimdata::SimulationBase
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art_root_io::tfile_support
  ROOT::Core  
  art::Persistency_Provenance
  art::Persistency_Common
  art::Utilities
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib::cetlib cetlib_except::cetlib_except
  ROOT::Core ROOT::Hist ROOT::Tree

  SERVICE_LIBRARIES
  larcorealg::Geometry
  larcore::Geometry_Geometry_service
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art_root_io::tfile_support
  ROOT::Core
  
  art::Persistency_Common
  art::Utilities
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib::cetlib cetlib_except::cetlib_except
  CLHEP::CLHEP
  ROOT::Core ROOT::Hist ROOT::Tree
  )

install_headers()
install_fhicl()
install_source()
#install_fw( LIST toyWaveform.txt)
#SUBDIRNAME (OpticalDetector )
