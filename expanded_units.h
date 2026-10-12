#ifndef OPENBW_EXPANDED_UNITS_H
#define OPENBW_EXPANDED_UNITS_H

#include "data_types.h"
#include <istream>
#include <sstream>
#include <string>
#include <limits>

namespace bwgame {

// Initial plain-text format: id,template_id,mineral_cost,gas_cost
// Entries must be contiguous, starting at the current registry size.
// The template must already exist, and can be a previously added unit.
// This is not a full unit definition or a sprite/weapon asset loader.
struct extended_unit_spec {
    int id;
    int template_id;
    int mineral_cost;
    int gas_cost;
};

inline bool parse_nonnegative_int(const std::string& text, int& value) {
    if (text.empty()) return false;
    for (char c : text) if (c < '0' || c > '9') return false;
    try {
        const unsigned long long number = std::stoull(text);
        if (number > static_cast<unsigned long long>(std::numeric_limits<int>::max())) return false;
        value = static_cast<int>(number);
        return true;
    } catch (...) { return false; }
}

inline bool parse_extended_unit_specs(std::istream& input,
                                     a_vector<extended_unit_spec>& output) {
    a_vector<extended_unit_spec> parsed;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream fields(line);
        std::string value;
        int numbers[4];
        for (int i = 0; i != 4; ++i) {
            if (!std::getline(fields, value, ',') || !parse_nonnegative_int(value, numbers[i]))
                return false;
        }
        if (std::getline(fields, value, ',')) return false;
        parsed.push_back({numbers[0], numbers[1], numbers[2], numbers[3]});
    }
    if (input.bad()) return false;
    output = std::move(parsed);
    return true;
}

inline bool apply_extended_unit_specs(unit_types_t& types,
                                      const a_vector<extended_unit_spec>& specs) {
    // Validate before mutation, including ordering and template availability.
    const size_t initial_size = types.vec.size();
    for (size_t i = 0; i < specs.size(); ++i) {
        const auto& spec = specs[i];
        if (spec.id < 0 || static_cast<size_t>(spec.id) != initial_size + i ||
            spec.template_id < 0 || static_cast<size_t>(spec.template_id) >= initial_size + i)
            return false;
    }
    types.vec.reserve(initial_size + specs.size());
    for (const auto& spec : specs) {
        unit_type_t definition = types.vec[static_cast<size_t>(spec.template_id)];
        definition.id = static_cast<UnitTypes>(spec.id);
        definition.mineral_cost = spec.mineral_cost;
        definition.gas_cost = spec.gas_cost;
        if (!append_extended_unit_type(types, definition.id, definition)) return false;
    }
    return true;
}

} // namespace bwgame
#endif
