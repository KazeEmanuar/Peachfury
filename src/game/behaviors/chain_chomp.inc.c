
/**
 * Behavior for bhvChainChomp, bhvChainChompChainPart, bhvWoodenPost, and bhvChainChompGate.
 * bhvChainChomp spawns its bhvWoodenPost in its behavior script. It spawns 5 chain
 * parts. Part 0 is the "pivot", which is positioned at the wooden post while
 * the chomp is chained up. Parts 1-4 are the other parts, starting from the
 * chain chomp and moving toward the pivot.
 * Processing order is bhvWoodenPost, bhvChainChompGate, bhvChainChomp, bhvChainChompChainPart.
 * The chain parts are processed starting at the post and ending at the chomp.
 */

/* oSubAction */
#define CHAIN_CHOMP_SUB_ACT_AI_INIT 0 // Initializes the AI
#define CHAIN_CHOMP_SUB_ACT_WANDER 1  // Wanders around the exhibit
#define CHAIN_CHOMP_SUB_ACT_IDLE 2    // Wait for a few moments before wandering again
#define CHAIN_CHOMP_SUB_ACT_BONE 3    // Playing with bone
#define CHAIN_CHOMP_SUB_ACT_THROW 4   // Throw bone at Mario (aggressive)
#define CHAIN_CHOMP_SUB_ACT_REST 5    // Resting in dog house
#define CHAIN_CHOMP_SUB_ACT_CONSUME 6 // Consume from bowl
#define CHAIN_CHOMP_SUB_ACT_PLAY 7    // Playing

#define CHAIN_CHOMP_SUB_ACT_TURN 8  // Aggressive: Turn torwards Mario
#define CHAIN_CHOMP_SUB_ACT_LUNGE 9 // Aggressive: Lunge at Mario

// Chain Chomp AI state
static f32 chain_chomp_wander_x;
static f32 chain_chomp_wander_z;
static s32 chain_chomp_wander_act;

static u16 chain_chomp_timer;

static f32 chain_chomp_spawn_x;
static f32 chain_chomp_spawn_y;
static f32 chain_chomp_spawn_z;

static struct Object *chain_chomp_bone;

#define CHAIN_CHOMP_HOME_X 1186
#define CHAIN_CHOMP_HOME_Z 3911

#define CHAIN_CHOMP_BOWL_X -1270
#define CHAIN_CHOMP_BOWL_Z 5400

#define CHAIN_CHOMP_FIELD_X 640
#define CHAIN_CHOMP_FIELD_Z 5700

#define CHAIN_CHOMP_MUD_X -1460
#define CHAIN_CHOMP_MUD_Z 4570

/**
 * Hitbox for chain chomp.
 */
static struct ObjectHitbox sChainChompHitbox = {
    /* interactType: */ INTERACT_MR_BLIZZARD,
    /* downOffset: */ 0,
    /* damageOrCoinValue: */ 3,
    /* health: */ 1,
    /* numLootCoins: */ 0,
    /* radius: */ 80,
    /* height: */ 160,
    /* hurtboxRadius: */ 80,
    /* hurtboxHeight: */ 160,
};

// Chain Chomp's bone
static struct ObjectHitbox sChompBoneHitbox = {
    /* interactType: */ INTERACT_MR_BLIZZARD,
    /* downOffset: */ 0,
    /* damageOrCoinValue: */ 1,
    /* health: */ 1,
    /* numLootCoins: */ 0,
    /* radius: */ 120,
    /* height: */ 50,
    /* hurtboxRadius: */ 120,
    /* hurtboxHeight: */ 50,
};

