{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.lksctp-tools
    pkgs.gcc
  ];
}
