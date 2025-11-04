# SnakeOff is an arcade snake game created as part of a training project assignment.

> This project implements a modular and easily expandable version of the classic Snake game in **C++17 and SFML 2.6**, compiled using **CMake**. 
> The game logic and rendering are separated, with rendering implemented as specialized modules (Snake/Items/Portals/Explosions). There is a separate Debug Overlay (F3) with an **FPS counter** 
> and frame timings, a difficulty system, pause/win/lose, a high score table, portals, power-ups, shader effects, and a procedural level generation system.

---

## Contents
- [Features](#features)
- [Project Structure](#project-structure)
- [Architecture](#architecture)
  - [Core](#core)
  - [World](#world)
  - [Entities](#entities)
  - [Systems](#systems)
  - [Render](#render)
  - [UI,VFX,Util,Resources](#uivfxutilresources)
- [Gameplay](#gameplay)
- [Controls and Input](#controls-and-input)
- [Difficulty and Settings](#difficulty-and-settings)
- [High Scores](#high-scores)
- [Debugging and Profiling](#debugging-and-profiling)
- [Expansion Plans](#Expansion Plans)
  - [New Power-ups](#New Power-ups)
  - [Power-downs](#Power-downs)
  - [Enemies](#Enemies)
  - [Level Bosses](#Level Bosses)
- [Performance and Optimizations](#Performance-and-Optimizations)
- [Roadmap](#roadmap)
- [License](#license)
- [Author](#author)


---

## Features
- Clean architecture: logic, states, resources, systems, and rendering are strictly separated by responsibility
- Multiple screens: **Menu / Difficulty / Settings / Play / Pause / Victory / GameOver / HighScores**
- **Portals**, **power-ups**, **explosion animations**, **HUD**, **shader effects**, **CameraShake**, **plug-in disappearing obstacles**
- **Debug Overlay (F3)**: FPS, Update timings, Render and render counters
- **Procedural Generation** of levels
- **Difficulty** system with parameters (speed, spawn chance, timings, etc.)
- **HighScores** saves data

---

## Project structure
```
include/
  core/        # Application, StateMachine, IGameState, Config, Resources, Input, Perf, DebugOverlay
  world/       # Grid, Level, CellType, Portal, ProcGen
  entities/    # Snake, Apple, NormalApple, Powerup, Entity
  systems/     # Collision, Spawner, Effects, Score, HighScores
  render/      # SnakeRenderer, ItemsRenderer, PortalsRenderer, ExplosionsRenderer, GridBatch etc.
  states/      # MenuState, DifficultyState, SettingsState, PlayState, PauseState, GameOverState, VictoryState, HighScoresState
  ui/          # HUD
  vfx/         # CameraShake
  util/        # Random, Types, Vec2
  resources/   # Wrappers/helpers for loading assets
src/           # Implementations of corresponding headers
CMakeLists.txt
README.md
data/          # highscores.tsv (created at runtime)
```

---

## Architecture

### Core
- **Application** - initialization of the window, main loop, and common subsystems.
- **StateMachine / IGameState** - screen-scene control. Each screen is a separate `IGameState`.
- **Config** - current difficulty and access to `DifficultyParams` (`paramsFor(Difficulty)`).
- **Resources** - centralized loading of fonts, textures, sounds, caching, and key lookup.
- **Input** - input abstraction layer: `InputAction` buttons.
- **Perf and DebugOverlay** — timers, counters, and overlay (F3) with FPS and frame timings.

### World
- **Grid and CellType** — discrete level grid and cell types.
- **Level** —  level description (dimensions, starting positions, exits, etc.).
- **Portal** — portal transitions.
- **ProcGen** — procedural generation of level content.

### Entities
- **Entity** — base type.
- **Snake** — the snake character itself and its head, body, and tail, turns, collisions, transfer through portals.
- **Apple and NormalApple** — pples, scoring logic.
- **Powerup** — basic power-up, expandable through enumeration and processing in `Effects`.

### Systems
- **Collision** — intersections between the snake and the world and its objects.
- **Spawner** — spawning apples, power-ups, effects based on probabilities and cooldowns.
- **Effects** — applying effects (acceleration, deceleration, etc.).
- **Score and HighScores** — score and high score table.

### Render
- Specialized renderers:
  - **SnakeRenderer** —  the snake and its segments;
  - **ItemsRenderer** — apples and power-ups;
  - **PortalsRenderer** — portals (including their “glow and pulse” parameters);
  - **ExplosionsRenderer** — explosion animation.
- The general idea: each render block receives a minimal slice of data and buffers ready for batching, without access to heavy state types.

### UI, VFX, Util, Resources
- **HUD** — points, hints, effect indicators.
- **CameraShake** — camera effects during events (explosions and collisions).
- **Random, Vec2, Types** — service utilities (including `thread_local` PRNG).
- **Resources** — loading assets once, returning via link.

---

## Gameplay
- You need to collect apples, avoid collisions with the outer perimeter walls and obstacles inside the perimeter, and, of course, with yourself.
- Portals transport you to other areas of the level; entering one takes you out of another.
- Power-ups temporarily change parameters (speed, extra points, debuff effects, and the ability to temporarily destroy obstacles inside the perimeter).
- There are wins and losses, which are represented by separate states. To win, you need to score 50 points in the first level, 150 in the second, and 350 in the last. When you score the required number of points, one cell of the outer perimeter of the level opens up.

---

## Controls and input system
The `core/Input` layer maps high-level actions (`InputAction`) to **SFML** keys. Examples of actions:
- Locomotion: `MoveUp/Down/Left/Right`, as well as `W/S/A/D`
- Pause: `Pause` key `P` during gameplay
- Restart and confirm: `Confirm`
- **Debug Overlay**: `ShowDebug` (key **F3**)

> Keys are set centrally in the input code; they can be easily changed globally if necessary.

---

## Difficulty and parameters
Difficulty types and parameters are described in `core/Config` and `util/Types`:
- `Difficulty` — list of difficulty levels
- `DifficultyParams` — aggregate with parameters (tick rate, spawn frequency/chances, etc.)
- `Config::paramsFor(Difficulty)` — get a set of parameters for the current level

Difficulty selection screen — `states/DifficultyState`.

---

## Highscores
The `systems/HighScores` system stores records and can **save and load** them from a file. The path is set at the **HighScoresState** state level:
```
const std::string path_ = "data/highscores.tsv";
```
**TSV** format (one line per record). The file is created automatically if the directory/file does not exist.

---

## Debugging and profiling
- Pressing **F3** opens the **Debug Overlay** with:
  - **FPS** (average value);
  - frame stage timings (Update, Render, and passes);
  - render counters (batches/primitives, etc., if enabled in the configuration);
- Timers (`PerfTimer`) and FPS counter (`FpsCounter`) are available in the code in `core/Perf.h`.

---

## Expansion plans

### New power-ups
### Power-downs
### Enemies
### Level bosses

---

## Performance and optimizations
Some key steps already implemented in the code base:
1. **Moving rendering to specialized modules**: Snake/Items/Portals/**Explosions**.
2. **Minimizing allocations**: vector reservation, reuse of temporary buffers per frame.
3. **Eliminating unnecessary dependencies**: point includes, forward declarations where possible.
4. **Resources**: unified asset loading and caching, no repeated disk accesses.
5. **Input layer**: centralized layout, no duplication or potential errors.
6. **DebugOverlay + Perf**: performance measurement directly on the frame, quick analysis.
7. **Random**: `thread_local` generator in `util/Random`, creation of deterministic seeds when necessary.
8. **ExplosionsRenderer**: separate batch, does not interfere with the main render passes.

What else can be done in the future:
- **Texture atlas** (group sprites, reduce texture switching);
- ***Instancing** for mass elements of the same type;
- **Object pools** for short-lived particles and sprites;

---

## Roadmap
- Texture atlas and minimization of state changes
- Extended power-up/down system
- Enemies and level bosses
- Improved effects (shader portal pulsations, explosion flashes)
- Logic autotests (collisions, spawns, points)
- UI/HUD localization

---

## License
MIT

---

###  Author
SnakeOff — is an educational demonstration project focused on architecture and performance. Author: Kuzmenko Pavel

