#define MINIAUDIO_IMPLEMENTATION
#include "LowAudio.h"
#include "VideoDesc.h"

//音频数据回调函数
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    VideoDesc* video_desc = (VideoDesc*)pDevice->pUserData;
    static AudioData audio_data;

    video_desc->GetAudioData(&audio_data);

    memcpy(pOutput, audio_data._data, audio_data.size);
}

LowAudio::LowAudio()
{
}

LowAudio::~LowAudio()
{
}

int LowAudio::InitAudio(int channels, int sample_rate,void * user_data)
{
    m_channels = channels;
    m_sample_rate = sample_rate;

    m_decoder.outputFormat = ma_format::ma_format_s16;
    m_decoder.outputChannels = m_channels;
    m_decoder.outputSampleRate = m_sample_rate;

    m_device_conf = ma_device_config_init(ma_device_type_playback);
    m_device_conf.playback.format = m_decoder.outputFormat;
    m_device_conf.playback.channels = m_decoder.outputChannels;
    m_device_conf.sampleRate = m_decoder.outputSampleRate;
    m_device_conf.dataCallback = data_callback;
    m_device_conf.pUserData = user_data;

    if (ma_device_init(NULL, &m_device_conf, &m_device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&m_decoder);
        return -3;
    }

    if (ma_device_start(&m_device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&m_device);
        ma_decoder_uninit(&m_decoder);
        return -4;
    }
    //ma_device_uninit(&m_device);
    return 0;
}
