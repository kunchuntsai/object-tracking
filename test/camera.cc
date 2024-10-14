#include <iostream>
#include "camera.h"
#include "opencv2/highgui.hpp"

int runCamera() {
    try {
        Camera cam(0, 640, 480, 30.0);  // Use default camera (index 0)
        
        std::cout << "Camera opened successfully" << std::endl;
        std::cout << "Width: " << cam.getWidth() << std::endl;
        std::cout << "Height: " << cam.getHeight() << std::endl;
        std::cout << "FPS: " << cam.getFPS() << std::endl;

        cv::namedWindow("Camera Stream", cv::WINDOW_AUTOSIZE);

        while (true) {
            cv::Mat frame = cam.getFrame();
            cv::imshow("Camera Stream", frame);

            // Press 'q' to quit
            if (cv::waitKey(1) == 'q') {
                break;
            }
        }

        cv::destroyAllWindows();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}