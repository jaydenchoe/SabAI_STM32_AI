/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_CONSTANTS_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_CONSTANTS_H_

#define K_PREDICTION_HISTORY_LENGTH 5
#define K_GESTURE_COUNT 4
const float kDetectionThreshold = 0.75f;
const int kPredictionSuppressionDuration = 5;
// The expected accelerometer data sample frequency
//const float kTargetHz = 25;

// What gestures are supported.
const int kWingGesture = 0;
const int kRingGesture = 1;
const int kSlopeGesture = 2;
const int kNoGesture = 3;

// These control the sensitivity of the detection algorithm. If you're seeing
// too many false positives or not enough true positives, you can try tweaking
// these thresholds. Often, increasing the size of the training set will give
// more robust results though, so consider retraining if you are seeing poor
// predictions.
//const float kDetectionThreshold = 0.8f;
//const int kPredictionHistoryLength = 5;
//const int kPredictionSuppressionDuration = 25;


#endif  // TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_CONSTANTS_H_
