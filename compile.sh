#!/usr/bin/env bash

nix shell nixpkgs#distrobox nixpkgs#podman -c \
  distrobox-enter clean-webots -- env WEBOTS_HOME=/usr/local/webots \
  make -C "/home/$USER/robot_maze_challenge/controllers/e-puck"
