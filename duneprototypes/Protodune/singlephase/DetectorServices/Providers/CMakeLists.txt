cet_enable_asserts()
art_make( NO_PLUGINS
          LIBRARY_NAME  ProtoDUNEspDataProviders
          LIB_LIBRARIES
                        lardataalg::DetectorInfo
                        lardata::headers
                        larcore::headers
  	                wda::wda
	                nuevdb::IFDatabase
                        larcorealg::Geometry
                        art::Framework_Core
                        art::Framework_Principal
                        art_root_io::RootDB
                        art::Utilities
                        messagefacility::MF_MessageLogger
                        art::Framework_Services_Registry
                        cetlib::cetlib cetlib_except::cetlib_except
                        canvas::canvas
                        ifdh::ifdh
                        ifdh_art::IFBeam_service
                        fhiclcpp::fhiclcpp
                        ROOT::Core ROOT::Hist ROOT::Tree
                        
        )


install_headers()
install_fhicl()
install_source()