void chompbone(void) {
    f32 dis;
    switch (o->oAction) {
        case 0: // Nothing
            o->oInteractType = INTERACT_IGLOO_BARRIER;
            break;
        case 1: // Thrown
        case 2: // Dropped
            // Do collision
            cur_obj_move_standard(78);
            cur_obj_update_floor_and_walls();

            // Bounce off walls
            if (o->oMoveFlags & OBJ_MOVE_HIT_WALL)
                o->oMoveAngleYaw = o->oWallAngle - (s16)(o->oMoveAngleYaw - o->oWallAngle);

            // Stop moving when laying on ground
            if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
                if ((o->oForwardVel *= 0.5f) < 1.0f) {
                    // Stopped moving
                    o->oForwardVel = 0.0f;
                    o->oAction = 0;
                }
            }

            // Hurt Mario
            if (o->oAction == 1) {
                // Thrown
                o->oInteractType = INTERACT_MR_BLIZZARD;
                obj_check_attacks(&sChompBoneHitbox, o->oAction);
            } else {
                // Dropped
                o->oInteractType = INTERACT_IGLOO_BARRIER;
            }
            break;
    }

    // Force back in bounds if too far out
    dis = sqr(o->oPosX - chain_chomp_spawn_x) + sqr(o->oPosZ - chain_chomp_spawn_z);
    if (o->oPosY < (chain_chomp_spawn_y - 100.0f) || dis >= 5000000.0f) {
        o->oMoveAngleYaw = atan2s(chain_chomp_spawn_z - o->oPosZ, chain_chomp_spawn_x - o->oPosX);
        o->oForwardVel = 50.0f;
        o->oVelY = 50.0f;
    }
}

/**
 * Update function for chain chomp part / pivot.
 */
void bhv_chain_chomp_chain_part_update(void) {
    struct ChainSegment *segment;

    if (o->parentObj->oAction == CHAIN_CHOMP_ACT_UNLOAD_CHAIN) {
        obj_mark_for_deletion(o);
    } else if (o->oBehParams2ndByte != CHAIN_CHOMP_CHAIN_PART_BP_PIVOT) {
        segment = &o->parentObj->oChainChompSegments[o->oBehParams2ndByte];

        // Set position relative to the pivot
        o->oPosX = o->parentObj->parentObj->oPosX + segment->posX;
        o->oPosY = o->parentObj->parentObj->oPosY + segment->posY;
        o->oPosZ = o->parentObj->parentObj->oPosZ + segment->posZ;
        ;
    } else if (o->parentObj->oChainChompReleaseStatus != CHAIN_CHOMP_NOT_RELEASED) {
        cur_obj_update_floor_and_walls();
        cur_obj_move_standard(78);
    }
}

/**
 * When mario gets close enough, allocate chain segments and spawn their objects.
 */
static void chain_chomp_act_uninitialized(void) {
    struct ChainSegment *segments;
    s32 i;

    if (o->oDistanceToMario < 6000.0f) {
        segments = mem_pool_alloc(gObjectMemoryPool, 5 * sizeof(struct ChainSegment));
        if (segments != NULL) {
            // Each segment represents the offset of a chain part to the pivot.
            // Segment 0 connects the pivot to the chain chomp itself. Segment
            // 1 connects the pivot to the chain part next to the chain chomp
            // (chain part 1), etc.
            o->oChainChompSegments = segments;
            for (i = 0; i <= 4; i++) {
                chain_segment_init(&segments[i]);
            }

            cur_obj_set_pos_to_home();

            // Spawn the pivot and set to parent
            if ((o->parentObj =
                     spawn_object(o, CHAIN_CHOMP_CHAIN_PART_BP_PIVOT, bhvChainChompChainPart))
                != NULL) {
                // Spawn the non-pivot chain parts, starting from the chain
                // chomp and moving toward the pivot
                for (i = 1; i <= 4; i++) {
                    spawn_object_relative(i, 0, 0, 0, o, MODEL_METALLIC_BALL, bhvChainChompChainPart);
                }

                o->oAction = CHAIN_CHOMP_ACT_MOVE;
                o->oSubAction = CHAIN_CHOMP_SUB_ACT_AI_INIT;
                // o->oWallHitboxRadius = 100.0f;
                chain_chomp_spawn_x = o->oPosX;
                chain_chomp_spawn_y = o->oPosY;
                chain_chomp_spawn_z = o->oPosZ;
                cur_obj_unhide();
            }
        }
    }
}

/**
 * Apply gravity to each chain part, and cap its distance to the previous
 * part as well as from the pivot.
 */
