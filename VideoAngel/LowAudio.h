#ifndef _LOW_AUDIO_
#define _LOW_AUDIO_

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <miniaudio.h>
#include <string>
//#include "CircleBuffer.h"
#include "RingBuffer.h"

class LowAudio;
typedef int (*GetAudioDataFun)(LowAudio* hadle, uint8_t** data, int64_t* size);

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

	std::thread m_data_thread;

public:

	//CircleBuffer m_audio_buff = {};
	RingBuffer m_audio_buff;

	bool m_buff_is_ok;

	/// @brief 用户指定指针数据，回调使用
	void* m_user_data;
	GetAudioDataFun m_low_data_callback;

private:
	int ThreadData();

public:
	int InitAudio(int channels, int sample_rate);

	void Start();

	std::shared_ptr<spdlog::logger> m_logger;

};
#endif // !LOW_AUDIO