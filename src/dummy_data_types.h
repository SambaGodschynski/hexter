#ifndef HEXTER_DUMMY_DATA_TYPES
#define HEXTER_DUMMY_DATA_TYPES

#define LADSPA_Data float
#define LADSPA_Descriptor void


typedef struct 
{
  unsigned long Bank;
  unsigned long Program;
  char *Name;
} DSSI_Program_Descriptor;

typedef void * sg_mutex_t;

extern int sg_mutex_trylock (sg_mutex_t *__mutex);


#endif
