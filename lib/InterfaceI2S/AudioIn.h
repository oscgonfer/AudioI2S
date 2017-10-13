
#ifndef _AUDIO_IN_H_INCLUDED
#define _AUDIO_IN_H_INCLUDED

#include <stddef.h>

class AudioAnalyser;

class AudioIn
{
public:
  AudioIn();
  virtual ~AudioIn();

  virtual long sampleRate() = 0;
  virtual int bitsPerSample() = 0;
  virtual int channels() = 0;
  virtual int datasize() = 0;

protected:
  friend class AudioAnalyser;

  void samplesRead(void* buffer, size_t size);

protected:
  virtual int begin() = 0;

  virtual int read(void* buffer, size_t size) = 0;
  virtual int reset() = 0;
  virtual void end() = 0;

protected:
  friend class AudioAnalyser;

  int setAnalyser(AudioAnalyser* analyser);

private:
  AudioAnalyser* _analyser;
};

#endif