static void chain_chomp_update_chain_segments(void) {
    struct ChainSegment *prevSegment;
    struct ChainSegment *segment;
    f32 offsetX;
    f32 offsetY;
    f32 offsetZ;
    f32 offset;
    f32 segmentVelY;
    f32 maxTotalOffset;
    s32 i;

    if (o->oVelY < -20.0f) {
        segmentVelY = o->oVelY;
    } else {
        segmentVelY = -20.0f;
    }

    segment = &o->oChainChompSegments[0];
    if ((segment->posY += segmentVelY) < 0.0f) {
        segment->posY = 0.0f;
    }

    // Segment 0 connects the pivot to the chain chomp itself, and segment i>0
    // connects the pivot to chain part i (1 is closest to the chain chomp).

    for (i = 1; i <= 4; i++) {
        prevSegment = &o->oChainChompSegments[i - 1];
        segment = &o->oChainChompSegments[i];

        // Apply gravity
        if ((segment->posY += segmentVelY) < 0.0f) {
            segment->posY = 0.0f;
        }

        // Cap distance to previous chain part (so that the tail follows the
        // chomp)

        offsetX = segment->posX - prevSegment->posX;
        offsetY = segment->posY - prevSegment->posY;
        offsetZ = segment->posZ - prevSegment->posZ;
        offset = sqrtf(offsetX * offsetX + offsetY * offsetY + offsetZ * offsetZ);

        if (offset > o->oChainChompMaxDistBetweenChainParts) {
            offset = o->oChainChompMaxDistBetweenChainParts / offset;
            offsetX *= offset;
            offsetY *= offset;
            offsetZ *= offset;
        }

        // Cap distance to pivot (so that it stretches when the chomp moves far
        // from the wooden post)

        offsetX += prevSegment->posX;
        offsetY += prevSegment->posY;
        offsetZ += prevSegment->posZ;
        offset = sqrtf(offsetX * offsetX + offsetY * offsetY + offsetZ * offsetZ);

        maxTotalOffset = o->oChainChompMaxDistFromPivotPerChainPart * (5 - i);
        if (offset > maxTotalOffset) {
            offset = maxTotalOffset / offset;
            offsetX *= offset;
            offsetY *= offset;
            offsetZ *= offset;
        }

        segment->posX = offsetX;
        segment->posY = offsetY;
        segment->posZ = offsetZ;
    }
}

/**
 * Lunging increases the maximum distance from the pivot and changes the maximum
 * distance between chain parts. Restore these values to normal.
 */
static void chain_chomp_restore_normal_chain_lengths(void) {
    approach_f32_ptr(&o->oChainChompMaxDistFromPivotPerChainPart, 750.0f / 5, 4.0f);
    o->oChainChompMaxDistBetweenChainParts = o->oChainChompMaxDistFromPivotPerChainPart;
}

// CuckyDev: Chain Chomp AI
static void chain_chomp_ai_new_wander() {
    // Think about what to do
    u16 think = (random_u16() >> 4) & 0xF;

    if (think >= 0xF) {
        // Go to bed
        chain_chomp_wander_x = CHAIN_CHOMP_HOME_X;
        chain_chomp_wander_z = CHAIN_CHOMP_HOME_Z;
        chain_chomp_wander_act = CHAIN_CHOMP_SUB_ACT_REST;
    } else if (think >= 0xB) {
        // Play with bone
        chain_chomp_bone = cur_obj_nearest_object_with_behavior(bhvChompBone);
        chain_chomp_wander_x = chain_chomp_bone->oPosX;
        chain_chomp_wander_z = chain_chomp_bone->oPosZ;
        chain_chomp_wander_act = CHAIN_CHOMP_SUB_ACT_BONE;
    } else if (think >= 0x9) {
        // Go to bowl
        chain_chomp_wander_x = CHAIN_CHOMP_BOWL_X;
        chain_chomp_wander_z = CHAIN_CHOMP_BOWL_Z;
        chain_chomp_wander_act = CHAIN_CHOMP_SUB_ACT_CONSUME;
    } else if (think >= 0x8) {
        // Play in field
        chain_chomp_wander_x = CHAIN_CHOMP_FIELD_X;
        chain_chomp_wander_z = CHAIN_CHOMP_FIELD_Z;
        chain_chomp_wander_act = CHAIN_CHOMP_SUB_ACT_PLAY;
    } else if (think >= 0x7) {
        // Play in mud
        chain_chomp_wander_x = CHAIN_CHOMP_MUD_X;
        chain_chomp_wander_z = CHAIN_CHOMP_MUD_Z;
        chain_chomp_wander_act = CHAIN_CHOMP_SUB_ACT_PLAY;
    } else {
        // Walk to random point
        f32 dist = 200.0f + (f32)(random_u16() % 1400);
        u16 dir = random_u16();
        chain_chomp_wander_x = chain_chomp_spawn_x + sins(dir) * dist;
        chain_chomp_wander_z = chain_chomp_spawn_z + coss(dir) * dist;
        chain_chomp_wander_act = CHAIN_CHOMP_SUB_ACT_IDLE;
    }

    // Enter wander state
    o->oSubAction = CHAIN_CHOMP_SUB_ACT_WANDER;
    chain_chomp_timer = 0;
}

