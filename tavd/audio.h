#ifndef __AUDIO_H__
#define __AUDIO_H__

/// audio driver
class Audio
{
  bool AudioInitialize;
public:
  Audio ();
  ~Audio ();

  bool Open (int Voices);
  void Close ();
  void BeginPlay ();
  void FinishPlay ();

  bool StreamInit (dword speed, dword flags);
  void StreamExit (void);
  void Stream_Update (void);
  void StreamSetSpeed (dword speed);
  dword StreamGetPosition (void);
};

#endif