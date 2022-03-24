#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <sndfile.h>
#include <math.h>


#define PI 3.14159265

// Declare function
void fir_filter(float* x, float* a, float* xt, float* res);
void iir_filter(float* x, float* a, float* b, float* xt, float* res);



//define large array out side of main to avoid crashing stack 
float sig_in[32*960000];
float sig_out[32*960000+2];
float xt[32*960000+2];
float delay[32*960000];

int main(int argc, char *argv[])
{
	int ii;

	//Require 2 arguments: input file and output file
	if(argc < 2)
	{
		printf("Not enough arguments \n");
		return -1;
	}

	SF_INFO sndInfo;
	SNDFILE *sndFile = sf_open(argv[1], SFM_READ, &sndInfo);
	if (sndFile == NULL) {
		fprintf(stderr, "Error reading source file '%s': %s\n", argv[1], sf_strerror(sndFile));
		return 1;
	}

	SF_INFO sndInfoOut = sndInfo;
	sndInfoOut.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	sndInfoOut.channels = 1;
	sndInfoOut.samplerate = sndInfo.samplerate;
	SNDFILE *sndFileOut = sf_open(argv[2], SFM_WRITE, &sndInfoOut);

	// Check format - 16bit PCM
	if (sndInfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		fprintf(stderr, "Input should be 16bit Wav\n");
		sf_close(sndFile);
		return 1;
	}

	// Check channels - mono
	if (sndInfo.channels != 1) {
		fprintf(stderr, "Wrong number of channels\n");
		sf_close(sndFile);
		return 1;
	}

	// The following code is a only a template, please rewrite
	
	float *buffer = malloc(sizeof(double));
	if (buffer == NULL) {
		fprintf(stderr, "Could not allocate memory for file\n");
		sf_close(sndFile);
		return 1;
	}

	for(ii=0; ii < sndInfo.frames; ii++){
		sf_readf_float(sndFile, buffer, 1);
		sig_in[ii] = *buffer;
	}
	
	// Used for FIR
	float h[15] = {0.0736, 0.1009, 0.1273, 0.1514, 0.1717, 0.1871, 0.1967, -0.9, 0.1967, 0.1871, 0.1717, 0.1514, 0.1273, 0.1009, 0.0736};
	fir_filter(sig_in, h, xt, sig_out);
	for(int i = 0; i < 960000+14; i++){
		sf_writef_float(sndFileOut, &sig_out[i], 1);
	}
	
	//Used for IIR
	float a[3] = {1, -1.8831, 0.9801};
	float b[3] = {1, -1.9021, 1};
	iir_filter(sig_in, b, a, xt, sig_out);
	for(int i = 0; i < 960000+3; i++){
		sf_writef_float(sndFileOut, &sig_out[i], 1);
	}
	
	

	sf_close(sndFile);
	sf_write_sync(sndFileOut);
	sf_close(sndFileOut);
	free(buffer);

	return 1;
}


// helper
void generate_signal(float L, float fs, float f, float* s){

	for(int i = 0; i < L; ++i){
        s[i] = sin(2*3.1415926*(float)i*f/fs);
	}

}

void iir_filter(float* x, float* a, float* b, float* xt, float* res) {
	
	for(int i = 0; i <2; ++i){
		xt[i] = 0;
	}
	for(int j = 2; j<32*960000+2; j++){
		xt[j] = x[j-2];
	}
	for(int k = 2; k<32*960000+2; k++){
		res[k] = a[0]*xt[k]+a[1]*xt[k-1]+a[2]*xt[k-2]-b[1]*res[k-1]-b[2]*res[k-2];
		res[k] = res[k]/b[0];
	}

}

void fir_filter(float* x, float* a, float* xt, float* res){
	for(int i = 0; i <14; ++i){
		xt[i] = 0;
	}
	for(int j = 14; j<32*960000+4; j++){
		xt[j] = x[j-14];
	}
	for(int k = 14; k<32*960000+4; k++){
		res[k] = a[0]*xt[k]+a[1]*xt[k-1]+a[2]*xt[k-2]+a[3]*xt[k-3]+a[4]*xt[k-4];
		res[k] = a[5]*xt[k-5]+a[6]*xt[k-6]+a[7]*xt[k-7]+a[8]*xt[k-8]+a[9]*xt[k-9];
		res[k] = a[10]*xt[k-10]+a[11]*xt[k-11]+a[12]*xt[k-12]+a[13]*xt[k-13]+a[14]*xt[k-14];
	
	}

}
