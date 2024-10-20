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
              echo "Object Tracking development environment: Version ${version}"
              echo "Checkout CMake config: cat result/logs/cmake_config.log"
            '';
          };
        };
      });
}