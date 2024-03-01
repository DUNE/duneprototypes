
art_make( BASENAME_ONLY
          MODULE_LIBRARIES lardataobj::RecoBase
                           lardataalg::DetectorInfo
                           lardataobj::AnalysisBase
                           larcorealg::Geometry
                           larcore::Geometry_Geometry_service
                           lardata::Utilities
                           larsim::Simulation
			   lardataobj::Simulation
                           larsim::MCCheater_BackTrackerService_service
                           larsim::MCCheater_ParticleInventoryService_service
                           nusimdata::SimulationBase
                           art::Framework_Core
                           art::Framework_Principal
                           art::Framework_Services_Registry
                           art_root_io::tfile_support
			   ROOT::Core
                           art::Persistency_Provenance
                           art::Persistency_Common
                           art::Utilities canvas::canvas
                           messagefacility::MF_MessageLogger
                           fhiclcpp::fhiclcpp
                           cetlib::cetlib cetlib_except::cetlib_except
                           ROOT::Core ROOT::Hist ROOT::Tree
	   		   #ProtoDUNEDataUtils
         )

install_headers()
install_fhicl()
install_source()

