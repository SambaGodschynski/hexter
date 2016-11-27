
// Bindings utilities

function WrapperObject() {
}
WrapperObject.prototype = Object.create(WrapperObject.prototype);
WrapperObject.prototype.constructor = WrapperObject;
WrapperObject.prototype.__class__ = WrapperObject;
WrapperObject.__cache__ = {};
Module['WrapperObject'] = WrapperObject;

function getCache(__class__) {
  return (__class__ || WrapperObject).__cache__;
}
Module['getCache'] = getCache;

function wrapPointer(ptr, __class__) {
  var cache = getCache(__class__);
  var ret = cache[ptr];
  if (ret) return ret;
  ret = Object.create((__class__ || WrapperObject).prototype);
  ret.ptr = ptr;
  return cache[ptr] = ret;
}
Module['wrapPointer'] = wrapPointer;

function castObject(obj, __class__) {
  return wrapPointer(obj.ptr, __class__);
}
Module['castObject'] = castObject;

Module['NULL'] = wrapPointer(0);

function destroy(obj) {
  if (!obj['__destroy__']) throw 'Error: Cannot destroy object. (Did you create it yourself?)';
  obj['__destroy__']();
  // Remove from cache, so the object can be GC'd and refs added onto it released
  delete getCache(obj.__class__)[obj.ptr];
}
Module['destroy'] = destroy;

function compare(obj1, obj2) {
  return obj1.ptr === obj2.ptr;
}
Module['compare'] = compare;

function getPointer(obj) {
  return obj.ptr;
}
Module['getPointer'] = getPointer;

function getClass(obj) {
  return obj.__class__;
}
Module['getClass'] = getClass;

// Converts big (string or array) values into a C-style storage, in temporary space

var ensureCache = {
  buffer: 0,  // the main buffer of temporary storage
  size: 0,   // the size of buffer
  pos: 0,    // the next free offset in buffer
  temps: [], // extra allocations
  needed: 0, // the total size we need next time

  prepare: function() {
    if (this.needed) {
      // clear the temps
      for (var i = 0; i < this.temps.length; i++) {
        Module['_free'](this.temps[i]);
      }
      this.temps.length = 0;
      // prepare to allocate a bigger buffer
      Module['_free'](this.buffer);
      this.buffer = 0;
      this.size += this.needed;
      // clean up
      this.needed = 0;
    }
    if (!this.buffer) { // happens first time, or when we need to grow
      this.size += 128; // heuristic, avoid many small grow events
      this.buffer = Module['_malloc'](this.size);
      assert(this.buffer);
    }
    this.pos = 0;
  },
  alloc: function(array, view) {
    assert(this.buffer);
    var bytes = view.BYTES_PER_ELEMENT;
    var len = array.length * bytes;
    len = (len + 7) & -8; // keep things aligned to 8 byte boundaries
    var ret;
    if (this.pos + len >= this.size) {
      // we failed to allocate in the buffer, this time around :(
      assert(len > 0); // null terminator, at least
      this.needed += len;
      ret = Module['_malloc'](len);
      this.temps.push(ret);
    } else {
      // we can allocate in the buffer
      ret = this.buffer + this.pos;
      this.pos += len;
    }
    var retShifted = ret;
    switch (bytes) {
      case 2: retShifted >>= 1; break;
      case 4: retShifted >>= 2; break;
      case 8: retShifted >>= 3; break;
    }
    for (var i = 0; i < array.length; i++) {
      view[retShifted + i] = array[i];
    }
    return ret;
  },
};

function ensureString(value) {
  if (typeof value === 'string') return ensureCache.alloc(intArrayFromString(value), HEAP8);
  return value;
}
function ensureInt8(value) {
  if (typeof value === 'object') return ensureCache.alloc(value, HEAP8);
  return value;
}
function ensureInt16(value) {
  if (typeof value === 'object') return ensureCache.alloc(value, HEAP16);
  return value;
}
function ensureInt32(value) {
  if (typeof value === 'object') return ensureCache.alloc(value, HEAP32);
  return value;
}
function ensureFloat32(value) {
  if (typeof value === 'object') return ensureCache.alloc(value, HEAPF32);
  return value;
}
function ensureFloat64(value) {
  if (typeof value === 'object') return ensureCache.alloc(value, HEAPF64);
  return value;
}


// VoidPtr
function VoidPtr() { throw "cannot construct a VoidPtr, no constructor in IDL" }
VoidPtr.prototype = Object.create(WrapperObject.prototype);
VoidPtr.prototype.constructor = VoidPtr;
VoidPtr.prototype.__class__ = VoidPtr;
VoidPtr.__cache__ = {};
Module['VoidPtr'] = VoidPtr;

  VoidPtr.prototype['__destroy__'] = VoidPtr.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_VoidPtr___destroy___0(self);
};
// SampleContainer
function SampleContainer(arg0) {
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  this.ptr = _emscripten_bind_SampleContainer_SampleContainer_1(arg0);
  getCache(SampleContainer)[this.ptr] = this;
};;
SampleContainer.prototype = Object.create(WrapperObject.prototype);
SampleContainer.prototype.constructor = SampleContainer;
SampleContainer.prototype.__class__ = SampleContainer;
SampleContainer.__cache__ = {};
Module['SampleContainer'] = SampleContainer;

SampleContainer.prototype['get'] = SampleContainer.prototype.get = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  return _emscripten_bind_SampleContainer_get_1(self, arg0);
};;

SampleContainer.prototype['set'] = SampleContainer.prototype.set = function(arg0, arg1) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  _emscripten_bind_SampleContainer_set_2(self, arg0, arg1);
};;

