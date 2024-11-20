#include "SoundTexture.h"
#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#include "dr_wav.h"
#include "dr_mp3.h"
#include "dr_flac.h"
#include <algorithm>

SoundTexture::SoundTexture() {
	glGenTextures(1, &m_textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SOUND_TEXTURE_LENGTH, SOUND_TEXTURE_LENGTH, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &m_textureID2);
	glBindTexture(GL_TEXTURE_2D, m_textureID2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SOUND_TEXTURE_LENGTH, SOUND_TEXTURE_LENGTH, 0, GL_RED, GL_FLOAT, nullptr);
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
	in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * FFT_SIZE);
	out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * FFT_SIZE);
	plan = fftwf_plan_dft_1d(FFT_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	//blackman window 초기화
	constexpr float a0 = 0.42f, a1 = 0.5f, a2 = 0.08f;
	for (size_t n = 0; n < FFT_SIZE; ++n) {
		float w = a0 - a1 * cos(2.0f * glm::pi<float>() * n / FFT_SIZE) + a2 * cos(4.0f * glm::pi<float>() * n / FFT_SIZE);
		blackManWindow[n] = w;
	}
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
	float newRowData[2][SOUND_TEXTURE_LENGTH] = { 0.0f, };
	int samplePosition = (int)(currentTime * m_sampleRate * m_channels);
	//정규화 시 기준 (WebAudio ref)
	float minMagnitude = -100.0f;
	float maxMagnitude = -30.0f;
	float smoothingFactor = 0.8f;

	if (samplePosition % 2 == 1) {
		samplePosition -= 1;
	}

	if (samplePosition + FFT_SIZE * m_channels < m_audioSamples.size())
	{
		if (m_channels == 1) {
			for (int i = samplePosition; i < FFT_SIZE; i++) {
				in[i][0] = m_audioSamples[i] * blackManWindow[i];
				in[i][1] = 0.0;
			}
			fftwf_execute(plan);
			for (int i = 0; i < SOUND_TEXTURE_LENGTH; i++) {
				float magnitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
				magnitude /= FFT_SIZE;
				magnitude = 20.0f * glm::log(magnitude + 1e-8f) / glm::log(10.0f);
				magnitude = glm::clamp(magnitude, minMagnitude, maxMagnitude);
				magnitude = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
				magnitude = (1.0f - smoothingFactor) * magnitude + smoothingFactor * m_textureBuffer[0][i];

				newRowData[0][i] = magnitude;
				m_currentEnergy += magnitude;
			}
		}
		else if (m_channels == 2) {
			//left
			for (int i = 0; i < FFT_SIZE; i++) {
				in[i][0] = m_audioSamples[samplePosition + i * 2] * blackManWindow[i];
				in[i][1] = 0.0;
			}
			fftwf_execute(plan);
			for (int i = 0; i < SOUND_TEXTURE_LENGTH; i++) {
				float magnitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
				magnitude /= FFT_SIZE;
				//std::cout << magnitude << std::endl;
				magnitude = 20.0f * glm::log(magnitude + 1e-8f) / glm::log(10.0f);
				magnitude = glm::clamp(magnitude, minMagnitude, maxMagnitude);
				magnitude = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
				magnitude = (1.0f - smoothingFactor) * magnitude + smoothingFactor * m_textureBuffer[0][i];

				newRowData[0][i] = magnitude;
				m_currentEnergy += magnitude;
			}

			//right
			for (int i = 0; i < FFT_SIZE; i++) {
				in[i][0] = m_audioSamples[samplePosition + i * 2 + 1] * blackManWindow[i];
				in[i][1] = 0.0;
			}
			fftwf_execute(plan);
			for (int i = 0; i < SOUND_TEXTURE_LENGTH; i++) {
				float magnitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
				magnitude /= FFT_SIZE;
				magnitude = 20.0f * glm::log(magnitude + 1e-8f) / glm::log(10.0f);
				magnitude = glm::clamp(magnitude, minMagnitude, maxMagnitude);
				magnitude = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
				magnitude = (1.0f - smoothingFactor) * magnitude + smoothingFactor * m_textureBuffer2[0][i];
				if (magnitude > 0.3f) {
					int a = 3;
				}
				newRowData[1][i] = magnitude;
				m_currentEnergy += magnitude;
			}
			m_currentEnergy /= 2.0f;
		}
		m_currentEnergy /= (float)SOUND_TEXTURE_LENGTH;
	}

	memmove(m_textureBuffer[1], m_textureBuffer[0], (SOUND_TEXTURE_LENGTH - 1) * SOUND_TEXTURE_LENGTH * sizeof(float));
	memcpy(m_textureBuffer[0], newRowData[0], SOUND_TEXTURE_LENGTH * sizeof(float));
	glActiveTexture(GL_TEXTURE0 + m_texBindIdx);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SOUND_TEXTURE_LENGTH, SOUND_TEXTURE_LENGTH, GL_RED, GL_FLOAT, m_textureBuffer);

	if (m_channels == 2) {
		memmove(m_textureBuffer2[1], m_textureBuffer2[0], (SOUND_TEXTURE_LENGTH - 1) * SOUND_TEXTURE_LENGTH * sizeof(float));
		memcpy(m_textureBuffer2[0], newRowData[1], SOUND_TEXTURE_LENGTH * sizeof(float));
		glActiveTexture(GL_TEXTURE0 + m_texBindIdx2);
		glBindTexture(GL_TEXTURE_2D, m_textureID2);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SOUND_TEXTURE_LENGTH, SOUND_TEXTURE_LENGTH, GL_RED, GL_FLOAT, m_textureBuffer2);
	}
}

