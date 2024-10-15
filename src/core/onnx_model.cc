#include "onnx_model.h"
#include "logger.h"

ONNXModel::ONNXModel() : env(ORT_LOGGING_LEVEL_WARNING, "ONNXModel") {}

ONNXModel& ONNXModel::getInstance() {
    static ONNXModel instance;
    return instance;
}

bool ONNXModel::loadModel(const std::string& model_path) {
    try {
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);

        session = Ort::Session(env, model_path.c_str(), session_options);

        size_t num_input_nodes = session.GetInputCount();
        size_t num_output_nodes = session.GetOutputCount();

        input_node_names.resize(num_input_nodes);
        output_node_names.resize(num_output_nodes);

        for (size_t i = 0; i < num_input_nodes; i++) {
            Ort::AllocatedStringPtr input_name = session.GetInputNameAllocated(i, allocator);
            input_node_names[i] = input_name.release();
        }

        for (size_t i = 0; i < num_output_nodes; i++) {
            Ort::AllocatedStringPtr output_name = session.GetOutputNameAllocated(i, allocator);
            output_node_names[i] = output_name.release();
        }

        // Get input node info
        Ort::TypeInfo type_info = session.GetInputTypeInfo(0);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        input_node_dims = tensor_info.GetShape();

        printModelInfo();

        LOG_INFO("ONNX model loaded successfully");
        return true;
    } catch (const Ort::Exception& e) {
        LOG_ERROR("Error loading ONNX model: " + std::string(e.what()));
        return false;
    }
}

void ONNXModel::printModelInfo() {
    LOG_INFO("Number of inputs = " + std::to_string(input_node_names.size()));
    LOG_INFO("Number of outputs = " + std::to_string(output_node_names.size()));

    LOG_INFO("Input node name: " + std::string(input_node_names[0]));
    LOG_INFO("Output node name: " + std::string(output_node_names[0]));

    LOG_INFO("Input shape: ");
    for (auto dim : input_node_dims) {
        LOG_INFO(std::to_string(dim) + " ");
    }
}

size_t ONNXModel::calculateInputTensorSize() {
    size_t input_tensor_size = 1;
    for (auto dim : input_node_dims) {
        if (dim > 0) {  // Handle dynamic dimensions
            input_tensor_size *= dim;
        }
    }
    return input_tensor_size;
}

std::vector<cv::Rect> ONNXModel::detect(const cv::Mat& image) {
    std::vector<float> input_tensor;
    preprocess(image, input_tensor);

    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor_ort = Ort::Value::CreateTensor<float>(memory_info, input_tensor.data(), input_tensor.size(),
                                                                  input_node_dims.data(), input_node_dims.size());

    try {
        auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor_ort, 1, output_node_names.data(), output_node_names.size());

        float* floatarr = output_tensors.front().GetTensorMutableData<float>();
        size_t num_elements = output_tensors.front().GetTensorTypeAndShapeInfo().GetElementCount();
        std::vector<float> output_tensor(floatarr, floatarr + num_elements);

        return postprocess(output_tensor, image.size());
    } catch (const Ort::Exception& e) {
        LOG_ERROR("Error during inference: " + std::string(e.what()));
        return std::vector<cv::Rect>();
    }
}

void ONNXModel::preprocess(const cv::Mat& input_image, std::vector<float>& input_tensor) {
    cv::Mat resized_image;
    cv::resize(input_image, resized_image, cv::Size(input_node_dims[2], input_node_dims[3]));

    cv::Mat float_image;
    resized_image.convertTo(float_image, CV_32F, 1.0 / 255);

    cv::Mat channels[3];
    cv::split(float_image, channels);

    size_t input_tensor_size = calculateInputTensorSize();
    input_tensor.resize(input_tensor_size);
    auto input_tensor_it = input_tensor.begin();

    for (int c = 0; c < 3; c++) {
        for (int h = 0; h < input_node_dims[2]; h++) {
            for (int w = 0; w < input_node_dims[3]; w++) {
                *input_tensor_it = channels[c].at<float>(h, w);
                ++input_tensor_it;
            }
        }
    }
}

std::vector<cv::Rect> ONNXModel::postprocess(const std::vector<float>& output_tensor, const cv::Size& original_image_size) {
    std::vector<cv::Rect> bounding_boxes;

    // This is a placeholder implementation. You'll need to adjust this based on your specific model's output format.
    // Typically, object detection models output bounding box coordinates, confidence scores, and class IDs.

    // Assuming the output tensor contains [x_min, y_min, x_max, y_max, confidence, class_id] for each detection
    const int values_per_detection = 6;
    const float confidence_threshold = 0.5;

    for (size_t i = 0; i < output_tensor.size(); i += values_per_detection) {
        float confidence = output_tensor[i + 4];
        if (confidence > confidence_threshold) {
            float x_min = output_tensor[i] * original_image_size.width;
            float y_min = output_tensor[i + 1] * original_image_size.height;
            float x_max = output_tensor[i + 2] * original_image_size.width;
            float y_max = output_tensor[i + 3] * original_image_size.height;

            bounding_boxes.emplace_back(x_min, y_min, x_max - x_min, y_max - y_min);
        }
    }

    return bounding_boxes;
}
