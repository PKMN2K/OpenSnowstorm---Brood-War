# Custom Marine integration smoke test

This opt-in headless test requires legally obtained StarCraft: Brood War data files and a map containing a placed custom Marine of ID 228. The normal CI builds do not bundle game content, so they cannot run this integration test automatically.

1. Create a test map (SCX/SCM) containing a unit record with ID 228 for a playable slot. Ensure the map is in UMS mode so authored units are retained. Use a map editor that supports extended unit IDs; standard editors may reject them.
2. Build `gfxtest` with UI support.
3. Run from the repository root (substitute your map and data directory):

```bash
cmake -S . -B build -DOPENSNOWSTORM_BUILD_UI=ON
cmake --build build --target gfxtest
OPENBW_EXTENDED_UNITS=mods/test_marine.units \
OPENBW_EXPECT_UNIT_ID=228 \
./build/gfxtest --map maps/custom_marine.scx --game-type ums \
  --headless-map 1 --data-dir /path/to/your/StarCraft/data
```

The command exits nonzero if the expected ID is missing from the registry, no corresponding instantiated unit exists after map loading, or no sprite is associated with that unit. It prints `custom-unit smoke: PASS` when those checks succeed.

**Scope:** This checks map placement, registration, instance creation, and sprite association only. It does not assert that graphics actually render, or that the unit can move, attack, be selected, or be trained. These require further tests.

**Important:** The mod definition inherits Marine's existing graphic and weapon references. It does not include or distribute any Blizzard content.