bool SoundTexture::loadAudioFile(const std::string& filePath)
{
	std::string ext = filePath.substr(filePath.find_last_of('.') + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if (ext == "wav") {
		return loadWavFile(filePath);
	}
	else if (ext == "mp3") {
		return loadMp3File(filePath);
	}
	else if (ext == "flac") {
		return loadFlacFile(filePath);
	}
	else {
		std::cerr << "Unsupported file format: " << ext << std::endl;
		return false;
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

bool SoundTexture::loadMp3File(const std::string& filePath)
{
	drmp3 mp3;
	if (!drmp3_init_file(&mp3, filePath.c_str(), nullptr)) {
		std::cerr << "Failed to open MP3 file: " << filePath << std::endl;
		return false;
	}

	m_channels = mp3.channels;
	if (m_channels > 2) {
		std::cerr << "max channel count is 2 " << std::endl;
		return false;
	}
	m_sampleRate = mp3.sampleRate;

	drmp3_uint64 totalPCMFrameCount = drmp3_get_pcm_frame_count(&mp3);

	std::vector<float> tempBuffer(totalPCMFrameCount * m_channels);

	m_eachSampleCount = totalPCMFrameCount;
	m_totalRunningTime = static_cast<float>(m_eachSampleCount) / m_sampleRate;

	drmp3_read_pcm_frames_f32(&mp3, totalPCMFrameCount, tempBuffer.data());
	drmp3_uninit(&mp3);

	m_audioSamples = std::move(tempBuffer);

	return true;
}

bool SoundTexture::loadFlacFile(const std::string& filePath)
{
	drflac* flac = drflac_open_file(filePath.c_str(), nullptr);
	if (!flac) {
		std::cerr << "Failed to open FLAC file: " << filePath << std::endl;
		return false;
	}

	m_channels = flac->channels;
	if (m_channels > 2) {
		std::cerr << "max channel count is 2 " << std::endl;
		return false;
	}
	m_sampleRate = flac->sampleRate;

	std::vector<float> tempBuffer(flac->totalPCMFrameCount * m_channels);

	drflac_read_pcm_frames_f32(flac, flac->totalPCMFrameCount, tempBuffer.data());
	drflac_close(flac);

	m_eachSampleCount = tempBuffer.size() / m_channels;
	m_totalRunningTime = static_cast<float>(m_eachSampleCount) / m_sampleRate;

	m_audioSamples = std::move(tempBuffer);

	return true;
}

int SoundTexture::getChannelCount()
{
	return m_channels;
}
