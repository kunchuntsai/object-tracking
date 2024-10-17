#include "onnx_model.h"
#include "logger.h"
#include "config.h"
#include <opencv2/dnn/dnn.hpp>

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
        input_node_dims = {1, 3, 640, 640};

        LOG_INFO("ONNX model loaded successfully");
        return true;
    } catch (const Ort::Exception& e) {
        LOG_ERROR("Error loading ONNX model: " + std::string(e.what()));
        return false;
    }
}

std::vector<cv::Rect> ONNXModel::detect(const cv::Mat& image) {
    std::vector<Ort::Value> input_tensors;
    preprocess(image, input_tensors);

    std::vector<Ort::Value> output_tensors;
    try {
        output_tensors = session.Run(Ort::RunOptions{nullptr}, input_node_names.data(), input_tensors.data(), input_tensors.size(), output_node_names.data(), output_node_names.size());
    } catch (const Ort::Exception& e) {
        LOG_ERROR("Error during inference: " + std::string(e.what()));
        return std::vector<cv::Rect>();
    }

    return postprocess(output_tensors.front(), image.size());
}

void ONNXModel::preprocess(const cv::Mat& input_image, std::vector<Ort::Value>& input_tensors) {
    cv::Mat blob = cv::dnn::blobFromImage(input_image, 1.0/255.0, cv::Size(640, 640), cv::Scalar(0, 0, 0), false, false);

    size_t input_tensor_size = blob.total();
    input_tensors.emplace_back(Ort::Value::CreateTensor<float>(memory_info, (float*)blob.data, input_tensor_size, input_node_dims.data(), input_node_dims.size()));
}

std::vector<cv::Rect> ONNXModel::postprocess(const Ort::Value& output_tensor, const cv::Size& original_image_size) {
    std::vector<cv::Rect> bounding_boxes;

    const float* output_data = output_tensor.GetTensorData<float>();
    size_t num_detected = output_tensor.GetTensorTypeAndShapeInfo().GetElementCount() / 7;

    for (size_t i = 0; i < num_detected; ++i) {
        size_t base_index = i * 7;
        float confidence = output_data[base_index + 5];

        if (confidence > Config::getConfidenceThreshold()) {
            float x = output_data[base_index + 1] / 640.0f * original_image_size.width;
            float y = output_data[base_index + 2] / 640.0f * original_image_size.height;
            float w = (output_data[base_index + 3] - output_data[base_index + 1]) / 640.0f * original_image_size.width;
            float h = (output_data[base_index + 4] - output_data[base_index + 2]) / 640.0f * original_image_size.height;

            bounding_boxes.emplace_back(x, y, w, h);
        }
    }

    return bounding_boxes;
}