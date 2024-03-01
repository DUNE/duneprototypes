cet_build_plugin(RawData311InputDriver art::service LIBRARIES
			EventDecoder_service
			dlardaq_service

			lardataobj::RawData
			lardata::Utilities
			larcorealg::Geometry
			art::Framework_Core
		        art::Framework_IO_Sources	
			art::Framework_Principal
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			messagefacility::MF_MessageLogger
			
			cetlib::cetlib
			ROOT::Core ROOT::Hist ROOT::Tree
			BASENAME_ONLY
)

cet_build_plugin(dlardaq art::service LIBRARIES
			lardataobj::RawData
			lardataobj::RecoBase
			larreco::RecoAlg
			larreco::Calorimetry
			larreco::RecoAlg_PMAlg
			larcorealg::Geometry
			larcore::Geometry_Geometry_service
			lardata::Utilities
			larsim::Simulation nug4::ParticleNavigation lardataobj::Simulation
		        art::Framework_IO_Sources	
			
			nusimdata::SimulationBase
			art::Framework_Core
			art::Framework_Principal
			art::Framework_Services_Registry
			art_root_io::tfile_support
			ROOT::Core
			
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			messagefacility::MF_MessageLogger
			
			cetlib::cetlib
			ROOT::Core ROOT::Hist ROOT::Tree
			BASENAME_ONLY
)


cet_build_plugin(EventDecoder art::service LIBRARIES
			dlardaq_service
			HuffDataCompressor_service
			
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
			
		        art::Framework_IO_Sources	
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			messagefacility::MF_MessageLogger
			
			cetlib::cetlib
			ROOT::Core ROOT::Hist ROOT::Tree
			BASENAME_ONLY
)


cet_build_plugin(HuffDataCompressor art::service LIBRARIES
			dlardaq_service

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
			
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			messagefacility::MF_MessageLogger
			
		        art::Framework_IO_Sources	
			cetlib::cetlib
			ROOT::Core ROOT::Hist ROOT::Tree
			BASENAME_ONLY
)




install_headers()
install_fhicl()
install_source()
install_scripts()
