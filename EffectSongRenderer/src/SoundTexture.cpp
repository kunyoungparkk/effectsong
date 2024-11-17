#include "SoundTexture.h"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

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

	//TODO: just fix it?
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	if (m_channels == 2) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, m_textureID2);
	}
	else {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}

	// FFT 메모리와 계획 설정
	in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * SOUND_TEXTURE_WIDTH * 2);
	out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * SOUND_TEXTURE_WIDTH * 2);
	plan = fftwf_plan_dft_1d(SOUND_TEXTURE_WIDTH * 2, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
}

SoundTexture::~SoundTexture() {
	//fftw_destroy_plan(plan);
	//fftw_free(in);
	//fftw_free(out);
	glDeleteTextures(1, &m_textureID);
	glDeleteTextures(1, &m_textureID2);
}

void SoundTexture::update(float currentTime) {
	m_currentEnergy = 0.0f;
	float newRowData[2][SOUND_TEXTURE_WIDTH];
	int samplePosition = (int)(currentTime * m_sampleRate);

	//음악이 끝났을 경우 (파싱된 데이터가 없을 경우 포함)
	if (samplePosition + SOUND_TEXTURE_WIDTH * 2 > m_totalRunningTime * m_sampleRate)
	{
		return;
	}

	if (m_channels == 1) {
		for (int i = samplePosition; i < SOUND_TEXTURE_WIDTH * 2; i++) {
			in[i][0] = m_audioSamples[i];
			in[i][1] = 0.0;
		}
		fftwf_execute(plan);
		for (int i = 0; i < SOUND_TEXTURE_WIDTH; i++) {
			float amplitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
			newRowData[0][i] = powf(glm::log(1.0 + amplitude / 40.0f), 0.3f);
			m_currentEnergy += amplitude;
		}
	}
	else if (m_channels == 2) {
		if (samplePosition % 2 == 1) {
			samplePosition -= 1;

		}

		//left
		for (int i = 0; i < SOUND_TEXTURE_WIDTH * 2; i++) {
			in[i][0] = m_audioSamples[samplePosition + i * 2];
			in[i][1] = 0.0;
		}
		fftwf_execute(plan);
		for (int i = 0; i < SOUND_TEXTURE_WIDTH; i++) {
			float amplitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
			newRowData[0][i] = glm::min(1.0f, powf(glm::log(1.0 + amplitude / 40.0f), 0.3f));
			m_currentEnergy += amplitude;

		}
		//right
		for (int i = 0; i < SOUND_TEXTURE_WIDTH * 2; i++) {
			in[i][0] = m_audioSamples[samplePosition + 1 + i * 2];
			in[i][1] = 0.0;
		}
		fftwf_execute(plan);
		for (int i = 0; i < SOUND_TEXTURE_WIDTH; i++) {
			float amplitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
			newRowData[1][i] = glm::min(1.0f, powf(glm::log(1.0 + amplitude / 40.0f), 0.3f));
			m_currentEnergy += amplitude;
		}
		m_currentEnergy /= 2.0f;
	}


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
