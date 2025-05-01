import os
from collections import OrderedDict

INPUT_FILE = 'src/data/pokemon/pokemon.sets'
OUTPUT_FILE = 'src/data/pokemon/pokemon_sets.h'

DEFAULT_IV = 31
DEFAULT_EV = 0
DEFAULT_MOVE = "MOVE_NONE"
DEFAULT_ITEM = "ITEM_NONE"
DEFAULT_NATURE = "NATURE_HARDY"
DEFAULT_ABILITY = "ABILITY_NONE"

# Stat order: HP, ATK, DEF, SPEED, SPATK, SPDEF
STAT_INDEX = {
    'hp': 0,
    'atk': 1,
    'def': 2,
    'spe': 3, 'speed': 3,
    'spa': 4, 'spatk': 4, 'spattack': 4,
    'spd': 5, 'spdef': 5, 'spdefense': 5
}

def sanitize(name):
    return name.replace("'", "").replace("%", "").replace("-", "_").replace(' ', '_')

def convert_species(name):
    return f"SPECIES_{sanitize(name).upper()}"

def convert_item(item):
    return f"ITEM_{sanitize(item).upper()}"

def convert_ability(ability):
    return f"ABILITY_{sanitize(ability).upper()}"

def convert_nature(nature):
    return f"NATURE_{nature.upper()}"

def convert_move(move):
    if "Hidden Power" in move:
        return "MOVE_HIDDEN_POWER"
    return f"MOVE_{sanitize(move).upper()}"

def parse_stat_line(text, default):
    stats = [default] * 6
    if not text:
        return stats
    for part in text.split('/'):
        part = part.strip()
        if part:
            value, stat = part.rsplit(' ', 1)
            stats[STAT_INDEX[stat.lower()]] = int(value)
    return stats

def parse_sets(text):
    lines = [line.strip() for line in text.strip().splitlines() if line.strip()]
    sets = OrderedDict()
    i = 0
    while i < len(lines):
        if lines[i].startswith("==="):
            display_name = lines[i].strip("= ")
            i += 1
            
            comment = lines[i].strip("= ") if i < len(lines) else ""
            i += 1
            
            species = "None"
            item = "None"
            if i < len(lines) and "@" in lines[i]:
                species, item = [x.strip() for x in lines[i].split("@")]
                i += 1
            
            data = {
                "display_name": display_name,
                "comment": comment,
                "species": species,
                "item": item,
                "ability": "None",
                "tera_type": "Normal",
                "nature": "Hardy",
                "evs": [DEFAULT_EV] * 6,
                "ivs": [DEFAULT_IV] * 6,
                "moves": []
            }
            
            # Parse remaining attributes
            while i < len(lines) and not lines[i].startswith("==="):
                line = lines[i]
                if line.startswith("Ability:"):
                    data["ability"] = line.split(":", 1)[1].strip()
                elif "Nature" in line:
                    data["nature"] = line.split()[0]
                elif line.startswith("EVs:"):
                    data["evs"] = parse_stat_line(line[4:].strip(), DEFAULT_EV)
                elif line.startswith("IVs:"):
                    data["ivs"] = parse_stat_line(line[4:].strip(), DEFAULT_IV)
                elif line.startswith("Tera Type:"):
                    data["tera_type"] = line.split(":", 1)[1].strip()
                elif line.startswith("-"):
                    data["moves"].append(line[2:].strip())
                i += 1
            
            sets[species] = data
        else:
            i += 1
    return sets

def generate_struct(species, data):
    species_key = convert_species(data["species"])
    moves = (data["moves"] + [""]*4)[:4]
    move_str = ', '.join(convert_move(m) if m else DEFAULT_MOVE for m in moves)
    tera_type = f"TYPE_{data['tera_type'].upper()}" if data["tera_type"] else "TYPE_NONE"
    
    return f"""    [{species_key}] = // {data['comment']}
    {{
        .name = _("{data['display_name']}"),
        .item = {convert_item(data['item'])},
        .ability = {convert_ability(data['ability'])},
        .teraType = {tera_type},
        .nature = {convert_nature(data['nature'])},
        .evs = {{{data['evs'][0]}, {data['evs'][1]}, {data['evs'][2]}, {data['evs'][3]}, {data['evs'][4]}, {data['evs'][5]}}},
        .ivs = {{{data['ivs'][0]}, {data['ivs'][1]}, {data['ivs'][2]}, {data['ivs'][3]}, {data['ivs'][4]}, {data['ivs'][5]}}},
        .moves = {{{move_str}}}
    }}"""

def main():
    with open(INPUT_FILE) as f:
        text = f.read()
    sets = parse_sets(text)
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        f.write('#include "constants/species.h"\n')
        f.write('#include "constants/items.h"\n')
        f.write('#include "constants/abilities.h"\n')
        f.write('#include "constants/moves.h"\n\n')
        f.write("//\n")
        f.write("// DO NOT MODIFY THIS FILE! It is auto-generated from pokemon.sets\n")
        f.write("//\n")
        f.write("const struct PokemonSets gPokemonSets[NUM_SPECIES] =\n{\n")
        f.write(",\n\n".join(generate_struct(species, data) for species, data in sets.items()))
        f.write("\n};\n")

if __name__ == "__main__":
    main()
