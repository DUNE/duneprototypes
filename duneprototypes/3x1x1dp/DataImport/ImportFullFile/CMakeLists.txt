cet_build_plugin(ImportFull311File art::source
                        LIBRARIES
			RawData311InputDriver_service

			lardataobj::RawData
			lardata::Utilities
			art::Framework_Core
			art::Framework_IO_Sources
			art::Framework_Principal
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			fhiclcpp::fhiclcpp
			messagefacility::MF_MessageLogger
			ROOT::Core ROOT::Hist ROOT::Tree
			BASENAME_ONLY
)


install_headers()
install_fhicl()
install_source()
install_scripts()