static void chain_chomp_ai_init() {
    // Start wandering
    chain_chomp_ai_new_wander();
}

static void chain_chomp_ai_wander() {
    f32 tgt_dist;
    s16 tgt_angle;
    o->oGravity = -4.0f;

    chain_chomp_restore_normal_chain_lengths();
    obj_face_pitch_approach(0, 0x100);

    tgt_dist = sqr(o->oPosX - chain_chomp_wander_x) + sqr(o->oPosZ - chain_chomp_wander_z);
    tgt_angle = atan2s(chain_chomp_wander_z - o->oPosZ, chain_chomp_wander_x - o->oPosX);

    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        // Go aggressive if Mario has entered the exhibit
        if (o->oChainChompRestrictedByChain) {
            o->oSubAction = CHAIN_CHOMP_SUB_ACT_TURN;
            return;
        }

        // Check if we've reached our target
        if (tgt_dist < 10000.0f) {
            // Reached target
            o->oSubAction = chain_chomp_wander_act;
            chain_chomp_timer = 0;
        } else {
            if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
                if (++chain_chomp_timer < 15) {
                    // Turn away from wall
                    cur_obj_rotate_yaw_toward(o->oWallAngle, 0x400);
                } else {
                    // Go idle (hit wall too many times)
                    o->oSubAction = CHAIN_CHOMP_SUB_ACT_IDLE;
                    return;
                }
            } else {
                // Turn torward target
                cur_obj_rotate_yaw_toward(tgt_angle, 0x400);
            }

            // Jump towards target
            cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP1);
            tgt_dist = sqrtf(tgt_dist);
            if (tgt_dist > 200.0f)
                o->oForwardVel = 10.0f;
            else
                o->oForwardVel = tgt_dist * 0.05f;
            o->oVelY = 20.0f;
        }
    } else {
        // Jumping towards target
        if (o->oMoveFlags & OBJ_MOVE_HIT_WALL)
            cur_obj_rotate_yaw_toward(o->oWallAngle, 0x200);
        else
            cur_obj_rotate_yaw_toward(tgt_angle, 0x190);
    }
}

static void chain_chomp_ai_idle() {
    // Slow down on ground and reset look
    obj_face_pitch_approach(0, 0x100);
    if ((o->oForwardVel *= 0.5f) < 1.0f)
        o->oForwardVel = 0.0f;

    // Wait for timer to expire
    if (chain_chomp_timer == 0)
        chain_chomp_timer = 30 + (random_u16() & 0x1F);
    else if (--chain_chomp_timer == 0)
        chain_chomp_ai_new_wander();
}

static void chain_chomp_ai_movebone() {
    // Set bone position
    f32 forward = coss(o->oFaceAnglePitch) * 225.0f;
    f32 up = sins(o->oFaceAnglePitch) * -225.0f;
    chain_chomp_bone->oPosX = o->oPosX + sins(o->oFaceAngleYaw) * forward;
    chain_chomp_bone->oPosY = o->oPosY + 225.0f + up;
    chain_chomp_bone->oPosZ = o->oPosZ + coss(o->oFaceAngleYaw) * forward;
    chain_chomp_bone->oMoveAngleYaw = o->oFaceAngleYaw;
}

static void chain_chomp_ai_bone() {
    // Stop moving
    if ((o->oForwardVel *= 0.5f) < 1.0f)
        o->oForwardVel = 0.0f;

    // Handle bone
    if (chain_chomp_timer < 20) {
        // Look down to pick up bone
        obj_face_pitch_approach(0x2000, 0x180);
    } else if (chain_chomp_timer < 500) {
        // Throw bone if Mario has entered the exhibit
        if (o->oChainChompRestrictedByChain) {
            o->oSubAction = CHAIN_CHOMP_SUB_ACT_THROW;
            chain_chomp_timer = 0;
            return;
        }

        // Look back up
        obj_face_pitch_approach(0, 0x180);

        // Shake bone around
        if (chain_chomp_timer & 0x20) {
            if (chain_chomp_timer & 0x4)
                o->oMoveAngleYaw = o->oAngleVelYaw = (o->oFaceAngleYaw -= 0x800);
            else
                o->oMoveAngleYaw = o->oAngleVelYaw = (o->oFaceAngleYaw += 0x800);
            if ((chain_chomp_timer & 0x8) == 0)
                cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP1);
        }
        chain_chomp_ai_movebone();
        chain_chomp_bone->oAction = 0;
    } else {
        // Drop bone
        chain_chomp_ai_movebone();
        chain_chomp_bone->oAction = 2;
        chain_chomp_bone->oForwardVel = 20.0f;
        chain_chomp_bone->oVelY = 0.0f;

        // Go idle
        o->oSubAction = CHAIN_CHOMP_SUB_ACT_IDLE;
        chain_chomp_timer = 0;
        return;
    }

    // Increment timer
    chain_chomp_timer++;
}

