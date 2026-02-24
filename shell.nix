{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  packages = with pkgs; [
    llvmPackages.clang-unwrapped
    llvmPackages.llvm
    bpftools         # Убедись, что имя правильное (bpftool или linuxPackages.bpftool)
    libbpf
    pkg-config
    libelf
    zlib
    iproute2
    linuxHeaders
  ];

  shellHook = ''
    export PATH="${pkgs.llvmPackages.clang-unwrapped}/bin:$PATH"
    export BPF_CLANG="${pkgs.llvmPackages.clang-unwrapped}/bin/clang"

    # Сбрасываем магию Nix, чтобы clang был "чистым"
    unset NIX_CFLAGS_COMPILE
    unset NIX_CFLAGS_LINK

    # === ИСПРАВЛЕНИЕ ЗДЕСЬ ===
    # Убираем .dev, так как хедеры лежат прямо в корне пакета
    export C_INCLUDE_PATH="${pkgs.libbpf}/include:${pkgs.linuxHeaders}/include:$C_INCLUDE_PATH"

    # Для линковки
    export LIBRARY_PATH="${pkgs.libbpf}/lib:${pkgs.libelf}/lib:$LIBRARY_PATH"

    hash -r
    echo "[nix-shell] Clang unwrapped ready."
    echo "            Include path: $C_INCLUDE_PATH"
  '';
}