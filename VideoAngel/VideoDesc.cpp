#include "VideoDesc.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

VideoDesc::VideoDesc()
{
	m_logger = spdlog::stdout_color_mt("VideoDesc");
	m_logger->info("VideoDesc初始化");
}

VideoDesc::~VideoDesc()
{
}

int VideoDesc::InitVideoDesc(std::string source_url)
{
	m_source_url = source_url;

	if (m_source_url.empty())
	{
		m_logger->error("数据源地址为空，返回值-1");
		return -1;
	}

	//设置优化参数，对视频源设置固有属性
	av_dict_set(&m_av_dictionary, "rtsp_transpot", "+udp+tcp", 0);
	av_dict_set(&m_av_dictionary, "rtsp_flage", "+prefer_tcp", 0);
	av_dict_set(&m_av_dictionary, "buffer_size", "10240000", 0);
	av_dict_set(&m_av_dictionary, "max_delay", "500", 0);
	av_dict_set(&m_av_dictionary, "stimeout", "2000", 0);

	int ret = 0;

	//打开数据源地址
	ret = avformat_open_input(&m_av_format_context, m_source_url.c_str(), nullptr, &m_av_dictionary);
	if (ret < 0)
	{
		m_logger->error(std::string("数据源地址打开失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
		return -2;
	}

	m_logger->info(std::string("数据源地址:") + m_source_url);

	//查找视频源的流信息
	ret = avformat_find_stream_info(m_av_format_context, nullptr);
	if (ret < 0)
	{
		m_logger->error(std::string("查找视频源流信息失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
		return -3;
	}

	//获取视频流索引
	for (int i = 0; i < m_av_format_context->nb_streams; i++)
	{
		if (m_av_format_context->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO)
		{
			m_video_index = i;
			m_logger->info(std::string("获取到视频流数据:index=") + std::to_string(i));
			continue;
		}
		if (m_av_format_context->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO)
		{
			m_audio_index = i;
			m_logger->info(std::string("获取到音频流数据:index=") + std::to_string(i));
			continue;
		}
	}

	//寻找视频流解码器
	if (m_video_index != -1)
	{
		m_av_video_codec = avcodec_find_decoder(m_av_format_context->streams[m_video_index]->codecpar->codec_id);
		if (m_av_video_codec == nullptr)
		{
			m_logger->warn("寻找视频流解码器失败,不存在视频流");
		}
		else
		{
			//初始化视频解码器上下文
			m_av_video_code_context = avcodec_alloc_context3(m_av_video_codec);

			//将视频流信息绑定
			ret = avcodec_parameters_to_context(m_av_video_code_context, m_av_format_context->streams[m_video_index]->codecpar);
			if (ret < 0)
			{
				m_logger->error(std::string("绑定视频流数据失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
				return -4;
			}

			//打开视频解码器
			ret = avcodec_open2(m_av_video_code_context, m_av_video_codec, nullptr);
			if (ret < 0)
			{
				m_logger->error(std::string("打开视频解码器失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
				return -5;
			}

			//设置视频转换参数
			m_video_sws_context = sws_getContext(
				m_av_video_code_context->width,
				m_av_video_code_context->height,
				m_av_video_code_context->pix_fmt,
				m_av_video_code_context->width,
				m_av_video_code_context->height,
				AV_PIX_FMT_BGR32,
				SWS_BILINEAR,
				nullptr, nullptr, nullptr);
		}
		m_video_width = m_av_video_code_context->width;
		m_video_height = m_av_video_code_context->height;
		m_video_fps = av_q2d(m_av_format_context->streams[m_video_index]->avg_frame_rate);
	}
	//寻找音频流解码器
	if (m_audio_index != -1)
	{
		m_av_audio_codec = avcodec_find_decoder(m_av_format_context->streams[m_audio_index]->codecpar->codec_id);
		if (m_av_audio_codec == nullptr)
		{
			m_logger->warn("寻找音频流解码器失败,不存在音频流");
		}
		else
		{
			//初始化音频解码上下文
			m_av_audio_code_context = avcodec_alloc_context3(m_av_audio_codec);

			//将音频流信息进行绑定
			ret = avcodec_parameters_to_context(m_av_audio_code_context, m_av_format_context->streams[m_audio_index]->codecpar);
			if (ret < 0)
			{
				m_logger->error(std::string("绑定音频流数据信息失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
				return -4;
			}

			//打开音频解码器
			ret = avcodec_open2(m_av_audio_code_context, m_av_audio_codec, nullptr);
			if (ret < 0)
			{
				m_logger->error(std::string("打开音频解码器失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
				return -5;
			}

			//设置音频转换参数,使用被弃用的swr_alloc_set_opts时转码的数据会异常
			//m_audio_swr_context = swr_alloc();
			 ret = swr_alloc_set_opts2(
			     &m_audio_swr_context,
			     &m_av_audio_code_context->ch_layout,
			     AVSampleFormat::AV_SAMPLE_FMT_S16,
			     m_av_audio_code_context->sample_rate,
			     &m_av_audio_code_context->ch_layout,
			     m_av_audio_code_context->sample_fmt,
			     m_av_audio_code_context->sample_rate,
			     0, nullptr);
			if (ret != 0)
			{
				m_logger->error(std::string("设置音频转化参数失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
				return -6;
			}

			ret = swr_init(m_audio_swr_context);
			if (ret != 0)
			{
				m_logger->error(std::string("初始化音频转化参数失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
				return -7;
			}
		}
		m_audio_channels = m_av_audio_code_context->channels;
		m_audio_sample_rate = m_av_audio_code_context->sample_rate;
	}
	m_av_packet = av_packet_alloc();
	m_av_video_frame_dest = av_frame_alloc();
	m_av_audio_frame_dest = av_frame_alloc();


	decode_thread = std::thread(&VideoDesc::ThreadDesc, this);
	decode_thread.detach();
	return 0;
}

int VideoDesc::GetVideoData(VideoData* video_data)
{
	if (m_video_frame_queue.size() <= 0)
	{
		m_logger->info("视频帧队列数据为空");
		Sleep(2);
		return -1;
	}

	AVFrame* frame = m_video_frame_queue.front();

	//将pts转换为时间毫秒
	video_data->pts = 1000 * frame->pts * av_q2d(m_av_format_context->streams[m_video_index]->time_base);

	if (m_audio_frame_queue.size() > 0)
	{
		int _pts_ = video_data->pts - 1000 * m_audio_frame_queue.front()->pts * av_q2d(m_av_format_context->streams[m_audio_index]->time_base) + 1000;
		//进行数据同步
		if (_pts_ > 0 && _pts_ < 2000)
		{
			//m_logger->info("睡眠时间：" + std::to_string(_pts_));
			Sleep(_pts_);
		}
	}
	else
	{
		Sleep(1000 / m_video_fps);
	}
	int ret;
	//int video_size = av_image_get_buffer_size(AV_PIX_FMT_BGR32, m_video_width, m_video_height, 4);
	int video_size = m_video_height * m_video_width * 4;
	if (video_size <= 0)
	{
		m_logger->warn(std::string("获取图像缓冲大小失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, video_size));
		ret = -6;
		goto END;
	}
	if (video_data->size != video_size)
	{
		if (video_data->_data)
		{
			delete[] video_data->_data;
			video_data->_data = nullptr;
		}
		video_data->size = video_size;
	}
	//初始化图像缓冲内存块
	if (video_data->_data == nullptr)
	{
		video_data->_data = new uint8_t[video_data->size];
		memset(video_data->_data, 0, video_data->size);
	}

	//初始化目的帧绑定数据缓冲
	ret = av_image_fill_arrays(m_av_video_frame_dest->data, m_av_video_frame_dest->linesize, video_data->_data, AV_PIX_FMT_BGR32, m_video_width, m_video_height, 4);
	if (ret < 0)
	{
		m_logger->warn(std::string("绑定目的帧数据缓冲失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
		ret = -4;
		goto END;
	}
	//视频格式数据转换，转码为RGB32
	ret = sws_scale(m_video_sws_context, frame->data, frame->linesize, 0, m_video_height, m_av_video_frame_dest->data, m_av_video_frame_dest->linesize);
	if (ret < 0)
	{
		m_logger->warn(std::string("视频格式数据转换,转码为RGB32失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
		ret = -5;
		goto END;
	}
	ret = 0;
END:
	m_video_frame_queue.pop();
	//av_frame_unref(frame);
	av_frame_free(&frame);
	//stbi_write_png("E:\\Test.png", m_video_width, m_video_height, 4, video_data->_data, 0);
	return ret;
}
FILE* file = fopen("E:\\Test.pcm", "wb+");
int VideoDesc::GetAudioData(AudioData* audio_data)
{
	if (m_audio_frame_queue.size() <= 0)
	{
		m_logger->info("音频帧队列数据为空");
		Sleep(2);
		return -1;
	}
	AVFrame* frame = m_audio_frame_queue.front();
	//计算输出音频数据的有效长度
	int ret;
	int audio_size;
	audio_size = av_samples_get_buffer_size(NULL, frame->ch_layout.nb_channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
	if (audio_size <= 0)
	{
		m_logger->warn(std::string("获取音频缓冲大小失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, audio_size));
		ret = -6;
		goto END;
	}

	//初始化音频缓冲内存块
	if (audio_data->size != audio_size)
	{
		if (audio_data->_data)
		{
			delete[] audio_data->_data;
			audio_data->_data = nullptr;
		}
		audio_data->size = audio_size;
	}
	if (audio_data->_data == nullptr)
	{
		audio_data->_data = new uint8_t[audio_data->size];
		memset(audio_data->_data, 0, audio_data->size);
	}

	//audio_data->size = av_samples_alloc(&audio_data->_data, NULL, m_av_frame_src->ch_layout.nb_channels, m_av_frame_src->nb_samples, AV_SAMPLE_FMT_S16, 1);

	//将pts转换为时间毫秒
	audio_data->pts = 1000 * frame->pts * av_q2d(m_av_format_context->streams[m_audio_index]->time_base);

	//绑定填充要转换的目的帧
	av_samples_fill_arrays(m_av_audio_frame_dest->data, m_av_audio_frame_dest->linesize, audio_data->_data, frame->ch_layout.nb_channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);

	//音频编码转换将数据取出
	ret = swr_convert(m_audio_swr_context, m_av_audio_frame_dest->data, frame->nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
	if (ret < 0)
	{
		m_logger->warn(std::string("音频数据转化失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
		ret = -5;
		goto END;
	}
	//fwrite(audio_data->_data, 1, audio_data->size, file);
	ret = 0;
END:
	m_audio_frame_queue.pop();
	//av_frame_unref(frame);
	av_frame_free(&frame);
	return ret;
}

int VideoDesc::GetVideoWidth()
{
	return m_video_width;
}

int VideoDesc::GetVideoHeight()
{
	return m_video_height;
}

int VideoDesc::GetAudioChannels()
{
	return m_audio_channels;
}

int VideoDesc::GetAudioSampleRate()
{
	return m_audio_sample_rate;
}

int VideoDesc::AVDecode()
{
	//开始读取数据源的数据包
	int ret = av_read_frame(m_av_format_context, m_av_packet);
	if (ret < 0)
	{
		m_logger->warn(std::string("读取数据包失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
		//ret = -1;
		goto END;
	}
	//解码视频数据包
	if (m_av_packet->stream_index == m_video_index)
	{
		AVFrame* frame = av_frame_alloc();
		//发送数据包到视频解码器上下文
		ret = avcodec_send_packet(m_av_video_code_context, m_av_packet);
		if (ret < 0)
		{
			m_logger->warn(std::string("发送数据包到视频解码器上下文失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
			ret = -2;
			goto END;
		}

		//获取到图片可用数据
		ret = avcodec_receive_frame(m_av_video_code_context, frame);
		if (ret != 0)
		{
			m_logger->warn(std::string("接收视频解码器输出的数据帧不可用 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
			ret = -3;
			goto END;
		}

		m_video_frame_queue.push(frame);
	}
	else if (m_av_packet->stream_index == m_audio_index)
	{
		AVFrame* frame = av_frame_alloc();
		//发送数据包到音频解码器上下文
		ret = avcodec_send_packet(m_av_audio_code_context, m_av_packet);
		if (ret < 0)
		{
			m_logger->warn(std::string("发送数据包到音频解码器上下文失败 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
			ret = -2;
			goto END;
		}
		//获取到图片可用数据
		ret = avcodec_receive_frame(m_av_audio_code_context, frame);
		if (ret != 0)
		{
			m_logger->warn(std::string("接收音频解码器输出的数据帧不可用 ") + av_make_error_string(m_av_errbuff, AV_ERROR_MAX_STRING_SIZE, ret));
			ret = -3;
			goto END;
		}
		m_audio_frame_queue.push(frame);
	}
END:
	av_packet_unref(m_av_packet);
	return ret;
}

void VideoDesc::ThreadDesc()
{
	while (true)
	{
		if (m_audio_frame_queue.size() > 100 && m_video_frame_queue.size() > 100)
		{
			Sleep(20);
		}
		if (AVDecode() == AVERROR_EOF)
		{
			return;
		}
	}
}
