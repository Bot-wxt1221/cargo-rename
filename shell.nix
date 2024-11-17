let
  pkgs = import <nixpkgs> { };
in
pkgs.mkShell {
  packages = with pkgs; [
    nix-prefetch-git
    cargo
    cacert
    (pkgs.python3.withPackages (python-pkgs: [
      python-pkgs.requests
      python-pkgs.tomli
      python-pkgs.tomli-w
    ]))
    (pkgs.writers.writePython3Bin "replace-workspace-values" {
      libraries = with pkgs.python3Packages; [
        tomli
        tomli-w
      ];
      flakeIgnore = [
        "E501"
        "W503"
      ];
    } (builtins.readFile ./replace-workspace-values.py))
    (pkgs.writers.writePython3Bin "fetch-cargo-vendor-util" {
      libraries = with pkgs.python3Packages; [
        requests
      ];
      flakeIgnore = [
        "E501"
      ];
    } (builtins.readFile ./fetch-cargo.py))
  ];
}
