#ifndef TRACKER_H
#define TRACKER_H

#include "frame.h"
#include "thread_safe_queue.h"
#include <opencv2/opencv.hpp>
#include <unordered_map>

class Tracker {
public:
    Tracker(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output);
    void run();
    bool getProcessedFrame(Frame& frame);

private:
    ThreadSafeQueue<Frame>& inputQueue;
    ThreadSafeQueue<Frame>& outputQueue;

    class Track {
    public:
        Track() : rect(0, 0, 0, 0), trackId(-1), timeSinceUpdate(0) {} // Default constructor
        Track(const cv::Rect& initialRect, int id);
        void predict();
        void update(const cv::Rect& newRect);

        cv::Rect rect;
        int trackId;
        int timeSinceUpdate;
    };

    std::unordered_map<int, Track> tracks;
    int nextTrackID;

    void updateTracks(Frame& frame);
    float calculateIoU(const cv::Rect& box1, const cv::Rect& box2);
};

#endif // TRACKER_H