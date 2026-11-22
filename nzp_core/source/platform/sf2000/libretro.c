#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "libretro.h"
#include "../../nzportable_def.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_environment_t environ_cb;

static uint16_t s_framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
static bool s_game_loaded = false;

#define NZP_CORE_STACK_SIZE (512 * 1024)
byte nzp_core_stack[NZP_CORE_STACK_SIZE] __attribute__((aligned(16), used));

bool retro_load_game_impl(const struct retro_game_info *info) __attribute__((used));
void retro_run_impl(void) __attribute__((used));

extern unsigned char rgui8_palette[256][3];
extern unsigned short rgui16_palette[256];
extern byte vid_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

void Sys_SendKeyEvents(void);

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
   if (environ_cb)
      environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt);
}

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) { audio_cb = cb; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }

void retro_set_controller_port_device(unsigned port, unsigned device) {}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "FreeHL";
   info->library_version  = "1.0.0";
   info->need_fullpath    = true;
   info->valid_extensions = "gba|pak|wad";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   memset(info, 0, sizeof(*info));
   info->timing.fps            = 30.0;
   info->timing.sample_rate    = 22050.0;
   info->geometry.base_width   = SCREEN_WIDTH;
   info->geometry.base_height  = SCREEN_HEIGHT;
   info->geometry.max_width    = SCREEN_WIDTH;
   info->geometry.max_height   = SCREEN_HEIGHT;
   info->geometry.aspect_ratio = 4.0 / 3.0;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_init(void)
{
}

void retro_deinit(void)
{
   if (host_initialized)
      Host_Shutdown();
}

void retro_reset(void)
{
}

#define HUNK_SIZE (36 * 1024 * 1024)
#define HUNK_MIN  (26 * 1024 * 1024)
static byte *s_hunk = NULL;
static int s_hunk_size = 0;

bool retro_load_game_impl(const struct retro_game_info *info)
{
   quakeparms_t parms;
   static char *argv[8];
   static char basedir[512];
   int argc = 0;

   if (!s_hunk) {
      int sz = HUNK_SIZE;
      while (sz >= HUNK_MIN) {
         s_hunk = (byte *)malloc(sz);
         if (s_hunk) {
            s_hunk_size = sz;
            break;
         }
         sz -= 1024 * 1024;
      }
      if (!s_hunk) return false;
   }

   argv[argc++] = "nzportable";

   static char gamedir[64];
   memset(basedir, 0, sizeof(basedir));
   gamedir[0] = '\0';

   if (info && info->path) {
      strncpy(basedir, info->path, sizeof(basedir) - 1);
      char *last_slash = strrchr(basedir, '/');
      if (last_slash) {
         *last_slash = '\0';
         size_t blen = strlen(basedir);
         if (blen >= 6 && strcasecmp(basedir + blen - 6, "/valve") == 0) {
            basedir[blen - 6] = '\0';
            strcpy(gamedir, "valve");
         } else if (blen >= 4 && strcasecmp(basedir + blen - 4, "/nzp") == 0) {
            basedir[blen - 4] = '\0';
            strcpy(gamedir, "nzp");
         } else {
            char test_progs[576];
            FILE *tf;
            snprintf(test_progs, sizeof(test_progs), "%s/valve/progs.dat", basedir);
            tf = fopen(test_progs, "rb");
            if (tf) {
               fclose(tf);
               strcpy(gamedir, "valve");
            } else {
               snprintf(test_progs, sizeof(test_progs), "%s/nzp/progs.dat", basedir);
               tf = fopen(test_progs, "rb");
               if (tf) {
                  fclose(tf);
                  strcpy(gamedir, "nzp");
               }
            }
         }
      } else {
         strcpy(basedir, ".");
      }
   } else {
      strcpy(basedir, ".");
   }

   argv[argc++] = "-basedir";
   argv[argc++] = basedir;

   if (gamedir[0]) {
      argv[argc++] = "-game";
      argv[argc++] = gamedir;
   }

   memset(&parms, 0, sizeof(parms));
   parms.basedir = basedir;
   parms.argc = argc;
   parms.argv = argv;
   parms.membase = s_hunk;
   parms.memsize = s_hunk_size;

   Host_Init(&parms);

   s_game_loaded = true;
   return true;
}

