#include "Voice.h"
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>

//constructor need not do anything
WavVoice::WavVoice() : words(nullptr), framesPerSyllable(nullptr) {}


//deletes words array
WavVoice::~WavVoice()
{
	if (words != nullptr) {
		for (int i = 0; i < nSyllables; i++) {
			delete words[i];
		}
		delete words;
	}

	if (framesPerSyllable != nullptr) {
		delete framesPerSyllable;
	}
}

//adds a file, creates the wavs and then converts them to buffers
void WavVoice::addFile(char *path)
{
	/*std::string line;
	std::ifstream file(path);
	if (file.is_open()) {
		while (std::getline(file,line)) {
			OutputDebugString(line.c_str());
			const char *cLine = line.c_str();
			char syllable[10];
			int i = 0;
			const char *countLine = cLine;
			while (*countLine != '\n' && *countLine != EOF) {
				if (*countLine == '-') {
					syllable[i] = 0;
					i = 0;
					OutputDebugString(syllable);
					//putSpeechToFile(syllable);
				}
				else {
					syllable[i] = *countLine;
					i++;
				}
				countLine++;
			}
			OutputDebugString("here");
			line = "";
			//putSpeechToFile(syllable);
		}
	}
	file.close();*/
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find('-', prev);
		if (pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		char *outFile = new char[59];
		putSpeechToFile(token.c_str(), outFile);
		filePaths.push_back(outFile);

		prev = pos + 1;
	} while (pos < str.length() && prev < str.length());
	
	//do buffer stuff
	nSyllables = filePaths.size();//set number of syllables
	words = new double*[nSyllables];
	framesPerSyllable = new int[nSyllables];
	int i = 0;
	for (std::vector<char*>::iterator it = filePaths.begin(); it != filePaths.end(); ++it) {
		SF_INFO sf_info;
		SNDFILE *file = sf_open(*it, SFM_READ, &sf_info);
		framesPerSyllable[i] = sf_info.frames;
		words[i] = new double[sf_info.frames];
		sf_readf_double(file, words[i], sf_info.frames);//read frames into buffer
		sf_close(file);
		i++;
		delete *it; //check for horrible memory error!!!
	}
	

}

//gets frame from words neatly
double WavVoice::getFrames(unsigned int syllable, unsigned int frame)
{
	if(syllable >= nSyllables) {
		return 0.0;
	}
	else {
		if (frame >= framesPerSyllable[syllable]) {
			return 0.0;
		}
		else {
			return (words[syllable])[frame];
		}
	}
}


//takes some text and makes it text to speech in a file
void WavVoice::putSpeechToFile(const char *text, char* out)
{
	char *str = new char[200];
	char *str2 = new char[12];
	itoa(GetTickCount(), str2, 10);
	char str3[59];
	strcpy(str3, "c:/Users/charl/Documents/vox/tmp/");
	strcat(str3, text);
	strcat(str3, str2);
	strcpy(str, "c:/users/charl/Documents/vox/tts.exe -f 7 -v 1 -o "); //i hate myself
	strcat(str, str3);
	strcat(str, ".wav ");
	strcat(str, text);
	OutputDebugString(str);
	system(str);
	delete str2;
	delete str;
	strcat(str3, "0.wav");//silly tts program
	strcpy(out, str3);
	
}
