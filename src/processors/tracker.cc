#include "tracker.h"
#include "onnx_model.h"
#include "logger.h"
#include <algorithm>

extern std::atomic<bool> shouldExit;

Tracker::Tracker(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output)
    : inputQueue(input), outputQueue(output), nextTrackID(1), useFixedBoundingBox(false) {
    // The ONNX model initialization is handled in main.cc
}

void Tracker::run() {
    while (!shouldExit) {
        Frame frame;
        if (inputQueue.pop(frame)) {
            if (frame.processed.empty()) { 
                LOG_ERROR("Frame is empty"); 
                continue; 
            }

            if (useFixedBoundingBox) {
                BoundingBox detectedBox = createFixedBoundingBox(frame.processed);
                frame.detections.clear();
                frame.detections.push_back(detectedBox);
            } else {
                // Perform object detection using the ONNX model
                std::vector<cv::Rect> detections = ONNXModel::getInstance().detect(frame.processed);

                // Update tracks
                updateTracks(detections, frame.processed.size());

                // Prepare detections with tracking IDs
                frame.detections.clear();
                for (const auto& track : tracks) {
                    BoundingBox box;
                    box.topLeft = cv::Point(track.second.rect.x, track.second.rect.y);
                    box.bottomRight = cv::Point(track.second.rect.x + track.second.rect.width, 
                                                track.second.rect.y + track.second.rect.height);
                    box.id = track.first;
                    frame.detections.push_back(box);
                }
            }

            outputQueue.push(std::move(frame));
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

BoundingBox Tracker::createFixedBoundingBox(const cv::Mat& image) {
    int width = image.cols;
    int height = image.rows;

    // Calculate the top-left corner of the central 50x50 pixel rectangle
    int x = (width - 50) / 2;
    int y = (height - 50) / 2;

    // Create a BoundingBox for the central 50x50 pixel rectangle
    BoundingBox central_box;
    central_box.topLeft = cv::Point(x, y);
    central_box.bottomRight = cv::Point(x + 50, y + 50);

    return central_box;
}

void Tracker::setUseFixedBoundingBox(bool use) {
    useFixedBoundingBox = use;
}