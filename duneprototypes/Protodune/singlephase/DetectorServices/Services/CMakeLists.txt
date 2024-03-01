cet_build_plugin(DetectorPropertiesServiceProtoDUNEsp art::service  LIBRARIES
	          ProtoDUNEspDataProviders
              lardataalg::DetectorInfo
              larcore::Geometry_Geometry_service
              larcorealg::Geometry
              art::Framework_Principal
              ifdh::ifdh
	      art_root_io::RootDB
	      art::Framework_Services_Registry
	      art::Utilities
	      canvas::canvas
	      SQLite::SQLite3
	      messagefacility::MF_MessageLogger
	      cetlib::cetlib cetlib_except::cetlib_except
	      ROOT::Core ROOT::Hist ROOT::Tree
              
)

install_headers()
install_fhicl()
install_source()
