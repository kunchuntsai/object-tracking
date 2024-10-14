#include "video.h"
#include <iostream>
#include <string>

int runVideo(const std::string& videoPath) {
    try {
        Video reader(videoPath);
        
        std::cout << "Total frames: " << reader.getFrameCount() << std::endl;
        std::cout << "FPS: " << reader.getFPS() << std::endl;
        std::cout << "Resolution: " << reader.getWidth() << "x" << reader.getHeight() << std::endl;

        // Example: Get the 50th frame
        cv::Mat frame50 = reader.getFrame(50);
        if (!frame50.empty()) {
            cv::imshow("Frame 50", frame50);
            cv::waitKey(0);
        }

        // Example: Iterate through all frames
        cv::Mat frame;
        int frameCount = 0;
        while (reader.readNextFrame(frame)) {
            // Process frame for object tracking
            // (Add your object tracking code here)

            // Display every 30th frame (adjust as needed)
            if (frameCount % 30 == 0) {
                cv::imshow("Video", frame);
                if (cv::waitKey(1) == 'q') {
                    break;
                }
            }
            frameCount++;
        }

        cv::destroyAllWindows();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}