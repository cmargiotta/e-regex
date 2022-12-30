{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils/master";
    devshell.url = "github:numtide/devshell/master";
  };
  outputs = { self, nixpkgs, flake-utils, devshell }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ devshell.overlay ];
          config.allowUnfree = true;
        };

      in
      rec {
        nixConfig.sandbox = "relaxed";
        devShell = pkgs.devshell.mkShell {
          name = "gst";
          commands = [
            {
              name = "build";
              help = "Automatically configure build folder and run build";
              command = "meson build && meson compile -C build";
            }
          ];
          env = [
          ];
          packages =
            with pkgs;
            [
              meson
              ninja
              clang15
            ];
        };
      });
}
