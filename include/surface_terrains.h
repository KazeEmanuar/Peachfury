#ifndef SURFACE_TERRAINS_H
#define SURFACE_TERRAINS_H

#define TERRAINSOUND 0xF0000000
#define PARTICLES 0x0F000000
#define CAMANGLE 0x00F00000
#define CAMFLAGS 0x000F0000
#define SPECIALPROPERTY 0x0000FF00
#define PROPERTYFLAGS 0x000000FF

#define SOUND_TERRAIN_DEFAULT 0 // e.g. air
#define SOUND_TERRAIN_GRASS 1 //dampened steps
#define SOUND_TERRAIN_WATER 2 //splish splash
#define SOUND_TERRAIN_STONE 3 //hard steps, kinda sound like theres friction on the floor
#define SOUND_TERRAIN_SPOOKY 4 // squeaky floor
#define SOUND_TERRAIN_SNOW 5
#define SOUND_TERRAIN_ICE 6
#define SOUND_TERRAIN_SAND 7
#define SOUND_TERRAIN_METAL 8
#define SOUND_TERRAIN_WOOD 9
#define PARTICLE_TERRAIN_NONE 0
#define PARTICLE_TERRAIN_NONE 1
#define PARTICLE_TERRAIN_WATER 2 //wave around mario
#define PARTICLE_TERRAIN_NONE 3
#define PARTICLE_TERRAIN_NONE 4
#define PARTICLE_TERRAIN_SNOW 5 //snow partciles
#define PARTICLE_TERRAIN_NONE 6
#define PARTICLE_TERRAIN_SAND 7 //dirt particles
#define PARTICLE_TERRAIN_LEAFS 8
#define PARTICLE_TERRAIN_ACTIVATENVIRONMENT 9 //for lava etc
#define PARTICLE_TERRAIN_SPARKLES 10
#define CAMERA_TERRAIN_DEFAULT 0x0
#define CAMERA_TERRAIN_NOSURFBACK 0x1
#define CAMERA_TERRAIN_KEEP_IN 0x2
#define CAMERA_TERRAIN_GOTHROUGH 0x3
#define SPECFLAG_BURNING 0x1
#define SPECFLAG_HANGABLE 0x2
#define SPECFLAG_CANGETSUCK 0x4
#define SPECFLAG_SINKSAND 0x8
#define SPECFLAG_SLOWDOWN 0x10
#define SPECFLAG_SLIPPERY 0x20
#define SPECFLAG_MEGASLIP 0x40
#define SPECFLAG_CLIMBABLE 0x80

#define TERRAIN(sound, particles, camAngle, camFlags, specialprop, Prop) (((u32) (sound) << 28) | ((u32) (particles) << 24) | ((u32) (camAngle) << 20) | ((u32) (camFlags) << 16)| ((u32) (specialprop) << 8) | ((u32) (Prop)))
#define SURFACETYPE(surf) (surf->type & 0xFF)

