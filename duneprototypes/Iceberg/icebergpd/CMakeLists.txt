cet_build_plugin(ICEBERGPDSSPMonitor art::module
              larcorealg_Geometry
              larcore_Geometry_Geometry_service
              lardataobj_RawData
              nusimdata::SimulationBase
              dunepdlegacy_Services_ChannelMap_IcebergChannelMapService_service
              ART_FRAMEWORK_CORE
              ART_FRAMEWORK_PRINCIPAL
              ART_FRAMEWORK_SERVICES_REGISTRY
              ART_ROOT_IO_TFILE_SUPPORT
              ROOT::Core
              ART_ROOT_IO_TFILESERVICE_SERVICE
              ART_PERSISTENCY_PROVENANCE
              messagefacility::MF_MessageLogger
              CETLIB
              ROOT_BASIC_LIB_LIST
              BASENAME_ONLY)

install_headers()
install_fhicl()
install_source()
install_scripts()
