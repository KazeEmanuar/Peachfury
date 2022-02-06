OUTPUT_ARCH (mips)

#define LINKER /* Removes externs from preprocessed script */
#include "segments.h"
#undef LINKER

#define BEGIN_SEG(name, addr) \
    _##name##SegmentStart = ADDR(.name); \
    _##name##SegmentRomStart = __romPos; \
    .name addr : AT(__romPos)

#define END_SEG(name) \
    _##name##SegmentEnd = ADDR(.name) + SIZEOF(.name); \
    _##name##SegmentRomEnd = __romPos + SIZEOF(.name); \
    __romPos += SIZEOF(.name);

#define BEGIN_NOLOAD(name) \
    _##name##SegmentNoloadStart = ADDR(.name.noload); \
    .name.noload (NOLOAD) :

#define END_NOLOAD(name) \
    _##name##SegmentNoloadEnd = ADDR(.name.noload) + SIZEOF(.name.noload);

#define MIO0_SEG(name, segAddr) \
   BEGIN_SEG(name##_mio0, segAddr) \
   { \
      KEEP(BUILD_DIR/bin/name.mio0.o(.data)); \
      . = ALIGN(0x10); \
   } \
   END_SEG(name##_mio0)

#define MIO0_EU_SEG(name, segAddr) \
   BEGIN_SEG(name##_mio0, segAddr) \
   { \
      KEEP(BUILD_DIR/bin/eu/name.mio0.o(.data)); \
      . = ALIGN(0x10); \
   } \
   END_SEG(name##_mio0)

   /* for unknown reasons the NOLOAD, despite not being used at all,
   is required otherwise the render thread breaks. Why this is needed, IDFK
   maybe for the 8byte alignment? but the 8byte aligmnent of WHAT?? */
#define STANDARD_LEVEL(name) \
   BEGIN_SEG(name##_segment_7, 0x07000000) \
   { \
      KEEP(BUILD_DIR/levels/name/leveldata.mio0.o(.data)); \
      . = ALIGN(0x10); \
   } \
   END_SEG(name##_segment_7) \
   BEGIN_SEG(name, 0x0E000000) \
   { \
      KEEP(BUILD_DIR/levels/name/script.o(.data)); \
      KEEP(BUILD_DIR/levels/name/geo.o(.data)); \
      KEEP(BUILD_DIR/levels/name/geo.o(.rodata*)); \
      KEEP(BUILD_DIR/levels/name/geo.o(.bss*)); \
      KEEP(BUILD_DIR/levels/name/geo.o(.text)); \
   } \
   END_SEG(name) \
   BEGIN_NOLOAD(name)\
   {\
      BUILD_DIR/src/game/main.o(.bss*);\
      . = ALIGN(0x8);\
   }\
   END_NOLOAD(name)\

#define STANDARD_OBJECTS(name, segAddr, geoAddr) \
   BEGIN_SEG(name##_mio0, segAddr) \
   { \
      KEEP(BUILD_DIR/actors/name.mio0.o(.data)); \
      . = ALIGN(0x10); \
   } \
   END_SEG(name##_mio0) \
   BEGIN_SEG(name##_geo, geoAddr) \
   { \
      KEEP(BUILD_DIR/actors/name##_geo.o(.data)); \
   } \
   END_SEG(name##_geo)

SECTIONS
{
   __romPos = 0;

   BEGIN_SEG(boot, 0x04000000)
   {
      KEEP(BUILD_DIR/asm/rom_header.o(.text));
      KEEP(BUILD_DIR/asm/boot.o(.text));
   }
   END_SEG(boot)

   . = 0x80000400;
   BEGIN_NOLOAD(zbuffer) {
      BUILD_DIR/src/buffers/zbuffer.o(.bss*);
   }
   END_NOLOAD(zbuffer)

   . = _zbufferSegmentNoloadEnd;
   BEGIN_NOLOAD(buffers)
   {
      . = ALIGN(0x100000);
      BUILD_DIR/src/buffers/buffers.o(.bss*);
      . = ALIGN(0x1000);
      BUILD_DIR/src/audio/globals_start.o(.bss*);
      . = ALIGN(0x1000);
      BUILD_DIR/src/audio/synthesis.o(.bss*);
      . = ALIGN(0x1000);
      BUILD_DIR/src/audio/heap.o(.bss*);
      . = ALIGN(0x1000);
      BUILD_DIR/src/audio/load.o(.bss*);
      . = ALIGN(0x1000);
      BUILD_DIR/src/audio/data.o(.bss*);
      . = ALIGN(0x1000);
      BUILD_DIR/src/audio*.o(.bss*);
      . = ALIGN(0x100000);
      BUILD_DIR/src/buffers/gfx_output_buffer.o(.bss*);
   }
   END_NOLOAD(buffers)

    . = _buffersSegmentNoloadEnd;
   BEGIN_SEG(main, .) SUBALIGN(16)
   {
      KEEP(BUILD_DIR/asm/entry.o(.text));
      BUILD_DIR/asm/clockspeed.o(.text);
      BUILD_DIR/src/game*.o(.text);
      BUILD_DIR/src/audio*.o(.text);
      */libultra_rom.a:*.o(.text);
      */libgcc.a:*.o(.text);

      BUILD_DIR/lib/rsp.o(.text);  

      . = ALIGN(0x100000);
      /* data */
      BUILD_DIR/src/game*.o(.*data*);
      BUILD_DIR/src/audio*.o(.*data*);
      */libultra_rom.a:*.o(.*data*);

      BUILD_DIR/lib/rsp.o(.data*);

      /* rodata */
      BUILD_DIR/src/game*.o(.rodata*);
      BUILD_DIR/src/audio*.o(.rodata*);
      */libultra_rom.a:*.o(.*rodata*);
      */libgcc.a:*.o(.*rodata*);

      BUILD_DIR/lib/rsp.o(.rodata*);
   }
   END_SEG(main)
   BEGIN_NOLOAD(main)
   {
      BUILD_DIR/src/game*.o(.*bss*);
      BUILD_DIR/src/audio*.o(.*bss*);
      */libultra_rom.a:*.o(COMMON);
      */libultra_rom.a:*.o(.scommon);
      */libultra_rom.a:*.o(.*bss*);
      . = ALIGN(0x8);
   }
   END_NOLOAD(main)
   _mainSegmentNoloadSizeLo = SIZEOF (.main.noload) & 0xffff;
   _mainSegmentNoloadSizeHi = SIZEOF (.main.noload) >> 16;

   . = _mainSegmentNoloadEnd;

   BEGIN_SEG(engine, .)
   {
      BUILD_DIR/src/engine*.o(.text);
      /* data */
      BUILD_DIR/src/engine*.o(.data*);
      BUILD_DIR/src/engine*.o(.sdata*);
      /* rodata */
      BUILD_DIR/src/engine*.o(.rodata*);
   }
   END_SEG(engine)
   BEGIN_NOLOAD(engine)
   {
      BUILD_DIR/src/engine*.o(.bss*);
      . = ALIGN(0x40);
   }
   END_NOLOAD(engine)
   
      . = ALIGN(0x40);
   BEGIN_NOLOAD(objpool)
   {
      BUILD_DIR/src/objpool*.o(.bss*);
      . = ALIGN(0x40);
   }
   END_NOLOAD(objpool)

 //  . = _engineSegmentNoloadEnd;
   . = 0x80400000;
   BEGIN_NOLOAD(framebuffers)
   {
      BUILD_DIR/src/buffers/framebuffers.o(.bss*);
      //. = ALIGN(0x40);
     // . = 0x80400000;
   }
   END_NOLOAD(framebuffers)
__mainPoolStart = 0x80500000;
//   __mainPoolStart = _engineSegmentNoloadEnd;
   __mainPoolSize = RAM_END - .;

   __expansionRamStart = 0x80400000;
   #ifndef USE_EXT_RAM
   ASSERT((. <= __expansionRamStart), "Error: RDRAM expanded into Expansion RAM, despite Expansion RAM not being defined.")
   #endif

   BEGIN_SEG(entry, 0x10000000)
   {
      BUILD_DIR/levels/entry.o(.data);
   }
   END_SEG(entry)

   /* load MIO0 and level data */
   MIO0_SEG(segment2, 0x02000000)
#ifdef VERSION_EU
   MIO0_EU_SEG(translation_en, 0x19000000)
   MIO0_EU_SEG(translation_fr, 0x19000000)
   MIO0_EU_SEG(translation_de, 0x19000000)
#endif

   /* mario actor group */
   STANDARD_OBJECTS(group0,  0x04000000, 0x17000000)

   /* load the other actor groups */
   STANDARD_OBJECTS(group1,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group2,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group3,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group4,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group5,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group6,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group7,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group8,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group9,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group10, 0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group11, 0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group12, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group13, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group14, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group15, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group16, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group17, 0x06000000, 0x0D000000)

   /* load the common actor groups */
   STANDARD_OBJECTS(common0, 0x08000000, 0x0F000000)
   STANDARD_OBJECTS(common1, 0x03000000, 0x16000000)

   /* use segmented addressing for behaviors */
   BEGIN_SEG(behavior, 0x13000000)
   {
      KEEP(BUILD_DIR/data/behavior_data.o(.data));
   }
   END_SEG(behavior)

   /* 0x8016F000 21D7D0-255EC0 [386F0] */
   BEGIN_SEG(goddard, SEG_GODDARD)
   {
      BUILD_DIR/src/menu*.o(.text);
      BUILD_DIR/src/menu*.o(.data*);
      BUILD_DIR/src/menu*.o(.rodata*);
   }
   END_SEG(goddard)
   BEGIN_NOLOAD(goddard)
   {
      BUILD_DIR/src/menu*.o(.bss*);
   }
   END_NOLOAD(goddard)

   ASSERT((. <= RAM_END), "Error: extended past pool end.")

   /* 0x268020 0x268020-0 [0] */
   BEGIN_SEG(intro, 0x14000000)
   {
      KEEP(BUILD_DIR/levels/intro/script.o(.data));
      KEEP(BUILD_DIR/levels/intro/geo.o(.data));
   }
   END_SEG(intro)
   BEGIN_SEG(intro_segment_7, 0x07000000)
   {
      KEEP(BUILD_DIR/levels/intro/leveldata.mio0.o(.data));
      . = ALIGN(0x10);
   }
   END_SEG(intro_segment_7)

   MIO0_SEG(debug_level_select, 0x07000000)
   MIO0_SEG(title_screen_bg, 0x0A000000)

   gMainLevels = __romPos;

   BEGIN_SEG(menu, 0x14000000)
   {
      KEEP(BUILD_DIR/levels/menu/script.o(.data));
      KEEP(BUILD_DIR/levels/menu/geo.o(.data));
   }
   END_SEG(menu)
   BEGIN_SEG(menu_segment_7, 0x07000000)
   {
      KEEP(BUILD_DIR/levels/menu/leveldata.mio0.o(.data));
      . = ALIGN(0x10);
   }
   END_SEG(menu_segment_7)

   BEGIN_SEG(scripts, 0x15000000)
   {
      BUILD_DIR/levels/scripts.o(.data);
   }
   END_SEG(scripts)

   MIO0_SEG(water_skybox, 0x0A000000)
   MIO0_SEG(ccm_skybox, 0x0A000000)
   MIO0_SEG(clouds_skybox, 0x0A000000)
   MIO0_SEG(bitfs_skybox, 0x0A000000)
   MIO0_SEG(wdw_skybox, 0x0A000000)
   MIO0_SEG(cloud_floor_skybox, 0x0A000000)
   MIO0_SEG(ssl_skybox, 0x0A000000)
   MIO0_SEG(bbh_skybox, 0x0A000000)
   MIO0_SEG(bidw_skybox, 0x0A000000)
   MIO0_SEG(bits_skybox, 0x0A000000)

   // Texture bins
   MIO0_SEG(fire, 0x09000000)
   MIO0_SEG(spooky, 0x09000000)
   MIO0_SEG(generic, 0x09000000)
   MIO0_SEG(water, 0x09000000)
   MIO0_SEG(sky, 0x09000000)
   MIO0_SEG(snow, 0x09000000)
   MIO0_SEG(cave, 0x09000000)
   MIO0_SEG(machine, 0x09000000)
   MIO0_SEG(mountain, 0x09000000)
   MIO0_SEG(grass, 0x09000000)
   MIO0_SEG(outside, 0x09000000)
   MIO0_SEG(inside, 0x09000000)
   MIO0_SEG(effect, 0x0B000000)

#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8)
#define DEFINE_LEVEL(_0, _1, _2, folder, _4, _5, _6, _7, _8, _9, _10) STANDARD_LEVEL(folder)

   #include "levels/level_defines.h"

#undef STUB_LEVEL
#undef DEFINE_LEVEL

   /* 4E9FA0-? [?] */
   BEGIN_SEG(assets, __romPos)
   {
      KEEP(BUILD_DIR/assets/mario_anim_data.o(.data));
      KEEP(BUILD_DIR/assets/mario_anim_data.o(.rodata));
      KEEP(BUILD_DIR/assets/demo_data.o(.data));
      KEEP(BUILD_DIR/sound/sound_data.o(.data));
   }
   END_SEG(assets)

   /* Discard everything not specifically mentioned above. */
   /DISCARD/ :
   {
      *(*);
   }
}