#if defined(SF2000) && defined(__mips__)
bool retro_load_game(const struct retro_game_info *info) __attribute__((naked));
bool retro_load_game(const struct retro_game_info *info)
{
   (void)info;
   __asm__ __volatile__(
      "addiu $sp, $sp, -24\n"
      "sw $ra, 20($sp)\n"
      "sw $s0, 16($sp)\n"
      "sw $s1, 12($sp)\n"
      "sw $a0, 8($sp)\n"
      "move $s0, $sp\n"
      "la $sp, nzp_core_stack + 524288\n"
      "lw $a0, 8($s0)\n"
      "jal retro_load_game_impl\n"
      "nop\n"
      "move $sp, $s0\n"
      "lw $s1, 12($sp)\n"
      "lw $s0, 16($sp)\n"
      "lw $ra, 20($sp)\n"
      "addiu $sp, $sp, 24\n"
      "jr $ra\n"
      "nop\n"
   );
}

void retro_run(void) __attribute__((naked));
void retro_run(void)
{
   __asm__ __volatile__(
      "addiu $sp, $sp, -24\n"
      "sw $ra, 20($sp)\n"
      "sw $s0, 16($sp)\n"
      "sw $s1, 12($sp)\n"
      "move $s0, $sp\n"
      "la $sp, nzp_core_stack + 524288\n"
      "jal retro_run_impl\n"
      "nop\n"
      "move $sp, $s0\n"
      "lw $s1, 12($sp)\n"
      "lw $s0, 16($sp)\n"
      "lw $ra, 20($sp)\n"
      "addiu $sp, $sp, 24\n"
      "jr $ra\n"
      "nop\n"
   );
}
#else
bool retro_load_game(const struct retro_game_info *info)
{
   return retro_load_game_impl(info);
}

void retro_run(void)
{
   retro_run_impl();
}
#endif

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
   return false;
}

