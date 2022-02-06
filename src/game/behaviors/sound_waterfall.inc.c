// sound_waterfall.inc.c

void bhv_waterfall_sound_loop(void) {
    cur_obj_play_sound_2(SOUND_ENV_WATERFALL1);
}



void bhv_bgm(void){
    switch (o->oBehParams2ndByte){
        case 0:
        play_sound(SOUND_ENV_THEAMAZON, gDefaultSoundArgs);

        break;

        case 1:
        play_sound(SOUND_ENV_UNDERWATERCAVE, gDefaultSoundArgs);

        break;
    }
}