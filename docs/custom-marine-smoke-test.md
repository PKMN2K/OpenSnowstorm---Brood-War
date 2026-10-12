# Custom Marine integration smoke test

This opt-in headless test requires legally obtained StarCraft: Brood War data files and a map containing a placed custom Marine of ID 228. The normal CI builds do not bundle game content, so they cannot run this integration test automatically.

1. Create a test map (SCX/SCM) containing a unit record with ID 228 for a playable slot. Ensure the map is in UMS mode so authored units are retained. Use a map editor that supports extended unit IDs; standard editors may reject them.
2. Build `gfxtest` with UI support.
3. Run from the repository root (substitute your map and data directory):

```bash
cmake -S . -B build -DOPENSNOWSTORM_BUILD_UI=ON
cmake --build build --target gfxtest
bash scripts/test_custom_marine.sh maps/custom_marine.scx /path/to/your/StarCraft/data
```

The command exits nonzero if the expected ID is missing from the registry, no corresponding instantiated unit exists after map loading, or no sprite is associated with that unit. It prints `custom-unit smoke: PASS` when those checks succeed.

**Scope:** This checks map placement, registration, instance creation, and sprite association only. It does not assert that graphics actually render, or that the unit can move, attack, be selected, or be trained. These require further tests.

**Important:** The mod definition inherits Marine's existing graphic and weapon references. It does not include or distribute any Blizzard content.

The shell runner checks required files and sets `OPENBW_EXTENDED_UNITS` and `OPENBW_EXPECT_UNIT_ID=228` automatically. You can optionally provide the path to a different `gfxtest` binary as a third argument. The integration test is not executed by GitHub-hosted CI because the repository does not supply the proprietary game data or a custom-unit map.

## Creating the custom map record

If your UMS map already contains a Marine belonging to the chosen local player, extract `staredit/scenario.chk` from a copy of its SCX/SCM archive, then run:

```bash
python3 scripts/patch_marine_chk.py scenario.chk scenario_custom.chk --owner 0
```

The script replaces the first qualifying placed Marine (vanilla type 0) with type 228 while preserving all other CHK bytes. Repack `scenario_custom.chk` inside a copied SCX/SCM archive under `staredit/scenario.chk`, then use that map with `scripts/test_custom_marine.sh`. Do not overwrite the original map. UMS player ownership must match the selected slot.

This patcher does **not** itself produce an SCX archive or execute the engine; you still need an MPQ-compatible archive editor and your own StarCraft data. It also does not verify unit movement or combat.
