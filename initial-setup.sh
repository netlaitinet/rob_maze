#!/usr/bin/env bash

nix shell nixpkgs#distrobox nixpkgs#podman -c \
  distrobox-create --yes --name clean-webots --image docker.io/library/ubuntu:24.04 || exit "$?"

nix shell nixpkgs#distrobox nixpkgs#podman -c \
  distrobox-enter clean-webots -- bash -e -c '
    sudo env DEBIAN_FRONTEND=noninteractive apt-get update
    sudo env DEBIAN_FRONTEND=noninteractive apt-get install -y ca-certificates curl build-essential
    curl --fail --location --output /tmp/webots.deb \
      https://github.com/cyberbotics/webots/releases/download/R2025a/webots_2025a_amd64.deb
    sudo env DEBIAN_FRONTEND=noninteractive apt-get install -y /tmp/webots.deb
    rm /tmp/webots.deb
  '
