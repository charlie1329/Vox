#pragma once

#include<sndfile.h>
#include<vector>

class WavVoice {
public:
	WavVoice();
	~WavVoice();
	void addFile(char *path);
	double getFrames(unsigned int word, unsigned int frame);
	int *framesPerSyllable;
	int nSyllables;

private:
	std::vector<char*> filePaths;
	double **words;
	void putSpeechToFile(const char *text, char *out);
};