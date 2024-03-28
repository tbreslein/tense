{
  description = "Small and minimal lib for matrix/vector types";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = inputs@{ self, ... }:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" ];

      perSystem = { system, pkgs, ... }:
        let
          buildInputs = with pkgs; [
            # build tools
            cmake
            gnumake
            ninja
            gcc13
            pkg-config
            just

            # dev tools
            cppcheck
            neocmakelsp
            cmake-format
          ];
          clang_stdenv = pkgs.llvmPackages_16.stdenv;
          gcc_stdenv = pkgs.gcc13Stdenv;
          cc = "clang"; # either "clang" or "gcc"
        in {
          devShells.default = if cc == "clang" then
            (pkgs.mkShell.override { stdenv = clang_stdenv; }) {
              CC = "${pkgs.clang_16}/bin/clang";
              CXX = "${pkgs.clang_16}/bin/clang++";
              buildInputs = buildInputs
                ++ (with pkgs; [ clang_16 llvmPackages_16.openmp ]);
            }
          else
            (pkgs.mkShell.override { stdenv = gcc_stdenv; }) {
              CC = "${pkgs.gcc13}/bin/gcc";
              CXX = "${pkgs.gcc13}/bin/g++";
              buildInputs = buildInputs ++ (with pkgs; [ gcc13 ]);
            };
        };
    };
}
