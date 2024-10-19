/**
 * @file image_process.h
 * @brief Defines the ImageProcessor class for various image processing operations
 */

#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <onnxruntime_cxx_api.h>

/**
 * @class ImageProcessor
 * @brief Static class providing various image processing methods
 */
class ImageProcessor {
public:
    /**
     * @brief Resize an image to a target width and height
     * @param frame Input image
     * @param targetWidth Desired width of the output image
     * @param targetHeight Desired height of the output image
     * @return Resized image
     */
    static cv::Mat resize(const cv::Mat& frame, int targetWidth, int targetHeight) {
        cv::Mat resizedFrame;
        cv::resize(frame, resizedFrame, cv::Size(targetWidth, targetHeight), 0, 0, cv::INTER_LINEAR);
        return resizedFrame;
    }

    /**
     * @brief Normalize an image by scaling pixel values to the range [0, 1]
     * @param frame Input image
     * @return Normalized image
     */
    static cv::Mat normalize(const cv::Mat& frame) {
        cv::Mat normalizedFrame;
        frame.convertTo(normalizedFrame, CV_32F, 1.0 / 255.0);
        return normalizedFrame;
    }

    /**
     * @brief Process a frame by resizing and normalizing (currently returns a clone of the input)
     * @param frame Input image
     * @param targetWidth Desired width of the output image
     * @param targetHeight Desired height of the output image
     * @return Processed image (currently a clone of the input)
     */
    static cv::Mat processFrame(const cv::Mat& frame, int targetWidth, int targetHeight) {
        // cv::Mat resizedFrame = resize(frame, targetWidth, targetHeight);
        // cv::Mat normalizedFrame = normalize(resizedFrame);
        // return normalizedFrame;
        return frame.clone();
    }

    /**
     * @brief Preprocess an image for ONNX model input
     * @param input_image Input image
     * @param memory_info ONNX runtime memory info
     * @param input_node_dims Dimensions of the input node
     * @return ONNX Value containing the preprocessed image data
     */
    static Ort::Value preprocessForONNX(const cv::Mat& input_image, const Ort::MemoryInfo& memory_info, const std::vector<int64_t>& input_node_dims) {
        // Ensure input_image is already resized to the correct dimensions (640x640)
        // input_node_dims[3]: width; input_node_dims[2]: height

        // Create blob from image
        cv::Mat blob = cv::dnn::blobFromImage(input_image, 1.0/255.0, cv::Size(input_node_dims[3], input_node_dims[2]), cv::Scalar(0, 0, 0), false, false);

        size_t input_tensor_size = blob.total() * blob.elemSize();

        // Create tensor
        auto tensor = Ort::Value::CreateTensor<float>(
            memory_info,
            reinterpret_cast<float*>(blob.data),
            input_tensor_size / sizeof(float),
            input_node_dims.data(),
            input_node_dims.size()
        );

        return tensor;
    }
};