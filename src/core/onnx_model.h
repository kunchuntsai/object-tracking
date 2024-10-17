#ifndef ONNX_MODEL_H
#define ONNX_MODEL_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

class ONNXModel {
public:
    static ONNXModel& getInstance();

    bool loadModel(const std::string& model_path);
    std::vector<cv::Rect> detect(const Ort::Value& input_tensor, const cv::Size& original_image_size);

    const Ort::MemoryInfo& getMemoryInfo() const { return memory_info; }
    const std::vector<int64_t>& getInputNodeDims() const { return input_node_dims; }

private:
    ONNXModel();
    ~ONNXModel() = default;
    ONNXModel(const ONNXModel&) = delete;
    ONNXModel& operator=(const ONNXModel&) = delete;

    Ort::Env env;
    Ort::Session session{nullptr};
    Ort::AllocatorWithDefaultOptions allocator;

    std::vector<const char*> input_node_names;
    std::vector<const char*> output_node_names;

    std::vector<int64_t> input_node_dims;

    std::vector<cv::Rect> postprocess(const Ort::Value& output_tensor, const cv::Size& original_image_size);

    Ort::SessionOptions session_options;
    Ort::MemoryInfo memory_info{ nullptr };
};

#endif // ONNX_MODEL_H