static void chain_chomp_ai_throw(void) {
    f32 spd;
    // Face upwards
    obj_face_pitch_approach(-0x2000, 0x180);

    // Turn towards Mario
    cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x600);

    // Update bone and check if should be thrown
    chain_chomp_ai_movebone();

    if (chain_chomp_timer >= 30 && o->oAngleToMario == o->oMoveAngleYaw) {
        // Throw bone at Mario
        cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP2);

        spd = o->oDistanceToMario / 22.5f;
        chain_chomp_bone->oAction = 1;
        chain_chomp_bone->oForwardVel = spd;
        chain_chomp_bone->oVelY = 20.0f;

        // Go idle
        o->oSubAction = CHAIN_CHOMP_SUB_ACT_IDLE;
        chain_chomp_timer = 40; // Give Mario extra time to react to the troll moment that just happened
        return;
    }

    // Increment timer
    chain_chomp_timer++;
}

static void chain_chomp_ai_rest(void) {
    // Stop moving
    o->oForwardVel = 0.0f;
    cur_obj_reverse_animation();

    // Handle state
    if (chain_chomp_timer < 500) {
        // Wake up sooner as Mario's in the exhibit
        if (o->oChainChompRestrictedByChain)
            chain_chomp_timer += 2;

        // Resting, face downwards
        obj_face_pitch_approach(0x1800, 0x40);
    } else if (chain_chomp_timer < 530) {
        // Waking up
        obj_face_pitch_approach(0, 0x80);
    } else {
        // Wake up
        cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP2);
        o->oSubAction = CHAIN_CHOMP_SUB_ACT_IDLE;
        chain_chomp_timer = 0;
        return;
    }

    // Increment timer
    chain_chomp_timer++;
}

static void chain_chomp_ai_consume(void) {
    // Stop moving
    o->oForwardVel = 0.0f;

    // Handle state
    if (chain_chomp_timer < 300) {
        // Immediately stop consuming once Mario enters the exhibit
        if (o->oChainChompRestrictedByChain)
            chain_chomp_timer = 300;

        // Eating, face downwards
        obj_face_pitch_approach(0x2000, 0x140);
        if ((chain_chomp_timer & 0xF) == 0)
            cur_obj_play_sound_2(SOUND_OBJ_WALKING_WATER);
    } else if (chain_chomp_timer < 330) {
        // Done eating
        obj_face_pitch_approach(0, 0x100);
    } else {
        // Enter idle state
        cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP2);
        o->oSubAction = CHAIN_CHOMP_SUB_ACT_IDLE;
        chain_chomp_timer = 0;
        return;
    }

    // Increment timer
    chain_chomp_timer++;
}

static void chain_chomp_ai_play(void) {
    u16 subtimer = chain_chomp_timer & 0x7F;

    // Stop moving
    o->oGravity = -4.0f;
    o->oForwardVel = 0.0f;
    o->oOpacity = 0;
    o->oAnimState = 2;
    // Handle state
    if (subtimer < 20) {
        // Rolling upside down
        o->oFaceAngleRoll += 0x668;
    } else if (subtimer < 90) {
        // Roll around
        if (subtimer & 4)
            o->oFaceAngleRoll -= 0x800;
        else
            o->oFaceAngleRoll += 0x800;
        if ((chain_chomp_timer & 0x3) == 0)
            cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP1);
    } else if (subtimer < 110) {
        // Roll right back up
        o->oFaceAngleRoll -= 0x666;
    } else {
        // Bark and reset roll
        if (subtimer == 110) {
            o->oVelY = 50.0f;
            cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP2);
        }
        o->oFaceAngleRoll = 0;

        // Enter idle if Mario is in the exhibit
        if (o->oChainChompRestrictedByChain) {
            // Enter idle state
            o->oSubAction = CHAIN_CHOMP_SUB_ACT_IDLE;
            chain_chomp_timer = 0;
            o->oFaceAngleRoll = 0;
            return;
        }
    }

    // Stop playing after 3 rolls
    if (++chain_chomp_timer >= 0x180) {
        // Enter idle state
        o->oSubAction = CHAIN_CHOMP_SUB_ACT_IDLE;
        chain_chomp_timer = 0;
        o->oFaceAngleRoll = 0;
        return;
    }
}

