#include <iostream>

///
#include "VideoGlfw.h"
#include "VideoDesc.h"
#include "LowAudio.h"
#include "Common.h"

int main(int argc, char* argv[])
{
	int ret;
	VideoGlfw video_glfw;
	ret = video_glfw.InitVideoGlfw(1920,1080);

	VideoDesc video_desc;
	ret = video_desc.InitVideoDesc(R"(C:\Users\sixhe\Desktop\dayu.mp3)");
	//ret = video_desc.InitVideoDesc(R"(E:\WebD\workdead.mp4)");

	LowAudio low_audio;
	//low_audio.InitAudio(video_desc.GetAudioChannels(), video_desc.GetAudioSampleRate(), (void*)&video_desc);

	VideoData* video_data = new VideoData;
	while (true)
	{
		//ret = video_desc.GetVideoData(video_data);
		//ret = video_glfw.ShowVideo(video_desc.GetVideoWidth(), video_desc.GetVideoHeight(), video_data->_data);
		Sleep(1);
	}

	std::cout << "hello word";
	system("pause");
}