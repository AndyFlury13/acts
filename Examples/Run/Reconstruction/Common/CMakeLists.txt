add_library(
  ActsExamplesRecTracksCommon SHARED
  RecInput.cpp
  RecTruthTracks.cpp
  RecCKFTracks.cpp
  RecCKFTracksFastrack.cpp
  SeedingExample.cpp
  fastrack/lib/ModelClass.cpp
  fastrack/lib/AlgorithmParameters.cpp
  fastrack/lib/ClusterMaker.cpp
  fastrack/lib/FastrackGeometry.cpp
  fastrack/lib/FastTrackExtrapolator.cpp
  fastrack/lib/FastTrackFitter.cpp
  fastrack/lib/FieldModel.cpp
  fastrack/lib/HitBank.cpp
  fastrack/lib/HitBin.cpp
  fastrack/lib/HitLayer.cpp
  fastrack/lib/LayerLinker.cpp
  fastrack/lib/Model.cpp
  fastrack/lib/Track.cpp
  fastrack/lib/TrackFinder.cpp
  fastrack/lib/TrackingFilter.cpp)

target_include_directories(
  ActsExamplesRecTracksCommon
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(
  ActsExamplesRecTracksCommon
  PUBLIC
    ActsExamplesFramework
    ActsExamplesCommon
    ActsExamplesDigitization
    ActsExamplesTrackFitting
    ActsExamplesTrackFinding
    ActsExamplesMagneticField
    ActsExamplesTruthTracking
    ActsExamplesIoCsv
    ActsExamplesIoPerformance)
set(CMAKE_CXX_FLAGS "-fopenmp")

if(ACTS_BUILD_PLUGIN_ONNX)
  target_compile_definitions(
    ActsExamplesRecTracksCommon PUBLIC ACTS_PLUGIN_ONNX)
  target_link_libraries(
    ActsExamplesRecTracksCommon PUBLIC ActsPluginOnnx)
endif()

install(
  TARGETS ActsExamplesRecTracksCommon
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
