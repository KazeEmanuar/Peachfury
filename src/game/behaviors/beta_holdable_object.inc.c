/**
 * Behavior for bhvBetaHoldableObject.
 * This is a simple implementation of a holdable object, probably used
 * for testing. This was previously assumed to be a beta shell, as there
 * are unused shell models left in the game; however, there is no evidence
 * to support this theory.
 */

/**
 * Initialization function for bhvBetaHoldableObject.
 * Just sets various physics constants for the object.
 */
void bhv_beta_holdable_object_init(void) {
    o->oGravity = 2.5;
    o->oFriction = 0.8;
    o->oBuoyancy = 1.3;
}

/**
 * Drop the object.
 */
static void beta_holdable_object_drop(void) {
    // Re-enable rendering
    cur_obj_enable_rendering();

    cur_obj_get_dropped();

    o->oHeldState = HELD_FREE;

    o->oForwardVel = 0;
    o->oVelY = 0;
}

/**
 * Throw the object.
 */
static void beta_holdable_object_throw(void) {
    cur_obj_enable_rendering();

    o->oHeldState = HELD_FREE;

    // This flag is never set, why is it cleared?
    o->oFlags &= ~OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW;

    // Set initial velocity
    o->oForwardVel = 40.0;
    o->oVelY = 20.0;
}

/**
 * Update function for bhvBetaHoldableObject.
 * Apply standard physics to the object if not held;
 * otherwise, handle holding logic.
 */
void bhv_beta_holdable_object_loop(void) {
    o->oBuoyancy = -.05f;
    switch (o->oHeldState) {
        case HELD_FREE:
            // Apply standard physics
            object_step();
            cur_obj_become_tangible();
            break;

        case HELD_HELD:
            // Disable rendering to hide the object while it's held
            obj_copy_pos_and_angle(o, gMarioObject);
            cur_obj_disable_rendering();
            cur_obj_become_intangible();
            break;

        case HELD_THROWN:
            beta_holdable_object_throw();
            break;

        case HELD_DROPPED:
            beta_holdable_object_drop();
            break;
    }
    if (o->oHeldState != HELD_HELD) {
        if (o->behavior == segmented_to_virtual(bhvBananaZoo)) {
            if (cur_obj_wait_then_blink(200, 20))
                obj_mark_for_deletion(o);
        }
    } else {
        o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        o->oTimer = 0;
    }
}
