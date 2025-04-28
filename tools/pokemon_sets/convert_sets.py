import json
import os
from collections import OrderedDict

# Configuration
INPUT_FILE = 'tools/pokemon_sets/pokemon_sets.json'
OUTPUT_FILE = 'src/data/pokemon/pokemon_sets.h'

# Default values
DEFAULT_IV = 31
DEFAULT_EV = 0
DEFAULT_MOVE = "MOVE_NONE"
DEFAULT_ITEM = "ITEM_NONE"
DEFAULT_NATURE = "NATURE_HARDY"
DEFAULT_ABILITY = "ABILITY_NONE"

def convert_species(name):
    """Convert species name to constant format"""
    name = name.replace("'", "").replace("%", "").replace("-", "_")
    return f"SPECIES_{name.upper().replace(' ', '_')}"

def convert_item(item):
    if isinstance(item, list):
        item = item[0]
    item = item.replace("'", "").replace("-", "_")
    return f"ITEM_{item.upper().replace(' ', '_')}"

def convert_nature(nature):
    if isinstance(nature, list):
        nature = nature[0]
    return f"NATURE_{nature.upper()}"

def convert_ability(ability):
    if isinstance(ability, list):
        ability = ability[0]
    ability = ability.replace("'", "").replace("-", "_").replace(" ", "_")
    return f"ABILITY_{ability.upper()}"

def convert_move(move):
    if isinstance(move, list):
        move = move[0]
    if "Hidden Power" in move:
        return "MOVE_HIDDEN_POWER"
    move = move.replace("'", "").replace("-", "_")
    return f"MOVE_{move.upper().replace(' ', '_')}"

def process_pokemon(set_data, species_name):
    """Generate array elements for one Pokémon set"""
    # Get display name (default to species name if not specified)
    display_name = set_data.get("name", species_name)
    
    # Process EVs
    evs = [DEFAULT_EV]*6
    if "evs" in set_data:
        ev_data = set_data["evs"]
        if isinstance(ev_data, list):
            ev_data = ev_data[0]
        stat_map = {"hp":0, "atk":1, "def":2, "spe":3, "spa":4, "spd":5}
        for stat, value in ev_data.items():
            if stat.lower() in stat_map:
                evs[stat_map[stat.lower()]] = value

    # Process IVs
    ivs = [DEFAULT_IV]*6
    if "ivs" in set_data:
        iv_data = set_data["ivs"]
        if isinstance(iv_data, list):
            iv_data = iv_data[0]
        stat_map = {"hp":0, "atk":1, "def":2, "spe":3, "spa":4, "spd":5}
        for stat, value in iv_data.items():
            if stat.lower() in stat_map:
                ivs[stat_map[stat.lower()]] = value

    # Process moves
    moves = []
    if "moves" in set_data:
        moves = [convert_move(m) for m in set_data["moves"][:4]]
    moves += [DEFAULT_MOVE] * (4 - len(moves))

    # Process other attributes
    item = convert_item(set_data["item"]) if "item" in set_data else DEFAULT_ITEM
    nature = convert_nature(set_data["nature"]) if "nature" in set_data else DEFAULT_NATURE
    ability = convert_ability(set_data["ability"]) if "ability" in set_data else DEFAULT_ABILITY

    return f"""        .name = _("{display_name}"),
        .item = {item},
        .ability = {ability},
        .nature = {nature},
        .evs = {{{', '.join(map(str, evs))}}},
        .ivs = {{{', '.join(map(str, ivs))}}},
        .moves = {{{', '.join(moves)}}}"""

def main():
    # Read JSON data
    with open(INPUT_FILE, 'r') as f:
        data = json.load(f, object_pairs_hook=OrderedDict)

    # Prepare output directory
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)

    # Generate header file
    with open(OUTPUT_FILE, 'w') as f:
        f.write("""#include "constants/species.h"
#include "constants/items.h"
#include "constants/abilities.h"
#include "constants/moves.h"

// Auto-generated from pokemon_sets.json
const struct PokemonSets gPokemonSets[NUM_SPECIES] =
{
""")
        
        species_dict = OrderedDict()
        for species, species_data in data.items():
            for set_name, set_data in species_data.items():
                species_key = convert_species(species)
                set_comment = f"// {set_name}"
                species_dict[species_key] = (process_pokemon(set_data, species), set_comment)
                break  # Only take first set per species

        # Write all species entries
        for i, (species, (set_code, set_comment)) in enumerate(species_dict.items()):
            f.write(f"    [{species}] = {set_comment}\n    {{\n{set_code}\n    }}")
            if i != len(species_dict) - 1:
                f.write(",\n\n")  # Comma and extra newline between entries
            else:
                f.write("\n")  # No comma after last entry
        
        f.write("""};
""")

if __name__ == '__main__':
    main()