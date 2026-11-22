#include "../../nzportable_def.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

void Sys_SendKeyEvents(void);

qboolean IN_PlatformHasMouse(void) { return false; }
qboolean IN_PlatformHasGamepad(void) { return true; }
void IN_SetMouseToRelative(bool relative) { (void)relative; }
void IN_PlatformClearPendingInput(void) {}
void IN_PlatformMouseMove(usercmd_t *cmd) { (void)cmd; }
void IN_GetAnalogStick(in_analog_stick_id_t stick, in_analog_stick_t *value) { (void)stick; value->x = value->y = 0; }
void IN_PlatformInit(void) {}
void IN_PlatformShutdown(void) {}
void IN_PlatformCommands(void) {}
void IN_PlatformMove(usercmd_t *cmd) { (void)cmd; }

qboolean isDedicated = false;

#define MAX_HANDLES 32
static FILE *sys_handles[MAX_HANDLES];

static int findhandle(void)
{
    int i;
    for (i = 1; i < MAX_HANDLES; i++) {
        if (!sys_handles[i])
            return i;
    }
    Sys_Error("out of handles");
    return -1;
}

static int filelength(FILE *f)
{
    int pos = ftell(f);
    fseek(f, 0, SEEK_END);
    int end = ftell(f);
    fseek(f, pos, SEEK_SET);
    return end;
}

int Sys_FileOpenRead(char *path, int *hndl)
{
    int i = findhandle();
    FILE *f = fopen(path, "rb");
    if (!f) {
        *hndl = -1;
        return -1;
    }
    sys_handles[i] = f;
    *hndl = i;
    return filelength(f);
}

void Sys_CaptureScreenshot(void)
{
}

int Sys_FileOpenWrite(char *path)
{
    int i = findhandle();
    FILE *f = fopen(path, "wb");
    if (!f)
        Sys_Error("Error opening %s: %s", path, strerror(errno));
    sys_handles[i] = f;
    return i;
}

void Sys_FileClose(int handle)
{
    if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle]) {
        fclose(sys_handles[handle]);
        sys_handles[handle] = NULL;
    }
}

void Sys_FileSeek(int handle, int position)
{
    if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle])
        fseek(sys_handles[handle], position, SEEK_SET);
}

int Sys_FileTell(int handle)
{
    if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle])
        return ftell(sys_handles[handle]);
    return -1;
}

int Sys_FileRead(int handle, void *dest, int count)
{
    if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle])
        return fread(dest, 1, count, sys_handles[handle]);
    return -1;
}

int Sys_FileWrite(int handle, void *data, int count)
{
    if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle])
        return fwrite(data, 1, count, sys_handles[handle]);
    return -1;
}

int Sys_FileTime(char *path)
{
    FILE *f = fopen(path, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return -1;
}

void Sys_mkdir(char *path)
{
}

extern qboolean console_enabled;

void Sys_Init(void)
{
    console_enabled = true;
}

void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length)
{
}

void Sys_Printf(char *fmt, ...)
{
    va_list argptr;
    char buf[1024];
    va_start(argptr, fmt);
    vsnprintf(buf, sizeof(buf), fmt, argptr);
    va_end(argptr);
#ifdef SF2000
    xlog("%s", buf);
#else
    fputs(buf, stdout);
#endif
}

void Sys_PrintSystemInfo(void)
{
    Con_Printf("SF2000 NZP v%4.1f\n", (double)(VERSION));
}

extern void lcd_bsod(const char *fmt, ...);

void Sys_SystemError(char *error)
{
    fprintf(stderr, "Sys_SystemError: %s\n", error);
#ifdef SF2000
    lcd_bsod("NZP ERROR:\n%s", error);
#endif
    Sys_Quit();
}

void Sys_Quit(void)
{
    Host_Shutdown();
    for (int i = 0; i < MAX_HANDLES; i++) {
        if (sys_handles[i]) {
            fclose(sys_handles[i]);
            sys_handles[i] = NULL;
        }
    }
    exit(1);
}

double Sys_FloatTime(void)
{
    struct timeval tp;
    struct timezone tzp;
    static int secbase = 0;

    gettimeofday(&tp, &tzp);
    if (!secbase) {
        secbase = tp.tv_sec;
        return tp.tv_usec / 1000000.0;
    }
    return (tp.tv_sec - secbase) + tp.tv_usec / 1000000.0;
}

char *Sys_ConsoleInput(void)
{
    return NULL;
}

void Sys_Sleep(void)
{
}

void Sys_HighFPPrecision(void)
{
}

void Sys_LowFPPrecision(void)
{
}

void Sys_SetFPCW(void)
{
}

void Sys_DebugLog(char *file, char *fmt, ...)
{
}
