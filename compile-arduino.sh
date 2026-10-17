#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

arduino-cli compile --warnings all -b arduino:avr:mega \
  --upload --port "${1:-/dev/ttyACM0}" \
  --build-path /tmp/arduino-build controllers/e-puck/
