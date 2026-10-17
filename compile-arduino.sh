#!/usr/bin/env bash
# arduino-cli compile --warnings all -b arduino:avr:mega \
arduino-cli compile -b arduino:avr:mega \
  --build-path /tmp/arduino-build controllers/e-puck/
  # --upload --port "${1:-/dev/ttyACM0}" \
