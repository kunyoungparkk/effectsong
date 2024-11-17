#pragma once
#include "common.h"
#include <fftw3.h>
#include <vector>
#define SOUND_TEXTURE_WIDTH 1024
#define SOUND_TEXTURE_HEIGHT 1024

class SoundTexture {
public:
    SoundTexture();
    ~SoundTexture();
    void update(float currentTime);
    bool loadWavFile(const std::string& filePath);

    int getChannelCount();

    float getCurrentEnergy() { return m_currentEnergy; }
private:
    std::vector<float> m_audioSamples;
    size_t m_eachSampleCount = 0;
    size_t m_sampleRate = 0;
    int m_channels = 0;
    float m_totalRunningTime = 0.0f;

    GLuint m_textureID;
    GLuint m_textureID2;

    int m_texBindIdx = 0;
    int m_texBindIdx2 = 0;

    float m_textureBuffer[SOUND_TEXTURE_HEIGHT][SOUND_TEXTURE_WIDTH] = { 0.0f };
    //use when stereo
    float m_textureBuffer2[SOUND_TEXTURE_HEIGHT][SOUND_TEXTURE_WIDTH] = { 0.0f };

    float m_currentEnergy = 0.0f;

    fftwf_complex* in, * out;
    fftwf_plan plan;
};
