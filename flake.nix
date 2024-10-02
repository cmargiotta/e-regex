{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    mach-nix.url = "mach-nix/3.5.0";
  };
  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      mach-nix,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      rec {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            meson
            ninja
            gcc14
            gnat14
            gdb
            valgrind
            clang-tools

            (mach-nix.lib."${system}".mkPython {
              requirements = ''
                quom
              '';
            })
          ];

          nativeBuildInputs = with pkgs; [ pkg-config ];
        };
      }
    );
}
