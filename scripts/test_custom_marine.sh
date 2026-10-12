#!/usr/bin/env bash
# Integration smoke test for registered custom unit ID 228.
# Requires a user-supplied UMS map containing the unit and Brood War data.
set -euo pipefail

if [[ $# -lt 2 || $# -gt 3 ]]; then
  echo "Usage: $0 <map.scx|map.scm> <StarCraft-data-directory> [gfxtest-binary]" >&2
  exit 2
fi
map_path=$1
data_dir=$2
binary=${3:-./build/gfxtest}
mod_file=mods/test_marine.units

for required in "$map_path" "$mod_file" "$binary"; do
  if [[ ! -f "$required" ]]; then
    echo "custom Marine smoke: missing file: $required" >&2
    exit 2
  fi
done
if [[ ! -d "$data_dir" ]]; then
  echo "custom Marine smoke: missing data directory: $data_dir" >&2
  exit 2
fi

export OPENBW_EXTENDED_UNITS="$mod_file"
export OPENBW_EXPECT_UNIT_ID=228
"$binary" --map "$map_path" --game-type ums --headless-map 1 --data-dir "$data_dir"
