#pragma once

#include "defines.h"

namespace gl
{
    class Clip
    {
    public:
        void Load(const char* path); // This should be called by the ResourceManager
        void Destroy(); // So should this.
        const unsigned char* const GetData() const;
        size_t GetLen() const;
    private:
        unsigned char* data = nullptr;
        size_t len = 0;
    };

    class AudioBuffer
    {
    public:
        size_t GetCurrent()const;
        void SetCurrent(const size_t newCurrent);
        const unsigned char* const GetBegin() const;
        void RemoveBytesAtBegin(const size_t nrOfBytes);

        void AddSound(const unsigned char* const data, const size_t len);
        void RemoveSound(size_t len);
        void Init();
        void Destroy();
    private:
        unsigned char* begin_ = nullptr;
        size_t end_ = gl::BUFFER_SIZE_IN_BYTES;
        size_t current_ = 0;
    };

    class AudioDevice
    {
    public:
        static void AudioCallback(void* userdata, unsigned char* output, int maxLen);
        void Init();
        void Enable();
        void Disable();
        void PlayClip(const Clip& clip);
        void StopClip(const Clip& clip);
        void Destroy();
    private:
        AudioBuffer buffer_;
        unsigned int id_ = 0;
    };
}//!gl