#ifndef I_HEXTER_EMSC_H
#define I_HEXTER_EMSC_H

#include "i_hexter.h"
#include <vector>
#include <cstring>

template<typename T>
class ArrayContainer {
public:
  ArrayContainer(size_t blockSize);
  virtual ~ArrayContainer();
  T get(size_t idx) const;
  void set(size_t idx, const T &value);
  T * data() { return _data; }
  const T * data() const { return _data; }
  size_t size() const { return _bs; }
  void copyTo(ArrayContainer &dst);
  void clear();
private:
  T *_data;
  size_t _bs = 0;
};

struct SampleContainer : public ArrayContainer<float> {
  SampleContainer(size_t blockSize) : ArrayContainer(blockSize) {}
};
struct ByteContainer : public ArrayContainer<Byte> {
  ByteContainer(size_t size) : ArrayContainer(size) {}
};

class EventContainer {
 public:
  EventContainer();
  void addNoteOn(int ticks, Byte value1, Byte value2);
  void addNoteOff(int ticks, Byte value1, Byte value2);
  size_t getNumEvents() const { return _events.size(); }
  const Event * getEvents() const { return _events.data(); }
  void clear() { _events.clear(); }
 private:
  std::vector<Event> _events;
};

class HexterJs : public Hexter 
{
public:
  HexterJs() : Hexter() {}
  void render(SampleContainer &_out, const EventContainer &events, size_t blockSize);
  void sendSysex(const ByteContainer &bytes);
  void fft(SampleContainer &inOut);
};


////////////////////////////////////////////////////////////////////////////////
template<typename T>
ArrayContainer<T>::ArrayContainer(size_t blockSize) : _data(NULL), _bs(blockSize)
{
  _data = new T[blockSize];
  clear();
}

template<typename T>
void ArrayContainer<T>::clear() 
{
  ::memset(_data, 0, sizeof(T)*_bs);
}


template<typename T>
ArrayContainer<T>::~ArrayContainer() 
{
  delete[] _data;
}

template<typename T>
T ArrayContainer<T>::get(size_t idx) const {
  if (idx>=_bs) {
    throw std::runtime_error("out of bounds");
  }
  return _data[idx];
}

template<typename T>
void ArrayContainer<T>::copyTo(ArrayContainer<T> &dst)
{
  if (size()!=dst.size()) {
    throw std::runtime_error("size mismatch");
  }
  ::memcpy(dst.data(), data(), size());
}

template<typename T>
void ArrayContainer<T>::set(size_t idx, const T &value) {
  if (idx>=_bs) {
    throw std::runtime_error("out of bounds");
  }
  _data[idx] = value;
}






#endif

