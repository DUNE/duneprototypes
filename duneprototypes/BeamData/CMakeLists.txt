
FILE (GLOB src_files *.cxx)

art_make_library(  LIBRARY_NAME  BeamData
		             SOURCE ${src_files}
			     LIBRARIES			
	                wda::wda
			ifdh::ifdh
			ifbeam::ifbeam
#			IFBeam_service
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
			fhiclcpp::fhiclcpp
                        ROOT::Core ROOT::Hist ROOT::Tree
			
        )

#art_make_exec( NAME RunInfoDUNE
#               SOURCE RunInfoDUNE.cc                 
#               LIBRARIES
#			RunHistoryDUNE
                         #wda
			 # nuevdb::IFDatabase
#               )

#art_make_exec( NAME getDUNEPedestals
#               SOURCE getDUNEPedestals.cc                 
#               LIBRARIES
#			RunHistoryDUNE
#               )


install_headers()
install_fhicl()
install_source()

#add_subdirectory(test)
#add_subdirectory(ProtoDUNEBeamSpill)
