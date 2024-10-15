#include <onnxruntime_cxx_api.h>
#include <iostream>
#include <vector>
#include <string>

void printVector(const std::vector<float>& vec) {
    std::cout << "[ ";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << vec[i];
    }
    std::cout << " ]" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_model.onnx>" << std::endl;
        return 1;
    }

    try {
        // Initialize environment
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

        // Initialize session options
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);

        // Create session
        Ort::Session session(env, argv[1], session_options);

        // Print model metadata
        Ort::AllocatorWithDefaultOptions allocator;

        size_t num_input_nodes = session.GetInputCount();
        size_t num_output_nodes = session.GetOutputCount();

        std::cout << "Number of inputs = " << num_input_nodes << std::endl;
        std::cout << "Number of outputs = " << num_output_nodes << std::endl;

        // Print input and output names
        std::vector<const char*> input_names;
        std::vector<const char*> output_names;

        for (size_t i = 0; i < num_input_nodes; i++) {
            Ort::AllocatedStringPtr input_name = session.GetInputNameAllocated(i, allocator);
            std::cout << "Input " << i << " : name=" << input_name.get() << std::endl;
            input_names.push_back(input_name.release());
        }

        for (size_t i = 0; i < num_output_nodes; i++) {
            Ort::AllocatedStringPtr output_name = session.GetOutputNameAllocated(i, allocator);
            std::cout << "Output " << i << " : name=" << output_name.get() << std::endl;
            output_names.push_back(output_name.release());
        }

        // Get input node info
        Ort::TypeInfo type_info = session.GetInputTypeInfo(0);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        std::vector<int64_t> input_shape = tensor_info.GetShape();

        std::cout << "Input shape: ";
        for (auto dim : input_shape) {
            std::cout << dim << " ";
        }
        std::cout << std::endl;

        // Create input tensor (example with float input)
        size_t input_tensor_size = 1;
        for (auto dim : input_shape) {
            if (dim > 0) {  // Handle dynamic dimensions
                input_tensor_size *= dim;
            }
        }
        std::vector<float> input_tensor_values(input_tensor_size, 1.0f);  // Initialize with 1.0

        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_values.size(), input_shape.data(), input_shape.size());

        // Run inference
        auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names.data(), &input_tensor, 1, output_names.data(), output_names.size());

        // Get pointer to output tensor float values
        float* floatarr = output_tensors.front().GetTensorMutableData<float>();

        // Get output tensor shape
        Ort::TensorTypeAndShapeInfo shape_info = output_tensors.front().GetTensorTypeAndShapeInfo();
        std::vector<int64_t> output_shape = shape_info.GetShape();

        // Calculate the total number of elements in the output tensor
        size_t num_elements = shape_info.GetElementCount();

        // Convert to vector and print
        std::vector<float> output_tensor_values(floatarr, floatarr + num_elements);
        std::cout << "Input:" << std::endl;
        printVector(input_tensor_values);
        std::cout << "Output:" << std::endl;
        printVector(output_tensor_values);

        // Free memory
        for (const char* name : input_names) allocator.Free(const_cast<void*>(static_cast<const void*>(name)));
        for (const char* name : output_names) allocator.Free(const_cast<void*>(static_cast<const void*>(name)));

        std::cout << "ONNX Runtime test successful!" << std::endl;
    }
    catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}