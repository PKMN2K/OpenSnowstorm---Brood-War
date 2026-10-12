#include "data_types.h"

#include <cstdio>

static int fail(const char* message) {
    std::fprintf(stderr, "test_unit_type_registry: FAIL (%s)\n", message);
    return 1;
}

int main() {
    // Registry storage baseline only. Runtime DAT loading and command dispatch
    // are not yet extended to support additional unit types.
    bwgame::unit_types_t registry;
    const size_t vanilla_id = static_cast<size_t>(bwgame::UnitTypes::Terran_Marine);
    const size_t extension_id = 300;

    registry.vec.resize(extension_id + 1);
    registry.vec[vanilla_id].id = bwgame::UnitTypes::Terran_Marine;
    registry.vec[vanilla_id].mineral_cost = 50;

    registry.vec[extension_id].id = static_cast<bwgame::UnitTypes>(extension_id);
    registry.vec[extension_id].mineral_cost = 75;

    if (registry.vec.size() != extension_id + 1) return fail("capacity");
    if (static_cast<size_t>(registry.vec[extension_id].id) != extension_id)
        return fail("extended ID");
    if (registry.vec[extension_id].mineral_cost != 75)
        return fail("extended metadata");
    if (registry.vec[vanilla_id].id != bwgame::UnitTypes::Terran_Marine ||
        registry.vec[vanilla_id].mineral_cost != 50)
        return fail("vanilla entry preserved");

    std::printf("test_unit_type_registry: PASS (storage only)\n");
    return 0;
}
