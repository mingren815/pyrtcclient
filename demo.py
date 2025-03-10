from rtc_client import RtcClient
import time
import tqdm
import pdb
sample_rate = 48000
channel = 2
bytePerSample = 2
duration_time_s = 0.1
url = "https://cd.nice2meet.cn:9810"

ak = "demo_access"
sk = "demo_secret"
roomid = '13350050355'
token = ""



client = RtcClient()
time.sleep(2)

##ret = client.load(url,token)
ret = client.load(url, ak, sk, True)

client.setAudioParams(sample_rate, channel, bytePerSample)

time.sleep(2)
## 
client.joinRoom(roomid,'90091','ai_agent')

## 
client.sendPublicMessage(0,b'hello everyone')

import cv2
import numpy as np
import wave
## 
with open('AudioInProxy.PCM','rb') as f:
    data = f.read()
step = int(sample_rate*channel*bytePerSample*duration_time_s)
data_len = len(data)
## 
videoFrame = cv2.imread(r"demo.png")
h,w = videoFrame.shape[:2]
for i in tqdm.tqdm(range(0,data_len,step)):
    chunk = data[i:i+step]
    time.sleep(duration_time_s)
    client.publishAudioStream(chunk,len(chunk))
    client.publishVedioStream(w,h, videoFrame.tobytes(), len(videoFrame.tobytes()))



# 
# for i in range(100):
#     frame = cv2.imread(r"demo.png")
#     h,w = frame.shape[:2]
#     time.sleep(0.04)
#     client.publishVedioStream(w,h, frame.tobytes(), len(frame.tobytes()))


#
buffer_size = 10240  # 

fromUserId="n2m-u-6B208D3A9B5D0A4FE3F80EAFF210E6A20522"
client.subAudioStream(fromUserId)
time.sleep(1)
alldata = []
for i in range(50):
    data  = client.getAudioStream(fromUserId, buffer_size)
    
    time.sleep(0.1)
    alldata+=data

alldata = np.array(alldata,dtype='int16').tobytes()

with wave.open("tts_wav.wav", 'wb') as wav_file:
    wav_file.setparams((1, 2, 16000, 0, 'NONE', 'not compressed'))
    wav_file.writeframes(alldata)
