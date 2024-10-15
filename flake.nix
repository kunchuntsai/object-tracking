{
  description = "Object Tracking development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-23.11-darwin";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
        requiredDeps = with pkgs; [
          opencv
          onnxruntime
          protobuf
          nlohmann_json
          re2
          eigen
          flatbuffers
        ];
        
        # Check if GDB is available for the current system
        gdbIfAvailable = if pkgs.stdenv.hostPlatform.isDarwin && pkgs.stdenv.hostPlatform.isAarch64
                         then [ ]
                         else [ pkgs.gdb ];

        # Define the version
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
              export ONNXRuntime_ROOT=${pkgs.onnxruntime}
              cmake . -DPROJECT_VERSION=${version}
              make
            '';
            
            installPhase = ''
              mkdir -p $out/bin
              find . -type f -executable -print0 | xargs -0 -I {} cp {} $out/bin/
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
              export ONNXRuntime_ROOT=${pkgs.onnxruntime}

              FLAG_FILE=".build_completed"

              function run_build {
                echo "Building and running ONNX Runtime test..."
                mkdir -p test/onnx_test/build
                cd test/onnx_test/build
                cmake ..
                make
                ./test_onnx
                cd ../../..
                touch $FLAG_FILE
              }

              if [ ! -f "$FLAG_FILE" ]; then
                run_build
              fi

              echo "C++ OpenCV and ONNX Runtime development environment (version ${version})"
              echo "To build: ./build.sh ${version}"
              echo "To run: ./result/bin/object-tracking [path_to_model] [--camera | <path_to_video>]"
              echo "To manually run the build process again, use the command: run_build"
            '';
          };
        };
      });
}