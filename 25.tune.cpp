#include <iostream>
#include <cmath>

#include <string.h>
#include <stdio.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define M_PI 3.14159265358979323846
#define MAX_FRAME_LENGTH 8192

struct PITCH_CONTEXT
{
	float gInFIFO[MAX_FRAME_LENGTH];
	float gOutFIFO[MAX_FRAME_LENGTH];
	float gFFTworksp[2 * MAX_FRAME_LENGTH];
	float gLastPhase[MAX_FRAME_LENGTH / 2 + 1];
	float gSumPhase[MAX_FRAME_LENGTH / 2 + 1];
	float gOutputAccum[2 * MAX_FRAME_LENGTH];
	float gAnaFreq[MAX_FRAME_LENGTH];
	float gAnaMagn[MAX_FRAME_LENGTH];
	float gSynFreq[MAX_FRAME_LENGTH];
	float gSynMagn[MAX_FRAME_LENGTH];
	long gRover;
};

/*
	FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
	Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
	time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
	and returns the cosine and sine parts in an interleaved manner, ie.
	fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
	must be a power of 2. It expects a complex input signal (see footnote 2),
	ie. when working with 'common' audio signals our input signal has to be
	passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
	of the frequencies of interest is in fftBuffer[0...fftFrameSize].
*/
void smbFft(float *fftBuffer, long fftFrameSize, long sign)
{
	float wr, wi, arg, *p1, *p2, temp;
	float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
	long i, j, le, le2, k;

	for (i = 2; i < 2 * fftFrameSize - 2; i += 2)
	{
		for (long bitm = 2, j = 0; bitm < 2 * fftFrameSize; bitm <<= 1)
		{
			if (i & bitm)
				j++;
			j <<= 1;
		}
		if (i < j)
		{
			p1 = fftBuffer + i;
			p2 = fftBuffer + j;
			temp = *p1;
			*(p1++) = *p2;
			*(p2++) = temp;
			temp = *p1;
			*p1 = *p2;
			*p2 = temp;
		}
	}
	for (k = 0, le = 2; k < (long)(log(fftFrameSize) / log(2.) + .5); k++)
	{
		le <<= 1;
		le2 = le >> 1;
		ur = 1.0;
		ui = 0.0;
		arg = M_PI / (le2 >> 1);
		wr = cos(arg);
		wi = sign * sin(arg);
		for (j = 0; j < le2; j += 2)
		{
			p1r = fftBuffer + j;
			p1i = p1r + 1;
			p2r = p1r + le2;
			p2i = p2r + 1;
			for (i = j; i < 2 * fftFrameSize; i += le)
			{
				tr = *p2r * ur - *p2i * ui;
				ti = *p2r * ui + *p2i * ur;
				*p2r = *p1r - tr;
				*p2i = *p1i - ti;
				*p1r += tr;
				*p1i += ti;
				p1r += le;
				p1i += le;
				p2r += le;
				p2i += le;
			}
			tr = ur * wr - ui * wi;
			ui = ur * wi + ui * wr;
			ur = tr;
		}
	}
}

/*
	12/12/02, smb
	PLEASE NOTE:
	There have been some reports on domain errors when the atan2() function was used
	as in the above code. Usually, a domain error should not interrupt the program flow
	(maybe except in Debug mode) but rather be handled "silently" and a global variable
	should be set according to this error. However, on some occasions people ran into
	this kind of scenario, so a replacement atan2() function is provided here.

	If you are experiencing domain errors and your program stops, simply replace all
	instances of atan2() with calls to the smbAtan2() function below.
*/
double smbAtan2(double x, double y)
{
	if (x == 0.0)
		return 0.0;

	double signx = -1.0;
	if (x > 0.0)
		signx = 1.0;

	if (y == 0.0)
		return signx * M_PI / 2.0;

	return atan2(x, y);
}

