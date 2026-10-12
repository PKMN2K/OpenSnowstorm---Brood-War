# Expanded unit system — preliminary architecture audit

Status: discovery only; no simulation behavior changed.

## Goal
Enable additional unit definitions with unique IDs while preserving all existing Brood War units and deterministic simulation.

## Confirmed entry points (initial repository inspection)
- `bwgame_state.h`: `global_state` owns loaded `units_dat`, `weapons_dat`, `upgrades_dat`, and `techdata_dat` byte buffers. Its `game_state` is documented as owning match-fixed type tables.
- `bwgame_tables.h`: includes static lookup tables, but the examined section is mostly geometry/combat-related. Do not assume this is the primary unit registry.
- `ui/gfxtest.cpp`: gameplay client/UI entry point; inspect production menus and command dispatch here and in its dependencies.
- `CMakeLists.txt`: C++14 build; SDL2-based client can be disabled with `OPENSNOWSTORM_BUILD_UI=OFF`; CTest has baseline checks.

## Next investigation
1. Locate `unit_types_t`, `unit_type_t`, and the unit DAT parsing/initialization logic. Record every fixed-count assumption and sentinel ID.
2. Trace how unit ID types are represented in serialization, map loading, triggers, orders, AI, and replay commands.
3. Trace the unit-to-sprite/image/GRP and iscript relationships. Determine how custom asset references are resolved.
4. Trace button layouts, prerequisite validation, production commands, and selection UI.
5. Establish an automated regression test demonstrating vanilla IDs retain behavior before accepting new IDs.
6. Prototype a new test-only unit with an appended ID and no reused vanilla ID; verify load, spawn, movement, combat, build eligibility, serialization, and determinism.

## Compatibility principle
Keep vanilla DAT parsing intact. Design a separate extension layer for additional definitions; do not widen legacy on-disk formats without an explicit compatibility/versioning strategy.

## Known risks / non-goals
- A DAT byte buffer being dynamically sized does **not** establish that all engine registries and ID types are extensible.
- No claim of expanded-unit runtime support is made at this stage.
- This audit is limited to an initial review of repository README and selected source headers, not a complete codebase scan or successful build.

## Follow-up: concrete type-system findings (2026-10-12)
Reviewed `data_types.h`, `game_types.h`, `bwenums.h`, and `bwgame_state.h`.

- `data_types.h:20-28`: `type_container<T>` stores definitions in `a_vector<T> vec`. This is a promising dynamically sized storage layer but does not prove that lookup, initialization, or gameplay code accepts extra entries.
- `data_types.h:30-76`: `id_type_for<unit_type_t>` maps to the `UnitTypes` enum. `type_id<T>` retains both an enum ID and a pointer. Expanding definitions requires auditing enum assumptions, pointer resolution, and sentinel handling.
- `data_types.h:114-170`: `unit_type_t` contains `UnitTypes id`, weapon references, build costs/times, portraits, and references to flingy, orders, and other units. Custom units need consistent cross-table references.
- `bwgame_state.h:138`: `game_state` owns `unit_types_t unit_types`.
- `bwenums.h:10`: `UnitTypes` is declared as `enum struct UnitTypes : int`; its existing named entries are a compatibility surface, not a reason to reuse a vanilla ID.
- `game_types.h:14-32`: `unit_id` is a live-unit *instance* identifier, with an 11-bit index and generation bits. Its 0x7ff mask is **not the unit-type count limit**. Do not conflate these two systems or modify instance IDs as a shortcut for adding unit types.

### Updated implementation direction
Start with a registry/loader audit and a test that validates additional `unit_type_t` entries can be resolved without changing vanilla enum IDs. Preserve existing enum assignments and map/replay formats. Investigate all initialization and dispatch paths before introducing the first custom unit.

### Unverified
No build or gameplay tests have run. The location of all hardcoded type-count loops, data parsing rules, UI button registration, and serialization constraints remains to be established.
