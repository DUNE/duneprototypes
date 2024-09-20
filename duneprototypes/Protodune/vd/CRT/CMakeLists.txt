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
 			art::Persistency_Common
			art::Persistency_Provenance
			art::Utilities canvas::canvas
			cetlib::cetlib cetlib_except::cetlib_except
			lardataobj::AnalysisBase
			larcorealg::Geometry
                        larcorealg::GeoAlgo
                        larcore::headers
			ROOT::Core ROOT::Hist ROOT::Tree
                        larcoreobj::headers
                        larsim::MCCheater_ParticleInventoryService_service
           DICT_LIBRARIES
                        larcorealg::GeoAlgo
                        lardataobj::Simulation
                        nusimdata::SimulationBase
)

install_headers()
install_source()
install_fhicl()
