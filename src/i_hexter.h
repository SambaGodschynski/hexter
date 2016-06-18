#ifndef I_HEXTER_H
#define I_HEXTER_H

extern "C" 
{
  #include "dummy_data_types.h"
  #include "hexter_types.h"
  #include "hexter.h"
  #include "hexter_synth.h"
  #include "dx7_voice.h"
  #include "dx7_voice_data.h"
}

#include <stdexcept>

typedef std::runtime_error HexterException;

typedef unsigned char Byte;


struct Event {
  enum Type { NONE, NOTEOFF, NOTEON, KEYPRESS, CONTROLLER, CHANPRESS, PITCHBEND };
  Type type;
  int tick;
  Byte value1;
  Byte value2;
  Event(Type type=NONE, int tick=0, Byte value1=0, Byte value2=0) : type(type), tick(tick), value1(value1), value2(value2) {}
};

class Hexter 
{
private:
  hexter_instance_t *instance;
  float *_bff;
  float tuning;
  float volume;
public:
  Hexter() : instance(NULL), _bff(NULL), tuning(440.0), volume(1.0) {}
  /**
  * init hexter 
  */
  void startUp(unsigned long sampleRate, size_t maxBlockSize);
  /**
  * releases hexter resources 
  */
  void tearDown();
  /**
   * renders a block
   */
  void render(float *out, size_t blockSize, const Event *events, size_t eventCount);
  void sendSysex(unsigned char *bytes, size_t numBytes);
  void setTuning(float tuning) { this->tuning = tuning; }
  void setVolume(float volume) { this->volume = volume; }
  float getTuning() const { return tuning; }
  float getVolume() const { return volume; }
  void allVoicesOff();
  void allNotesOff();
  
};

#endif
