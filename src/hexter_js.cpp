#include "hexter_js.h"
#include <exception>
#include <math.h>


namespace {
  template <typename T>
  void _fft(T *r, T *i, size_t numSamples);
}



EventContainer::EventContainer()
{
}
void EventContainer::addNoteOn(int ticks, Byte value1, Byte value2)
{
  _events.emplace_back(Event(Event::NOTEON, ticks, value1, value2));
}
void EventContainer::addNoteOff(int ticks, Byte value1, Byte value2)
{
  _events.emplace_back(Event(Event::NOTEOFF, ticks, value1, value2));
}

void HexterJs::render(SampleContainer &_out, const EventContainer &events, size_t blockSize)
{
  // void render(float *out, size_t blockSize, const Event *events, size_t eventCount);
  Hexter::render(_out.data(), blockSize, events.getEvents(), events.getNumEvents());
}

void HexterJs::fft(SampleContainer &inOut)
{
  size_t bs = inOut.size();
  float *r = inOut.data();
  float *i = new float[bs];
  ::memset(i, 0, sizeof(float)*bs);
  _fft(r, i, bs);
  // calculate amp
  for (size_t idx=0; idx<bs; ++idx) {
    float amp = ::sqrt (::pow(r[idx]/(float)bs,2) + ::pow(i[idx]/(float)bs,2));
    if (idx<(bs>>2)) {
      i[idx] = amp; // cp to inOut
    } else {
      r[idx] = 0;
    }
  }
  delete[] i;
}

void HexterJs::sendSysex(const ByteContainer &bytes)
{
  Hexter::sendSysex(const_cast<Byte*>(bytes.data()), bytes.size());
}


////////////////////////////////////////////////////////////////////////////////

namespace {
  /**
   * @brief calculates fft for inplace for r and i.
   * where r is the discrete signal and i should be 0. the result is a complex number.
   * The amplitude for a frq can be calculated in that way:
   *    amp = sqrt ( pow( r[index]/numSamples, 2 ) + pow ( i[index]/numSamples, 2 ) )
   *
   * origin: fftguru.com
   */
  template <typename T>
  void _fft(T *r, T *i, size_t numSamples) {
    if (numSamples==0) {
      return;
    }
    if ((numSamples & (numSamples-1))!=0 || numSamples==1) {
      // numSamples is not power of 2
      throw std::runtime_error("fft numsamples != power of 2");
    }
    long M = numSamples/2, lo = 0, hi, k, m, delta; 
    long J = 0, K, L, N2 = numSamples/2;
    T t, t1, x, y, twcos, twsin, pi = (T)3.1415926535897932;
    for (delta = M; delta > 0; delta = delta/2) {
      x = pi/delta; hi = 0;
      for (k = 0; k < M/delta; k++) {
	lo = hi; hi = lo + delta;
	for (m = 0; m < delta; m++) {
	  t = r[lo] - r[hi] ; r[lo] = r[lo] + r[hi] ; r[hi] = t ;
	  t1 = i[lo] - i[hi] ; i[lo] = i[lo] + i[hi] ; i[hi] = t1 ;
	  if ( m > 0 && delta > 1 ) { // do twiddle multiply, but not for twiddles of 0
	    y = m*x;
	    twcos = cos (y) ; twsin = -sin (y) ;
	    t = (r[hi] - i[hi])*twsin ;
	    r[hi] = t + r[hi]*(twcos - twsin) ;
	    i[hi] = t + i[hi]*(twcos + twsin) ;
	  } // end if
	  lo++;               
	  hi++;
	} // end for over m
      } // end for over k
    } // end for over delta
    //******** bit reverse for radix 2 *****
    for (L = 0; L < (numSamples-2); L++) {
      if (L < J) {
	t = r[L]; r[L] = r[J]; r[J] = t;
	t1 = i[L]; i[L] = i[J]; i[J] = t1;
      }
      K = N2;
      while (K <= J) {
	J = J - K; K = K/2;
      } // end while
      J = J + K;
    } // end for ***** bit reverse done ******
  }
}
