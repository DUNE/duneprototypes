# dunetpc/dune/Protodune/singlephase/Utility/CMakeLists.txt

art_make(BASENAME_ONLY
         LIB_LIBRARIES
         dunecore::ArtSupport
         dunecore::DuneCommon_Utility
         art::Utilities
         canvas::canvas
         cetlib::cetlib
         ROOT::Core ROOT::Hist ROOT::Tree
         NO_PLUGINS
        )

# Install the Root dictionary in the build and product lib directories.
#install(FILES TickModTreeData_Dict_rdict.pcm DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
#install(FILES TickModTreeData_Dict_rdict.pcm DESTINATION ${CMAKE_INSTALL_PREFIX}/${${product}_lib_dir})

install_source()
install_headers()

add_subdirectory(test)
