import json
import os

INPUT_FILE = 'tools/pokemon_sets/pokemon_sets.json'
OUTPUT_FILE = 'tools/pokemon_sets/missing_sets.txt'

def main():
    with open(INPUT_FILE, 'r') as f:
        data = json.load(f)

    missing_species = [species for species, sets in data.items() if "TODO: Set" in sets]

    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        for species in missing_species:
            f.write(f"{species}\n")

if __name__ == '__main__':
    main()
