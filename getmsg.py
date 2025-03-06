from rtc_client import RtcClient,MessagesType
import time
import tqdm
import pdb
# import asyncio
# asyncio.sleep(0.1)
# sample_rate = 48000
# channel = 2
# bitsize = 2
sample_rate = 16000
channel = 1
bitsize = 2
duration_time_s = 0.1
# url = "https://rtc-dev.uicloud.com/r/#/join"
url = "https://rtc.uicloud.com"

token = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJjbGllbnRJZCI6Ik1jcjdaOFBhSjRpM0JENTIiLCJhcHBJZCI6IjE4NjE2NzIyMDQ1OTY3NTIzODUiLCJ0cCI6ImNjZyIsImF1ZCI6IjE4NjE2NzIyMDQ1OTY3NTIzODUiLCJleHAiOjE3MzM4MDkzNTIsImlhdCI6MTczMzgwODc1MiwiaXNzIjoiaHR0cHM6Ly9hdXRoLnVpY2xvdWQuY29tLyJ9.QhXnZcFCm8_KyrUlY6DrdU3ILEiVHrMLwxCpzOBnX7E5-QBnUcJ_G-tjfN2-vg8qE97-LUpJICbK9-G0ymCc_mJj0Oo-yWLQLlDHzijjUoWyK14hpHZrLXFta8RoRK9Pv7uoUtj6QiI8QSUVn1tHJtJlVagpruBcDJ1oEjiYjgU"

roomid = '20589675'


# class MyRtcClient(RtcClient):

#     def __init__(self) -> None:
#         super().__init__()


#     def onInitResult(result):
#         super().onInitResult(result)
#         print("#############my onInitResult##############")

# token = 

client = RtcClient()
time.sleep(2)

ret = client.load(url,token)
time.sleep(2)

client.joinRoom(roomid,'90011','ai_agent')
client.sendPublicMessage(0,b'hello everyone') #
# with open('tts_wav.pcm','rb') as f:
#     data = f.read()
time.sleep(3)

# pdb.set_trace()
while True:
    time.sleep(0.1)
    if hasattr(client,"getPublicHistoryMessageByType"):
        msgs = client.getPublicHistoryMessageByType(False,0,100)
    else:
        msgs = client.getPublicHistoryMessage(0,100)
    print(msgs)
    for msg in msgs:
        print(msg.fromId,msg.message)
print(msg)
time.sleep(3)