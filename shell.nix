{ pkgs ? import<nixos-unstable>{} }:
(import ../hup/default.nix {
  inherit pkgs;
  stdenv = pkgs.gcc11Stdenv;
  #stdenv = pkgs.llvmPackages_10.stdenv;
}).cppjinja
