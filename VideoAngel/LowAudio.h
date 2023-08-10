#ifndef _LOW_AUDIO_
#define _LOW_AUDIO_

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <miniaudio.h>
#include <string>

class LowAudio
{
public:
	LowAudio();
	~LowAudio();
private:
	ma_decoder m_decoder;
	ma_device_config m_device_conf;
	ma_device m_device;

	int m_channels;
	int m_sample_rate;


public:
	int InitAudio(int channels, int sample_rate, void* user_data);

};
#endif // !LOW_AUDIO