// Surface Types
#define SURFACE_DEFAULT TERRAIN(0,0,0,0,0,0)       // Environment default
#define SURFACE_BURNING TERRAIN(0,PARTICLE_TERRAIN_ACTIVATENVIRONMENT,0,0,SPECFLAG_BURNING,0x0001)       // Lava / Frostbite (in SL), but is used mostly for Lava
#define SURFACE_HANGABLE TERRAIN(0,0,0,0,SPECFLAG_HANGABLE,0x0005)      // Ceiling that Mario can climb on
#define SURFACE_DEATH_PLANE TERRAIN(0,0,0,0,0,0x000A)   // Death floor
#define SURFACE_FLOWING_WATER TERRAIN(SOUND_TERRAIN_WATER,PARTICLE_TERRAIN_WATER,0,0,0,0x000E) // Water (flowing), has parameters
#define SURFACE_VERY_SLIPPERY TERRAIN(5,0,0,0,SPECFLAG_MEGASLIP,0x0013) // Very slippery, mostly used for slides
#define SURFACE_SLIPPERY TERRAIN(2,0,0,0,SPECFLAG_SLIPPERY,0x0014)      // Slippery
#define SURFACE_NOT_SLIPPERY TERRAIN(3,0,0,0,SPECFLAG_CLIMBABLE,0x0015)       // Non-slippery, climbable
#define SURFACE_INSTANT_WARP_1B    TERRAIN(0,0,0,0,0,0x001B)
#define SURFACE_INSTANT_WARP_1C    TERRAIN(0,0,0,0,0,0x001C)
#define SURFACE_INSTANT_WARP_1D    TERRAIN(0,0,0,0,0,0x001D)
#define SURFACE_INSTANT_WARP_1E    TERRAIN(0,0,0,0,0,0x001E)
#define SURFACE_SHALLOW_QUICKSAND TERRAIN(7,7,0,0,SPECFLAG_SINKSAND,0x0021)
#define SURFACE_DEEP_QUICKSAND TERRAIN(0,0,0,0,0,0x22)           // Quicksand (lethal, slow, depth of 160 units)
#define SURFACE_INSTANT_QUICKSAND TERRAIN(0,0,0,0,0,0x23)         // Quicksand (lethal, instant)
#define SURFACE_DEEP_MOVING_QUICKSAND TERRAIN(0,0,0,0,0,0x24)       // Moving quicksand (flowing, depth of 160 units)
#define SURFACE_SHALLOW_MOVING_QUICKSAND TERRAIN(0,0,0,0,0,0x25)    // Moving quicksand (flowing, depth of 25 units)
#define SURFACE_QUICKSAND TERRAIN(0,0,0,0,0,0x26)                 // Moving quicksand (60 units)
#define SURFACE_MOVING_QUICKSAND TERRAIN(0,0,0,0,0,0x27)         // Moving quicksand (flowing, depth of 60 units)
#define SURFACE_NOISE_DEFAULT TERRAIN(1,1,0,0,0,0x0029)            // Default floor with noise
#define SURFACE_NOISE_SLIPPERY TERRAIN(1,1,0,0,SPECFLAG_SLIPPERY,0x0029)           // Slippery floor with noise
#define SURFACE_HORIZONTAL_WIND TERRAIN(0,0,0,0,0,0x2C) 
#define SURFACE_INSTANT_MOVING_QUICKSAND TERRAIN(0,0,0,0,0,0x2D)  // Quicksand (lethal, flowing)
#define SURFACE_ICE TERRAIN(6,0,0,0,SPECFLAG_MEGASLIP,0x0013)                      // Slippery Ice, in snow levels and THI's water floor
#define SURFACE_HARD TERRAIN(3,0,0,0,0,0x0030)                      // Hard floor (Always has fall damage)
#define SURFACE_WARP TERRAIN(0,0,0,0,0,0x0032)                     // Surface warp
#define SURFACE_TIMER_START TERRAIN(0,0,0,0,0,0x0033)              // Timer start (Peach's secret slide)
#define SURFACE_TIMER_END TERRAIN(0,0,0,0,0,0x0034)                // Timer stop (Peach's secret slide)
#define SURFACE_HARD_SLIPPERY TERRAIN(2,0,0,0,SPECFLAG_SLIPPERY,0x0035)             // Hard and slippery (Always has fall damage)
#define SURFACE_HARD_VERY_SLIPPERY TERRAIN(5,0,0,0,SPECFLAG_MEGASLIP,0x0036)       // Hard and very slippery (Always has fall damage)
#define SURFACE_HARD_NOT_SLIPPERY TERRAIN(3,0,0,0,SPECFLAG_CLIMBABLE,0x0037)          // Hard and Non-slippery (Always has fall damage)
#define SURFACE_VERTICAL_WIND TERRAIN(0,0,0,0,0,0x0038)            // Death at bottom with vertical wind
#define SURFACE_NO_CAM_COLLISION SURFACE_DEFAULT       // Surface with no cam collision flag
#define SURFACE_VANISH_CAP_WALLS TERRAIN(0,0,0,0,0,0x007B)    // Vanish cap walls, pass through them with Vanish Cap
#define SURF_KEEP_CAM_IN_A5 TERRAIN(0,0,0,CAMERA_TERRAIN_GOTHROUGH, 0, 0x00A5)        // Painting wobble (BOB Left)
#define SURFACE_PAINTING_WOBBLE_A6 TERRAIN(0,0,0,0,2,0x00A6) // Painting wobble (BOB Left)
#define SURFACE_PAINTING_WOBBLE_A7 TERRAIN(0,0,0,0,0,0x00A7) // Painting wobble (BOB Middle)
#define SURFACE_PAINTING_WOBBLE_A8 0x00A8 // leave
#define SURFACE_PAINTING_WOBBLE_A9 0x00A9 // leave
#define SURFACE_PAINTING_WOBBLE_AA TERRAIN(0,0,0,CAMERA_TERRAIN_NOSURFBACK, 0, 0x00AA) // Painting wobble (CCM Middle)
#define SURFACE_PAINTING_WOBBLE_AB TERRAIN(0,0,0,0,1,0x00AB) // Painting wobble (CCM Right)
#define SURFACE_PAINTING_WOBBLE_AC 0x00AC // NO WK COLL
#define SURFACE_PAINTING_WOBBLE_AD TERRAIN(SOUND_TERRAIN_WATER,0,0,0,SPECFLAG_CLIMBABLE,0x00AD) 
#define SURFACE_PAINTING_WOBBLE_AE 0x00AE // leave
#define SURFACE_PAINTING_WOBBLE_AF 0x00AF // leave
#define SURFACE_CAMCAST_THROUGH TERRAIN(0,0,0,CAMERA_TERRAIN_GOTHROUGH, 0, 0x00B0)    // Painting wobble (JRB Middle)
#define SURFACE_PAINTING_WARP_FA TERRAIN(SOUND_TERRAIN_STONE,PARTICLE_TERRAIN_SAND,0,0,0,0x00FA) 
#define SURFACE_PAINTING_WARP_FB TERRAIN(SOUND_TERRAIN_SAND,PARTICLE_TERRAIN_SAND,0,0,0,0x00FB)   // Painting warp (THI Tiny - Middle)
#define SURFACE_PAINTING_WARP_FC TERRAIN(SOUND_TERRAIN_SPOOKY,0,0,0,0,0x00FC)   // Painting warp (THI Tiny - Right)






