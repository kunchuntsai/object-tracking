{
  description = "Object Tracking development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-23.11-darwin";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        onnxruntimeVersion = "1.16.1";
        onnxruntimeGitHash = "e6f6f19370642d39492125464d162d7bef2e96d6";

        onnxruntime-custom = pkgs.stdenv.mkDerivation {
          pname = "onnxruntime-custom";
          version = onnxruntimeVersion;

          src = pkgs.fetchFromGitHub {
            owner = "microsoft";
            repo = "onnxruntime";
            rev = onnxruntimeGitHash;
            sha256 = "sha256-5saPD8UnsKsB4jtwCHh9lye1e2536YDy46uxFtTVvz4=";
            fetchSubmodules = true;
          };

          nativeBuildInputs = with pkgs; [
            cmake
            python3
            protobuf
            git
            pkg-config
          ];

          buildInputs = with pkgs; [
            openblas
            opencv
            libiconv
            darwin.apple_sdk.frameworks.CoreML
            darwin.apple_sdk.frameworks.Foundation
            darwin.apple_sdk.frameworks.Accelerate
          ];

          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
            "-Donnxruntime_RUN_ONNX_TESTS=OFF"
            "-Donnxruntime_GENERATE_TEST_REPORTS=OFF"
            "-Donnxruntime_USE_COREML=ON"
            "-Donnxruntime_BUILD_SHARED_LIB=ON"
            "-Donnxruntime_ENABLE_PYTHON=OFF"
            "-DCMAKE_OSX_ARCHITECTURES=arm64"
            "-DCMAKE_PREFIX_PATH=${pkgs.libiconv}"
          ];

          prePatch = ''
            find . -type f \( -name "*.h" -o -name "*.cc" -o -name "*.mm" \) -print0 | xargs -0 sed -i '
              s/ATOMIC_VAR_INIT *(0)/(0)/g
              s/ATOMIC_VAR_INIT(0)/(0)/g
              s/NSYNC_ATOMIC_UINT32_INIT_/(0)/g
              s/NSYNC_MU_INIT/{ (0), 0 }/g
              s/NSYNC_CV_INIT/{ (0), 0 }/g
            '

            # Modify the CoreML model.mm file
            sed -i '/Copy MLMultiArray data to ORT tensor buffer/,/if (!copy_status.IsOK())/ c\
              // Copy MLMultiArray data to ORT tensor buffer\
              {\
                // disable size check as old API does not return buffer length\
                copy_status = CopyMLMultiArrayBuffer(data.dataPointer, output_buffer, data, tensor_info, std::nullopt);\
              }\
              if (!copy_status.IsOK())
            ' onnxruntime/core/providers/coreml/model/model.mm
          '';

          preConfigure = ''
            patchShebangs .
          '';

          NIX_CFLAGS_COMPILE = toString [
            "-Wno-deprecated-declarations"
            "-Wno-error=deprecated-declarations"
            "-Wno-error=unused-but-set-variable"
            "-Wno-error=unused-variable"
            "-Wno-error=sign-compare"
          ];

          configurePhase = ''
            runHook preConfigure
            cmake -S cmake -B build $cmakeFlags
            runHook postConfigure
          '';

          buildPhase = ''
            runHook preBuild
            cmake --build build
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall
            mkdir -p $out/{lib,include}
            cp -r include/* $out/include/
            find build -name "*.dylib" -exec cp {} $out/lib/ \;
            runHook postInstall
          '';

          meta = with pkgs.lib; {
            description = "ONNX Runtime with CoreML support";
            homepage = "https://github.com/microsoft/onnxruntime";
            license = licenses.mit;
            platforms = platforms.darwin;
          };
        };

        requiredDeps = with pkgs; [
          opencv
          onnxruntime-custom
          protobuf
          nlohmann_json
          re2
          eigen
          flatbuffers
          libiconv
        ];
        
        gdbIfAvailable = if pkgs.stdenv.hostPlatform.isDarwin && pkgs.stdenv.hostPlatform.isAarch64
                         then [ ]
                         else [ pkgs.gdb ];

        version = "0.1.0";
      in
      {
        packages = {
          default = pkgs.stdenv.mkDerivation {
            pname = "object-tracking";
            inherit version;
            src = ./.;
            nativeBuildInputs = with pkgs; [ cmake pkg-config ];
            buildInputs = requiredDeps;

            preBuildPhase = ''
              echo '#pragma once' > version.h.in
              echo '#define OBJECT_TRACKING_VERSION "@PROJECT_VERSION@"' >> version.h.in
            '';

            buildPhase = ''
              export ONNXRuntime_ROOT=${onnxruntime-custom}
              cmake . -DPROJECT_VERSION=${version} | tee cmake_config.log
              make
            '';
            
            installPhase = ''
              mkdir -p $out/bin $out/logs
              find . -type f -executable -print0 | xargs -0 -I {} cp {} $out/bin/
              cp cmake_config.log $out/logs/
            '';
          };
        };

        devShells = {
          default = pkgs.mkShell {
            buildInputs = requiredDeps ++ (with pkgs; [ 
              cmake
              gcc
              pkg-config
            ]) ++ gdbIfAvailable;

            shellHook = ''
              export ONNXRuntime_ROOT=${onnxruntime-custom}
              echo "Object Tracking development environment: Version ${version}"
              echo "Using custom-built ONNX Runtime version ${onnxruntimeVersion}"
              echo "Checkout CMake config: cat result/logs/cmake_config.log"
            '';
          };
        };
      });
}