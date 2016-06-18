
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <cstring>

#include "i_hexter.h"

#define HX_BEGIN_CALL checkInstance(instance);//std::cout<<__FUNCTION__<<std::endl;
#define HX_END_CALL

static inline void checkInstance(hexter_instance_t *instance)
{
  if (instance==NULL) {
    throw HexterException("hexter not initialized"); 
  }
}

hexter_synth_t hexter_synth;

extern "C" int sg_mutex_trylock (sg_mutex_t *__mutex)
{
  return 1;
}


static void
hexter_cleanup(hexter_instance_t* instance);

extern "C" int
dssp_voicelist_mutex_lock(void)
{
    return 0; //pthread_mutex_lock(&hexter_synth.mutex);
}

extern "C" int
dssp_voicelist_mutex_unlock(void)
{
    return 0; //pthread_mutex_unlock(&hexter_synth.mutex);
}

/*
 * hexter_deactivate
 *
 * implements LADSPA (*deactivate)()
 */
extern "C" void
hexter_deactivate(hexter_instance_t *instance)
{
    hexter_instance_all_voices_off(instance);  /* stop all sounds immediately */
}

static void
patchbank_load(unsigned char *raw_patch_data, size_t length, dx7_patch_t *firstpatch)
{
    int count=0;
    int patchstart=0;
    int midshift=0;
    int datastart=0;
    int i=0;
    int op=0;
    
    count = 0;
    datastart = 0;
    for (patchstart = 0; patchstart + midshift + 5 < length; patchstart++) {
        
        if (raw_patch_data[patchstart] == 0xf0 &&
            raw_patch_data[patchstart + 1 + midshift] == 0x43 &&
            raw_patch_data[patchstart + 2 + midshift] <= 0x0f &&
            raw_patch_data[patchstart + 3 + midshift] == 0x09 &&
            raw_patch_data[patchstart + 5 + midshift] == 0x00 &&
            patchstart + 4103 + midshift < length &&
            raw_patch_data[patchstart + 4103 + midshift] == 0xf7) {  /* DX7 32 voice dump */
            
            memmove(raw_patch_data + count * DX7_VOICE_SIZE_PACKED,
                    raw_patch_data + patchstart + 6 + midshift, 4096);
            count += 32;
            patchstart += (DX7_DUMP_SIZE_VOICE_BULK - 1);

        } else if (raw_patch_data[patchstart] == 0xf0 && 
                   raw_patch_data[patchstart + midshift + 1] == 0x43 && 
                   raw_patch_data[patchstart + midshift + 2] <= 0x0f && 
                   raw_patch_data[patchstart + midshift + 4] == 0x01 && 
                   raw_patch_data[patchstart + midshift + 5] == 0x1b &&
                   patchstart + midshift + 162 < length &&
                   raw_patch_data[patchstart + midshift + 162] == 0xf7) {  /* DX7 single voice (edit buffer) dump */
            
            unsigned char buf[DX7_VOICE_SIZE_PACKED]; /* to avoid overlap in dx7_patch_pack() */

            dx7_patch_pack(raw_patch_data + patchstart + midshift + 6,
                           (dx7_patch_t *)buf, 0);
            memcpy(raw_patch_data + count * DX7_VOICE_SIZE_PACKED,
                   buf, DX7_VOICE_SIZE_PACKED);

            count += 1;
            patchstart += (DX7_DUMP_SIZE_VOICE_SINGLE - 1);
        }
         /* finally, copy patchdata to the right location */
      
         memcpy(firstpatch, raw_patch_data + datastart, 128 * count);
        //memset(firstpatch, 0, 128*count);
    }

}




/*
 * hexter_cleanup
 *
 * implements LADSPA (*cleanup)()
 */
static void
hexter_cleanup(hexter_instance_t *instance)
{
    int i;

    if (instance) {
        hexter_instance_t *inst, *prev;

        hexter_deactivate(instance);

        prev = NULL;
        for (inst = hexter_synth.instances; inst; inst = inst->next) {
            if (inst == instance) {
                if (prev)
                    prev->next = inst->next;
                else
                    hexter_synth.instances = inst->next;
                break;
            }
            prev = inst;
        }
        hexter_synth.instance_count--;

        if (instance->patches) free(instance->patches);
        free(instance);
    }

    if (!hexter_synth.instance_count && hexter_synth.initialized) {
        for (i = 0; i < HEXTER_MAX_POLYPHONY; i++) {
            if (hexter_synth.voice[i]) {
                free(hexter_synth.voice[i]);
                hexter_synth.voice[i] = NULL;
            }
        }

        hexter_synth.initialized = 0;
    }
}

