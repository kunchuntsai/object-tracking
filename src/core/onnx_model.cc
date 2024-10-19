#include "onnx_model.h"
#include "logger.h"
#include "config.h"
#include <opencv2/dnn/dnn.hpp>
#include <chrono>

// Define fixed input dimensions, as the model's expected input
const int INPUT_WIDTH = 640;
const int INPUT_HEIGHT = 640;

ONNXModel::ONNXModel() : env(ORT_LOGGING_LEVEL_WARNING, "ONNXModel") {}

ONNXModel& ONNXModel::getInstance() {
    static ONNXModel instance;
    return instance;
}

bool ONNXModel::loadModel(const std::string& model_path) {
    try {
        // Enable multithreading
        session_options.SetInterOpNumThreads(4);  // Adjust based on your CPU cores
        session_options.SetIntraOpNumThreads(2);

        // Enable graph optimizations
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

        // Try to append available execution providers
        #ifdef __APPLE__
        appendCoreMLExecutionProvider();
        #endif

        #ifdef _WIN32
        appendCUDAExecutionProvider();
        #endif

        session = Ort::Session(env, model_path.c_str(), session_options);

        memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

        input_node_names = {"images"};
        output_node_names = {"output"};
        input_node_dims = {1, 3, INPUT_HEIGHT, INPUT_WIDTH};

        LOG_INFO("ONNX model loaded successfully with input dimensions: %ldx%ldx%ldx%ld",
             input_node_dims[0], input_node_dims[1], input_node_dims[2], input_node_dims[3]);
        return true;
    } catch (const Ort::Exception& e) {
        LOG_ERROR("Error loading ONNX model: %s", e.what());
        return false;
    }
}

void ONNXModel::appendCoreMLExecutionProvider() {
    #if __has_include(PROVIDER_HEADER(coreml))
        #include PROVIDER_HEADER(coreml)
        OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_CoreML(session_options, 0);
        if (status != nullptr) {
            const char* error_message = Ort::GetApi().GetErrorMessage(status);
            LOG_WARNING("Failed to append CoreML execution provider: %s", error_message);
            Ort::GetApi().ReleaseStatus(status);
        } else {
            LOG_INFO("CoreML execution provider appended successfully");
        }
    #else
        LOG_WARNING("CoreML execution provider not available");
    #endif
}

void ONNXModel::appendCUDAExecutionProvider() {
    #if __has_include(PROVIDER_HEADER(cuda))
        #include PROVIDER_HEADER(cuda)
        OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 0);
        if (status != nullptr) {
            const char* error_message = Ort::GetApi().GetErrorMessage(status);
            LOG_WARNING("Failed to append CUDA execution provider: %s", error_message);
            Ort::GetApi().ReleaseStatus(status);
        } else {
            LOG_INFO("CUDA execution provider appended successfully");
        }
    #else
        LOG_WARNING("CUDA execution provider not available");
    #endif
}

std::vector<cv::Rect> ONNXModel::detect(const Ort::Value& input_tensor, const cv::Size& original_image_size) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Ort::Value> input_tensors;
    input_tensors.push_back(std::move(const_cast<Ort::Value&>(input_tensor)));

    std::vector<Ort::Value> output_tensors;
    try {
        output_tensors = session.Run(Ort::RunOptions{nullptr},
            input_node_names.data(), input_tensors.data(), input_tensors.size(),
            output_node_names.data(), output_node_names.size());
    } catch (const Ort::Exception& e) {
        LOG_ERROR("Error during inference: %s", e.what());
        return std::vector<cv::Rect>();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    LOG_DEBUG("[ONNXModel] Inference time: %lld µs", duration.count());

    return postprocess(output_tensors.front(), original_image_size);
}

std::vector<cv::Rect> ONNXModel::postprocess(const Ort::Value& output_tensor, const cv::Size& original_image_size) {
    auto start = std::chrono::high_resolution_clock::now();

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

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    LOG_DEBUG("[ONNXModel] Postprocessing time: %lld µs", duration.count());

    return bounding_boxes;
}