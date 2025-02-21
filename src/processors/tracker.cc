#include "tracker.h"
#include "onnx_model.h"
#include "logger.h"
#include "config.h"
#include <algorithm>
#include <chrono>
#include <atomic>

extern std::atomic<bool> shouldExit;
extern std::atomic<long long> totalTrackerTime;

Tracker::Tracker(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output)
    : inputQueue(input), outputQueue(output), nextTrackID(1) {
}

void Tracker::run() {
    ONNXModel& model = ONNXModel::getInstance();
    
    while (!shouldExit) {
        Frame frame;
        if (inputQueue.pop(frame)) {
            auto start = std::chrono::high_resolution_clock::now();

            if (frame.processed.empty()) { 
                LOG_ERROR("[Tracker] Frame.processed is empty");
                continue; 
            }

            if (!frame.onnx_input.has_value()) {
                LOG_ERROR("[Tracker] Frame has no ONNX input tensor");
                frame.detections.clear();
                continue;
            }

            // Perform object detection using the ONNX model
            auto detect_start = std::chrono::high_resolution_clock::now();
            frame.detections = model.detect(frame.onnx_input.value(), frame.original.size());
            auto detect_end = std::chrono::high_resolution_clock::now();
            auto detect_time = std::chrono::duration_cast<std::chrono::nanoseconds>(detect_end - detect_start).count();
            LOG_DEBUG("[Tracker] ONNX detection time: %.3f ms", detect_time / 1e6);

            // Update tracks and associate track IDs with detections
            auto update_start = std::chrono::high_resolution_clock::now();
            updateTracks(frame);
            auto update_end = std::chrono::high_resolution_clock::now();
            auto update_time = std::chrono::duration_cast<std::chrono::nanoseconds>(update_end - update_start).count();
            LOG_DEBUG("[Tracker] Track update time: %.3f ms", update_time / 1e6);

            outputQueue.push(std::move(frame));

            auto end = std::chrono::high_resolution_clock::now();
            auto total_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            LOG_DEBUG("[Tracker] Frame processing time: %.3f ms", total_time / 1e6);

            // Update the totalTrackerTime
            totalTrackerTime += total_time;
        }
    }
}


bool Tracker::getProcessedFrame(Frame& frame) {
    return outputQueue.pop(frame);
}

void Tracker::updateTracks(Frame& frame) {
    // Predict new locations of existing tracks
    for (auto& track : tracks) {
        track.second.predict();
    }

    // Associate detections with existing tracks
    std::vector<int> unassignedDetections;
    frame.trackIDs.clear();
    frame.trackIDs.resize(frame.detections.size(), -1);  // Initialize with -1 (no track)

    for (size_t i = 0; i < frame.detections.size(); ++i) {
        bool assigned = false;
        for (auto& track : tracks) {
            if (calculateIoU(frame.detections[i], track.second.rect) > 0.5) {
                track.second.update(frame.detections[i]);
                frame.trackIDs[i] = track.first;  // Assign track ID to detection
                assigned = true;
                break;
            }
        }
        if (!assigned) {
            unassignedDetections.push_back(i);
        }
    }

    // Create new tracks for unassigned detections
    for (int i : unassignedDetections) {
        tracks[nextTrackID] = Track(frame.detections[i], nextTrackID);
        frame.trackIDs[i] = nextTrackID;  // Assign new track ID to detection
        nextTrackID++;
    }

    // Remove old tracks
    for (auto it = tracks.begin(); it != tracks.end();) {
        if (it->second.timeSinceUpdate > 10) {
            it = tracks.erase(it);
        } else {
            ++it;
        }
    }
}

float Tracker::calculateIoU(const cv::Rect& box1, const cv::Rect& box2) {
    int x1 = std::max(box1.x, box2.x);
    int y1 = std::max(box1.y, box2.y);
    int x2 = std::min(box1.x + box1.width, box2.x + box2.width);
    int y2 = std::min(box1.y + box1.height, box2.y + box2.height);

    if (x2 <= x1 || y2 <= y1) return 0.0f;

    float intersectionArea = (x2 - x1) * (y2 - y1);
    float unionArea = box1.area() + box2.area() - intersectionArea;

    return intersectionArea / unionArea;
}

// Track class implementation
Tracker::Track::Track(const cv::Rect& initialRect, int id) 
    : rect(initialRect), trackId(id), timeSinceUpdate(0) {}

void Tracker::Track::predict() {
    // Simple prediction: assume the object stays in the same place
    timeSinceUpdate++;
}

void Tracker::Track::update(const cv::Rect& newRect) {
    rect = newRect;
    timeSinceUpdate = 0;
}