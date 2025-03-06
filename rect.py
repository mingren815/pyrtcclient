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
# url = "https://rtc-dev.uicloud.com/r/#/join"
url = "https://v.nice2meet.cn"

ak = "W68FH19QN2NI186THUX3XXG4CBMYKHXL"
sk = "HM9ECOGZIBR6OOWG00G3EFVDILOZSHPT"
roomid = '12345678930'
token = "YzhiZmNlYjhmOGYzNmVhMDE5NDdkNTQ0ZDg5ZTkxZTFiZjMyMTdjYg=="

import cv2
import numpy as np


client = RtcClient()
time.sleep(2)

# 设置离开房间回调,需要调用leave方法，否则数字人会重复进入会议
def handleLeaveIndication(reason, fromId):
    print(f"User {fromId} left the room with reason: {reason}")
    client.leave(reason)

# 指定回调的python函数
client.setLeaveIndicationCallback(handleLeaveIndication)

## 加入房间
client.loadAndJoinRoom(url, token, True, roomid, '90091', 'ai_agent', 10)
time.sleep(2)

# 按帧发送视频
for i in range(200):
    frame = cv2.imread(r"human.png")
    h,w = frame.shape[:2]
    time.sleep(0.04)
    client.publishVedioStream(w,h, frame.tobytes(), len(frame.tobytes()))

client.leave(0)

time.sleep(4)

client.loadAndJoinRoom(url,token, True, roomid, '90091', 'ai_agent', 10)
print('after joinroom')
for i in range(200):
    frame = cv2.imread(r"human.png")
    h,w = frame.shape[:2]
    time.sleep(0.04)
    client.publishVedioStream(w,h, frame.tobytes(), len(frame.tobytes()))
client.leave(0)
