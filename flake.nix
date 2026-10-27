{
  description = "littleboot";

  inputs = {
    nixpkgs.url = "nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nur-theverygaming = {
      url = "github:theverygaming/nix-repo";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    funnyarch = {
      url = "github:theverygaming/funnyarch";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      nur-theverygaming,
      funnyarch,
    }:
    { }
    // flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
        common-deps = with pkgs; [
          mtools
          dosfstools
        ];
      in
      rec {
        devShells.m68k = pkgs.stdenv.mkDerivation {
          name = "littleboot dev m68k";
          buildInputs =
            with pkgs;
            [
              pkgsCross.m68k.buildPackages.gcc
              nur-theverygaming.packages.${system}.minivmac
            ]
            ++ common-deps;
          ARCH = "m68k";
          CROSS_COMPILE = "m68k-unknown-linux-gnu-";
        };
        devShells.funnyarch = pkgs.stdenv.mkDerivation {
          name = "littleboot dev funnyarch";
          buildInputs =
            with pkgs;
            [
              funnyarch.packages.${system}.toolchain
              funnyarch.packages.${system}.gcc
            ]
            ++ common-deps;
          ARCH = "funnyarch";
          CROSS_COMPILE = "funnyarch-none-none-";
          shellHook = ''
            alias funnyarch-emu='${funnyarch.packages.${system}.emulator}/bin/funnyarch-emulator -r ${
              funnyarch.packages.${system}.rom
            }'
          '';
        };
        packages.littleboot-headers = pkgs.runCommand "littleboot-headers" { } ''
          mkdir -p "$out"/include/littleboot
          cp ${./include/proto.h} "$out"/include/littleboot/proto.h
        '';
        packages.littleboot-funnyarch = pkgs.stdenv.mkDerivation rec {
          name = "littleboot-funnyarch";

          src = ./.;

          buildInputs = with pkgs; [
            funnyarch.packages.${system}.toolchain
            funnyarch.packages.${system}.gcc
          ];

          ARCH = "funnyarch";
          CROSS_COMPILE = "funnyarch-none-none-";

          installPhase = ''
            cp startup.bin $out
          '';
        };
      }
    );
}
