/**
 * @file tracker.h
 * @brief Header file for the Tracker class, responsible for object tracking.
 */

#ifndef TRACKER_H
#define TRACKER_H

#include "frame.h"
#include "thread_safe_queue.h"
#include <opencv2/opencv.hpp>
#include <unordered_map>

/**
 * @class Tracker
 * @brief Implements object tracking functionality using a simple tracking algorithm.
 */
class Tracker {
public:
    /**
     * @brief Constructor for the Tracker class.
     * @param input Reference to the input queue of frames to be processed.
     * @param output Reference to the output queue where processed frames will be placed.
     */
    Tracker(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output);

    /**
     * @brief Main processing loop for the Tracker.
     *
     * This method continuously takes frames from the input queue, processes them for tracking,
     * and places the results in the output queue.
     */
    void run();

    /**
     * @brief Retrieve a processed frame with tracking information.
     * @param frame Reference to a Frame object where the processed frame will be stored.
     * @return bool True if a frame was successfully retrieved, false otherwise.
     */
    bool getProcessedFrame(Frame& frame);

private:
    ThreadSafeQueue<Frame>& inputQueue; ///< Reference to the input queue
    ThreadSafeQueue<Frame>& outputQueue; ///< Reference to the output queue

    /**
     * @class Track
     * @brief Represents a single tracked object.
     */
    class Track {
    public:
        /**
         * @brief Default constructor for Track.
         */
        Track() : rect(0, 0, 0, 0), trackId(-1), timeSinceUpdate(0) {}

        /**
         * @brief Constructor for Track with initial bounding box and ID.
         * @param initialRect Initial bounding box of the tracked object.
         * @param id Unique identifier for the track.
         */
        Track(const cv::Rect& initialRect, int id);

        /**
         * @brief Predict the new position of the track.
         */
        void predict();

        /**
         * @brief Update the track with a new bounding box.
         * @param newRect New bounding box for the tracked object.
         */
        void update(const cv::Rect& newRect);

        cv::Rect rect; ///< Current bounding box of the tracked object
        int trackId; ///< Unique identifier for the track
        int timeSinceUpdate; ///< Time elapsed since the last update
    };

    std::unordered_map<int, Track> tracks; ///< Map of active tracks
    int nextTrackID; ///< Next available track ID

    /**
     * @brief Update existing tracks with new frame information.
     * @param frame Frame containing new detection information.
     */
    void updateTracks(Frame& frame);

    /**
     * @brief Calculate the Intersection over Union (IoU) between two bounding boxes.
     * @param box1 First bounding box.
     * @param box2 Second bounding box.
     * @return float IoU value between 0 and 1.
     */
    float calculateIoU(const cv::Rect& box1, const cv::Rect& box2);
};

#endif // TRACKER_H