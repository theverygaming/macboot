{
  description = "macboot";

  inputs = {
    nixpkgs.url = "nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nur-theverygaming = {
      url = "github:theverygaming/nix-repo";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      nur-theverygaming,
    }:
    { }
    // flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in
      rec {
        devShells.m68k = pkgs.stdenv.mkDerivation {
          name = "macboot dev";
          buildInputs = with pkgs; [
            pkgsCross.m68k.buildPackages.gcc
            mtools
            dosfstools
            nur-theverygaming.packages.${system}.minivmac
          ];
        };
      }
    );
}
