// tox_box.c.inc

s8 D_8032F8F0[] = { 4, 1, 4, 1, 6, 1, 6, 1, 5, 1, 5, 1, 6, 1, 6, 1, 5, 1, 2, 4, 1, 4, 1, 4, 1, 2,
                    5, 1, 5, 1, 7, 1, 7, 1, 4, 1, 4, 1, 7, 1, 7, 1, 5, 1, 5, 1, 5, 1, 2, 4, 1, -1 };
s8 D_8032F924[] = { 4, 1, 4, 1, 7, 1, 7, 1, 7, 1, 2, 6, 1, 6, 1, 6, 1, 5,
                    1, 5, 1, 6, 1, 5, 1, 5, 1, 2, 4, 1, 4, 1, 7, 1, -1 };
s8 D_8032F948[] = { 4, 1, 4, 1, 4, 1, 4, 1, 4, 1, 2, 5, 1, 5, 1, 5, 1, 5,
                    1, 5, 1, 7, 1, 2, 6, 1, 6, 1, 5, 1, 2, 4, 1, 7, 1, -1 };
s8 *D_8032F96C[] = { D_8032F8F0, D_8032F924, D_8032F948 };

void tox_box_shake_screen(void) {
    if (o->oDistanceToMario < 3000.0f)
        cur_obj_shake_screen(SHAKE_POS_SMALL);
}

void tox_box_move(f32 forwardVel, f32 a1, s16 deltaPitch, s16 deltaRoll)
{
}

void tox_box_act_4(void) {
    tox_box_move(64.0f, 0.0f, 0x800, 0);
}
void tox_box_act_5(void) {
    tox_box_move(-64.0f, 0.0f, -0x800, 0);
}
void tox_box_act_6(void) {
    tox_box_move(0.0f, -64.0f, 0, 0x800);
}
void tox_box_act_7(void) {
    tox_box_move(0.0f, 64.0f, 0, -0x800);
}

void tox_box_act_1(void) {
}

void tox_box_act_2(void) {
}

void tox_box_act_3(void) {
}

void tox_box_act_0(void) {
}

void (*sToxBoxActions[])(void) = { tox_box_act_0, tox_box_act_1, tox_box_act_2, tox_box_act_3,
                                   tox_box_act_4, tox_box_act_5, tox_box_act_6, tox_box_act_7 };

void bhv_tox_box_loop(void) {
    cur_obj_call_action_function(sToxBoxActions);
    load_object_collision_model();
}
