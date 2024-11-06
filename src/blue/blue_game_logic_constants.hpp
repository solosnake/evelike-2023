#ifndef blue_game_logic_constants_hpp
#define blue_game_logic_constants_hpp

#include "solosnake/blue/blue_fixed_angle.hpp"

// Game constants. These are not exposed as manipulating these might
// break the game.

/// How many milliseconds long a frame tick is considered to be.
#define BLUE_FRAME_TICK_DURATION_MS                (100)

/// BLUE_FRAME_TICK_DURATION_MS as a floating point value.
#define BLUE_FRAME_TICK_DURATION_MS_F              (100.0f)

/// The number of CPU cycles the default Machine must spin for to execute one
/// Machine instruction. Fewer is better. This is the baseline value which is
/// modified upwards or downwards by the machines Hardware.
#define BLUE_DEFAULT_CYCLES_PER_INSTRUCTION         (100)

/// How many ticks it takes to construct a single Component of a Machine when
/// building a Machine. This value can be modified up or down by the Machine's
/// Hardware. The time required to build a complete Machine depends on the
/// number of components, the ticks to build one components, and the build
/// complexity of the Blueprint being used, as follows:
/// TICKSNEEDED = TICKSFORONECOMPONENT x COMPONENTCOUNT x ( 1 + COMPLEXITY )
#define BLUE_DEFAULT_TICKS_TO_BUILD_1_COMPONENT     (5)

/// How much cap it will cost the Machine to build 10 components. This value
/// is modified upwards or downwards by the machines Hardware.
#define BLUE_DEFAULT_CAP_TO_BUILD_10_COMPONENTS     (100)

/// How many instructions per cycle a baseline Machine is considered to have.
/// This value is used purely in determining a relative measure of the Machine
/// CPU speed.
#define BLUE_CPU_BASELINE_INSTRUCTIONS_PER_CYCLE    (2.0f)

/// Mobility is a factor in calculating a Machine engine's turning power. It
/// is calculated as 1000 x MOBILITY x ENGINEPOWER / MACHINEMASS. The expected
/// magnitude of this figure is about 300.
#define BLUE_DEFAULT_MOBILITY                       (1.0f)

/// This is the upper limit on the capacitance any Machine's Hardware can store
/// at any one time.
#define BLUE_DEFAULT_MAX_CAPACITOR                  (1000)

/// The game works in discrete units of movement, and not floating points.
/// A tile is considered to be this many units wide.
#define BLUE_INT32_TILE_WIDTH                       (1000000)

/// Half the distance of a tile's width.
#define BLUE_INT32_HALF_TILE_WIDTH                  (BLUE_INT32_TILE_WIDTH/2)

/// BLUE_INT32_TILE_WIDTH as a constant floating point.
#define BLUE_FLOAT32_TILE_WIDTH                     (1000000.0f)

/// Lower bound on a machines movement across a tile. We require that a moving
/// Machine actually moves, so we clamp its propulsion calculations to this
/// value. Only machines which have the ability to move will have a minimum
/// movement speed.
#define BLUE_MIN_MACHINE_MOVE_SPEED                 (1)

/// Upper bound on any machines movement across a tile. No Machine can move
/// faster than this. Only machines which have the ability to move will have a
/// maximum movement speed.
#define BLUE_MAX_MACHINE_MOVE_SPEED                 (BLUE_INT32_TILE_WIDTH / 10)

/// The complete circle is subdivided into BLUE_INT32_TWO_PI units, and this
/// is the slowest rotation speed a Machine can reach while turning. Speeds
/// slower than this are clamped to this value. Only machines which have the
/// ability to turn will have a minimum turn speed.
#define BLUE_MIN_MACHINE_TURN_SPEED                 (1)

/// This is the fastest rotation speed any Machine can reach while turning.
/// Speeds faster than this are clamped to this value. Only machines which
/// have the ability to turn will have a maximum turn speed.
#define BLUE_MAX_MACHINE_TURN_SPEED                 (BLUE_INT32_SIXTH_PI / 10)

