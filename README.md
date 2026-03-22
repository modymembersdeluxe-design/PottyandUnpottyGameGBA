# Potty and Unpotty Game GBA – Deluxe Long Rebuild

This project is a **safe, family-friendly Game Boy Advance prototype** with a
retro 2000s style and a longer "Deluxe" gameplay loop.

## Deluxe design goals
- Longer session flow with multiple mini-games.
- Ready for AI-assisted replacement of sprites, sounds, and visual effects.
- Build output is a `.gba` ROM for emulator/device testing.

## Current playable modes
1. **Dash to Potty** (arcade movement + hazards/bonuses)
2. **Memory Signals** (repeat D-pad sequence)
3. **Rhythm Clap** (tap A on timing window)
4. **Deluxe Results** after 5 successful rounds

## Build requirements
- [devkitPro](https://devkitpro.org/) with `devkitARM`
- `libgba`
- `make`

## Build
```bash
make
```

ROM output:
- `build/potty_unpotty.gba`

## Controls
- D-Pad: movement and memory inputs
- A: confirm/start/timing tap
- START: pause (during mini-games)

## Project structure
- `src/main.c` - game loop, mode state machine, rendering, mini-game logic
- `include/game.h` - shared game structs and APIs
- `Makefile` - build pipeline to `.gba`

## Asset expansion plan (for artists/AI)
- Replace rectangle placeholders with OBJ sprite sheets.
- Add short SFX (collect, miss, win, pause) via m4a or Maxmod.
- Add parallax backgrounds and mode-specific palettes.
- Add extra mini-games and unlockable costume/sprite variants.


## Full platform build tutorial
- See `BUILD_TUTORIAL.md` for Windows, macOS, and Linux setup steps.
