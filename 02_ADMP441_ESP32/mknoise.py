import wave
import numpy as np

fnames='noise.wav'
fs=44100
timlen=10 #sec

N=fs*timlen
nstat = np.random.RandomState()
factScl = 0.90
streamRand = nstat.randn(N) * factScl
#factNrm = np.max(np.abs(streamRand))
factNrm = 1
streamNrm = streamRand / factNrm
streamPeakSup = np.where( streamNrm < 1.0 , streamNrm , 1.0)
streamPeakSup = np.where( streamPeakSup > -1.0 , streamPeakSup , -1.0)
factFixedPoint = 32767
stream= np.int16(streamPeakSup * factFixedPoint)
objw = wave.Wave_write(fnames)
objw.setnchannels(1)
objw.setsampwidth(2)
objw.setframerate(fs)
objw.writeframes(stream)
objw.close()
