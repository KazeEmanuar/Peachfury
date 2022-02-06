/**
 * @file intro_lakitu.inc.c
 * This file implements lakitu's behvaior during the intro cutscene.
 * It's also used during the ending cutscene.
 */


/**
 * Add the camera's position to `offset`, rotate the point to be relative to the camera's focus, then
 * set lakitu's location.
 */
void intro_lakitu_set_offset_from_camera(struct Object *o, Vec3f offset) {
    f32 dist;
    Vec3s focusAngles;
    s16 offsetPitch, offsetYaw;

    vec3f_add(offset, gCamera->pos);
    vec3f_get_dist_and_angle(gCamera->pos, gCamera->focus,
                             &dist, &focusAngles[0], &focusAngles[1]);
    vec3f_get_dist_and_angle(gCamera->pos, offset, &dist, &offsetPitch, &offsetYaw);
    vec3f_set_dist_and_angle(gCamera->pos, offset, dist,
                             focusAngles[0] + offsetPitch, focusAngles[1] + offsetYaw);
    vec3f_to_object_pos(o, offset);
}

void intro_lakitu_set_focus(struct Object *o, Vec3f newFocus) {
    UNUSED Vec3f unusedVec3f;
    Vec3f origin;
    f32 dist;
    s16 pitch, yaw;
    UNUSED u32 unused;

    // newFocus is an offset from lakitu's origin, not a point in the world.
    vec3f_set(origin, 0.f, 0.f, 0.f);
    vec3f_get_dist_and_angle(origin, newFocus, &dist, &pitch, &yaw);
    o->oFaceAnglePitch = pitch;
    o->oFaceAngleYaw = yaw;
}

/**
 * Move lakitu along the spline `offset`, relative to the camera, and face him towards the corresponding
 * location along the spline `focus`.
 */
s32 intro_lakitu_set_pos_and_focus(struct Object *o, struct CutsceneSplinePoint offset[],
                                   struct CutsceneSplinePoint focus[]) {
    Vec3f newOffset, newFocus;
    s32 splineFinished = 0;
    s16 splineSegment = o->oIntroLakituSplineSegment;

    if ((move_point_along_spline(newFocus, offset, &splineSegment, &(o->oIntroLakituSplineSegmentProgress)) == 1)
        || (move_point_along_spline(newOffset, focus, &splineSegment, &(o->oIntroLakituSplineSegmentProgress)) == 1))
        splineFinished += 1;

    o->oIntroLakituSplineSegment = splineSegment;
    intro_lakitu_set_offset_from_camera(o, newOffset);
    intro_lakitu_set_focus(o, newFocus);
    return splineFinished;
}

void bhv_intro_lakitu_loop(void) {
}
#undef TIMER
