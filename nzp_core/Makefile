TARGET_NAME := nzp

CC := gcc
AR := ar
RANLIB := ranlib

SRCDIR := source
PLATDIR := $(SRCDIR)/platform/sf2000

SOURCES_C := \
	$(PLATDIR)/libretro.c \
	$(PLATDIR)/sys_sf2000.c \
	$(PLATDIR)/vid_sf2000.c \
	$(PLATDIR)/snd_sf2000.c \
	$(PLATDIR)/net_sf2000.c \
	$(PLATDIR)/common.c \
	$(PLATDIR)/music.c \
	$(PLATDIR)/net_main.c \
	$(PLATDIR)/d_edge.c \
	$(PLATDIR)/d_fill.c \
	$(PLATDIR)/d_init.c \
	$(PLATDIR)/d_modech.c \
	$(PLATDIR)/d_polyse.c \
	$(PLATDIR)/d_scan.c \
	$(PLATDIR)/d_scan_nspirec.c \
	$(PLATDIR)/d_sky.c \
	$(PLATDIR)/d_sprite.c \
	$(PLATDIR)/d_surf.c \
	$(PLATDIR)/d_vars.c \
	$(PLATDIR)/d_zpoint.c \
	$(PLATDIR)/r_aclip.c \
	$(PLATDIR)/r_alias.c \
	$(PLATDIR)/r_bsp.c \
	$(PLATDIR)/r_draw.c \
	$(PLATDIR)/r_edge.c \
	$(PLATDIR)/r_light.c \
	$(PLATDIR)/r_main.c \
	$(PLATDIR)/r_misc.c \
	$(PLATDIR)/r_sky.c \
	$(PLATDIR)/r_sprite.c \
	$(PLATDIR)/r_surf.c \
	$(PLATDIR)/screen.c \
	$(PLATDIR)/r_hyena_soft.c \
	$(PLATDIR)/cpu/cpu_main.c \
	$(PLATDIR)/cpu/cpu_model.c \
	$(SRCDIR)/chase.c \
	$(SRCDIR)/cl_demo.c \
	$(SRCDIR)/cl_input.c \
	$(SRCDIR)/cl_main.c \
	$(SRCDIR)/cl_parse.c \
	$(SRCDIR)/cl_slist.c \
	$(SRCDIR)/cl_tent.c \
	$(SRCDIR)/cmd.c \
	$(SRCDIR)/console.c \
	$(SRCDIR)/crc.c \
	$(SRCDIR)/crypter.c \
	$(SRCDIR)/cvar.c \
	$(SRCDIR)/host.c \
	$(SRCDIR)/host_cmd.c \
	$(SRCDIR)/images.c \
	$(SRCDIR)/input.c \
	$(SRCDIR)/keys.c \
	$(SRCDIR)/mathlib.c \
	$(SRCDIR)/matrixlib.c \
	$(SRCDIR)/net_loop.c \
	$(SRCDIR)/net_vcr.c \
	$(SRCDIR)/snd_dma.c \
	$(SRCDIR)/snd_mem.c \
	$(SRCDIR)/snd_mix.c \
	$(SRCDIR)/snd_music.c \
	$(SRCDIR)/startup.c \
	$(SRCDIR)/sv_main.c \
	$(SRCDIR)/sv_move.c \
	$(SRCDIR)/sv_phys.c \
	$(SRCDIR)/sv_user.c \
	$(SRCDIR)/system.c \
	$(SRCDIR)/view.c \
	$(SRCDIR)/wad.c \
	$(SRCDIR)/world.c \
	$(SRCDIR)/zone.c \
	$(SRCDIR)/menu/menu_accessibility.c \
	$(SRCDIR)/menu/menu_audio.c \
	$(SRCDIR)/menu/menu_bindings.c \
	$(SRCDIR)/menu/menu_bios.c \
	$(SRCDIR)/menu/menu.c \
	$(SRCDIR)/menu/menu_configuration.c \
	$(SRCDIR)/menu/menu_controls.c \
	$(SRCDIR)/menu/menu_credits.c \
	$(SRCDIR)/menu/menu_custommaps.c \
	$(SRCDIR)/menu/menu_gamesettings.c \
	$(SRCDIR)/menu/menu_helper.c \
	$(SRCDIR)/menu/menu_loadscreen.c \
	$(SRCDIR)/menu/menu_lobby.c \
	$(SRCDIR)/menu/menu_main.c \
	$(SRCDIR)/menu/menu_pause.c \
	$(SRCDIR)/menu/menu_stockmaps.c \
	$(SRCDIR)/menu/menu_sys.c \
	$(SRCDIR)/menu/menu_video.c \
	$(SRCDIR)/qcvm/cJSON.c \
	$(SRCDIR)/qcvm/pr_cmds.c \
	$(SRCDIR)/qcvm/pr_edict.c \
	$(SRCDIR)/qcvm/pr_exec.c \
	$(SRCDIR)/qcvm/pr_json.c \
	$(SRCDIR)/render/r_decals.c \
	$(SRCDIR)/render/r_draw.c \
	$(SRCDIR)/render/r_dxt.c \
	$(SRCDIR)/render/r_entity_fragments.c \
	$(SRCDIR)/render/r_hud.c \
	$(SRCDIR)/render/r_mdl.c \
	$(SRCDIR)/render/r_particles.c \
	$(SRCDIR)/render/r_pixels.c \
	$(SRCDIR)/render/r_surface.c \
	$(SRCDIR)/render/r_texture.c \
	$(SRCDIR)/tests/test_handler.c \
	$(SRCDIR)/tests/test_map_boot.c \
	$(SRCDIR)/tests/test_pathfind.c \
	$(SRCDIR)/tests/test_qcvm.c \
	$(SRCDIR)/tests/test_restart.c

COMMON_FLAGS := -DPLATFORM_DIRECTORY=sf2000 -DPLATFORM_RENDERER=cpu \
	-DSOFTWARE_RENDERER -DMAX_AI_COUNT=5 \
	-DOLD_SCREEN_API -DPLATFORM_GAMEFILES_IN_PAK -DMENU_HIDE_MAP_DESCRIPTIONS \
	-DPLATFORM_CONFIRM_IS_ENTER -D__LIBRETRO__ -Isource -Isource/platform/sf2000

ifeq ($(platform), sf2000)
	MIPS ?= /opt/mips32-mti-elf/2019.09-03-2/bin/mips-mti-elf-
	CC = $(MIPS)gcc
	AR = $(MIPS)ar
	RANLIB = $(MIPS)ranlib
	CFLAGS += -EL -march=mips32 -mtune=mips32r2 -msoft-float -G0 -mno-abicalls -fno-pic
	CFLAGS += -O3 -ffast-math -fsingle-precision-constant -fno-math-errno -fno-trapping-math \
	          -fomit-frame-pointer -finline-functions -funroll-loops -fno-strict-aliasing \
	          -ffunction-sections -fdata-sections -DSF2000 -DNO_THREADS $(COMMON_FLAGS)
	TARGET := $(TARGET_NAME)_libretro_sf2000.a
else
	CFLAGS += -O2 -g -fPIC $(COMMON_FLAGS)
	TARGET := $(TARGET_NAME)_libretro.so
	LDFLAGS += -shared -lm
endif

OBJS := $(SOURCES_C:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
ifeq ($(platform), sf2000)
	$(AR) rcs $@ $(OBJS)
	$(RANLIB) $@
else
	$(CC) $(OBJS) $(LDFLAGS) -o $@
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET_NAME)_libretro_sf2000.a $(TARGET_NAME)_libretro.so

.PHONY: all clean
