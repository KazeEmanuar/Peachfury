// king_bobomb.c.inc

// Copy of geo_update_projectile_pos_from_parent
Gfx *geo_update_held_mario_pos(s32 run, UNUSED struct GraphNode *node, Mat4 mtx) {
    Mat4 sp20;
    struct Object *sp1C;

    if (run ) {
        sp1C = (struct Object *) gCurGraphNodeObject;
        if (sp1C->prevObj != NULL) {
            create_transformation_from_matrices(sp20, mtx, gCurGraphNodeCamera->matrixPtr);
            obj_update_pos_from_parent_transformation(sp20, sp1C->prevObj);
            obj_set_gfx_pos_from_pos(sp1C->prevObj);
        }
    }
    return NULL;
}


int mario_is_far_below_object(f32 arg0) {
    return (arg0 < o->oPosY - gMarioObject->oPosY);
}
