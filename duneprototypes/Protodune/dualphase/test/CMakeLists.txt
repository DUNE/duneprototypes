cet_build_plugin(		recobWireCheck art::module LIBRARIES
			lardataobj::RecoBase
                        lardata::ArtDataHelper
                        lardata::Utilities
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
                        
                        cetlib::cetlib cetlib_except::cetlib_except
                        BASENAME_ONLY
)

install_fhicl()
install_source()