static bool _mainInit = false;

static void initMain()
{
  if (_mainInit) {
    return;
  }
  _mainInit = true;
  hexter_synth.initialized = 0;
  hexter_synth.mutex_grab_failed = 0;
  dx7_voice_init_tables();
}


/**
 * init hexter 
 */
void Hexter::startUp(unsigned long sample_rate, size_t maxBlockSize)
{
      initMain();
      
      int i;

      if (!hexter_synth.initialized) {

	  hexter_synth.instance_count = 0;
	  hexter_synth.instances = NULL;
	  hexter_synth.nugget_remains = 0;
	  hexter_synth.note_id = 0;
	  hexter_synth.global_polyphony = HEXTER_DEFAULT_POLYPHONY;

	  for (i = 0; i < HEXTER_MAX_POLYPHONY; i++) {
	      hexter_synth.voice[i] = dx7_voice_new();
	      if (!hexter_synth.voice[i]) {
		  DEBUG_MESSAGE(-1, " hexter_instantiate: out of memory!\n");
		  hexter_cleanup(NULL);
		  return;
	      }
	  }

	  hexter_synth.initialized = 1;
      }

      instance = (hexter_instance_t *)calloc(1, sizeof(hexter_instance_t));
      if (!instance) {
	  hexter_cleanup(NULL);
	  return;
      }
      instance->next = hexter_synth.instances;
      hexter_synth.instances = instance;
      hexter_synth.instance_count++;


      if (!(instance->patches = (dx7_patch_t *)malloc(128 * DX7_VOICE_SIZE_PACKED))) {
	  DEBUG_MESSAGE(-1, " hexter_instantiate: out of memory!\n");
	  hexter_cleanup(instance);
	  return;
      }

      instance->sample_rate = (float)sample_rate;
      dx7_eg_init_constants(instance);  /* depends on sample rate */

      instance->polyphony = HEXTER_DEFAULT_POLYPHONY;
      instance->monophonic = DSSP_MONO_MODE_OFF;
      instance->max_voices = instance->polyphony;
      instance->current_voices = 0;
      instance->last_key = 0;
      instance->pending_program_change = -1;
      instance->current_program = 0;
      instance->overlay_program = -1;      
      
      hexter_data_performance_init(instance->performance_buffer);
      hexter_data_patches_init(instance->patches);
      hexter_instance_select_program(instance, 0, 0);
      hexter_instance_init_controls(instance);
      
      // hexter activate
      hexter_instance_all_voices_off(instance);  /* stop all sounds immediately */
      instance->current_voices = 0;
      dx7_lfo_reset(instance);
      
      hexter_instance_handle_monophonic(instance, "off");

 
      // SAMBA BEGIN
      
      _bff = new float[maxBlockSize];
      
      instance->volume = &volume;
      instance->tuning = &tuning;
      instance->output = _bff;
      // SAMBA END
      
}

void Hexter::sendSysex(unsigned char *bytes, size_t numBytes) {
  HX_BEGIN_CALL
    patchbank_load(bytes, numBytes, instance->patches);
    hexter_instance_select_program(instance, 0, 0);
  HX_END_CALL
}

void Hexter::allVoicesOff() {
  HX_BEGIN_CALL
    hexter_synth_all_voices_off();
  HX_END_CALL
}
void Hexter::allNotesOff() {
  HX_BEGIN_CALL
    hexter_instance_all_notes_off(instance);
  HX_END_CALL
}



/**
 * releases hexter resources 
 */
void Hexter::tearDown()
{
  HX_BEGIN_CALL
    if (instance!=NULL) {
      hexter_cleanup(instance);
      instance=NULL;
    }
    if (_bff!=NULL) {
      delete[] _bff;
    }

  HX_END_CALL
}