/**
 * Turn toward mario. Wait a bit and then enter the lunging sub-action.
 */
static void chain_chomp_sub_act_turn(void) {
    o->oGravity = -4.0f;

    chain_chomp_restore_normal_chain_lengths();
    obj_face_pitch_approach(0, 0x100);

    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        // Return to wander if Mario's left the exhibit
        if (!o->oChainChompRestrictedByChain) {
            chain_chomp_ai_new_wander();
            return;
        }

        cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x600);
        if (abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) < 0x800) {
            if (o->oTimer > 15) {
                if (cur_obj_check_anim_frame(0)) {
                    cur_obj_reverse_animation();
                    if (o->oTimer > 20) {
                        // Increase the maximum distance from the pivot and enter
                        // the lunging sub-action.
                        cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP2);

                        o->oSubAction = CHAIN_CHOMP_SUB_ACT_LUNGE;
                        o->oChainChompMaxDistFromPivotPerChainPart = 900.0f / 5;

                        o->oForwardVel = 50.0f + o->oDistanceToMario * 0.025f;
                        if (o->oForwardVel > 80.f){
                            o->oForwardVel = 80.f;
                        }
                        o->oVelY = 20.0f + (o->oPosY < gMarioState->floorHeight) * (-o->oPosY + gMarioState->floorHeight)/15.f;
                        o->oGravity = -3.0f;
                        o->oChainChompTargetPitch = obj_get_pitch_from_vel();
                    }
                } else {
                    o->oTimer -= 1;
                }
            } else {
                o->oForwardVel = 0.0f;
            }
        } else {
            cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP1);
            o->oForwardVel = 10.0f;
            o->oVelY = 20.0f;
        }
    } else {
        cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x250);
        o->oTimer = 0;
    }
}

static void chain_chomp_sub_act_lunge(void) {
    f32 val04;
    if (o->oForwardVel > 5.0f) {
        o->oOpacity = 0;
        o->oAnimState = 2;
    }

    if (o->oForwardVel > 1.0f) {
        if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
            o->oForwardVel *= 0.8f; // Drag on the floor
            obj_face_pitch_approach(0, 0x100);
        } else {
            obj_face_pitch_approach((s16)(o->oVelY * -0x80), 0x100);
        }

        // TODO: What is this
        if ((val04 = 900.0f - o->oChainChompDistToPivot) > 220.0f) {
            val04 = 220.0f;
        }

        o->oChainChompMaxDistBetweenChainParts =
            val04 / 220.0f * o->oChainChompMaxDistFromPivotPerChainPart;
        o->oTimer = 0;
    } else {
        // Stopped
        o->oForwardVel = 0.0f;
        o->oSubAction = CHAIN_CHOMP_SUB_ACT_TURN;
    }

    if (o->oTimer < 30) {
        cur_obj_reverse_animation();
    }
}

/**
 * All chain chomp movement behavior, including the cutscene after being
 * released.
 */
