#include <stdio.h>
#include "musicPlayer.h"

static FILE *file = NULL;

typedef struct {
    char ChunkID[4];
    unsigned int ChunkSize;
    char Format[4];
    char Subchunk1ID[4];
    unsigned int Subchunk1Size;
    unsigned short AudioFormat;
    unsigned short NumChannels;
    unsigned int SampleRate;
    unsigned int ByteRate;
    unsigned short BlockAlign;
    unsigned short BitsPerSample;
    char Subchunk2ID[4];
    unsigned int Subchunk2Size;
} WavHeader;

void parseWaveFile(char *audioPath, call_back_data *audioData)
{
    // open the wav file
    file = fopen(audioPath, "r");
    if (!file) {
        printf("Error: could not open read in wav file");
        return;
    }
    WavHeader header;
    fread(&header, sizeof(header), 1, file);
     if (header.AudioFormat != 1) {
        printf("Error: only PCM audio format is supported\n");
        return ;
    }
    
    if (header.BitsPerSample != 16) {
        printf("Error: only 16-bit audio is supported\n");
        return;
    }
    

    audioData->offset = 0;
    audioData->numSamples = header.Subchunk2Size / header.BlockAlign;
    audioData->sampleRate = header.SampleRate;
    audioData->numChannels = header.NumChannels;


    // The PCM data in a wave file starts after the header:
	const int PCM_DATA_OFFSET = 44;
    // Get file size
    fseek(file, 0, SEEK_END);
	long int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
	audioData->numSamples  = (sizeInBytes / header.BitsPerSample) * 8;

    audioData->file = malloc(sizeInBytes); 
    fseek(file, sizeof(header), SEEK_SET);
    fread(audioData->file, sizeInBytes, 1, file);
    

    printf("audio format: %d\n", header.AudioFormat);
    printf("bit depth: %d\n", header.BitsPerSample);
    printf("Number of samples: %lu\n", audioData->numSamples);
    printf("Sample rate: %d\n",audioData->sampleRate);
    printf("Number of channels: %d\n", header.NumChannels);

}

void closeFile(call_back_data *audioData)
{
    free(audioData->file);
    fclose(file);
}