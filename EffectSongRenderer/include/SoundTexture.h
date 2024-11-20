#pragma once
#include "common.h"
#include <fftw3.h>
#include <vector>
#define SOUND_TEXTURE_LENGTH 1024
#define FFT_SIZE SOUND_TEXTURE_LENGTH * 2

class SoundTexture {
public:
    SoundTexture();
    ~SoundTexture();
    void update(float currentTime, bool isPlay);

    int getChannelCount();

    float getCurrentEnergy() { return m_currentEnergy; }

    bool loadAudioFile(const std::string& filePath);
private:
    bool loadWavFile(const std::string& filePath);
    bool loadMp3File(const std::string& filePath);
    bool loadFlacFile(const std::string& filePath);

    std::vector<float> m_audioSamples;
    size_t m_eachSampleCount = 0;
    size_t m_sampleRate = 0;
    int m_channels = 0;
    float m_totalRunningTime = 0.0f;

    GLuint m_textureID;
    GLuint m_textureID2;

    int m_texBindIdx = 5;
    int m_texBindIdx2 = 6;

    float m_textureBuffer[SOUND_TEXTURE_LENGTH][SOUND_TEXTURE_LENGTH] = { 0.0f };
    //use when stereo
    float m_textureBuffer2[SOUND_TEXTURE_LENGTH][SOUND_TEXTURE_LENGTH] = { 0.0f };

    float m_currentEnergy = 0.0f;

    fftwf_complex* in, * out;
    fftwf_plan plan;
    float blackManWindow[FFT_SIZE];
};
