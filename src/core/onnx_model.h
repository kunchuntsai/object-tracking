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
    std::vector<cv::Rect> detect(const cv::Mat& image);

private:
    ONNXModel() = default;
    ~ONNXModel() = default;
    ONNXModel(const ONNXModel&) = delete;
    ONNXModel& operator=(const ONNXModel&) = delete;

    Ort::Env env;
    Ort::Session session{nullptr};
    Ort::AllocatorWithDefaultOptions allocator;

    std::vector<const char*> input_node_names;
    std::vector<const char*> output_node_names;

    std::vector<int64_t> input_node_dims;
    std::vector<int64_t> output_node_dims;

    void preprocess(const cv::Mat& input_image, std::vector<float>& input_tensor);
    std::vector<cv::Rect> postprocess(const std::vector<float>& output_tensor, const cv::Size& original_image_size);
};

#endif // ONNX_MODEL_H