static void chain_chomp_act_move(void) {
    f32 maxDistToPivot;
    f32 oldPivotY;
    f32 lunge_dist;
    f32 home_dist;

    // Unload chain if mario is far enough
    if (o->oChainChompReleaseStatus == CHAIN_CHOMP_NOT_RELEASED && o->oDistanceToMario > 8000.0f) {
        o->oAction = CHAIN_CHOMP_ACT_UNLOAD_CHAIN;
        o->oForwardVel = o->oVelY = 0.0f;
    } else {
        cur_obj_update_floor_and_walls();

        // Force lunge if out of bounds
        /*
        lunge_dist = sqrtf(sqr(chain_chomp_spawn_x - o->oPosX) + sqr(chain_chomp_spawn_z - o->oPosZ));
        if ((o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND)
            && (o->oPosY < (chain_chomp_spawn_y - 100.0f) || lunge_dist >= 2400.0f)) {
            u16 lunge_dir = atan2s(chain_chomp_spawn_z - o->oPosZ, chain_chomp_spawn_x - o->oPosX);
            o->oSubAction = CHAIN_CHOMP_SUB_ACT_LUNGE;
            o->oMoveAngleYaw = o->oAngleVelYaw = lunge_dir;
            o->oForwardVel = lunge_dist / 75.0f;
            o->oVelY = 150.0f;
            o->oGravity = -4.0f;
        }*/

        // Check if Mario is in the exhibit
        home_dist = sqr(gMarioState->pos[0] - chain_chomp_spawn_x)
                    + sqr(gMarioState->pos[1] - chain_chomp_spawn_y)
                    + sqr(gMarioState->pos[2] - chain_chomp_spawn_z);
        o->oChainChompRestrictedByChain =
            home_dist < 6000000.0f
            && (SURFACETYPE(gMarioState->floor) == SURFACE_PAINTING_WARP_FB || home_dist < 3250000.0f)
            && (gMarioState->floorHeight < 700.f);
            if (!cur_obj_nearest_object_with_behavior(bhvChompGate)){
                o->oChainChompRestrictedByChain = 1;
                o->oChainChompReleaseStatus = 1;
            }

        switch (o->oSubAction) {
            // AI
            case CHAIN_CHOMP_SUB_ACT_AI_INIT:
                chain_chomp_ai_init();
                break;
            case CHAIN_CHOMP_SUB_ACT_WANDER:
                chain_chomp_ai_wander();
                break;
            case CHAIN_CHOMP_SUB_ACT_IDLE:
                chain_chomp_ai_idle();
                break;
            case CHAIN_CHOMP_SUB_ACT_BONE:
                chain_chomp_ai_bone();
                break;
            case CHAIN_CHOMP_SUB_ACT_THROW:
                chain_chomp_ai_throw();
                break;
            case CHAIN_CHOMP_SUB_ACT_REST:
                chain_chomp_ai_rest();
                break;
            case CHAIN_CHOMP_SUB_ACT_CONSUME:
                chain_chomp_ai_consume();
                break;
            case CHAIN_CHOMP_SUB_ACT_PLAY:
                chain_chomp_ai_play();
                break;
            // Aggressive
            case CHAIN_CHOMP_SUB_ACT_TURN:
                chain_chomp_sub_act_turn();
                break;
            case CHAIN_CHOMP_SUB_ACT_LUNGE:
                chain_chomp_sub_act_lunge();
                break;
        }


        cur_obj_move_standard(78);

        // Move chain down towards ground when Chain Chomp is on the ground
        if ((o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) && o->parentObj->oPosY > o->oFloorHeight)
            if ((o->parentObj->oPosY -= 20.0f) < o->oFloorHeight)
                o->parentObj->oPosY = o->oFloorHeight;

        // Segment 0 connects the pivot to the chain chomp itself

        o->oChainChompSegments[0].posX = o->oPosX - o->parentObj->oPosX;
        o->oChainChompSegments[0].posY = o->oPosY - o->parentObj->oPosY;
        o->oChainChompSegments[0].posZ = o->oPosZ - o->parentObj->oPosZ;

        o->oChainChompDistToPivot =
            sqrtf(o->oChainChompSegments[0].posX * o->oChainChompSegments[0].posX
                  + o->oChainChompSegments[0].posY * o->oChainChompSegments[0].posY
                  + o->oChainChompSegments[0].posZ * o->oChainChompSegments[0].posZ);

        // If the chain is fully stretched
        maxDistToPivot = o->oChainChompMaxDistFromPivotPerChainPart * 5;
        if (o->oChainChompDistToPivot > maxDistToPivot) {
            f32 ratio = maxDistToPivot / o->oChainChompDistToPivot;
            o->oChainChompDistToPivot = maxDistToPivot;

            o->oChainChompSegments[0].posX *= ratio;
            o->oChainChompSegments[0].posY *= ratio;
            o->oChainChompSegments[0].posZ *= ratio;

            // CuckyDev: Release Chain Chomp from chain permanently
            // Move pivot like the chain chomp is pulling it along
            oldPivotY = o->parentObj->oPosY;

            o->parentObj->oPosX = o->oPosX - o->oChainChompSegments[0].posX;
            o->parentObj->oPosY = o->oPosY - o->oChainChompSegments[0].posY;
            o->parentObj->oVelY = o->parentObj->oPosY - oldPivotY;
            o->parentObj->oPosZ = o->oPosZ - o->oChainChompSegments[0].posZ;
        }

        chain_chomp_update_chain_segments();

        obj_check_attacks(&sChainChompHitbox, o->oAction);
    }
}