void retro_unload_game(void)
{
   s_game_loaded = false;
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

void *retro_get_memory_data(unsigned id) { return NULL; }
size_t retro_get_memory_size(unsigned id) { return 0; }
size_t retro_serialize_size(void) { return 0; }
bool retro_serialize(void *data, size_t size) { return false; }
bool retro_unserialize(const void *data, size_t size) { return false; }
void retro_cheat_reset(void) {}
void retro_cheat_set(unsigned index, bool enabled, const char *code) {}

enum {
   PAD_UP = RETRO_DEVICE_ID_JOYPAD_UP,
   PAD_DOWN = RETRO_DEVICE_ID_JOYPAD_DOWN,
   PAD_LEFT = RETRO_DEVICE_ID_JOYPAD_LEFT,
   PAD_RIGHT = RETRO_DEVICE_ID_JOYPAD_RIGHT,
   PAD_A = RETRO_DEVICE_ID_JOYPAD_A,
   PAD_B = RETRO_DEVICE_ID_JOYPAD_B,
   PAD_X = RETRO_DEVICE_ID_JOYPAD_X,
   PAD_Y = RETRO_DEVICE_ID_JOYPAD_Y,
   PAD_L = RETRO_DEVICE_ID_JOYPAD_L,
   PAD_R = RETRO_DEVICE_ID_JOYPAD_R,
   PAD_START = RETRO_DEVICE_ID_JOYPAD_START,
   PAD_SELECT = RETRO_DEVICE_ID_JOYPAD_SELECT,
   PAD_COUNT = 16
};

static int PadKeyForButton(int id, qboolean game)
{
   if (game) {
      switch (id) {
      case PAD_UP:     return '8';            /* +forward */
      case PAD_DOWN:   return '5';            /* +back */
      case PAD_LEFT:   return '4';            /* +moveleft */
      case PAD_RIGHT:  return '6';            /* +moveright */
      case PAD_A:      return K_RIGHTARROW;   /* look right */
      case PAD_Y:      return K_LEFTARROW;    /* look left */
      case PAD_X:      return K_UPARROW;      /* look up */
      case PAD_B:      return K_DOWNARROW;    /* look down */
      case PAD_L:      return K_CTRL;         /* +aim */
      case PAD_R:      return K_DELETE;       /* +attack */
      case PAD_SELECT: return '2';            /* +use / rebuild barriers */
      case PAD_START:  return K_ESCAPE;       /* pause/menu */
      default:         return 0;
      }
   }

   switch (id) {
   case PAD_UP:     return K_UPARROW;
   case PAD_DOWN:   return K_DOWNARROW;
   case PAD_LEFT:   return K_LEFTARROW;
   case PAD_RIGHT:  return K_RIGHTARROW;
   case PAD_A:      return K_ENTER;
   case PAD_B:      return K_ESCAPE;
   case PAD_X:      return K_ENTER;
   case PAD_Y:      return K_ESCAPE;
   case PAD_SELECT: return 0;
   case PAD_START:  return K_ESCAPE;
   default:         return 0;
   }
}

static int s_active_key[PAD_COUNT];
static uint16_t s_suppress_pad = 0;
static qboolean s_was_game = false;
static qboolean s_pad_context_init = false;

void Sys_SendKeyEvents(void)
{
   uint16_t down_pad = 0;
   qboolean game;
   int id;

   if (!input_poll_cb || !input_state_cb)
      return;

   input_poll_cb();

   for (id = 0; id < PAD_COUNT; id++) {
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, id))
         down_pad |= (1u << id);
   }

   game = (key_dest == key_game);

   if (!s_pad_context_init || game != s_was_game) {
      for (id = 0; id < PAD_COUNT; id++) {
         if (s_active_key[id]) {
            Key_Event(s_active_key[id], false);
            s_active_key[id] = 0;
         }
      }
      s_suppress_pad = down_pad;
      s_was_game = game;
      s_pad_context_init = true;
   }

   for (id = 0; id < PAD_COUNT; id++) {
      uint16_t bit = (1u << id);
      int key = PadKeyForButton(id, game);

      if (down_pad & bit) {
         if (!(s_suppress_pad & bit) && !s_active_key[id] && key) {
            Key_Event(key, true);
            s_active_key[id] = key;
         }
      } else {
         s_suppress_pad &= ~bit;
         if (s_active_key[id]) {
            Key_Event(s_active_key[id], false);
            s_active_key[id] = 0;
         }
      }
   }
}

void retro_run_impl(void)
{
   static double s_last_frame_time = 0.0;
   double now;
   double dt;

   if (!s_game_loaded)
      return;

   Sys_SendKeyEvents();

   now = Sys_FloatTime();
   if (s_last_frame_time == 0.0)
      s_last_frame_time = now - (1.0 / 30.0);
   dt = now - s_last_frame_time;
   s_last_frame_time = now;

   /* Use real elapsed time so weapon/reload animations do not run in slow
    * motion when the console renders below 30 fps. Clamp huge stalls. */
   if (dt < 0.001)
      dt = 0.001;
   else if (dt > 0.200)
      dt = 0.200;

   Host_Frame(dt);

   extern void Sys_SendAudioFrames(void);
   Sys_SendAudioFrames();

   // Blit vid_buffer 8-bit to s_framebuffer 16-bit RGB565
   uint32_t *dst32 = (uint32_t *)s_framebuffer;
   uint8_t *src8 = (uint8_t *)vid_buffer;
   int count = (SCREEN_WIDTH * SCREEN_HEIGHT) / 2;

   for (int i = 0; i < count; i++) {
      uint8_t p0 = src8[i * 2 + 0];
      uint8_t p1 = src8[i * 2 + 1];
      uint16_t c0 = rgui16_palette[p0];
      uint16_t c1 = rgui16_palette[p1];
      dst32[i] = ((uint32_t)c1 << 16) | (uint32_t)c0;
   }

   if (video_cb)
      video_cb(s_framebuffer, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH * sizeof(uint16_t));
}
