# Plants vs. Zombies — Project Roadmap

## 1. Completed Tasks (Current Progress)

### Core Architecture & Managers
- [x] **Asset & Resource System (`AssetManager`, `TextureManager`, `TexturePackage`)**
  - Centralized resource warehouse and flyweight texture loader using Raylib.
  - Image/texture caching to prevent redundant file loading (`TexturePackage`).
  - Text asset rendering manager (`TextManager`) with custom font handling.
- [x] **Input Management (`InputManager`)**
  - Unified input system mapping mouse clicks, position, dragging, and key controls.
- [x] **Reanim Engine & Timeline Parser (`ReanimParser`)**
  - XML/Reanim format parsing for animations (keyframe transformations, tracks, frames).
  - Forward-filling state inheritance for missing delta fields (`<t></t>`).
  - Parent-child matrix transformations, scaling, rotations, shearing, and timeline clips handling.

### World & Grid Architecture
- [x] **World Engine (`World`, `Map`, `DayMap`, `Grid`, `Cell`)**
  - Grid cell system (5 rows x 9 columns) for lawn layout management.
  - Cell state tracking (occupied status, plant type indexing).
  - Day map environment base setup and rendering hooks.

### UI & Screen Management
- [x] **State & Screen Flow (`Screen`, `GameplayScreen`, `GameLoop`)**
  - Main game loop orchestrator managing screen updates and rendering cycles.
  - Base screen abstract system allowing seamless screen transitions.
- [x] **Plant Selection & Seed Bank System (`ChoosePlants`, `SeedBank`)**
  - Pre-level plant selection UI allowing players to pick plant cards.
  - Interactive seed bank display bar for chosen seeds during pre-game stage.

### Entity Foundations
- [x] **Plant Hierarchy (`Plant`)**
  - Base `Plant` class structure categorized into Shooter, Sun Produce, and Defensive plant types.
- [x] **Plant Metadata & Attributes System (`PlantMetadata` / `PlantStats`)**
  - [x] Define struct/class holding fundamental plant stats:
    - Base Health.
    - Sun Cost.
    - Projectile Attack Range (e.g. whole lane vs. standard range).
    - Projectile Cooldown / Attack Rate.
    - Base Damage & Excited/Buffed (Plant Food/Supercharged) Damage.
  - [x] Integrate metadata config into `PlantFactory` for modular plant instantiation.
  
### Gameplay Mediator Architecture
- [x] **Gameplay Mediator Interface (`IGameplayMediator` / `GameplayMediator`)**
  - Mediator class constructed for subclasses to interact through `World`.
  - Collision & Target Queries, Spawning & Management, Economy Hooks.

### Combat System & Hitboxes
- [x] **Hitbox & Collision Engine**
  - Hitboxes updated for Plants and Zombies.

---

## 2. Updated Priority Milestones & Roadmap

### Priority 1: Particle System & Visual Effects
- [ ] **Particle System Engine**
  - [ ] Implement generic particle system for visual effects.
  - [ ] Implement Sun entity/particle dropping from the sky (economy).
  - [ ] Implement Zombie death animations (e.g. head popping off, dust/particles).

### Priority 2: Zombie Wave Management & Spawning
- [ ] **Wave Spawner Engine**
  - [ ] Implement zombie wave management for each gameplay session.
  - [ ] Control wave timing, lane selection, and zombie types spawned.
  - [ ] Handle huge waves and final waves.

### Priority 3: Reanim Animation State Machine & Clip Controller
- [ ] **Multi-State Animation System (`ReanimInstance` / `AnimationController`)**
  - [ ] Refactor animation player to support multi-state transitions (Idle, Shooting/Attacking, Blinking, Degraded/Damaged, Excited/Supercharged).
  - [ ] Implement explicit animation clip boundaries (`m_clipStart`, `m_clipEnd`, `m_clipLoopStart`) to enable dynamic switching beyond default idle loops.
  - [ ] Add state trigger events (e.g., transition from `anim_idle` to `anim_shooting` when a zombie enters the row).

### Priority 4: Combat System & Projections
- [ ] **Projectile Mechanics & Combat**
  - [ ] Implement `Projectile` / `Pea` class (velocity, travel distance, impact detection, damage application).
  - [ ] Implement lane-based projections (e.g., raycasting/range checking along rows to detect zombie presence).
  - [ ] Implement Plant actions (Peashooter firing upon target detection; Sunflower generating Sun).
- [ ] **Zombie System & Pathing**
  - [ ] Implement base `Zombie` class and subclasses (`NormalZombie`, `ConeheadZombie`, `BucketheadZombie`).
  - [ ] Implement zombie lane pathing, walk/eat animation state switching, and eating logic when colliding with plant hitboxes.

### Priority 5: Economy, Plant Placement & Interactions
- [x] **Sun Economy System**
  - [x] Implement plant-produced sun drops with click-to-collect logic.
  - [x] Connect Seed Card Sun cost checks and cooldown timers.
  - [x] Block plant placement when balance is insufficient.
  - [x] HUD sun counter display (panel + icon + centered text).
- [ ] **Plant Placement & Removal**
  - [ ] Drag-and-drop / click-to-place plant preview on grid cells.
  - [ ] Shovel tool implementation for plant removal.
  - [ ] Lawn mowers line defense system.

### Priority 6: Audio, Screen Flow & Diagrams / Documentation
- [ ] **Screen Flow & Architecture Design**
  - [ ] Draw/Document UML Class Diagram & Gameplay / Rendering Sequence Workflows.
  - [ ] Implement Main Menu, Victory, and Game Over screen states.
- [ ] **Audio & SFX**
  - [ ] Raylib `AudioDevice` SFX and Background Music integration.
