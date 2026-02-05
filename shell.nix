{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  packages = with pkgs; [
    llvmPackages.clang-unwrapped
    llvmPackages.llvm
    bpftools
    libbpf
    pkg-config
    libelf
    zlib
    iproute2
  ];

  shellHook = ''
    export PATH="${pkgs.llvmPackages.clang-unwrapped}/bin:$PATH"
    export BPF_CLANG="${pkgs.llvmPackages.clang-unwrapped}/bin/clang"

    unset NIX_CFLAGS_COMPILE
    unset NIX_CFLAGS_LINK

    hash -r

    echo "[nix-shell] clang -> $(command -v clang)"
  '';
}
