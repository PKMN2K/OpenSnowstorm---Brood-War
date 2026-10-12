#include "expanded_units.h"
#include <sstream>
#include <fstream>

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

    if (bwgame::find_unit_type(registry, static_cast<bwgame::UnitTypes>(extension_id)) != &registry.vec[extension_id])
        return fail("runtime resolver extended ID");
    if (bwgame::find_unit_type(registry, bwgame::UnitTypes::Terran_Marine) != &registry.vec[vanilla_id])
        return fail("runtime resolver vanilla ID");
    if (bwgame::find_unit_type(registry, static_cast<bwgame::UnitTypes>(extension_id + 1)) != nullptr)
        return fail("runtime resolver out of range");
    if (bwgame::find_unit_type(registry, static_cast<bwgame::UnitTypes>(-1)) != nullptr)
        return fail("runtime resolver negative ID");

    if (registry.vec.size() != extension_id + 1) return fail("capacity");
    if (static_cast<size_t>(registry.vec[extension_id].id) != extension_id)
        return fail("extended ID");
    if (registry.vec[extension_id].mineral_cost != 75)
        return fail("extended metadata");
    if (registry.vec[vanilla_id].id != bwgame::UnitTypes::Terran_Marine ||
        registry.vec[vanilla_id].mineral_cost != 50)
        return fail("vanilla entry preserved");

    // Registration requires contiguous IDs and cannot overwrite vanilla entries.
    bwgame::unit_types_t loaded;
    loaded.vec.resize(extension_id);
    loaded.vec[vanilla_id].id = bwgame::UnitTypes::Terran_Marine;
    loaded.vec[vanilla_id].mineral_cost = 50;
    bwgame::unit_type_t new_unit{};
    new_unit.mineral_cost = 125;
    if (bwgame::append_extended_unit_type(loaded, bwgame::UnitTypes::Terran_Marine, new_unit))
        return fail("reject existing ID");
    if (bwgame::append_extended_unit_type(loaded, static_cast<bwgame::UnitTypes>(extension_id + 1), new_unit))
        return fail("reject noncontiguous ID");
    if (!bwgame::append_extended_unit_type(loaded, static_cast<bwgame::UnitTypes>(extension_id), new_unit))
        return fail("append new ID");
    if (loaded.vec.size() != extension_id + 1 ||
        bwgame::find_unit_type(loaded, static_cast<bwgame::UnitTypes>(extension_id))->mineral_cost != 125)
        return fail("registered metadata");
    if (loaded.vec[vanilla_id].mineral_cost != 50) return fail("registration altered vanilla");

    // Parse external records and apply to a cloned vanilla unit definition.
    std::istringstream text_spec("300,0,125,25\n301,300,175,50\n");
    bwgame::a_vector<bwgame::extended_unit_spec> specs;
    if (!bwgame::parse_extended_unit_specs(text_spec, specs) || specs.size() != 2)
        return fail("parse external specs");
    bwgame::unit_types_t external;
    external.vec.resize(300);
    external.vec[0].id = bwgame::UnitTypes::Terran_Marine;
    external.vec[0].mineral_cost = 50;
    if (!bwgame::apply_extended_unit_specs(external, specs)) return fail("apply external specs");
    if (external.vec.size() != 302 || external.vec[300].mineral_cost != 125 ||
        external.vec[301].mineral_cost != 175 || external.vec[301].gas_cost != 50 ||
        external.vec[0].mineral_cost != 50) return fail("external specs metadata");
    std::istringstream invalid("300,0,-1,20\n");
    if (bwgame::parse_extended_unit_specs(invalid, specs)) return fail("reject negative field");
    bwgame::a_vector<bwgame::extended_unit_spec> bad{{303, 0, 20, 0}};
    if (bwgame::apply_extended_unit_specs(external, bad))
        return fail("reject skipped ID");

    // A real mod file creates ID 228 by reusing Marine's graphics, weapon,
    // animations, orders, and movement metadata through template inheritance.
    std::ifstream marine_file("mods/test_marine.units");
    if (!marine_file) return fail("open Marine test definition");
    bwgame::a_vector<bwgame::extended_unit_spec> marine_specs;
    if (!bwgame::parse_extended_unit_specs(marine_file, marine_specs))
        return fail("parse Marine test definition");
    bwgame::unit_types_t marine_registry;
    marine_registry.vec.resize(228);
    auto& original_marine = marine_registry.vec[vanilla_id];
    original_marine.id = bwgame::UnitTypes::Terran_Marine;
    original_marine.mineral_cost = 50;
    original_marine.gas_cost = 0;
    original_marine.sight_range = 7;
    original_marine.flingy.id = static_cast<bwgame::FlingyTypes>(12);
    if (!bwgame::apply_extended_unit_specs(marine_registry, marine_specs))
        return fail("register Marine test definition");
    const auto* duplicate = bwgame::find_unit_type(marine_registry, static_cast<bwgame::UnitTypes>(228));
    if (!duplicate || marine_registry.vec.size() != 229 ||
        duplicate->mineral_cost != 125 || duplicate->gas_cost != 0 ||
        duplicate->sight_range != marine_registry.vec[vanilla_id].sight_range ||
        duplicate->flingy.id != marine_registry.vec[vanilla_id].flingy.id ||
        duplicate->id == marine_registry.vec[vanilla_id].id)
        return fail("Marine test inheritance");
    if (marine_registry.vec[vanilla_id].mineral_cost != 50) return fail("vanilla Marine modified");

    std::printf("test_unit_type_registry: PASS (storage only)\n");
    return 0;
}
