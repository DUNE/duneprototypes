include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

art_make( MODULE_LIBRARIES
                        lardataobj::Simulation
                        lardata::headers
                        nusimdata::SimulationBase
			art::Framework_Core
			art::Framework_Principal
			art::Framework_Services_Registry
			art_root_io::tfile_support
			ROOT::Core
<<<<<<< HEAD
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			cetlib::cetlib cetlib_except::cetlib_except
			lardataobj::AnalysisBase
			larcorealg::Geometry
                        larcore::headers
			ROOT::Core ROOT::Hist ROOT::Tree
                        larcoreobj::headers
                        larsim::MCCheater_ParticleInventoryService_service
           DICT_LIBRARIES
                        lardataobj::Simulation
                        nusimdata::SimulationBase
=======
 			art::Persistency_Common canvas
			art::Persistency_Provenance canvas
			art::Utilities canvas
			cetlib::cetlib cetlib_except
			lardataobj_AnalysisBase
			ROOT_BASIC_LIB_LIST
                        larcoreobj_SimpleTypesAndConstants
                        larcorealg::GeoAlgo
>>>>>>> 82760826859464f08cb018489c16009e21304cea
)

install_headers()
install_source()
install_fhicl()