//all the below types are worthless 
#define SURFACE_PAINTING_WOBBLE_B1 0x00B1 // Painting wobble (JRB Right)
#define SURFACE_PAINTING_WOBBLE_B2 0x00B2 // Painting wobble (LLL Left)
#define SURFACE_PAINTING_WOBBLE_B3 0x00B3 // Painting wobble (LLL Middle)
#define SURFACE_PAINTING_WOBBLE_B4 0x00B4 // Painting wobble (LLL Right)
#define SURFACE_PAINTING_WOBBLE_B5 0x00B5 // Painting wobble (SSL Left)
#define SURFACE_PAINTING_WOBBLE_B6 0x00B6 // Painting wobble (SSL Middle)
#define SURFACE_PAINTING_WOBBLE_B7 0x00B7 // Painting wobble (SSL Right)
#define SURFACE_PAINTING_WOBBLE_B8 0x00B8 // Painting wobble (Unused - Left)
#define SURFACE_PAINTING_WOBBLE_B9 0x00B9 // Painting wobble (Unused - Middle)
#define SURFACE_PAINTING_WOBBLE_BA 0x00BA // Painting wobble (Unused - Right)
#define SURFACE_PAINTING_WOBBLE_BB                                                                     \
    0x00BB // Painting wobble (DDD - Left), makes the painting wobble if touched
#define SURFACE_PAINTING_WOBBLE_BC 0x00BC // Painting wobble (Unused, DDD - Middle)
#define SURFACE_PAINTING_WOBBLE_BD 0x00BD // Painting wobble (Unused, DDD - Right)
#define SURFACE_PAINTING_WOBBLE_BE 0x00BE // Painting wobble (WDW Left)
#define SURFACE_PAINTING_WOBBLE_BF 0x00BF // Painting wobble (WDW Middle)
#define SURFACE_PAINTING_WOBBLE_C0 0x00C0 // Painting wobble (WDW Right)
#define SURFACE_PAINTING_WOBBLE_C1 0x00C1 // Painting wobble (THI Tiny - Left)
#define SURFACE_PAINTING_WOBBLE_C2 0x00C2 // Painting wobble (THI Tiny - Middle)
#define SURFACE_PAINTING_WOBBLE_C3 0x00C3 // Painting wobble (THI Tiny - Right)
#define SURFACE_PAINTING_WOBBLE_C4 0x00C4 // Painting wobble (TTM Left)
#define SURFACE_PAINTING_WOBBLE_C5 0x00C5 // Painting wobble (TTM Middle)
#define SURFACE_PAINTING_WOBBLE_C6 0x00C6 // Painting wobble (TTM Right)
#define SURFACE_PAINTING_WOBBLE_C7 0x00C7 // Painting wobble (Unused, TTC - Left)
#define SURFACE_PAINTING_WOBBLE_C8 0x00C8 // Painting wobble (Unused, TTC - Middle)
#define SURFACE_PAINTING_WOBBLE_C9 0x00C9 // Painting wobble (Unused, TTC - Right)
#define SURFACE_PAINTING_WOBBLE_CA 0x00CA // Painting wobble (Unused, SL - Left)
#define SURFACE_PAINTING_WOBBLE_CB 0x00CB // Painting wobble (Unused, SL - Middle)
#define SURFACE_PAINTING_WOBBLE_CC 0x00CC // Painting wobble (Unused, SL - Right)
#define SURFACE_PAINTING_WOBBLE_CD 0x00CD // Painting wobble (THI Huge - Left)
#define SURFACE_PAINTING_WOBBLE_CE 0x00CE // Painting wobble (THI Huge - Middle)
#define SURFACE_PAINTING_WOBBLE_CF 0x00CF // Painting wobble (THI Huge - Right)
#define SURFACE_PAINTING_WOBBLE_D0                                                                     \
    0x00D0 // Painting wobble (HMC & COTMC - Left), makes the painting wobble if touched
