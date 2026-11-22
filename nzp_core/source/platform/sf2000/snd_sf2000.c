#include "../../nzportable_def.h"
#include "../../sound.h"
#include "libretro.h"

/* Power-of-two buffer size for sound mixing (2048 stereo samples = 4096 words) */
#define AUDIO_DMA_SAMPLES 2048
#define SAMPLES_PER_FRAME (22050 / 30)

static short audio_dma_buffer[AUDIO_DMA_SAMPLES * 2];
static short audio_frame_out[SAMPLES_PER_FRAME * 2];

extern retro_audio_sample_batch_t audio_batch_cb;
extern int sound_started;
extern int soundtime;
extern void GetSoundtime(void);

static int audio_read_pos = 0;

qboolean SNDDMA_Init(void)
{
	shm = &sn;
	shm->splitbuffer = 0;
	shm->samplebits = 16;
	shm->speed = 22050;
	shm->channels = 2;
	shm->samples = AUDIO_DMA_SAMPLES * 2; /* 4096 halfwords, power of 2 */
	shm->samplepos = 0;
	shm->submission_chunk = 1;
	shm->buffer = (unsigned char *)audio_dma_buffer;

	memset(audio_dma_buffer, 0, sizeof(audio_dma_buffer));
	audio_read_pos = 0;
	paintedtime = 0;
	soundtime = 0;
	return true;
}

int SNDDMA_GetDMAPos(void)
{
	return audio_read_pos;
}

void SNDDMA_Submit(void)
{
}

void SNDDMA_Shutdown(void)
{
}

void Sys_SendAudioFrames(void)
{
	if (!audio_batch_cb || !sound_started || !shm)
		return;

	/* Advance DMA time and mix up to current frame's requirement */
	GetSoundtime();

	int endtime = soundtime + SAMPLES_PER_FRAME;
	S_PaintChannels(endtime);

	/* Copy exactly SAMPLES_PER_FRAME from circular DMA buffer */
	int lpos = audio_read_pos;
	for (int i = 0; i < SAMPLES_PER_FRAME; i++) {
		int idx = ((lpos + i) & (AUDIO_DMA_SAMPLES - 1)) * 2;
		audio_frame_out[i * 2 + 0] = audio_dma_buffer[idx + 0];
		audio_frame_out[i * 2 + 1] = audio_dma_buffer[idx + 1];

		/* Clear read samples to prevent echoes/clicks if no new sound plays */
		audio_dma_buffer[idx + 0] = 0;
		audio_dma_buffer[idx + 1] = 0;
	}

	audio_batch_cb(audio_frame_out, SAMPLES_PER_FRAME);

	audio_read_pos = (audio_read_pos + SAMPLES_PER_FRAME) & (AUDIO_DMA_SAMPLES - 1);
}
