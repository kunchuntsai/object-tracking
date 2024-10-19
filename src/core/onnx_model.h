/**
 * @file onnx_model.h
 * @brief Defines the ONNXModel class for ONNX model operations
 */

#ifndef ONNX_MODEL_H
#define ONNX_MODEL_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

/**
 * @class ONNXModel
 * @brief Singleton class for ONNX model operations
 */
class ONNXModel {
public:
    /**
     * @brief Get the singleton instance of ONNXModel
     * @return Reference to the ONNXModel instance
     */
    static ONNXModel& getInstance();

    /**
     * @brief Load an ONNX model from a file
     * @param model_path Path to the ONNX model file
     * @return true if the model was successfully loaded, false otherwise
     */
    bool loadModel(const std::string& model_path);

    /**
     * @brief Perform object detection using the loaded ONNX model
     * @param input_tensor Input tensor for the model
     * @param original_image_size Size of the original input image
     * @return Vector of detected object bounding boxes
     */
    std::vector<cv::Rect> detect(const Ort::Value& input_tensor, const cv::Size& original_image_size);

    /**
     * @brief Get the memory info for ONNX runtime
     * @return Reference to the Ort::MemoryInfo object
     */
    const Ort::MemoryInfo& getMemoryInfo() const { return memory_info; }

    /**
     * @brief Get the input node dimensions
     * @return Reference to the vector of input node dimensions
     */
    const std::vector<int64_t>& getInputNodeDims() const { return input_node_dims; }

private:
    ONNXModel();
    ~ONNXModel() = default;
    ONNXModel(const ONNXModel&) = delete;
    ONNXModel& operator=(const ONNXModel&) = delete;

    Ort::Env env; /**< ONNX runtime environment */
    Ort::Session session{nullptr}; /**< ONNX runtime session */
    Ort::AllocatorWithDefaultOptions allocator; /**< ONNX runtime allocator */

    std::vector<const char*> input_node_names; /**< Names of input nodes */
    std::vector<const char*> output_node_names; /**< Names of output nodes */

    std::vector<int64_t> input_node_dims; /**< Dimensions of input nodes */

    /**
     * @brief Post-process the output tensor to get bounding boxes
     * @param output_tensor Output tensor from the model
     * @param original_image_size Size of the original input image
     * @return Vector of detected object bounding boxes
     */
    std::vector<cv::Rect> postprocess(const Ort::Value& output_tensor, const cv::Size& original_image_size);

    Ort::SessionOptions session_options; /**< ONNX runtime session options */
    Ort::MemoryInfo memory_info{ nullptr }; /**< ONNX runtime memory info */
};

#endif // ONNX_MODEL_H