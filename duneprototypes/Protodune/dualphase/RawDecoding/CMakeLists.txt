cet_build_plugin(PDDPChannelMap art::service LIBRARIES
                        art::Framework_Core
			art::Framework_IO_Sources
                        art::Framework_Principal
                        art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			fhiclcpp::fhiclcpp
                        messagefacility::MF_MessageLogger
                        cetlib::cetlib
                        ROOT::Core ROOT::Hist ROOT::Tree
                        BASENAME_ONLY
)

cet_build_plugin(PDDPRawInput art::source LIBRARIES
			PDDPRawInputDriver_service
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
                        cetlib::cetlib
                        ROOT::Core ROOT::Hist ROOT::Tree
                        BASENAME_ONLY
)

cet_build_plugin(PDDPRawInputDriver art::service LIBRARIES
			PDDPChannelMap_service
			pthread
			lardataobj::RawData
                        lardata::Utilities
                        dunecore::DuneObj
                        art::Framework_Core
			art::Framework_IO_Sources
                        art::Framework_Principal
                        art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			fhiclcpp::fhiclcpp
                        messagefacility::MF_MessageLogger
                        cetlib::cetlib
                        ROOT::Core ROOT::Hist ROOT::Tree
                        BASENAME_ONLY
)

cet_build_plugin(PDDPTPCRawDecoderTest art::module LIBRARIES
			lardataobj::RawData
                        art::Framework_Core
			art::Framework_Principal
			art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
                        
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			messagefacility::MF_MessageLogger
			
			cetlib::cetlib cetlib_except::cetlib_except
			ROOT::Core ROOT::Hist ROOT::Tree
              		BASENAME_ONLY
)


install_headers()
install_fhicl()
install_source()
install_scripts()

