import os

INPUT_FILE = 'src/data/pokemon/pokemon.sets'
OUTPUT_FILE = 'tools/pokemon_sets/missing_sets.txt'

def find_missing_sets():
    missing_species = []
    current_species = None
    has_todo = False
    
    with open(INPUT_FILE, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith("==="):
                # First === line is the species name
                if current_species is not None and has_todo:
                    missing_species.append(current_species)
                
                current_species = line.strip("= ")
                has_todo = False
                
                # Check the next line for TODO
                next_line = f.readline().strip()
                if "TODO" in next_line:
                    has_todo = True
            elif current_species is not None and "TODO" in line:
                has_todo = True
    
    # Check the last species
    if current_species is not None and has_todo:
        missing_species.append(current_species)
    
    return missing_species

def main():
    missing_species = find_missing_sets()
    
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        for species in missing_species:
            f.write(f"{species}\n")

if __name__ == '__main__':
    main()