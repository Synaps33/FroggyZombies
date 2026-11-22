#include "../../nzportable_def.h"
#include "d_local.h"

viddef_t	vid;				// global video state

#define	BASEWIDTH	320
#define	BASEHEIGHT	240

byte	vid_buffer[BASEWIDTH*BASEHEIGHT];
byte	render_buffer[BASEWIDTH*BASEHEIGHT];
short	zbuffer[BASEWIDTH*BASEHEIGHT];
byte	surfcache[SURFCACHE_SIZE_AT_320X200];

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];

unsigned char rgui8_palette[ 256 ][ 3 ];
unsigned short rgui16_palette[ 256 ];

byte align256_colormap[ 0x4000 + 255 ];

void VID_UpdatePalette(void)
{
	int i;
	for( i = 0; i < 256; i++ )
	{
		rgui16_palette[ i ] = ( rgui8_palette[ i ][ 2 ] >> 3 ) | ( ( rgui8_palette[ i ][ 1 ] >> 2 ) << 5 ) | ( ( rgui8_palette[ i ][ 0 ] >> 3 ) << 11 );
	}
}

void	VID_SetPalette (unsigned char *palette)
{
	memcpy( rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );
	VID_UpdatePalette();
}

void	VID_ShiftPalette (unsigned char *palette)
{
	memcpy( rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );
	VID_UpdatePalette();
}

void	VID_Init (unsigned char *palette)
{
	byte *aligned_colormap;
	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.scale = vid.height/STD_UI_HEIGHT;
	vid.numpages = 1;

	aligned_colormap = ( byte * )( ( ( ( size_t )&align256_colormap[ 0 ] ) + 255 ) & ~0xff );
	memcpy( aligned_colormap, host_colormap, 0x4000 );
	vid.colormap = aligned_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;
	
	d_pzbuffer = zbuffer;
	D_InitCaches (surfcache, sizeof(surfcache));

	memcpy( rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );
	VID_UpdatePalette();
}

void	VID_Shutdown (void)
{
}

void	VID_Update (vrect_t *rects)
{
}

void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}

void D_EndDirectRect (int x, int y, int width, int height)
{
}