SampleContainer.prototype['copyTo'] = SampleContainer.prototype.copyTo = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_SampleContainer_copyTo_1(self, arg0);
};;

SampleContainer.prototype['clear'] = SampleContainer.prototype.clear = function() {
  var self = this.ptr;
  _emscripten_bind_SampleContainer_clear_0(self);
};;

  SampleContainer.prototype['__destroy__'] = SampleContainer.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_SampleContainer___destroy___0(self);
};
// EventContainer
function EventContainer() {
  this.ptr = _emscripten_bind_EventContainer_EventContainer_0();
  getCache(EventContainer)[this.ptr] = this;
};;
EventContainer.prototype = Object.create(WrapperObject.prototype);
EventContainer.prototype.constructor = EventContainer;
EventContainer.prototype.__class__ = EventContainer;
EventContainer.__cache__ = {};
Module['EventContainer'] = EventContainer;

EventContainer.prototype['addNoteOn'] = EventContainer.prototype.addNoteOn = function(arg0, arg1, arg2) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  if (arg2 && typeof arg2 === 'object') arg2 = arg2.ptr;
  _emscripten_bind_EventContainer_addNoteOn_3(self, arg0, arg1, arg2);
};;

EventContainer.prototype['addNoteOff'] = EventContainer.prototype.addNoteOff = function(arg0, arg1, arg2) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  if (arg2 && typeof arg2 === 'object') arg2 = arg2.ptr;
  _emscripten_bind_EventContainer_addNoteOff_3(self, arg0, arg1, arg2);
};;

EventContainer.prototype['getNumEvents'] = EventContainer.prototype.getNumEvents = function() {
  var self = this.ptr;
  return _emscripten_bind_EventContainer_getNumEvents_0(self);
};;

EventContainer.prototype['clear'] = EventContainer.prototype.clear = function() {
  var self = this.ptr;
  _emscripten_bind_EventContainer_clear_0(self);
};;

  EventContainer.prototype['__destroy__'] = EventContainer.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_EventContainer___destroy___0(self);
};
// ByteContainer
function ByteContainer(arg0) {
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  this.ptr = _emscripten_bind_ByteContainer_ByteContainer_1(arg0);
  getCache(ByteContainer)[this.ptr] = this;
};;
ByteContainer.prototype = Object.create(WrapperObject.prototype);
ByteContainer.prototype.constructor = ByteContainer;
ByteContainer.prototype.__class__ = ByteContainer;
ByteContainer.__cache__ = {};
Module['ByteContainer'] = ByteContainer;

ByteContainer.prototype['get'] = ByteContainer.prototype.get = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  return _emscripten_bind_ByteContainer_get_1(self, arg0);
};;

ByteContainer.prototype['set'] = ByteContainer.prototype.set = function(arg0, arg1) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  _emscripten_bind_ByteContainer_set_2(self, arg0, arg1);
};;

ByteContainer.prototype['copyTo'] = ByteContainer.prototype.copyTo = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ByteContainer_copyTo_1(self, arg0);
};;

ByteContainer.prototype['clear'] = ByteContainer.prototype.clear = function() {
  var self = this.ptr;
  _emscripten_bind_ByteContainer_clear_0(self);
};;

  ByteContainer.prototype['__destroy__'] = ByteContainer.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_ByteContainer___destroy___0(self);
};
// HexterJs
function HexterJs() {
  this.ptr = _emscripten_bind_HexterJs_HexterJs_0();
  getCache(HexterJs)[this.ptr] = this;
};;
HexterJs.prototype = Object.create(WrapperObject.prototype);
HexterJs.prototype.constructor = HexterJs;
HexterJs.prototype.__class__ = HexterJs;
HexterJs.__cache__ = {};
Module['HexterJs'] = HexterJs;

HexterJs.prototype['startUp'] = HexterJs.prototype.startUp = function(arg0, arg1) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  _emscripten_bind_HexterJs_startUp_2(self, arg0, arg1);
};;

HexterJs.prototype['tearDown'] = HexterJs.prototype.tearDown = function() {
  var self = this.ptr;
  _emscripten_bind_HexterJs_tearDown_0(self);
};;

HexterJs.prototype['render'] = HexterJs.prototype.render = function(arg0, arg1, arg2) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  if (arg1 && typeof arg1 === 'object') arg1 = arg1.ptr;
  if (arg2 && typeof arg2 === 'object') arg2 = arg2.ptr;
  _emscripten_bind_HexterJs_render_3(self, arg0, arg1, arg2);
};;

HexterJs.prototype['sendSysex'] = HexterJs.prototype.sendSysex = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_HexterJs_sendSysex_1(self, arg0);
};;

HexterJs.prototype['fft'] = HexterJs.prototype.fft = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_HexterJs_fft_1(self, arg0);
};;

HexterJs.prototype['allNotesOff'] = HexterJs.prototype.allNotesOff = function() {
  var self = this.ptr;
  _emscripten_bind_HexterJs_allNotesOff_0(self);
};;

HexterJs.prototype['allVoicesOff'] = HexterJs.prototype.allVoicesOff = function() {
  var self = this.ptr;
  _emscripten_bind_HexterJs_allVoicesOff_0(self);
};;

  HexterJs.prototype['__destroy__'] = HexterJs.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_HexterJs___destroy___0(self);
};
(function() {
  function setupEnums() {
    
  }
  if (Module['calledRun']) setupEnums();
  else addOnPreMain(setupEnums);
})();