#define SURFACE_PAINTING_WOBBLE_D1 0x00D1 // Painting wobble (Unused, HMC & COTMC - Middle)
#define SURFACE_PAINTING_WOBBLE_D2 0x00D2 // Painting wobble (Unused, HMC & COTMC - Right)
#define SURFACE_PAINTING_WARP_D3 0x00D3   // Painting warp (BOB Left)
#define SURFACE_PAINTING_WARP_D4 0x00D4   // Painting warp (BOB Middle)
#define SURFACE_PAINTING_WARP_D5 0x00D5   // Painting warp (BOB Right)
#define SURFACE_PAINTING_WARP_D6 0x00D6   // Painting warp (CCM Left)
#define SURFACE_PAINTING_WARP_D7 0x00D7   // Painting warp (CCM Middle)
#define SURFACE_PAINTING_WARP_D8 0x00D8   // Painting warp (CCM Right)
#define SURFACE_PAINTING_WARP_D9 0x00D9   // Painting warp (WF Left)
#define SURFACE_PAINTING_WARP_DA 0x00DA   // Painting warp (WF Middle)
#define SURFACE_PAINTING_WARP_DB 0x00DB   // Painting warp (WF Right)
#define SURFACE_PAINTING_WARP_DC 0x00DC   // Painting warp (JRB Left)
#define SURFACE_PAINTING_WARP_DD 0x00DD   // Painting warp (JRB Middle)
#define SURFACE_PAINTING_WARP_DE 0x00DE   // Painting warp (JRB Right)
#define SURFACE_PAINTING_WARP_DF 0x00DF   // Painting warp (LLL Left)
#define SURFACE_PAINTING_WARP_E0 0x00E0   // Painting warp (LLL Middle)
#define SURFACE_PAINTING_WARP_E1 0x00E1   // Painting warp (LLL Right)
#define SURFACE_PAINTING_WARP_E2 0x00E2   // Painting warp (SSL Left)
#define SURFACE_PAINTING_WARP_E3 0x00E3   // Painting warp (SSL Medium)
#define SURFACE_PAINTING_WARP_E4 0x00E4   // Painting warp (SSL Right)
#define SURFACE_PAINTING_WARP_E5 0x00E5   // Painting warp (Unused - Left)
#define SURFACE_PAINTING_WARP_E6 0x00E6   // Painting warp (Unused - Medium)
#define SURFACE_PAINTING_WARP_E7 0x00E7   // Painting warp (Unused - Right)
#define SURFACE_PAINTING_WARP_E8 0x00E8   // Painting warp (DDD - Left)
#define SURFACE_PAINTING_WARP_E9 0x00E9   // Painting warp (DDD - Middle)
#define SURFACE_PAINTING_WARP_EA 0x00EA   // Painting warp (DDD - Right)
#define SURFACE_PAINTING_WARP_EB 0x00EB   // Painting warp (WDW Left)
#define SURFACE_PAINTING_WARP_EC 0x00EC   // Painting warp (WDW Middle)
#define SURFACE_PAINTING_WARP_ED 0x00ED   // Painting warp (WDW Right)
#define SURFACE_PAINTING_WARP_EE 0x00EE   // Painting warp (THI Tiny - Left)
#define SURFACE_PAINTING_WARP_EF 0x00EF   // Painting warp (THI Tiny - Middle)
#define SURFACE_PAINTING_WARP_F0 0x00F0   // Painting warp (THI Tiny - Right)
#define SURFACE_PAINTING_WARP_F1 0x00F1   // Painting warp (TTM Left)
#define SURFACE_PAINTING_WARP_F2 0x00F2   // Painting warp (TTM Middle)
#define SURFACE_PAINTING_WARP_F3 0x00F3   // Painting warp (TTM Right)
#define SURFACE_TTC_PAINTING_1 0x00F4     // Painting warp (TTC Left)
#define SURFACE_TTC_PAINTING_2 0x00F5     // Painting warp (TTC Medium)
#define SURFACE_TTC_PAINTING_3 0x00F6     // Painting warp (TTC Right)
#define SURFACE_PAINTING_WARP_F7 0x00F7   // Painting warp (SL Left)
#define SURFACE_PAINTING_WARP_F8 0x00F8   // Painting warp (SL Middle)
#define SURFACE_PAINTING_WARP_F9 0x00F9   // Painting warp (SL Right)
#define SURFACE_WOBBLING_WARP 0x00FD      // Pool warp (HMC & DDD)
#define SURFACE_TRAPDOOR 0x00FF           // Bowser Left trapdoor, has no action defined
//all the above types are worthless 


