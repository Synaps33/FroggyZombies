#!/usr/bin/env bash
set -euo pipefail

# FroggyZombies - SD Card Installer for Data Frog SF2000 & GB300
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
SD_ROOT="${1:-}"

if [ -z "$SD_ROOT" ]; then
    for p in /media/* /run/media/*/* /mnt/*; do
        if [ -d "$p/system/Deimos/cores" ] || [ -d "$p/cores" ] || [ -d "$p/ROMS" ]; then
            SD_ROOT="$p"
            break
        fi
    done
fi

if [ -z "$SD_ROOT" ] || [ ! -d "$SD_ROOT" ]; then
    echo "No SD card detected. Usage: $0 /path/to/sd-card"
    exit 1
fi

echo "Installing FroggyZombies to: $SD_ROOT"

# Ensure target directories
mkdir -p "$SD_ROOT/ROMS/nzp/nzp"
mkdir -p "$SD_ROOT/system/Deimos/cores"
mkdir -p "$SD_ROOT/cores"

# Copy cores if built
if [ -f "/tmp/nzp.gb3k" ]; then
    cp -f "/tmp/nzp.gb3k" "$SD_ROOT/system/Deimos/cores/nzp.gb3k"
    cp -f "/tmp/nzp.gb3k" "$SD_ROOT/system/Deimos/cores/nzpsafe.gb3k"
    cp -f "/tmp/nzp.gb3k" "$SD_ROOT/ROMS/nzp/nzp.gb3k"
    cp -f "/tmp/nzp.gb3k" "$SD_ROOT/cores/nzp.gb3k"
fi

if [ -f "/tmp/nzp.sf2k" ]; then
    cp -f "/tmp/nzp.sf2k" "$SD_ROOT/system/Deimos/cores/nzp.sf2k"
    cp -f "/tmp/nzp.sf2k" "$SD_ROOT/system/Deimos/cores/nzpsafe.sf2k"
    cp -f "/tmp/nzp.sf2k" "$SD_ROOT/ROMS/nzp/nzp.sf2k"
    cp -f "/tmp/nzp.sf2k" "$SD_ROOT/cores/nzp.sf2k"
fi

# GBA launcher shortcuts
printf 'nzp;nzp;NZPortable.gba' > "$SD_ROOT/ROMS/nzp/NZPortable.gba"
printf 'nzpsafe;nzp;NZPortableSafe.gba' > "$SD_ROOT/ROMS/nzp/NZPortableSafe.gba"

sync
echo "Installation completed! Eject your SD card and start playing."
