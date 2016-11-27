
#include <emscripten.h>

extern "C" {

// Not using size_t for array indices as the values used by the javascript code are signed.
void array_bounds_check(const int array_size, const int array_idx) {
  if (array_idx < 0 || array_idx >= array_size) {
    EM_ASM_INT({
      throw 'Array index ' + $0 + ' out of bounds: [0,' + $1 + ')';
    }, array_idx, array_size);
  }
}

// VoidPtr

void EMSCRIPTEN_KEEPALIVE emscripten_bind_VoidPtr___destroy___0(void** self) {
  delete self;
}

// SampleContainer

SampleContainer* EMSCRIPTEN_KEEPALIVE emscripten_bind_SampleContainer_SampleContainer_1(int arg0) {
  return new SampleContainer(arg0);
}

float EMSCRIPTEN_KEEPALIVE emscripten_bind_SampleContainer_get_1(SampleContainer* self, int arg0) {
  return self->get(arg0);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_SampleContainer_set_2(SampleContainer* self, int arg0, float arg1) {
  self->set(arg0, arg1);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_SampleContainer_copyTo_1(SampleContainer* self, SampleContainer* arg0) {
  self->copyTo(*arg0);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_SampleContainer_clear_0(SampleContainer* self) {
  self->clear();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_SampleContainer___destroy___0(SampleContainer* self) {
  delete self;
}

// EventContainer

EventContainer* EMSCRIPTEN_KEEPALIVE emscripten_bind_EventContainer_EventContainer_0() {
  return new EventContainer();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_EventContainer_addNoteOn_3(EventContainer* self, int arg0, char arg1, char arg2) {
  self->addNoteOn(arg0, arg1, arg2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_EventContainer_addNoteOff_3(EventContainer* self, int arg0, char arg1, char arg2) {
  self->addNoteOff(arg0, arg1, arg2);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_EventContainer_getNumEvents_0(EventContainer* self) {
  return self->getNumEvents();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_EventContainer_clear_0(EventContainer* self) {
  self->clear();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_EventContainer___destroy___0(EventContainer* self) {
  delete self;
}

// ByteContainer

ByteContainer* EMSCRIPTEN_KEEPALIVE emscripten_bind_ByteContainer_ByteContainer_1(int arg0) {
  return new ByteContainer(arg0);
}

float EMSCRIPTEN_KEEPALIVE emscripten_bind_ByteContainer_get_1(ByteContainer* self, int arg0) {
  return self->get(arg0);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ByteContainer_set_2(ByteContainer* self, int arg0, float arg1) {
  self->set(arg0, arg1);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ByteContainer_copyTo_1(ByteContainer* self, ByteContainer* arg0) {
  self->copyTo(*arg0);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ByteContainer_clear_0(ByteContainer* self) {
  self->clear();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ByteContainer___destroy___0(ByteContainer* self) {
  delete self;
}

// HexterJs

HexterJs* EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs_HexterJs_0() {
  return new HexterJs();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs_startUp_2(HexterJs* self, int arg0, int arg1) {
  self->startUp(arg0, arg1);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs_tearDown_0(HexterJs* self) {
  self->tearDown();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs_render_3(HexterJs* self, SampleContainer* arg0, EventContainer* arg1, int arg2) {
  self->render(*arg0, *arg1, arg2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs_sendSysex_1(HexterJs* self, ByteContainer* arg0) {
  self->sendSysex(*arg0);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs_fft_1(HexterJs* self, SampleContainer* arg0) {
  self->fft(*arg0);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs_allNotesOff_0(HexterJs* self) {
  self->allNotesOff();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs_allVoicesOff_0(HexterJs* self) {
  self->allVoicesOff();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_HexterJs___destroy___0(HexterJs* self) {
  delete self;
}

}