#define SURFACE_IS_QUICKSAND(cmd)                                                                      \
    (cmd >= 0x21 && cmd < 0x28) // Doesn't include SURFACE_INSTANT_MOVING_QUICKSAND
#define SURFACE_IS_NOT_HARD(cmd) (cmd != SURFACE_HARD && !(cmd >= 0x35 && cmd <= 0x37))

#define SURFACE_CLASS_DEFAULT 0x0000
#define SURFACE_CLASS_VERY_SLIPPERY 0x0013
#define SURFACE_CLASS_SLIPPERY 0x0014
#define SURFACE_CLASS_NOT_SLIPPERY 0x0015

#define SURFACE_FLAG_DYNAMIC (1 << 0)
#define SURFACE_FLAG_NO_CAM_COLLISION (1 << 1)

// These are effectively unique "surface" types like those defined higher
// And they are used as collision commands to load certain functions
#define TERRAIN_LOAD_VERTICES 0,0x0040 // Begins vertices list for collision triangles
#define TERRAIN_LOAD_CONTINUE                                                                          \
    0, 0x0041                      // Stop loading vertices but continues to load other collision commands
#define TERRAIN_LOAD_END 0,0x0042 // End the collision list
#define TERRAIN_LOAD_OBJECTS 0,0x0043     // Loads in certain objects for level start
#define TERRAIN_LOAD_ENVIRONMENT 0,0x0044 // Loads water/HMC gas

#define TERRAIN_LOAD_IS_SURFACE_TYPE_LOW(cmd) (cmd < 0x40)
#define TERRAIN_LOAD_IS_SURFACE_TYPE_HIGH(cmd) (cmd >= 0x65)

// Terrain types defined by the level script command terrain_type (cmd_31)
#define TERRAIN_GRASS 0x0000
#define TERRAIN_STONE 0x0001
#define TERRAIN_SNOW 0x0002
#define TERRAIN_SAND 0x0003
#define TERRAIN_SPOOKY 0x0004
#define TERRAIN_WATER 0x0005
#define TERRAIN_SLIDE 0x0006
#define TERRAIN_MASK 0x0007

// These collision commands are unique "surface" types like those defined higher

// Collision Data Routine Initiate
#define COL_INIT() TERRAIN_LOAD_VERTICES

// Collision Vertices Read Initiate
#define COL_VERTEX_INIT(vtxNum) vtxNum

// Collision Vertex
#define COL_VERTEX(x, y, z) x, y, z

// Collision Tris Initiate
//surfType is now 2 s16s
#define COL_TRI_INIT(surfType, triNum) surfType>>16, surfType&0xFFFF, triNum

// Collision Tri
#define COL_TRI(v1, v2, v3) v1, v2, v3

// Collision Tri With Special Params
#define COL_TRI_SPECIAL(v1, v2, v3, param) v1, v2, v3, param

// Collision Tris Stop Loading
#define COL_TRI_STOP() TERRAIN_LOAD_CONTINUE

// End Collision Data
#define COL_END() TERRAIN_LOAD_END

// Special Object Initiate
#define COL_SPECIAL_INIT(num) TERRAIN_LOAD_OBJECTS, num

// Water Boxes Initiate
#define COL_WATER_BOX_INIT(num) TERRAIN_LOAD_ENVIRONMENT, num

// Water Box
#define COL_WATER_BOX(id, x1, z1, x2, z2, y) id, x1, z1, x2, z2, y

#endif // SURFACE_TERRAINS_H
