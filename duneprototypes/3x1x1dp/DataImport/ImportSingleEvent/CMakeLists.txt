cet_build_plugin(ImportSingle311Event art::module
			LIBRARIES
                        EventDecoder_service

			lardataobj::RawData
			lardataobj::RecoBase
			larreco::RecoAlg
			larreco::Calorimetry
			larreco::RecoAlg_PMAlg
			larcorealg::Geometry
			larcore::Geometry_Geometry_service
			lardata::Utilities
			larsim::Simulation nug4::ParticleNavigation lardataobj::Simulation
			
			nusimdata::SimulationBase
			art::Framework_Core
			art::Framework_Principal
			art::Framework_Services_Registry
			art_root_io::tfile_support
			ROOT::Core
			art_root_io::TFileService_service
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			messagefacility::MF_MessageLogger
			ROOT::Core ROOT::Hist ROOT::Tree
			BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
install_scripts()
