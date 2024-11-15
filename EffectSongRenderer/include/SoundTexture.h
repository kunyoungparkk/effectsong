#pragma once
#include "common.h"
#include <vector>
#define SOUND_TEXTURE_WIDTH 512
#define SOUND_TEXTURE_HEIGHT 512

class SoundTexture {
public:
    SoundTexture();
    ~SoundTexture();
    void update(float currentTime);
    void bind(int texIdx, int texIdx2);
    bool loadWavFile(const std::string& filePath);

    int getChannelCount();
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

    //TODO: FFTW ºÐ¼®
    //fftw_complex* in, * out;
    //fftw_plan plan;
};
