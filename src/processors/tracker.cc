#include "tracker.h"
#include "onnx_model.h"
#include "logger.h"
#include "config.h"
#include <algorithm>
#include <chrono>

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
                LOG_ERROR("[Tracker] Frame is empty");
                continue; 
            }

            LOG_DEBUG("[Tracker] Processing frame");

            // Perform object detection using the ONNX model
            if (frame.onnx_input.has_value()) {
                frame.detections = model.detect(frame.onnx_input.value(), frame.original.size());
                LOG_DEBUG("[Tracker] Detected objects: %zu", frame.detections.size());

                // Update tracks
                updateTracks(frame.detections, frame.processed.size());

                LOG_DEBUG("[Tracker] Updated tracks %zu", tracks.size());
            } else {
                LOG_ERROR("[Tracker] Frame has no ONNX input tensor");
                frame.detections.clear();
            }

            outputQueue.push(std::move(frame));

            auto end = std::chrono::high_resolution_clock::now();
            totalTrackerTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            LOG_DEBUG("[Tracker] Frame processed and pushed to output queue");
        }
    }
}

bool Tracker::getProcessedFrame(Frame& frame) {
    return outputQueue.pop(frame);
}

void Tracker::updateTracks(const std::vector<cv::Rect>& detections, const cv::Size& frameSize) {
    // Predict new locations of existing tracks
    for (auto& track : tracks) {
        track.second.predict();
    }

    // Associate detections with existing tracks
    std::vector<int> unassignedDetections;
    for (size_t i = 0; i < detections.size(); ++i) {
        bool assigned = false;
        for (auto& track : tracks) {
            if (calculateIoU(detections[i], track.second.rect) > 0.5) {
                track.second.update(detections[i]);
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
        tracks[nextTrackID] = Track(detections[i], nextTrackID);
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