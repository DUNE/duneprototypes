include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

art_make( MODULE_LIBRARIES
                        lardataobj_Simulation
                        nusimdata::SimulationBase
			ART_FRAMEWORK_CORE
			ART_FRAMEWORK_PRINCIPAL
			ART_FRAMEWORK_SERVICES_REGISTRY
			ART_ROOT_IO_TFILE_SUPPORT
			ROOT::Core
 			art::Persistency_Common canvas
			art::Persistency_Provenance canvas
			art::Utilities canvas
			cetlib::cetlib cetlib_except
			lardataobj_AnalysisBase
			ROOT_BASIC_LIB_LIST
                        larcoreobj_SimpleTypesAndConstants
                        larcorealg::GeoAlgo
)

install_headers()
install_source()
install_fhicl()