/*
 * hexter_handle_pending_program_change
 */
static inline void
hexter_handle_pending_program_change(hexter_instance_t *instance)
{

    hexter_instance_select_program((hexter_instance_t *)instance, 0,
                                   instance->pending_program_change);
    instance->pending_program_change = -1;
}


/*
 * hexter_handle_event
 */
static inline void
hexter_handle_event(hexter_instance_t *instance, const Event *event)
{
    switch (event->type) {
      case Event::NOTEOFF:
        hexter_instance_note_off(instance, event->value1, event->value2);
        break;
      case Event::NOTEON:
        if (event->value2 > 0)
            hexter_instance_note_on(instance, event->value1, event->value2);
        else
            hexter_instance_note_off(instance, event->value1, 64); /* shouldn't happen, but... */
        break;
      case Event::KEYPRESS:
        hexter_instance_key_pressure(instance, event->value1, event->value2);
        break;
      case Event::CONTROLLER:
        hexter_instance_control_change(instance, event->value1, event->value2);
        break;
      case Event::CHANPRESS:
        hexter_instance_channel_pressure(instance, event->value1);
        break;
      case Event::PITCHBEND:
        hexter_instance_pitch_bend(instance, event->value1);
        break;
      /* SND_SEQ_EVENT_PGMCHANGE - shouldn't happen */
      /* SND_SEQ_EVENT_SYSEX - shouldn't happen */
      /* SND_SEQ_EVENT_CONTROL14? */
      /* SND_SEQ_EVENT_NONREGPARAM? */
      /* SND_SEQ_EVENT_REGPARAM? */
      default:
        break;
    }
}

void Hexter::render(float *out, size_t sample_count, const Event *events, size_t event_count)
{
  HX_BEGIN_CALL
      unsigned long samples_done = 0;
      unsigned long this_pending_event_tick = 0;
      unsigned long next_pending_event_tick = 0;
      unsigned long burst_size = 0;
      unsigned long event_index = 0;
      memset(instance->output, 0, sizeof(LADSPA_Data) * sample_count);
      if (instance->pending_program_change > -1) {
            hexter_handle_pending_program_change(instance);
      }
      
      while (samples_done < sample_count) {
        if (!hexter_synth.nugget_remains)
            hexter_synth.nugget_remains = HEXTER_NUGGET_SIZE;

        /* process any ready events */
        while (next_pending_event_tick <= samples_done) {
            this_pending_event_tick = next_pending_event_tick;
            next_pending_event_tick = sample_count;

              while (event_index < event_count
                     && events[event_index].tick == this_pending_event_tick) {
                   hexter_handle_event(instance, &events[event_index]);
                   event_index++;
              }
              if (event_index < event_count
                    && events[event_index].tick < next_pending_event_tick) {
                    next_pending_event_tick = events[event_index].tick;
              }
	}
	/* calculate the sample count (burst_size) for the next
	* hexter_synth_render_voices() call to be the smallest of:
	* - control calculation quantization size (HEXTER_NUGGET_SIZE,
	*     in samples)
	* - the number of samples remaining in an already-begun nugget
	*     (hexter_synth.nugget_remains)
	* - the number of samples left in this run
	* - the number of samples until the next event is ready
	*/
	burst_size = HEXTER_NUGGET_SIZE;
	if (hexter_synth.nugget_remains < burst_size) {
	    /* we're still in the middle of a nugget, so reduce the burst size
	     * to end when the nugget ends */
	    burst_size = hexter_synth.nugget_remains;
	}
	if (sample_count - samples_done < burst_size) {
	    /* reduce burst size to end at end of this run */
	    burst_size = sample_count - samples_done;
	} else if (next_pending_event_tick - samples_done < burst_size) {
	    /* reduce burst size to end when next event is ready */
	    burst_size = next_pending_event_tick - samples_done;
	}

	/* render the burst */
	hexter_synth_render_voices(samples_done, burst_size,
		(burst_size == hexter_synth.nugget_remains));
	samples_done += burst_size;
	hexter_synth.nugget_remains -= burst_size; 
	 
    }
  memcpy(out, instance->output, sizeof(LADSPA_Data) * sample_count);
  HX_END_CALL
}
