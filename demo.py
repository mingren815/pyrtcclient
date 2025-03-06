from rtc_client import RtcClient
import time
import tqdm
import pdb
# sample_rate = 48000
# channel = 2
# bitsize = 2
sample_rate = 16000
channel = 1
bitsize = 2
duration_time_s = 0.1
url = "https://cd.nice2meet.cn:9810"

ak = "demo_access"
sk = "demo_secret"
roomid = '13350050355'
token = ""



client = RtcClient()
time.sleep(2)

##ret = client.load(url,token)
ret = client.load(url, ak, sk)
time.sleep(2)
## 加入房间
client.joinRoom(roomid,'90091','ai_agent')

## 发送文本
client.sendPublicMessage(0,b'hello everyone') #


## 发送音频
with open('AudioOutProxy.PCM','rb') as f:
    data = f.read()
step = int(sample_rate*channel*bitsize*duration_time_s)
data_len = len(data)
for i in tqdm.tqdm(range(0,data_len,step)):
    chunk = data[i:i+step]
    time.sleep(duration_time_s)
    client.publishAudioStream(chunk,len(chunk))

import cv2
import numpy as np
import wave

# 按帧发送视频
for i in range(100):
    frame = cv2.imread(r"demo.png")
    h,w = frame.shape[:2]
    time.sleep(0.04)
    client.publishVedioStream(w,h, frame.tobytes(), len(frame.tobytes()))


#接收音频
buffer_size = 10240  # 假设你需要的缓冲区大小为 1024 字节

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
