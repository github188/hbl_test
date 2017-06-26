/**	@file asf_lib.h
*	@brief Brief:asf lib encapsulate\n
*
*	----------------------------------------------------------------------\n
*	Log:\n
*	Date		Version		Developer		Content\n
*	2006/07/01	1.0.0      	dvs5000			Create\n
*	2006/12/01	2.0.0      	xuyg			port for arm,vx\n
*	2006/12/21	2.1.0	caoyf			byte order support\n
*	----------------------------------------------------------------------\n
*
*	Describe:\n
*
*/

#ifndef __libasf_h__
#define __libasf_h__

#ifdef WIN32
#pragma comment(lib, "libasf.lib")
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define RECFLAG_AUDIO			0x01
#define RECFLAG_VIDEO			0x02
#define RECFLAG_KEYFRAME		0x04
#define RECFLAG_VIDEOKEYFRAME	(RECFLAG_VIDEO|RECFLAG_KEYFRAME)

typedef struct{
	unsigned long	Width;
	unsigned long	Height;
	unsigned short	BitsPerPixel;
	unsigned short	FramePerSecond;
	unsigned long	Compression;
}libasfVideo;

typedef struct{
	unsigned short	Codec;
	unsigned short	Channels;
	unsigned long	SamplesPerSecond;
	unsigned short	BitsPerSample;
}libasfAudio;

typedef struct{
	char*	data;
	int	len;
}libasfBuf;

void*		libasf_new();
int			libasf_open(void* handle, 
#ifdef WIN32
						const wchar_t* 
#else
						const char*
#endif
						filepath);
int		libasf_close(void* handle);		/* return: filesize, -1 means error */
void		libasf_delete(void* handle);
int		libasf_error(void* handle);
int		libasf_video(void* handle, const void* video);
int		libasf_audio(void* handle, const void* audio);
int		libasf_write(void* handle, const void* buf, int cnt, unsigned long sampletime, unsigned char flag);

#ifdef __cplusplus
}
#endif

#endif

