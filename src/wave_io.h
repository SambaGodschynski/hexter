#ifndef W_WAVE_IO
#define W_WAVE_IO

#define u_long unsigned long
#define u_short unsigned short

/* Definitions for Microsoft WAVE format */

#define RIFF		0x46464952	
#define WAVE		0x45564157
#define FMT		0x20746D66
#define DATA		0x61746164
#define PCM_CODE	1
#define WAVE_MONO	1
#define WAVE_STEREO	2

/* it's in chunks like .voc and AMIGA iff, but my source say there
   are in only in this combination, so I combined them in one header;
   it works on all WAVE-file I have
*/
typedef struct Waveheader {
  u_long	main_chunk;	/* 'RIFF' */
  u_long	length;		/* filelen */
  u_long	chunk_type;	/* 'WAVE' */

  u_long	sub_chunk;	/* 'fmt ' */
  u_long	sc_len;		/* length of sub_chunk, =16 */
  u_short	format;		/* should be 1 for PCM-code */
  u_short	modus;		/* 1 Mono, 2 Stereo */
  u_long	sample_fq;	/* frequence of sample */
  u_long	byte_p_sec;
  u_short	byte_p_spl;	/* samplesize; 1 or 2 bytes */
  u_short	bit_p_spl;	/* 8, 12 or 16 bit */ 

  char	data_chunk[4];	/* 'data' */
  u_long	data_length;	/* samplecount */
} WaveHeader;

  
int read_wave(short **wave, unsigned int *n_wave, int *freq, int *bits, char *in_name,
              struct Waveheader *header);

int write_wave(short *wave, unsigned int n_wave, int freq, int bits, char *out_name,
               struct Waveheader *header);

#endif // W_WAVE_IO