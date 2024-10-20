#include "unit_test.h"
#include "onnx_model.h"
#include "logger.h"
#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

extern std::string onnx_model_path;

TEST(ONNXRuntimeInstallation) {
    // Check if we can create an ONNX Runtime environment
    try {
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
        LOG_INFO("ONNX Runtime environment created successfully");

        // Get ONNX Runtime version
        const auto& api = Ort::GetApi();
        std::cout << "ONNX Runtime version available" << std::endl;

        // List available providers
        Ort::AllocatorWithDefaultOptions allocator;
        auto available_providers = Ort::GetAvailableProviders();
        std::cout << "Available providers:" << std::endl;
        for (const auto& provider : available_providers) {
            std::cout << " - " << provider << std::endl;
        }

        ASSERT_TRUE(true);  // If we've made it this far, the test passes
    } catch (const Ort::Exception& e) {
        LOG_ERROR("ONNX Runtime error: %s", e.what());
        ASSERT_TRUE(false);  // Test fails if we catch an exception
    }
}

TEST(ONNXModelLoading) {
    ONNXModel& model = ONNXModel::getInstance();
    ASSERT_TRUE(model.loadModel(onnx_model_path));
}

TEST(ONNXModelInputDimensions) {
    ONNXModel& model = ONNXModel::getInstance();
    model.loadModel(onnx_model_path);

    const auto& input_dims = model.getInputNodeDims();
    ASSERT_EQUAL(input_dims.size(), 4);
    ASSERT_EQUAL(input_dims[0], 1);  // Batch size
    ASSERT_EQUAL(input_dims[1], 3);  // Channels
    ASSERT_EQUAL(input_dims[2], 640);  // Height
    ASSERT_EQUAL(input_dims[3], 640);  // Width
}

TEST(ONNXModelInference) {
    ONNXModel& model = ONNXModel::getInstance();
    model.loadModel(onnx_model_path);

    // Create a dummy input image
    cv::Mat input_image(640, 640, CV_8UC3, cv::Scalar(255, 0, 0));
    cv::Size original_size(1280, 720);  // Simulating an original image size

    // Prepare input tensor
    std::vector<float> input_tensor_values(1 * 3 * 640 * 640);
    for (int c = 0; c < 3; c++) {
        for (int h = 0; h < 640; h++) {
            for (int w = 0; w < 640; w++) {
                input_tensor_values[c * 640 * 640 + h * 640 + w] = input_image.at<cv::Vec3b>(h, w)[c] / 255.0f;
            }
        }
    }

    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_values.size(), model.getInputNodeDims().data(), model.getInputNodeDims().size());

    // Run inference
    std::vector<cv::Rect> bounding_boxes = model.detect(input_tensor, original_size);

    // Check if we got any detections
    ASSERT_FALSE(bounding_boxes.empty());

    // Check if bounding boxes are within the original image size
    for (const auto& bbox : bounding_boxes) {
        ASSERT_TRUE(bbox.x >= 0 && bbox.x < original_size.width);
        ASSERT_TRUE(bbox.y >= 0 && bbox.y < original_size.height);
        ASSERT_TRUE(bbox.width > 0 && bbox.x + bbox.width <= original_size.width);
        ASSERT_TRUE(bbox.height > 0 && bbox.y + bbox.height <= original_size.height);
    }
}

TEST(ONNXModelExceptionHandling) {
    ONNXModel& model = ONNXModel::getInstance();

    // Test loading non-existent model
    ASSERT_FALSE(model.loadModel("non_existent_model.onnx"));

    // Test inference with uninitialized model
    cv::Mat dummy_image(640, 640, CV_8UC3, cv::Scalar(0));
    std::vector<float> dummy_tensor(1 * 3 * 640 * 640, 0.0f);
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value dummy_input_tensor = Ort::Value::CreateTensor<float>(memory_info, dummy_tensor.data(), dummy_tensor.size(), model.getInputNodeDims().data(), model.getInputNodeDims().size());

    // We expect an empty vector of bounding boxes when the model is not properly initialized
    std::vector<cv::Rect> result = model.detect(dummy_input_tensor, cv::Size(1280, 720));
    ASSERT_TRUE(result.empty());
}