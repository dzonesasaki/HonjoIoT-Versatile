import wave
import numpy as np

fnamel='dat01.txt' #change this file name

fnames= fnamel[0:-3]+'wav'
fs=44100

strmOrg=np.loadtxt(fnamel)
N=len(strmOrg)
timlen=N/fs #sec

factScl = 0.90
myRms = np.sqrt(np.mean(strmOrg*strmOrg))
myMax = np.max(np.abs(strmOrg))

#streamNrm = strmOrg / myRms * factScl
streamNrm = strmOrg / myMax * factScl

streamPeakSup = np.where( streamNrm < 1.0 , streamNrm , 1.0)
streamPeakSup = np.where( streamPeakSup > -1.0 , streamPeakSup , -1.0)
streamOut = streamPeakSup

scaleFixedPoint = 32767
stream= np.int16(streamOut * scaleFixedPoint)

objw = wave.Wave_write(fnames)
objw.setnchannels(1)
objw.setsampwidth(2)
objw.setframerate(fs)
objw.writeframes(stream)
objw.close()

#from playsound import playsound
#playsound(fnames)
