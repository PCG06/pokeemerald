# Gen 5 Black/White Battle UI

This branch replaces the battle interface with the one
from Pokémon Black and White. It contains the battle UI and nothing else, so it can be
merged into an existing project on its own. Battle backgrounds are left alone.

| | |
| --- | --- |
| ![The action box: FIGHT, BAG, POKéMON and RUN in a four panel box beside the message box, with the animated cursor framing FIGHT](docs/screenshots/action-box.png) | ![The move box: four panels tinted by move type, each showing its PP, above the BW healthboxes and type icons](docs/screenshots/move-box.png) |

## Configuration

Everything lives in `include/config/bw_battle_ui.h`:

| Setting | Default | What it does |
| --- | --- | --- |
| `BW_BATTLE_UI` | `TRUE` | Master switch for everything below. |
| `BW_BATTLE_UI_TEXTBOX` | `TRUE` | The BW message box. |
| `BW_BATTLE_UI_INPUTBOX` | `TRUE` | The action box, move box and cursor. Requires the textbox. |

Turning the textbox off returns the lower half of the screen to the Gen 3 look. Turning
off only the input box keeps the BW message box but restores the Gen 3 menus. The
healthboxes and type icons are a separate port and are **not** switched off by any of
these, so the top half stays BW either way.

Type icons are controlled by expansion's own `B_SHOW_TYPES` in `include/config/battle.h`,
which this branch sets to `SHOW_TYPES_ALWAYS`.

One requirement to be aware of: `B_MOVE_REARRANGEMENT_IN_BATTLE` must be `GEN_4` or later,
which is the expansion default. The BW move box has nowhere to show the Gen 3 move
swapping prompt, so a lower setting stops the build with an explanatory error rather than
producing a broken menu.

## Credits

Almost none of this is my own work. It is a port, and it exists because of:

- **[EternalCode](https://github.com/PlatinumMaster/EternalCode-BWHealthBars-BPRE)** for the
  original Black/White health bar design, graphics and FireRed implementation.
- **[PlatinumMaster](https://github.com/PlatinumMaster)** for maintaining a buildable
  source of that health bar implementation.
- **[NicoSwag](https://github.com/NicoSwag/pokeemerald-expansion/tree/nicos_cool_ui)** for
  the Nico's Cool UI battle type-icon artwork and layout.
- **[mudskipper13](https://github.com/mudskipper13/pokeemerald/tree/feature/bwBattleUI)** for
  the Black/White message box, action box, move box and cursor, and the outlined battle
  UI font.

If you use this branch, please credit myself and all of the above.

---

# About `pokeemerald-expansion`

![Gif that shows debugging functionality that is unique to pokeemerald-expansion such as rerolling Trainer ID, Cheat Start, PC from Debug Menu, Debug PC Fill, Pokémon Sprite Visualizer, Debug Warp to Map, and Battle Debug Menu](https://github.com/user-attachments/assets/cf9dfbee-4c6b-4bca-8e0a-07f116ef891c) ![Gif that shows overworld functionality that is unique to pokeemerald-expansion such as indoor running, BW2 style map popups, overworld followers, DNA Splicers, Gen 1 style fishing, OW Item descriptions, Quick Run from Battle, Use Last Ball, Wild Double Battles, and Catch from EXP](https://github.com/user-attachments/assets/383af243-0904-4d41-bced-721492fbc48e) ![Gif that shows off a number of modern Pokémon battle mechanics happening in the pokeemerald-expansion engine: 2 vs 1 battles, modern Pokémon, items, moves, abilities, fully customizable opponents and partners, Trainer Slides, and generational gimmicks](https://github.com/user-attachments/assets/50c576bc-415e-4d66-a38f-ad712f3316be)

<!-- If you want to re-record or change these gifs, here are some notes that I used: https://files.catbox.moe/05001g.md -->

**`pokeemerald-expansion`** is a GBA ROM hack base that equips developers with a comprehensive toolkit for creating Pokémon ROM hacks. **`pokeemerald-expansion`** is built on top of [pret's `pokeemerald`](https://github.com/pret/pokeemerald) decompilation project. **It is not a playable Pokémon game on its own.**

# [Features](FEATURES.md)

**`pokeemerald-expansion`** offers hundreds of features from various [core series Pokémon games](https://bulbapedia.bulbagarden.net/wiki/Core_series), along with popular quality-of-life enhancements designed to streamline development and improve the player experience. A full list of those features can be found in [`FEATURES.md`](FEATURES.md).

# [Credits](CREDITS.md)

 [![](https://img.shields.io/github/all-contributors/rh-hideout/pokeemerald-expansion/upcoming)](CREDITS.md)

If you use **`pokeemerald-expansion`**, please credit **RHH (Rom Hacking Hideout)**. Optionally, include the version number for clarity.

```
Based off RHH's pokeemerald-expansion 1.16.2 https://github.com/rh-hideout/pokeemerald-expansion/
```

Please consider [crediting all contributors](CREDITS.md) involved in the project!

# Choosing `pokeemerald` or **`pokeemerald-expansion`**

- **`pokeemerald-expansion`** supports multiplayer functionality with other games built on **`pokeemerald-expansion`**. It is not compatible with official Pokémon games.
- If compatibility with official games is important, use [`pokeemerald`](https://github.com/pret/pokeemerald). Otherwise, we recommend using **`pokeemerald-expansion`**.
- **`pokeemerald-expansion`** incorporates regular updates from `pokeemerald`, including bug fixes and documentation improvements.

# [Getting Started](INSTALL.md)

❗❗ **Important**: Do not use GitHub's "Download Zip" option as it will not include commit history. This is necessary if you want to update or merge other feature branches.

If you're new to git and GitHub, [Team Aqua's Asset Repo](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/) has a [guide to forking and cloning the repository](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/wiki/The-Basics-of-GitHub). Then you can follow one of the following guides:

## 📥 [Installing **`pokeemerald-expansion`**](INSTALL.md)
## 🏗️ [Building **`pokeemerald-expansion`**](INSTALL.md#Building-pokeemerald-expansion)
## 🚚 [Migrating from **`pokeemerald`**](INSTALL.md#Migrating-from-pokeemerald)
## 🚀 [Updating **`pokeemerald-expansion`**](INSTALL.md#Updating-pokeemerald-expansion)

# [Documentation](https://rh-hideout.github.io/pokeemerald-expansion/)

For detailed documentation, visit the [pokeemerald-expansion documentation page](https://rh-hideout.github.io/pokeemerald-expansion/).

# [Contributions](CONTRIBUTING.md)
If you are looking to [report a bug](CONTRIBUTING.md#Bug-Report), [open a pull request](CONTRIBUTING.md#Pull-Requests), or [request a feature](CONTRIBUTING.md#Feature-Request), our [`CONTRIBUTING.md`](CONTRIBUTING.md) has guides for each.

# [Community](https://discord.gg/6CzjAG6GZk)

[![](https://dcbadge.limes.pink/api/server/6CzjAG6GZk)](https://discord.gg/6CzjAG6GZk)

Our community uses the [ROM Hacking Hideout (RHH) Discord server](https://discord.gg/6CzjAG6GZk) to communicate and organize. Most of our discussions take place there, and we welcome anybody to join us!
