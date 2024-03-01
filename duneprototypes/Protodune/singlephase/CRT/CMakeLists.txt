add_subdirectory(data)
add_subdirectory(alg)

# temporary fix for an extra semicolon in the root v6_22_06a TVirtualPaveStats header
cet_add_compiler_flags(CXX -Wno-pedantic)

art_make( MODULE_LIBRARIES
                        lardataalg::DetectorInfo
                        lardataobj::RawData
                        lardata::headers
                        larevt::SpaceCharge
                        lardataalg::DetectorInfo
			larcorealg::Geometry
                        larcorealg::CoreUtils
			larcore::Geometry_Geometry_service
                        lardataobj::Simulation
                        nusimdata::SimulationBase
			lardataobj::RecoBase
			lardataobj::AnalysisBase
			larsim::MCCheater_ParticleInventoryService_service
			larsim::MCCheater_BackTrackerService_service
			larsim::MCCheater_PhotonBackTrackerService_service
			art::Framework_Core
			art::Framework_Principal
		        art_root_io::TFileService_service	
			art_root_io::tfile_support
			ROOT::Gpad
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			messagefacility::MF_MessageLogger
			cetlib::cetlib cetlib_except::cetlib_except
			ROOT::Core ROOT::Hist ROOT::Tree ROOT::Graf
                        ROOT::GenVector
                        ROOT::Geom
)

install_headers()
install_fhicl()
install_source()
install_scripts()