/**
 * Hide and free the chain chomp segments. The chain objects will unload
 * themselves when they see that the chain chomp is in this action.
 */
static void chain_chomp_act_unload_chain(void) {
    cur_obj_hide();
    mem_pool_free(gObjectMemoryPool, o->oChainChompSegments);

    o->oAction = CHAIN_CHOMP_ACT_UNINITIALIZED;

    if (o->oChainChompReleaseStatus != CHAIN_CHOMP_NOT_RELEASED) {
        obj_mark_for_deletion(o);
    }
}

/**
 * Update function for chain chomp.
 */
void bhv_chain_chomp_update(void) {
    obj_update_blinking(&o->oOpacity, 20, 40, 4);
    switch (o->oAction) {
        case CHAIN_CHOMP_ACT_UNINITIALIZED:
            chain_chomp_act_uninitialized();
            break;
        case CHAIN_CHOMP_ACT_MOVE:
            chain_chomp_act_move();
            break;
        case CHAIN_CHOMP_ACT_UNLOAD_CHAIN:
            chain_chomp_act_unload_chain();
            break;
    }
}

/**
 * Update function for wooden post.
 */
void bhv_wooden_post_update(void) {
    // When ground pounded by mario, drop by -45 + -20
    if (!o->oWoodenPostMarioPounding) {
        if ((o->oWoodenPostMarioPounding = cur_obj_is_mario_ground_pounding_platform())) {
            cur_obj_play_sound_2(SOUND_GENERAL_POUND_WOOD_POST);
            o->oWoodenPostSpeedY = -70.0f;
        }
    } else if (approach_f32_ptr(&o->oWoodenPostSpeedY, 0.0f, 25.0f)) {
        // Stay still until mario is done ground pounding
        o->oWoodenPostMarioPounding = cur_obj_is_mario_ground_pounding_platform();
    } else if ((o->oWoodenPostOffsetY += o->oWoodenPostSpeedY) < -190.0f) {
        // Once pounded, if this is the chain chomp's post, release the chain
        // chomp
        o->oWoodenPostOffsetY = -190.0f;
        if (o->parentObj != o) {
            play_puzzle_jingle();
            o->parentObj->oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_TRIGGER_CUTSCENE;
            o->parentObj = o;
        }
    }

    if (o->oWoodenPostOffsetY != 0.0f) {
        o->oPosY = o->oHomeY + o->oWoodenPostOffsetY;
    } else if (!(o->oBehParams & WOODEN_POST_BP_NO_COINS_MASK)) {
        // Reset the timer once mario is far enough
        if (o->oDistanceToMario > 400.0f) {
            o->oTimer = o->oWoodenPostTotalMarioAngle = 0;
        } else {
            // When mario runs around the post 3 times within 200 frames, spawn
            // coins
            o->oWoodenPostTotalMarioAngle += (s16)(o->oAngleToMario - o->oWoodenPostPrevAngleToMario);
            if (absi(o->oWoodenPostTotalMarioAngle) > 0x30000 && o->oTimer < 200) {
                obj_spawn_loot_yellow_coins(o, 5, 20.0f);
                set_object_respawn_info_bits(o, 1);
            }
        }

        o->oWoodenPostPrevAngleToMario = o->oAngleToMario;
    }
}

/**
 * Init function for chain chomp gate.
 */
void bhv_chain_chomp_gate_init(void) {
    o->parentObj = cur_obj_nearest_object_with_behavior(bhvChainChomp);
}

/**
 * Update function for chain chomp gate
 */
void bhv_chain_chomp_gate_update(void) {
    if (o->parentObj->oChainChompHitGate) {
        spawn_mist_particles_with_sound(SOUND_GENERAL_WALL_EXPLOSION);
        set_camera_shake_from_point(SHAKE_POS_SMALL, o->oPosX, o->oPosY, o->oPosZ);
        spawn_mist_particles_variable(0, 0x7F, 200.0f);
        spawn_triangle_break_particles(30, 0x8A, 3.0f, 4);
        obj_mark_for_deletion(o);
    }
}
