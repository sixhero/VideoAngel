#ifndef _VIDEO_DESC_
#define _VIDEO_DESC_
//日志库
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

#include <queue>
//#include <s>

#include "Common.h"

class VideoDesc
{
public:
	VideoDesc();
	~VideoDesc();

private:
	/// @brief 快速日志句柄
	std::shared_ptr<spdlog::logger> m_logger;

	/// @brief 数据源的地址,外部提供
	std::string m_source_url;

	/// @brief 上下文的类型字典，数据源类型格式等
	AVDictionary* m_av_dictionary = nullptr;

	/// @brief FFmpeg的全局上下文
	AVFormatContext* m_av_format_context = nullptr;

	/// @brief 视频解码器上下文
	AVCodecContext* m_av_video_code_context = nullptr;

	/// @brief 音频解码器上下文
	AVCodecContext* m_av_audio_code_context = nullptr;

	/// @brief 具体的视频解码器
	const AVCodec* m_av_video_codec = nullptr;

	/// @brief 具体的音频解码器
	const AVCodec* m_av_audio_codec = nullptr;

	/// @brief 视频流索引值
	int8_t m_video_index = -1;

	/// @brief 音频流索引值
	int8_t m_audio_index = -1;

	/// @brief 视频帧转换参数
	SwsContext* m_video_sws_context = nullptr;

	/// @brief 音频帧转换参数
	SwrContext* m_audio_swr_context = nullptr;

	/// @brief 视频尺寸宽度
	int m_video_width = 0;

	/// @brief 视频尺寸高度
	int m_video_height = 0;

	/// @brief FFmpeg库函数出错时存放的错误信息
	char  m_av_errbuff[AV_ERROR_MAX_STRING_SIZE] = { 0 };

	/// @brief 流数据包
	AVPacket* m_av_packet = nullptr;

	///// @brief 视频源数据帧
	//AVFrame* m_av_frame_src = nullptr;

	/// @brief 指定格式转换后的目的数据帧
	AVFrame* m_av_frame_dest = nullptr;

	//音频数据的通道数
	int m_audio_channels;

	//音频数据的采样码率
	int m_audio_sample_rate;

	/// @brief 存放视频帧
	std::queue<AVFrame*> m_video_frame_queue;

	/// @brief 存放音频帧
	std::queue<AVFrame*> m_audio_frame_queue;

	/// @brief 解码线程
	std::thread decode_thread;

private:
	/// @brief 解包函数
	/// @return 
	int AVDecode();

	/// @brief 解包线程函数
	void ThreadDesc();

public:

	/// @brief 打开源地址并初始化解码器
	/// @param source_url 源地址
	/// @return 
	int InitVideoDesc(std::string source_url);

	int GetVideoData(VideoData *video_data);

	int GetAudioData(AudioData* audio_data);

	int GetVideoWidth();

	int GetVideoHeight();

	int GetAudioChannels();

	int GetAudioSampleRate();


};

#endif // !_VIDEO_DESC_
