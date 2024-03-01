cet_build_plugin(PDSPmatch art::module LIBRARIES
                        lardataobj::RawData
                        lardataobj::RecoBase
                        dunepdlegacy::Overlays
                        artdaq_core::artdaq-core_Data
                        dunecore::Geometry
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_RegistrY
                        art_root_io::tfile_support
                        ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        dunepdlegacy::dunepdlegacy_rce_dataaccess
                        BASENAME_ONLY
)

cet_build_plugin(PDSPmatchmc art::module LIBRARIES
                        lardataobj::RawData
                        lardataobj::RecoBase
                        dunepdlegacy::Overlays
                        artdaq_core::artdaq-core_Data
                        dunecore::Geometry
                        nusimdata::SimulationBase
                        larsim_MCCheater_PhotonBackTrackerService_service
                        larsim_MCCheater_ParticleInventoryService_service
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support
                        ROOT::Core
			art_root_io::TFileService_service
			art::Persistency_Provenance
                        messagefacility::MF_MessageLogger
                        dunepdlegacy::dunepdlegacy_rce_dataaccess
                        BASENAME_ONLY
)


install_fhicl()
install_source()
install_scripts()
