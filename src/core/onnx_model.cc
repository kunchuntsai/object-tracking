#include "onnx_model.h"
#include "logger.h"
#include "config.h"
#include <opencv2/dnn/dnn.hpp>

// Define fixed input dimensions, as the model's expected input
// This should be retrieved from model.getInputNodeDims()
const int INPUT_WIDTH = 640;
const int INPUT_HEIGHT = 640;

ONNXModel::ONNXModel() : env(ORT_LOGGING_LEVEL_WARNING, "ONNXModel") {}

ONNXModel& ONNXModel::getInstance() {
    static ONNXModel instance;
    return instance;
}

bool ONNXModel::loadModel(const std::string& model_path) {
    try {
        session_options.SetInterOpNumThreads(1);
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_DISABLE_ALL);

        session = Ort::Session(env, model_path.c_str(), session_options);

        memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

        input_node_names = {"images"};
        output_node_names = {"output"};
        input_node_dims = {1, 3, INPUT_HEIGHT, INPUT_WIDTH};

        LOG_INFO("ONNX model loaded successfully with input dimensions: %dx%d", INPUT_WIDTH, INPUT_HEIGHT);
        return true;
    } catch (const Ort::Exception& e) {
        LOG_ERROR("Error loading ONNX model: %s", e.what());
        return false;
    }
}

std::vector<cv::Rect> ONNXModel::detect(const Ort::Value& input_tensor, const cv::Size& original_image_size) {
    std::vector<Ort::Value> input_tensors;
    input_tensors.push_back(std::move(const_cast<Ort::Value&>(input_tensor)));

    std::vector<Ort::Value> output_tensors;
    try {
        output_tensors = session.Run(Ort::RunOptions{nullptr}, input_node_names.data(), input_tensors.data(), input_tensors.size(), output_node_names.data(), output_node_names.size());
    } catch (const Ort::Exception& e) {
        LOG_ERROR("Error during inference: %s", e.what());
        return std::vector<cv::Rect>();
    }

    return postprocess(output_tensors.front(), original_image_size);
}

std::vector<cv::Rect> ONNXModel::postprocess(const Ort::Value& output_tensor, const cv::Size& original_image_size) {
    std::vector<cv::Rect> bounding_boxes;

    const float* output_data = output_tensor.GetTensorData<float>();
    size_t num_detected = output_tensor.GetTensorTypeAndShapeInfo().GetElementCount() / 7;

    for (size_t i = 0; i < num_detected; ++i) {
        size_t base_index = i * 7;
        float confidence = output_data[base_index + 5];

        if (confidence > Config::getConfidenceThreshold()) {
            float x = output_data[base_index + 1] / INPUT_WIDTH * original_image_size.width;
            float y = output_data[base_index + 2] / INPUT_HEIGHT * original_image_size.height;
            float w = (output_data[base_index + 3] - output_data[base_index + 1]) / INPUT_WIDTH * original_image_size.width;
            float h = (output_data[base_index + 4] - output_data[base_index + 2]) / INPUT_HEIGHT * original_image_size.height;

            bounding_boxes.emplace_back(x, y, w, h);
        }
    }

    return bounding_boxes;
}