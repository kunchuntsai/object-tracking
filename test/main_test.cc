#include "unit_test.h"
#include <string>
#include <stdexcept>
#include <filesystem>
#include <iostream>

std::string onnx_model_path;

bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path-to-onnx-model>" << std::endl;
        return 1;
    }

    onnx_model_path = argv[1];

    if (!ends_with(onnx_model_path, ".onnx")) {
        std::cerr << "Error: The provided file does not have a .onnx extension." << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(onnx_model_path)) {
        std::cerr << "Error: The specified ONNX file does not exist." << std::endl;
        return 1;
    }

    std::cout << "Using ONNX model: " << onnx_model_path << std::endl;

    UnitTest::runAllTests();

    return 0;
}