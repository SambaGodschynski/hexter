#include "i_hexter.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits.h>

extern "C" {
    #include "wave_io.h"
}

typedef unsigned char Byte;
typedef std::vector<Byte> Bytes;
typedef std::vector<Event> Events;
typedef std::vector<float> AudioBuffer;
static const size_t VOICE_BYTES = 164;
static const size_t BLOCK_SIZE = 44100*2; 
static const unsigned long SAMPLE_RATE = 44100;

void writeWav(const std::string &path, const AudioBuffer &buffer);

Bytes loadSysex(const std::string &path) {
    std::fstream file(path.c_str(), std::ios::binary | std::ios::in);
    char buffer[VOICE_BYTES];
    file.read (&buffer[0], VOICE_BYTES);
    Bytes result(&buffer[0], &buffer[VOICE_BYTES-1]);
    return result;
}


Events createOnOffEvents(Byte pitch, Byte vel, int length)
{
    Events result;
    result.emplace_back( Event(Event::NOTEON, 0, vel, pitch) );
    result.emplace_back( Event(Event::NOTEOFF, length, 0, pitch) );
    return result;
}

void output(const float *data, size_t length) 
{
    std::for_each(&data[0], &data[length], [](float x){
        std::cout<<x<<" ";
    });
}

int main(int argc, const char** argv)
{
    if (argc!=6) {
        std::cout<<"usage: "<<std::endl<<"        "
            <<argv[0]
            <<" <dx7 voice sysex file> <out wave file> <pitch> <velocity> <length in samples>"
            <<std::endl;
        return 1;
    }
    std::string path(argv[1]);
    std::string opath(argv[2]);
    Byte pitch = (Byte)std::stoi(argv[3]);
    Byte vel = (Byte)std::stoi(argv[4]);
    Byte length = (Byte)std::stoi(argv[5]);
    float tmp[BLOCK_SIZE] = {0};
    AudioBuffer buffer(length);
    auto syx = loadSysex(path);
    Hexter hexter;
    hexter.startUp(SAMPLE_RATE, BLOCK_SIZE);
    hexter.sendSysex(syx.data(), VOICE_BYTES);
    Events events = createOnOffEvents(pitch, vel, length);
    hexter.render(&tmp[0], BLOCK_SIZE, events.data(), events.size());
    buffer.insert(buffer.begin(), &tmp[0], &tmp[BLOCK_SIZE]);
    //output(&tmp[0], BLOCK_SIZE);
    hexter.tearDown();
    writeWav(opath, buffer);
    return 0;
}

std::vector<short> castToShort(const AudioBuffer &buffer){
    std::vector<short> res;
    res.reserve(buffer.size());
    for (float x : buffer) {
        res.emplace_back( x * SHRT_MAX );
    }
    return res;
}

void writeWav(const std::string &path, const AudioBuffer &buffer)
{
    Waveheader header = {0};
    header.main_chunk = RIFF;
    header.chunk_type = WAVE;
    header.sub_chunk = FMT;
    header.sc_len = 16;
    header.format = 1;
    header.modus = 1;
    header.sample_fq = SAMPLE_RATE;
    //header.byte_p_sec = 88200;
    header.byte_p_spl = 2;
    header.bit_p_spl = 16;
    auto sdata = castToShort(buffer);
    write_wave(sdata.data(), sdata.size(), (int)SAMPLE_RATE, 
        16, (char*)path.c_str(), &header);
    std::cout<< "wrote " << path << std::endl;
}