void InitPitchContext(PITCH_CONTEXT *ctx)
{
	memset(ctx->gInFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
	memset(ctx->gOutFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
	memset(ctx->gFFTworksp, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
	memset(ctx->gLastPhase, 0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
	memset(ctx->gSumPhase, 0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
	memset(ctx->gOutputAccum, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
	memset(ctx->gAnaFreq, 0, MAX_FRAME_LENGTH * sizeof(float));
	memset(ctx->gAnaMagn, 0, MAX_FRAME_LENGTH * sizeof(float));
	ctx->gRover = 0;
}

/*
	Routine smbPitchShift(). See top of file for explanation
	Purpose: doing pitch shifting while maintaining duration using the Short
	Time Fourier Transform.
	Author: (c)1999-2009 Stephan M. Bernsee <smb [AT] dspdimension [DOT] com>
*/
void smbPitchShift(PITCH_CONTEXT *ctx, float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata)
{
	double magn, phase, tmp, window, real, imag;
	double freqPerBin, expct;
	long i, k, qpd, index, inFifoLatency, stepSize, fftFrameSize2;

	/* set up some handy variables */
	fftFrameSize2 = fftFrameSize / 2;
	stepSize = fftFrameSize / osamp;
	freqPerBin = sampleRate / (double)fftFrameSize;
	expct = 2. * M_PI * (double)stepSize / (double)fftFrameSize;
	inFifoLatency = fftFrameSize - stepSize;
	if (ctx->gRover == 0)
		ctx->gRover = inFifoLatency;

	/* main processing loop */
	for (i = 0; i < numSampsToProcess; i++)
	{

		/* As long as we have not yet collected enough data just read in */
		ctx->gInFIFO[ctx->gRover] = indata[i];
		outdata[i] = ctx->gOutFIFO[ctx->gRover - inFifoLatency];
		ctx->gRover++;

		/* now we have enough data for processing */
		if (ctx->gRover >= fftFrameSize)
		{
			ctx->gRover = inFifoLatency;

			/* do windowing and re,im interleave */
			for (k = 0; k < fftFrameSize; k++)
			{
				window = -.5 * cos(2. * M_PI * (double)k / (double)fftFrameSize) + .5;
				ctx->gFFTworksp[2 * k] = ctx->gInFIFO[k] * window;
				ctx->gFFTworksp[2 * k + 1] = 0.;
			}

			/* ***************** ANALYSIS ******************* */
			/* do transform */
			smbFft(ctx->gFFTworksp, fftFrameSize, -1);

			/* this is the analysis step */
			for (k = 0; k <= fftFrameSize2; k++)
			{
				/* de-interlace FFT buffer */
				real = ctx->gFFTworksp[2 * k];
				imag = ctx->gFFTworksp[2 * k + 1];

				/* compute magnitude and phase */
				magn = 2. * sqrt(real * real + imag * imag);
				phase = atan2(imag, real);

				/* compute phase difference */
				tmp = phase - ctx->gLastPhase[k];
				ctx->gLastPhase[k] = phase;

				/* subtract expected phase difference */
				tmp -= (double)k * expct;

				/* map delta phase into +/- Pi interval */
				qpd = tmp / M_PI;
				if (qpd >= 0)
					qpd += qpd & 1;
				else
					qpd -= qpd & 1;
				tmp -= M_PI * (double)qpd;

				/* get deviation from bin frequency from the +/- Pi interval */
				tmp = osamp * tmp / (2. * M_PI);

				/* compute the k-th partials' true frequency */
				tmp = (double)k * freqPerBin + tmp * freqPerBin;

				/* store magnitude and true frequency in analysis arrays */
				ctx->gAnaMagn[k] = magn;
				ctx->gAnaFreq[k] = tmp;
			}

			/* ***************** PROCESSING ******************* */
			/* this does the actual pitch shifting */
			memset(ctx->gSynMagn, 0, fftFrameSize * sizeof(float));
			memset(ctx->gSynFreq, 0, fftFrameSize * sizeof(float));
			for (k = 0; k <= fftFrameSize2; k++)
			{
				index = k * pitchShift;
				if (index <= fftFrameSize2)
				{
					ctx->gSynMagn[index] += ctx->gAnaMagn[k];
					ctx->gSynFreq[index] = ctx->gAnaFreq[k] * pitchShift;
				}
			}

			/* ***************** SYNTHESIS ******************* */
			/* this is the synthesis step */
			for (k = 0; k <= fftFrameSize2; k++)
			{

				/* get magnitude and true frequency from synthesis arrays */
				magn = ctx->gSynMagn[k];
				tmp = ctx->gSynFreq[k];

				/* subtract bin mid frequency */
				tmp -= (double)k * freqPerBin;

				/* get bin deviation from freq deviation */
				tmp /= freqPerBin;

				/* take osamp into account */
				tmp = 2. * M_PI * tmp / osamp;

				/* add the overlap phase advance back in */
				tmp += (double)k * expct;

				/* accumulate delta phase to get bin phase */
				ctx->gSumPhase[k] += tmp;
				phase = ctx->gSumPhase[k];

				/* get real and imag part and re-interleave */
				ctx->gFFTworksp[2 * k] = magn * cos(phase);
				ctx->gFFTworksp[2 * k + 1] = magn * sin(phase);
			}

			/* zero negative frequencies */
			for (k = fftFrameSize + 2; k < 2 * fftFrameSize; k++)
				ctx->gFFTworksp[k] = 0.;

			/* do inverse transform */
			smbFft(ctx->gFFTworksp, fftFrameSize, 1);

			/* do windowing and add to output accumulator */
			for (k = 0; k < fftFrameSize; k++)
			{
				window = -.5 * cos(2. * M_PI * (double)k / (double)fftFrameSize) + .5;
				ctx->gOutputAccum[k] += 2. * window * ctx->gFFTworksp[2 * k] / (fftFrameSize2 * osamp);
			}
			for (k = 0; k < stepSize; k++)
				ctx->gOutFIFO[k] = ctx->gOutputAccum[k];

			/* shift accumulator */
			memmove(ctx->gOutputAccum, ctx->gOutputAccum + stepSize, fftFrameSize * sizeof(float));

			/* move input FIFO */
			for (k = 0; k < inFifoLatency; k++)
				ctx->gInFIFO[k] = ctx->gInFIFO[k + stepSize];
		}
	}
}
/*
class CWavData;

bool OpenWavFile(const char *filename, CWavData &wavData);
bool WriteWavFile(const char *filename, const CWavData &wavData);
*/
class CWavData
{
	friend bool OpenWavFile(const char *filename, CWavData &wavData);
	friend bool WriteWavFile(const char *filename, const CWavData &wavData);

public:
	CWavData()
	{
		m_pDataBuf = NULL;
		m_totalSamples = 0;
		m_sampleRate = 0;
		m_channels = 0;
	}
	CWavData(drwav *wav)
	{
		InitFromDrWave(wav);
	}
	CWavData(size_t totalSamples, int sampleRate, int channels)
	{
		size_t totalSize = totalSamples * sizeof(float) * channels;
		m_pDataBuf = new float[totalSize];
		if (m_pDataBuf != NULL)
		{
			memset(m_pDataBuf, 0, totalSize);
			m_totalSamples = totalSamples;
			m_sampleRate = sampleRate;
			m_channels = channels;
		}
	}
	virtual ~CWavData()
	{
		Release();
	}
	bool IsValid() const { return (m_pDataBuf != NULL); }

	void InitFromDrWave(drwav *wav)
	{
		assert(m_pDataBuf == NULL);

		size_t totalSize = (size_t)wav->totalPCMFrameCount * sizeof(float) * wav->channels;
		m_pDataBuf = new float[totalSize];
		if (m_pDataBuf != NULL)
		{
			memset(m_pDataBuf, 0, totalSize);
			m_totalSamples = (size_t)wav->totalPCMFrameCount;
			m_sampleRate = wav->sampleRate;
			m_channels = wav->channels;
		}
	}
	// channel 0: first channel, 1: second channel
	size_t ReadFramess(size_t pos, size_t framesToRead, int channel, float *outBuf, size_t bufSzie)
	{
		if ((m_pDataBuf == NULL) || ((framesToRead * sizeof(float)) > bufSzie))
			return 0;

		size_t thisRead = (m_totalSamples - pos) > framesToRead ? framesToRead : (m_totalSamples - pos);
		float *pStart = m_pDataBuf + (pos * m_channels);
		if (m_channels == 1)
		{
			memcpy(outBuf, pStart, thisRead * sizeof(float));
		}
		else
		{
			for (size_t i = 0; i < thisRead; i++)
			{
				*outBuf++ = *(pStart + channel);
				pStart += m_channels;
			}
		}

		return thisRead;
	}
	size_t WriteFramess(size_t pos, size_t framesToWrite, int channel, float *inBuf)
	{
		if (m_pDataBuf == NULL)
			return 0;

		size_t thisWrite = (m_totalSamples - pos) > framesToWrite ? framesToWrite : (m_totalSamples - pos);
		float *pStart = m_pDataBuf + (pos * m_channels);
		if (m_channels == 1)
		{
			memcpy(pStart, inBuf, thisWrite * sizeof(float));
		}
		else
		{
			for (size_t i = 0; i < thisWrite; i++)
			{
				*(pStart + channel) = *inBuf++;
				pStart += m_channels;
			}
		}

		return thisWrite;
	}

	void Release()
	{
		if (m_pDataBuf != NULL)
		{
			delete[] m_pDataBuf;
		}
		m_sampleRate = 0;
		m_channels = 0;
		m_totalSamples = 0;
	}
	size_t GetTotalSamples() const { return m_totalSamples; }
	int GetSampleRate() const { return m_sampleRate; }
	int GetBitsPerSample() const { return 32; }
	int GetChannels() const { return m_channels; }
	const float *GetRawBuffer() const { return m_pDataBuf; }
	float *GetRawBuffer() { return m_pDataBuf; }

protected:
	float *m_pDataBuf;
	int m_sampleRate;
	int m_channels;
	size_t m_totalSamples;
};

bool OpenWavFile(const char *filename, CWavData &wavData)
{
	drwav wav;
	if (!drwav_init_file(&wav, filename, NULL))
		return false;

	wavData.InitFromDrWave(&wav);
	drwav_uint64 readSamples = drwav_read_pcm_frames_f32(&wav, wavData.GetTotalSamples(), wavData.GetRawBuffer());
	drwav_uninit(&wav);
	return (readSamples == wavData.GetTotalSamples());
}

bool WriteWavFile(const char *filename, const CWavData &wavData)
{
	drwav_data_format format;
	format.container = drwav_container_riff;   // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
	format.format = DR_WAVE_FORMAT_IEEE_FLOAT; // <-- Any of the DR_WAVE_FORMAT_* codes.
	format.channels = wavData.GetChannels();
	format.sampleRate = (drwav_uint32)wavData.GetSampleRate();
	format.bitsPerSample = wavData.GetBitsPerSample();

	drwav wav;
	if (drwav_init_file_write(&wav, filename, &format, NULL))
	{
		drwav_uint64 samplesWritten = drwav_write_pcm_frames(&wav, wavData.GetTotalSamples(), wavData.GetRawBuffer());
		drwav_uninit(&wav);
		if (samplesWritten == wavData.GetTotalSamples())
		{
			return true;
		}
	}

	return false;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		std::cout << "usage: tune.exe infile.wav outfile.wav" << std::endl;
		return 1;
	}

	int numChannels = 1;
	size_t bufferLengthFrames = MAX_FRAME_LENGTH;
	int ret = 0;
	long semitones = 8;										   // shift up by 3 semitones
	float pitchShift = (float)std::pow(2.0, semitones / 12.0); // convert semitones to factor

	CWavData inWav;
	bool bLoaded = OpenWavFile(argv[1], inWav);

	CWavData outWav(inWav.GetTotalSamples(), inWav.GetSampleRate(), inWav.GetChannels());

	PITCH_CONTEXT pitch_ctx_left, pitch_ctx_right;
	InitPitchContext(&pitch_ctx_left);
	InitPitchContext(&pitch_ctx_right);

	int nSampleRate = inWav.GetSampleRate();
	float *DataBuf = new (std::nothrow) float[bufferLengthFrames];
	if (DataBuf != nullptr)
	{
		size_t thisPos = 0;
		while (true)
		{
			size_t framesRead = inWav.ReadFramess(thisPos, bufferLengthFrames, 0, DataBuf, bufferLengthFrames * sizeof(float));
			// framesRead = inWav.ReadFramess(thisPos, bufferLengthFrames, 1, DataBuf, bufferLengthFrames * sizeof(float));
			if (framesRead > 0)
			{
				smbPitchShift(&pitch_ctx_left, pitchShift, framesRead, 2048, 4, nSampleRate, DataBuf, DataBuf);
				size_t framesWritten = outWav.WriteFramess(thisPos, framesRead, 0, DataBuf);
				if (framesWritten != framesRead)
				{
					std::cout << "fail to write out data!" << std::endl;
					break;
				}

				thisPos += framesRead;
			}
			if (framesRead < bufferLengthFrames)
			{
				std::cout << "trans format successfully!" << std::endl;
				WriteWavFile(argv[2], outWav);
				break;
			}
		}

		delete[] DataBuf;
	}

	return 0;
}
