# Apple Variants (Bonus/Poison/Confuse)

## Objectives (MVP)
- BonusApple: +P*mult points, spawn TTL.
- PoisonApple: Temporary snake speed boost (V * k) for T seconds.
- ConfuseApple: Invert controls for T seconds, visual indicator.
- General effects interface (stack/timers), cancel on death/restart.
- VFX: snake flash when eaten, color/shake; separate SFX.
- Difficulty-balanced (spawn chance, duration).

## Technical Tasks
- systems/Effects: timed effects on PlayState (tick/update, clear on reset).
- entities/apples: Normal/Bonus/Poison/Confuse (a common basic interface already exists).
- Spawner: weights for apple types + spawn disabling in walls/tails (available).
- HUD: indicators for active effects and remaining time.
- Config: duration/multiplier/difficulty weighting parameters.

## Test Plan
- Eating each type produces the expected effect and visual.
- The effect ends after T seconds, stacking effects work (several in a row).
- Restarting/death clears the state.