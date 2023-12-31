#define MINIAUDIO_IMPLEMENTATION
#include "LowAudio.h"
#include "VideoDesc.h"
#include <iostream>
#include <chrono>

FILE* file_low = fopen("E:\\Test3.pcm", "wb+");

//音频数据回调函数
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    LowAudio* low_audio = (LowAudio*)pDevice->pUserData;
    int size = frameCount * 2 * 2;
    int use_size = low_audio->m_audio_buff.get_used_size();
    if (size > use_size)
    {
        //缓冲区数据不足，禁止获取数据
        low_audio->m_logger->info("缓冲区数据不足，禁止获取数据");
        low_audio->m_buff_is_ok = false;
        Sleep(10);
    	return;
    }
    if (low_audio->m_buff_is_ok)
    {
        low_audio->m_audio_buff.read_data((uint8_t*)pOutput, size);
        //fwrite(pOutput, 1, size, file_low);
    }
}

LowAudio::LowAudio()
{
    m_buff_is_ok = false;
    m_logger = spdlog::stdout_color_mt("LowAudio");
    m_logger->info("初始化！！！");
}

LowAudio::~LowAudio()
{
}

int LowAudio::InitAudio(int channels, int sample_rate)
{
    m_channels = channels;
    m_sample_rate = sample_rate;

    //数据缓冲区大小设置为1秒左右的空间大小
    m_audio_buff.Init(sample_rate * channels * 2);

    m_decoder.outputFormat = ma_format::ma_format_s16;
    m_decoder.outputChannels = m_channels;
    m_decoder.outputSampleRate = m_sample_rate;

    m_device_conf = ma_device_config_init(ma_device_type_playback);
    m_device_conf.playback.format = m_decoder.outputFormat;
    m_device_conf.playback.channels = m_decoder.outputChannels;
    m_device_conf.sampleRate = m_decoder.outputSampleRate;
    m_device_conf.dataCallback = data_callback;
    m_device_conf.pUserData = this;

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

void LowAudio::Start()
{
    m_data_thread = std::thread(&LowAudio::ThreadData, this);
    m_data_thread.detach();
}
FILE* file_low_audio = fopen("E:\\Test2.pcm", "wb+");
int LowAudio::ThreadData()
{
    uint8_t** data = new uint8_t*;
    int64_t data_size = 0;
    uint32_t write_size;
    while (true)
    {
        if (m_low_data_callback(this, data, &data_size) == 0)
        {
            //fwrite(*data, 1, data_size, file_low_audio);
            write_size = 0;

            while (data_size != (write_size += m_audio_buff.write_data(*data+write_size, data_size-write_size)))
            {
                m_buff_is_ok = true;
                Sleep(10);
            }
        }
    }
    delete data;    
}
