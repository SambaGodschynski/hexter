#include "i_hexter.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sndfile.hh>
#include <limits.h>

typedef unsigned char Byte;
typedef std::vector<Byte> Bytes;
typedef std::vector<Event> Events;
typedef std::vector<float> AudioBuffer;
static const size_t VOICE_BYTES = 164;
static const size_t BLOCK_SIZE = 512; 
static const unsigned long SAMPLE_RATE = 44100;
static const unsigned int MILLIS_TAIL = 3000;

void writeWav(const std::string &path, const AudioBuffer &buffer);

Bytes loadSysex(const std::string &path) {
    std::fstream file(path.c_str(), std::ios::binary | std::ios::in);
    char buffer[VOICE_BYTES];
    file.read (&buffer[0], VOICE_BYTES);
    Bytes result(&buffer[0], &buffer[VOICE_BYTES-1]);
    return result;
}

inline int millisToTicks(int millis)
{
    return (double)(millis * 90.);
}

Events createOnOffEvents(Byte pitch, Byte vel, int millis)
{
    auto ticks = millisToTicks(millis);
    Events result;
    result.emplace_back( Event(Event::NOTEON, 0, pitch, vel) );
    // in the case that the end is < BLOCK_SIZE, will be ignored otherwise
    result.emplace_back( Event(Event::NOTEOFF, ticks, pitch, 0) );
    return result;
}

void render(Hexter &hx, AudioBuffer &buffer, int pitch, int vel, int millis)
{
    int n_blocks = ceil((float)(millis + MILLIS_TAIL)/1000.0f * (float)SAMPLE_RATE);
    int stop = ceil((float)(millis/1000.0f) * (float)SAMPLE_RATE);
    float tmp[BLOCK_SIZE] = {0};
    buffer.reserve(n_blocks);
    Events events = createOnOffEvents(pitch, vel, millis);
    do {
        if (stop<=0 && stop!=INT_MIN) {
            events.emplace_back( Event(Event::NOTEOFF, 0, pitch, 0 ));
            stop = INT_MIN;
        }
        hx.render(&tmp[0], BLOCK_SIZE, events.data(), events.size());
        buffer.insert(buffer.end(), &tmp[0], &tmp[BLOCK_SIZE]);
        events.clear();
        stop -= BLOCK_SIZE;
        n_blocks -= BLOCK_SIZE;
    } while ( n_blocks > 0 );
}

int main(int argc, const char** argv)
{
    if (argc!=6) {
        std::cout<<"usage: "<<std::endl<<"        "
            <<argv[0]
            <<" <dx7 voice sysex file> <out wave file> <pitch> <velocity> <length in millis>"
            <<std::endl;
        return 1;
    }
    std::string path(argv[1]);
    std::string opath(argv[2]);
    int pitch = std::stoi(argv[3]);
    int vel = std::stoi(argv[4]);
    int length = std::stoi(argv[5]);
    AudioBuffer buffer;
    auto syx = loadSysex(path);
    Hexter hexter;
    hexter.startUp(SAMPLE_RATE, BLOCK_SIZE);
    hexter.sendSysex(syx.data(), VOICE_BYTES);
    render(hexter, buffer, pitch, vel, length);
    hexter.tearDown();
    writeWav(opath, buffer);
    return 0;
}


void writeWav(const std::string &path, const AudioBuffer &buffer)
{
    SndfileHandle wav(
			path.c_str(),
			SFM_WRITE,
			SF_FORMAT_WAV | SF_FORMAT_PCM_16,
			1,
			44100 
	);
    wav.writef(buffer.data(), buffer.size());
}

