#include "SoundTexture.h"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
//#include <fftw3.h>

SoundTexture::SoundTexture() {
    glGenTextures(1, &m_textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SOUND_TEXTURE_WIDTH, SOUND_TEXTURE_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &m_textureID2);
    glBindTexture(GL_TEXTURE_2D, m_textureID2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SOUND_TEXTURE_WIDTH, SOUND_TEXTURE_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // FFT 메모리와 계획 설정
   /* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * SOUND_TEXTURE_WIDTH);
    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * SOUND_TEXTURE_WIDTH);
    plan = fftw_plan_dft_1d(SOUND_TEXTURE_WIDTH, in, out, FFTW_FORWARD, FFTW_ESTIMATE);*/
}

SoundTexture::~SoundTexture() {
    //fftw_destroy_plan(plan);
    //fftw_free(in);
    //fftw_free(out);
    glDeleteTextures(1, &m_textureID);
    glDeleteTextures(1, &m_textureID2);
}

void SoundTexture::update(float currentTime) {
    float newRowData[2][SOUND_TEXTURE_WIDTH];
    /*60FPS 고정 가정*/
    //현재 샘플 위치
    int samplePosition = (int)(currentTime * m_sampleRate);
    //합칠 샘플 개수
    int sumSampleCount = FIXED_DELTATIME * m_sampleRate;
    int maxSampleIdx = samplePosition + sumSampleCount;
    if (maxSampleIdx > m_sampleRate * m_totalRunningTime) {
        maxSampleIdx = (int)(m_sampleRate * m_totalRunningTime);
        sumSampleCount = maxSampleIdx - samplePosition;
    }
    if (m_channels == 1) {
        float sum = 0.0f;
        for (int i = samplePosition; i < maxSampleIdx; i++) {
            sum += m_audioSamples[i];
        }
        int fillCount = (int)(sum / sumSampleCount * SOUND_TEXTURE_WIDTH);
        for (int i = 0; i < fillCount; i++) {
            newRowData[0][i] = 1.0f;
        }
        for (int i = fillCount; i < SOUND_TEXTURE_WIDTH; i++) {
            newRowData[0][i] = 0.0f;
        }
    }
    else if (m_channels == 2) {
        float sumLeft = 0.0f;
        float sumRight = 0.0f;

        samplePosition *= 2;
        maxSampleIdx *= 2;

        for (int i = samplePosition; i < maxSampleIdx; i+=2) {
            sumLeft += glm::abs(m_audioSamples[i]);
            sumRight += glm::abs(m_audioSamples[i + 1]);
        }
        int fillCount = (int)(sumLeft / sumSampleCount * SOUND_TEXTURE_WIDTH);
        for (int i = 0; i < fillCount; i++) {
            newRowData[0][i] = 1.0f;
        }
        for (int i = fillCount; i < SOUND_TEXTURE_WIDTH; i++) {
            newRowData[0][i] = 0.0f;
        }
        
        fillCount = (int)(sumRight / sumSampleCount * SOUND_TEXTURE_WIDTH);
        for (int i = 0; i < fillCount; i++) {
            newRowData[1][i] = 1.0f;
        }
        for (int i = fillCount; i < SOUND_TEXTURE_WIDTH; i++) {
            newRowData[1][i] = 0.0f;
        }
    }

    //TODO
    //// FFT 계산 준비
    //for (int i = 0; i < SOUND_TEXTURE_WIDTH; i++) {
    //    in[i][0] = audioSamples[i];
    //    in[i][1] = 0.0;
    //}
    //fftw_execute(plan);
    //// 진폭 계산 및 newRowData에 저장
    //for (int i = 0; i < SOUND_TEXTURE_WIDTH; i++) {
    //    newRowData[i] = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
    //}

    memmove(m_textureBuffer[1], m_textureBuffer[0], (SOUND_TEXTURE_HEIGHT - 1) * SOUND_TEXTURE_WIDTH * sizeof(float));
    memcpy(m_textureBuffer[0], newRowData[0], SOUND_TEXTURE_WIDTH * sizeof(float));
    glActiveTexture(GL_TEXTURE0 + m_texBindIdx);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SOUND_TEXTURE_WIDTH, SOUND_TEXTURE_HEIGHT, GL_RED, GL_FLOAT, m_textureBuffer);

    if (m_channels == 2) {
        memmove(m_textureBuffer2[1], m_textureBuffer2[0], (SOUND_TEXTURE_HEIGHT - 1) * SOUND_TEXTURE_WIDTH * sizeof(float));
        memcpy(m_textureBuffer2[0], newRowData[1], SOUND_TEXTURE_WIDTH * sizeof(float));
        glActiveTexture(GL_TEXTURE0 + m_texBindIdx2);
        glBindTexture(GL_TEXTURE_2D, m_textureID2);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SOUND_TEXTURE_WIDTH, SOUND_TEXTURE_HEIGHT, GL_RED, GL_FLOAT, m_textureBuffer2);
    }
}

void SoundTexture::bind(int texIdx, int texIdx2) {
    m_texBindIdx = texIdx;
    m_texBindIdx2 = texIdx2;

    glActiveTexture(GL_TEXTURE0 + m_texBindIdx);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    if (m_channels == 2) {
        glActiveTexture(GL_TEXTURE0 + m_texBindIdx2);
        glBindTexture(GL_TEXTURE_2D, m_textureID2);
    }
    else {
        glActiveTexture(GL_TEXTURE0 + m_texBindIdx2);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }
}

bool SoundTexture::loadWavFile(const std::string& filePath)
{
    // WAV 파일 열기
    drwav wav;
    if (!drwav_init_file(&wav, filePath.c_str(), nullptr)) {
        std::cerr << "Failed to open WAV file: " << filePath << std::endl;
        return false;
    }

    // 샘플 개수 계산 및 버퍼 준비
    m_channels = wav.channels;
    if (m_channels > 2) {
        std::cerr << "max channel count is 2 " << std::endl;
        return false;
    }
    m_eachSampleCount = wav.totalPCMFrameCount;
    m_sampleRate = wav.sampleRate;
    m_totalRunningTime = (float)m_eachSampleCount / m_sampleRate;

    size_t totalSampleCount = m_eachSampleCount * m_channels;
    m_audioSamples.resize(totalSampleCount);

    // PCM 데이터 읽기 (float 포맷으로 변환)
    drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, m_audioSamples.data());

    // WAV 파일 닫기
    drwav_uninit(&wav);

    return true;
}

int SoundTexture::getChannelCount()
{
    return m_channels;
}
