#!/usr/bin/env bash

"/home/$USER/rob_maze/compile.sh" || exit "$?"

nix shell nixpkgs#distrobox nixpkgs#podman -c \
  distrobox-enter clean-webots -- env GIO_EXTRA_MODULES= \
  webots --mode=fast "/home/$USER/rob_maze/worlds/maze.wbt"

