// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Detector/TrackingVolume.hpp"
#include "Acts/EventData/detail/surface_getter.hpp"
#include "Acts/EventData/detail/trackstate_manipulation.hpp"
#include "Acts/Fitter/detail/VoidKalmanComponents.hpp"
#include "Acts/Layers/Layer.hpp"
#include "Acts/Surfaces/Surface.hpp"

namespace Acts {

/// @brief Propagator Actor plugin for the KalmanFilter
///
/// @tparam track_states_t is any iterable std::container of
/// boost::variant TrackState objects.
///
/// @tparam updator_t The Kalman updator used for this fitter
///
/// @tparam calibrator_t The Measurement calibrator for Fittable
/// measurements to be calibrated
///
/// The KalmanActor does not rely on the measurements to be
/// sorted along the track.
template <typename track_states_t,
          typename updator_t    = VoidKalmanUpdator,
          typename calibrator_t = VoidKalmanComponents>
class KalmanActor
{
public:
  /// Shorthand for readability
  using MeasurementSurfaces = std::multimap<const Layer*, const Surface*>;

  /// Explicit constructor with updagor and calibrator
  explicit KalmanActor(updator_t    pUpdator    = updator_t(),
                       calibrator_t pCalibrator = calibrator_t())
    : m_updator(std::move(pUpdator)), m_calibrator(std::move(pCalibrator))
  {
  }

  /// Simple result struct to be returned
  /// It mainly acts as an internal state state which is
  /// created for every propagation/extrapolation step
  struct this_result
  {
    // Move the result into the fitted states
    track_states_t fittedStates;

    // Counter for handled states
    size_t processedStates = 0;

    // The index map for accessing the track state in order
    std::map<const Surface*, size_t> accessIndex;
  };

  using result_type = this_result;

  track_states_t trackStates = {};

  /// @brief Kalman actor operation
  ///
  /// @tparam propagator_state_t is the type of Propagagor state
  ///
  /// @param state is the mutable propagator state object
  /// @param result is the mutable result state object
  template <typename propagator_state_t>
  void
  operator()(propagator_state_t& state, result_type& result) const
  {
    // Initialization of the Actor:
    // Only when track states are not set
    if (result.fittedStates.empty()) {
      // -> Move the TrackState vector
      // -> Feed the KalmanSequencer with the measurements to be fitted
      initialize(state, result);
    }

    // Waiting for a current surface that appears in the measurement list:
    auto surface = state.navigation.currentSurface;
    if (surface) {
      // Check if the surface is in the measurement map
      // -> Get the measurement / calibrate
      // -> Create the predicted state
      // -> Perform the kalman update
      // -> Check outlier behavior (@todo)
      // -> Fill strack state information & update stepper information
      update(surface, state, result);
    }

    // Smooth and throw a stop condition when all track states have been handled
    if (processedStates == trackStates.size()) }

private:
  /// @brief Kalman actor operation : update
  ///
  /// @tparam propagator_state_t is the type of Propagagor state
  ///
  /// @param surface The surface where the update happens
  /// @param state The mutable propagator state object
  /// @param result The mutable result state object
  template <typename propagator_state_t>
  void
  update(const Surface*      surface,
         propagator_state_t& state,
         result_type&        result) const
  {
    // Try to find the surface in the measurement surfaces
    auto cindexItr = result.accessIndex.find(surface);
    if (cindexItr != result.accessIndex.end()) {
      // Transport & bind the stateto the current surface
      auto boundState = state.stepping.bind(*surface, true);
      // Get the current VariantTrackState
      auto trackState = result.fittedStates[cindexItr->second];
      // Perform the update
      auto updPar = m_updator(trackState, boundState);
      // if the update is successful, set covariance and
      if (updPar) {
        const auto& updPos = updPar->position();
        const auto& updMom = updPar->momentum();
        state.stepping.update(updPos, updMom.normalized(), updMom.norm());
        state.stepping.cov = *(updPar->covariance());
      }
      // We count the processed state
      ++result.processedStates;
    }
  }

  /// @brief Kalman actor operation : initialize
  ///
  /// @tparam propagator_state_t is the type of Propagagor state
  ///
  /// @param state is the mutable propagator state object
  /// @param result is the mutable result state object
  template <typename propagator_state_t>
  void
  initialize(propagator_state_t& state, result_type& result) const
  {
    // Create the multimap
    MeasurementSurfaces measurementSurfaces;
    // Move the track states
    result.fittedStates = std::move(trackStates);
    // Memorize the index to access the state
    size_t stateIndex = 0;
    for (auto& tState : result.fittedStates) {
      // Get the Surface
      auto surface = &(detail::getSurface(tState));
      // Get the associated Layer to this Surface
      auto layer = surface->associatedLayer();
      if (layer == nullptr) {
        // Find the intersection to allocate the layer
        auto surfaceIntersection
            = surface->intersectionEstimate(state.stepping.position(),
                                            state.stepping.direction(),
                                            state.stepping.navDir,
                                            false);
        // Allocate the layer via the tracking geometry search
        if (surfaceIntersection and state.navigation.worldVolume) {
          auto intersection = surfaceIntersection.position;
          auto layerVolume
              = state.navigation.worldVolume->trackingVolume(intersection);
          layer = layerVolume ? layerVolume->associatedLayer(intersection)
                              : nullptr;
        }
      }
      // Insert the surface into the measurementsurfaces multimap
      if (layer) {
        measurementSurfaces.insert(
            std::pair<const Layer*, const Surface*>(layer, surface));
        // Insert the fitted state into the fittedStates map
        result.accessIndex[surface] = stateIndex;
      }
      ++stateIndex;
    }
    // Feed the KalmanSequencer with the measurement surfaces
    state.navigation.sequence.externalSurfaces = std::move(measurementSurfaces);
  }

  /// The private navigation debug logging
  ///
  /// It needs to be fed by a lambda function that returns a string,
  /// that guarantees that the lambda is only called in the
  /// state.options.debug == true
  /// case in order not to spend time when not needed.
  ///
  /// @tparam propagator_state_t Type of the propagator state
  ///
  /// @param[in,out] state the propagator state for the debug flag,
  ///      prefix and length
  /// @param logAction is a callable function that returns a stremable object
  template <typename propagator_state_t>
  void
  debugLog(propagator_state_t&                 state,
           const std::function<std::string()>& logAction) const
  {
    if (state.options.debug) {
      std::stringstream dstream;
      std::string       ds = (state.stepping.navDir == forward) ? "K->" : "<-K";
      dstream << ds << std::setw(state.options.debugPfxWidth);
      dstream << "KalmanActor"
              << " | ";
      dstream << std::setw(state.options.debugMsgWidth) << logAction() << '\n';
      state.options.debugString += dstream.str();
    }
  }

  /// The Kalman updator
  updator_t m_updator;

  /// The measuremetn calibrator
  calibrator_t m_calibrator;
};

}  // namespace Acts
