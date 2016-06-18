#include "i_hexter.h"
#include <iostream>

int main(int argc, char **argv)
{
  static const int BlockSize = 256;
  using namespace std;
  Hexter h;
  h.startUp(44100, BlockSize);
  h.setTuning(440);
  h.setVolume(1);
  float out[BlockSize];
  Event ev;
  ev.type = Event::NOTEON;
  ev.value1 = 50;
  ev.value2 = 100;
  Event events[] = {ev};
 
  h.render(&out[0], BlockSize, &events[0], 1);
  
  cout<<"testData[";
  for (int i=0; i<BlockSize; ++i) {
    cout<<out[i]<<",";
  }
  cout<<"]"<<endl;
  
  h.tearDown();
  return 0;
}
