#pragma once

#ifndef __OPEN_AI_TRANSCRIPTION_DATA__
#define __OPEN_AI_TRANSCRIPTION_DATA__

#include <string>

class TranslationData {
public:
	std::string filePath; // formats: mp3, mp4, mpeg, mpga, m4a, wav, or webm.
	std::string model = "whisper-1"; // eg. whisper-1
	std::string prompt;
	std::string response_format = "json"; // options: json, text, srt, verbose_json, or vtt.
	double temperature; // between 0 and 1

	long requestTimeOutInSecond = 10L;

	TranslationData() {}
};

class TranscriptionData : public TranslationData {
public:

	std::string language;

	TranscriptionData()
	{
		language = "en"; // follow the rule of ISO-639-1
	}
};

#endif // !__OPEN_AI_TRANSCRIPTION_DATA__