/// A Machine is not allowed be commanded to make more face turns than this.
/// A single face turn is turning the Machine to face the next hexagon facing
/// (turning left or right) from its current facing. 83 x 6 = 498.
#define BLUE_MAX_FACE_TURNS_LIMIT                   (83 * 6)

/// To calculate a Machine's movement speed across tiles, the default speed is
/// modified by the Hardware modifiers (which depend on the components) and the
/// max speed is modified upwards or downwards and clamped to the min and max.
#define BLUE_DEFAULT_MAX_SPEED                      (BLUE_MAX_MACHINE_MOVE_SPEED)

/// To calculate a Machine's turn speed across tile facing directions, the
/// default turn speed is modified by the Hardware modifiers (which depend on
/// the components) and the max turn speed is modified upwards or downwards and
/// clamped to the min and max.
#define BLUE_DEFAULT_MAX_TURN_SPEED                 (BLUE_MAX_MACHINE_TURN_SPEED)

/// The cap contribution per frame to a Machine from a tile is calculated as
/// the Sun strength time this value times the number of solar panels.
/// TODO: Consider replacing with integer value and have integer only
/// arithmetic.
#define BLUE_SUN_STRENGTH_TO_CAP_FACTOR             (1.0f)

/// When a Machine explodes, the capacitance energy stored is released. Each
/// unit of remaining stored cap in the exploding Machine releases this much
/// damage.
#define BLUE_EXPLOSIVE_DMG_PER_CAPACITANCE          (1.0f)

/// Maximum number of permitted components on any one Machine.
#define BLUE_MAX_MACHINE_COMPONENTS                 (23)

#define BLUE_EXPLOSIVE_DMG_PER_COMPONENT            (1.0f)
#define BLUE_MAX_EXPLOSION_RANGE_F                  (4.0f)
#define BLUE_EXPLOSIVE_RANGE_PER_COMPONENT_F        (BLUE_MAX_EXPLOSION_RANGE_F/BLUE_MAX_MACHINE_COMPONENTS)
#define BLUE_REFINED_PANGUITE_METALS                (1)
#define BLUE_REFINED_PANGUITE_NONMETALS             (1)
#define BLUE_REFINED_PANGUITE_SEMIMETALS            (1)
#define BLUE_REFINED_PANGUITE_ALKALIS               (1)
#define BLUE_REFINED_KAMACITE_METALS                (1)
#define BLUE_REFINED_KAMACITE_NONMETALS             (1)
#define BLUE_REFINED_KAMACITE_SEMIMETALS            (1)
#define BLUE_REFINED_KAMACITE_ALKALIS               (1)
#define BLUE_REFINED_ATAXITE_METALS                 (1)
#define BLUE_REFINED_ATAXITE_NONMETALS              (1)
#define BLUE_REFINED_ATAXITE_SEMIMETALS             (1)
#define BLUE_REFINED_ATAXITE_ALKALIS                (1)
#define BLUE_REFINED_CHONDRITE_METALS               (1)
#define BLUE_REFINED_CHONDRITE_NONMETALS            (1)
#define BLUE_REFINED_CHONDRITE_SEMIMETALS           (1)
#define BLUE_REFINED_CHONDRITE_ALKALIS              (1)
#define BLUE_REFINING_PANGUITE_FACTOR               (1.0f)
#define BLUE_REFINING_KAMACITE_FACTOR               (2.0f)
#define BLUE_REFINING_ATAXITE_FACTOR                (4.0f)
#define BLUE_REFINING_CHONDRITE_FACTOR              (5.0f)
#define BLUE_MAX_CREDITS_PER_MACHINE                (0xFFFF)


#define BLUE_EXPLOSION_DURATION_TICKS               (20)
#define BLUE_FIRING_DURATION_TICKS                  (5)
#define BLUE_DEFAULT_SOUND_FX_RADIUS                (35.0f)
#define BLUE_DEFAULT_SOUND_FX_VOLUME                (1.0f)

#endif
