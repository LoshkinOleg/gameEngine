#include "audio_device.h"

#include <assert.h>

#include <SDL2/SDL.h>

void gl::Clip::Load(const char* path)
{
    // TODO: ensure this is a wav file
    SDL_AudioSpec desiredSpecs;
    unsigned int length;
    SDL_AudioSpec* recievedSpecs = SDL_LoadWAV_RW
    (
        SDL_RWFromFile(path, "rb"),
        1,
        &desiredSpecs,
        &data,
        &length
    );
    len = length;
    if (recievedSpecs == nullptr)
    {
        EngineError("Could not read WAV file!");
    }
}
void gl::Clip::Destroy()
{
    SDL_FreeWAV(data);
}
const unsigned char* const gl::Clip::GetData() const
{
    return data;
}
size_t gl::Clip::GetLen() const
{
    return len;
}

size_t gl::AudioBuffer::GetCurrent()const
{
    return current_;
}
void gl::AudioBuffer::SetCurrent(const size_t newCurrent)
{
    current_ = newCurrent;
}
const unsigned char* const gl::AudioBuffer::GetBegin() const
{
    return begin_;
}
void gl::AudioBuffer::RemoveBytesAtBegin(const size_t nrOfBytes)
{
    const size_t nonTransferredData = current_ - nrOfBytes;
    memcpy(begin_, begin_ + nrOfBytes, nonTransferredData);
    current_ = nonTransferredData;
}

void gl::AudioBuffer::AddSound(const unsigned char* const data, const size_t len)
{
    assert(current_ + len <= end_);

    memcpy(begin_ + current_, data, len);
    current_ += len;
}
void gl::AudioBuffer::RemoveSound(size_t len)
{
    // Note: You can only remove the last queued sound this way.
    current_ = (current_ - len) > 0 ? current_ - len : 0;
}
void gl::AudioBuffer::Init()
{
    begin_ = new unsigned char[end_];
}
void gl::AudioBuffer::Destroy()
{
    delete begin_;
}

void gl::AudioDevice::AudioCallback(void* userdata, unsigned char* output, int maxLen)
{
    // TODO: figure out where the crackling comes from at the end of sound. Should I fill the remainder of the buffer with silence?

    AudioBuffer* input = (AudioBuffer*)userdata;
    const size_t len = maxLen;
    const unsigned char* const begin = input->GetBegin();
    const size_t current = input->GetCurrent();

    if (current == 0) // There's no data to write.
    {
        // So just fill the buffer with silence.
        assert(len == gl::NR_OF_SAMPLES * gl::BYTES_PER_SAMPLE);
        constexpr const unsigned char silence[gl::NR_OF_SAMPLES] = { 0 };
        for (size_t i = 0; i < gl::BYTES_PER_SAMPLE; i++)
        {
            SDL_memcpy(output + (i * gl::NR_OF_SAMPLES), &silence, gl::NR_OF_SAMPLES);
        }
        return;
    }

    if (len <= current) // There's more data in the buffer than the device can process.
    {
        SDL_memcpy(output, begin, len); // Send what we can.

        // And move over the rest to the beginning of the buffer for the next callback.
        input->RemoveBytesAtBegin(len);
    }
    else // Just transfer everything.
    {
        SDL_memcpy(output, begin, current);
        input->SetCurrent(0);
    }
}
void gl::AudioDevice::Init()
{
    SDL_AudioSpec deviceSpecs;
    deviceSpecs.freq = gl::DSP_FREQUENCY;
    deviceSpecs.format = gl::AUDIO_FORMAT;
    deviceSpecs.channels = gl::NR_OF_CHANNELS;
    deviceSpecs.samples = gl::NR_OF_SAMPLES;
    deviceSpecs.callback = AudioCallback;
    deviceSpecs.userdata = &buffer_;
    id_ = SDL_OpenAudioDevice(nullptr, false, &deviceSpecs, nullptr, 0);
    if (id_ == 0)
    {
        EngineError(SDL_GetError());
    }
    buffer_.Init();
    Enable();
}
void gl::AudioDevice::Enable()
{
    SDL_PauseAudioDevice(id_, 0);
}
void gl::AudioDevice::Disable()
{
    SDL_PauseAudioDevice(id_, 1);
}
void gl::AudioDevice::PlayClip(const Clip& clip)
{
    buffer_.AddSound(clip.GetData(), clip.GetLen());
}
void gl::AudioDevice::StopClip(const Clip& clip)
{
    buffer_.RemoveSound(clip.GetLen());
}
void gl::AudioDevice::Destroy()
{
    Disable();
    SDL_CloseAudioDevice(id_);
    buffer_.Destroy();
}