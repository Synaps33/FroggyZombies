#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dlfcn.h>
#include "libretro.h"

static void video_cb(const void *data, unsigned width, unsigned height, size_t pitch) {}
static void audio_cb(int16_t left, int16_t right) {}
static size_t audio_batch_cb(const int16_t *data, size_t frames) { return frames; }
static void input_poll_cb(void) {}
static int16_t input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id) { return 0; }
static bool environ_cb(unsigned cmd, void *data) { return true; }

int main(int argc, char **argv)
{
    void *h = dlopen("./nzp_libretro.so", RTLD_NOW);
    if (!h) {
        printf("dlopen: %s\n", dlerror());
        return 1;
    }

    void (*set_env)(retro_environment_t) = dlsym(h, "retro_set_environment");
    void (*set_video)(retro_video_refresh_t) = dlsym(h, "retro_set_video_refresh");
    void (*set_audio)(retro_audio_sample_t) = dlsym(h, "retro_set_audio_sample");
    void (*set_audio_batch)(retro_audio_sample_batch_t) = dlsym(h, "retro_set_audio_sample_batch");
    void (*set_input_poll)(retro_input_poll_t) = dlsym(h, "retro_set_input_poll");
    void (*set_input_state)(retro_input_state_t) = dlsym(h, "retro_set_input_state");
    void (*r_init)(void) = dlsym(h, "retro_init");
    bool (*r_load_game)(const struct retro_game_info *) = dlsym(h, "retro_load_game");
    void (*r_run)(void) = dlsym(h, "retro_run");

    set_env(environ_cb);
    set_video(video_cb);
    set_audio(audio_cb);
    set_audio_batch(audio_batch_cb);
    set_input_poll(input_poll_cb);
    set_input_state(input_state_cb);

    r_init();

    struct retro_game_info info;
    memset(&info, 0, sizeof(info));
    info.path = "/home/Sajnaps/nzportable-sf2000/data/host/nzp/nzp.pak";

    if (!r_load_game(&info)) {
        printf("load failed\n");
        return 1;
    }

    // Try to execute command "map ndu"
    void (*cbuf_addtext)(char *) = dlsym(h, "Cbuf_AddText");
    if (cbuf_addtext) {
        printf("Issuing 'map ndu' command...\n");
        cbuf_addtext("map ndu\n");
    }

    for (int i = 0; i < 150; i++) {
        r_run();
    }
    printf("Successfully ran 150 frames with map ndu!\n");
    return 0;
}
