 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wave_io.h"

int write_wave(short *wave, unsigned int n_wave, int freq, int bits, char *out_name,
               struct Waveheader *header)
  {
  FILE *outfile;//, *logfile;
  unsigned int i;
  char byte;
  //char log_name[255];

  /*update header*/
  header->sample_fq=freq;  
  header->data_length=(n_wave*header->byte_p_spl)/header->modus;
  header->length=header->data_length+sizeof(struct Waveheader)-8;
 
  outfile = fopen(out_name,"wb");
  if (outfile == NULL)
    {
		fprintf(stderr,"Couldn't open %s\n",out_name); 
		exit(0);  
    }

  fwrite(header,sizeof(struct Waveheader),1,outfile);
    
  for (i=0; i < n_wave; i++)
    {
    /*check for 16 bit data*/
    if (header->bit_p_spl==16)
      fwrite(&(wave[i]), sizeof(short),1,outfile);
	  else
      {
      byte=(char)wave[i];
      fwrite(&byte, sizeof(char),1,outfile);
      }
	}

  fclose(outfile);
  return(1);
  }

int read_wave(short **wave, unsigned int *n_wave, int *freq, int *bits, char *in_name,
              struct Waveheader *header)
  {

  FILE *infile;
  unsigned int i, index; 

  int j;
  char ch, byte;

  u_long n_samples;

  infile = fopen(in_name,"rb");
  if (infile == NULL)
    {
    fprintf(stderr,"Couldn't open %s\n",in_name); 
    exit(0);  
    }
  
  /*read header*/
  fread(header,sizeof(struct Waveheader),1,infile);

  fseek(infile,0,SEEK_SET);

  /*find data chunk*/
  j=0;
  for(i=0; i < 100; i++)
    {
    fread(&ch,sizeof(char),1,infile);
    if (ch=='d')
      {
      j++;
      index=i;
      }
    if (ch=='a')
      {
      if ((index==(i-1)) && ((j==1)||(j==3)) )
        {
        j++;
        index=i;
        }
      else
        j=0;
      }   
    if (ch=='t')
      {
      if ((index==(i-1)) && (j==2))
        {
        j++;
        index=i;
        }
      else
        j=0;
      }
    /*found tag "data"*/
    if (j==4)
      break;
    }

  /*read length of data in bytes*/
  fread(&n_samples,sizeof(u_long),1,infile);
   
  
  /*change to uniform header length*/
  header->sc_len=16;
  sprintf(header->data_chunk,"data");
  header->data_length=n_samples;

  *freq = (int)header->sample_fq;  
  *bits = header->bit_p_spl;

  /*get number of samples*/
  *n_wave =header->modus*header->data_length/header->byte_p_spl;
  /*allocate memory for samples*/
  *wave = (short*)malloc(sizeof(short)*(*n_wave));
  

  if (*wave == NULL)
    return(-1);

  for (i=0; i < *n_wave; i++)
    {
    /*if 16 bit data, read short values*/
    if (*bits == 16)
      fread(&((*wave)[i]), sizeof(short),1,infile);
    /*otherwise read char and convert to short*/
    else
      {
      fread(&byte, sizeof(char),1,infile);
      (*wave)[i]=(short)byte;
      }
    }

  fclose(infile);
  return(1);
  }