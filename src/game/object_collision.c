#include <PR/ultratypes.h>

#include "sm64.h"
#include "debug.h"
#include "interaction.h"
#include "mario.h"
#include "object_list_processor.h"
#include "spawn_object.h"
#include "OPT_FOR_SIZE.h"



int detect_object_hitbox_overlap(struct Object *a, struct Object *b) {
#define sp3C (a->oPosY - a->hitboxDownOffset)
#define sp38 (b->oPosY - b->hitboxDownOffset)
#define dx (a->oPosX - b->oPosX)
#define dz (a->oPosZ - b->oPosZ)
#define collisionRadius (a->hitboxRadius + b->hitboxRadius)
#define distance (sqrtf(dx * dx + dz * dz))
#define sp20 (a->hitboxHeight + sp3C)
#define sp1C (b->hitboxHeight + sp38)
    if (((sp3C < sp1C) && (sp20 > sp38)) && (collisionRadius > distance)) {
        if (a->numCollidedObjs >= 4) {
            return 0;
        }
        if (b->numCollidedObjs >= 4) {
            return 0;
        }
        a->collidedObjs[a->numCollidedObjs] = b;
        b->collidedObjs[b->numCollidedObjs] = a;
        a->collidedObjInteractTypes |= b->oInteractType;
        b->collidedObjInteractTypes |= a->oInteractType;
        a->numCollidedObjs++;
        b->numCollidedObjs++;
        return 1;
    } else {
        return 0;
    }
}

int detect_object_hurtbox_overlap(struct Object *a, struct Object *b) {
#define sp3C (a->oPosY - a->hitboxDownOffset)
#define sp38 (b->oPosY - b->hitboxDownOffset)
#define sp34 (a->oPosX - b->oPosX)
#define sp2C (a->oPosZ - b->oPosZ)
#define sp28 (a->hurtboxRadius + b->hurtboxRadius)
#define sp24 (sqrtf(sp34 * sp34 + sp2C * sp2C))
#define sp20 (a->hitboxHeight + sp3C)
#define sp1C (b->hurtboxHeight + sp38)

    if (a == gMarioObject) {
        b->oInteractionSubtype |= INT_SUBTYPE_DELAY_INVINCIBILITY;
    }

    if (((sp3C < sp1C) && (sp20 > sp38)) && (sp28 > sp24)) {
        if (a == gMarioObject) {
            b->oInteractionSubtype &= ~INT_SUBTYPE_DELAY_INVINCIBILITY;
        }
        return 1;
    } else {
        return 0;
    }

    //! no return value
}
#undef sp1C
#undef sp20
#undef sp24
#undef sp28
#undef sp2C
#undef sp34
#undef sp38
#undef sp3C
void clear_object_collision(struct Object *a) {
    struct Object *sp4 = (struct Object *) a->header.next;

    while (sp4 != a) {
        sp4->numCollidedObjs = 0;
        sp4->collidedObjInteractTypes = 0;
        if (sp4->oIntangibleTimer > 0) {
            sp4->oIntangibleTimer--;
        }
        sp4 = (struct Object *) sp4->header.next;
    }
}

void check_collision_in_list(struct Object *a, struct Object *b, struct Object *c) {
    if (a->oIntangibleTimer == 0) {
        while (b != c) {
            if (b->oIntangibleTimer == 0) {
                if (detect_object_hitbox_overlap(a, b) && b->hurtboxRadius != 0.0f) {
                    detect_object_hurtbox_overlap(a, b);
                }
            }
            b = (struct Object *) b->header.next;
        }
    }
}

void check_player_object_collision(void) {
    struct Object *sp1C = (struct Object *) &gObjectLists[OBJ_LIST_PLAYER];
    struct Object *sp18 = (struct Object *) sp1C->header.next;

    while (sp18 != sp1C) {
        check_collision_in_list(sp18, (struct Object *) sp18->header.next, sp1C);
        check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_POLELIKE].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_POLELIKE]);
        check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_LEVEL].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_LEVEL]);
        check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_GENACTOR].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_GENACTOR]);
        check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_PUSHABLE].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE]);
        check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_SURFACE].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_SURFACE]);
        check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_DESTRUCTIVE].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_DESTRUCTIVE]);
        sp18 = (struct Object *) sp18->header.next;
    }
}

void check_pushable_object_collision(void) {
    struct Object *sp1C = (struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE];
    struct Object *sp18 = (struct Object *) sp1C->header.next;

    while (sp18 != sp1C) {
        check_collision_in_list(sp18, (struct Object *) sp18->header.next, sp1C);
        sp18 = (struct Object *) sp18->header.next;
    }
}

void check_destructive_object_collision(void) {
    struct Object *sp1C = (struct Object *) &gObjectLists[OBJ_LIST_DESTRUCTIVE];
    struct Object *sp18 = (struct Object *) sp1C->header.next;

    while (sp18 != sp1C) {
        if (sp18->oDistanceToMario < 2000.0f && !(sp18->activeFlags & ACTIVE_FLAG_UNK9)) {
            check_collision_in_list(sp18, (struct Object *) sp18->header.next, sp1C);
            check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_GENACTOR].next,
                                    (struct Object *) &gObjectLists[OBJ_LIST_GENACTOR]);
            check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_PUSHABLE].next,
                                    (struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE]);
            check_collision_in_list(sp18, (struct Object *) gObjectLists[OBJ_LIST_SURFACE].next,
                                    (struct Object *) &gObjectLists[OBJ_LIST_SURFACE]);
        }
        sp18 = (struct Object *) sp18->header.next;
    }
}

void detect_object_collisions(void) {
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_POLELIKE]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_PLAYER]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_GENACTOR]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_LEVEL]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_SURFACE]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_DESTRUCTIVE]);
    check_player_object_collision();
    check_destructive_object_collision();
    check_pushable_object_collision();
}
