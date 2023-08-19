#define MINIAUDIO_IMPLEMENTATION
#include "LowAudio.h"
#include "VideoDesc.h"

FILE* file_pcm = fopen(R"(C:\Users\sixhe\Desktop\Test.pcm)","rb");

//音频数据回调函数
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    LowAudio* low_audio = (LowAudio*)pDevice->pUserData;
    int size = frameCount * 2 * 2;
    int free_size = low_audio->m_audio_buff.GetBufferUsed();
    if (size > free_size)
    {
        //缓冲区数据不足，禁止获取数据
        low_audio->m_buff_is_ok = false;
        Sleep(1);
    	return;
    }
    if (low_audio->m_buff_is_ok)
    {
        low_audio->m_audio_buff.ReadBuffer((uint8_t*)pOutput, size);
        //fread_s(pOutput, size, 1, size, file_pcm);
    }
}

LowAudio::LowAudio()
{
    m_buff_is_ok = false;
}

LowAudio::~LowAudio()
{
}

int LowAudio::InitAudio(int channels, int sample_rate)
{
    m_channels = channels;
    m_sample_rate = sample_rate;

    //数据缓冲区大小设置为1秒左右的空间大小
    m_audio_buff.InitCircleBuffer(sample_rate * channels * 2);

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

int LowAudio::ThreadData()
{
    uint8_t** data = new uint8_t*;
    int64_t data_size;
    while (true)
    {
        m_low_data_callback(this, data, &data_size);
        while (true != m_audio_buff.WriteBuffer(*data, data_size))
        {
            //缓冲区已满允许回调音频数据
            m_buff_is_ok = true;
            Sleep(1);
        }
    }
    delete data;    
}
