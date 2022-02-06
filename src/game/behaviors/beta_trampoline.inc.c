/**
 * Behavior for bhvBetaTrampolineTop and bhvBetaTrampolineSpring.
 * This was a trampoline that was never finished. The model and collision
 * are nearly finished, but the code was abandoned very early on
 * in its development. The trampoline consists of 3 objects: the top,
 * the spring, and the base. The base is a static object with no behavior.
 */

/**
 * Update function for bhvBetaTrampolineSpring.
 * The spring continuously updates to be 75 units below the top.
 * It then uses its displacement from its original position
 * (i.e. how much the trampoline has compressed) to calculate its
 * vertical scale factor, so that it compresses visually along with
 * the trampoline. The devs were on the right track with the math,
 * but it is incomplete.
 */
void bhv_beta_trampoline_spring_loop(void) {
}

/**
 * Update function for bhvBetaTrampolineTop.
 * This spawns the other 2 trampoline parts when initialized,
 * and sets a boolean for whether Mario's on or off the trampoline.
 * The trampoline top never actually moves, so the spring will never
 * do anything.
 */
void bhv_beta_trampoline_top_loop(void) {
    cur_obj_set_model(MODEL_TRAMPOLINE);

    // When initialized, spawn the rest of the trampoline
    if (!o->oHiddenBlueCoinSwitch) {
        struct Object *trampolinePart;
        o->oPosY += 150.f;
        o->oHomeY += 150.f;
        trampolinePart = spawn_object(o, MODEL_TRAMPOLINE_CENTER, bhvBetaTrampolineSpring);
        trampolinePart->oPosY -= 130.0f;
        o->oHiddenBlueCoinSwitch = trampolinePart;

        trampolinePart = spawn_object(o, MODEL_TRAMPOLINE_BASE, bhvStaticObject);
        trampolinePart->oPosY -= 150.0f;
    }

    // Update o->oBetaTrampolineMarioOnTrampoline, and reset
    // the trampoline's position if Mario's not on it.
    // Since the trampoline never moves, this doesn't do anything.
    // Maybe they intended to decrease the trampoline's position
    // when Mario's on it in this if statement?
    if (gMarioObject->platform == o) {
        o->oAction = 1;
        o->oVelY -= 1.5f;
        if (gMarioState->action == ACT_GROUND_POUND_LAND) {
            o->oVelY -= 1.5f;
        }
    } else if (o->oAction) {
        o->oAction = 0;
        if ((gMarioState->action == ACT_JUMP) || (gMarioState->action == ACT_BACKFLIP)
            || (gMarioState->action == ACT_TRIPLE_JUMP) || (gMarioState->action == ACT_SIDE_FLIP)) {
            gMarioState->vel[1] += ((o->oHomeY - o->oPosY) / 1.5f);
        }
    }
    o->oVelY += (o->oHomeY + 25.f - o->oPosY) / 30.f;
    o->oVelY *= 0.95f;
    o->oPosY += o->oVelY;
    //o->oHiddenBlueCoinSwitch->oPosY = o->oPosY - 75.f;
    o->oHiddenBlueCoinSwitch->header.gfx.scale[1] = ((o->oPosY - o->oHomeY) / 130.f) + 1.f;
    // This function is from mario_step.c, and is empty.
    // It was probably intended to be used to "let the game know"
    // that the trampoline is currently in use. This potential
    // trampoline infrastructure is found in mario_step.c. See
    // that file for more details.
    // stub_mario_step_2();
}
