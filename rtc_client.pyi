from typing import Optional, Tuple, Union

class RtcClientInterface:
    pass  # 这里应该定义接口中的方法，但具体方法未给出

class RtcClientBase:
    pass  # 同上

class RtcClient(RtcClientInterface, RtcClientBase):

    def __init__(self) -> None:
        pass

    def __del__(self) -> None:
        pass

    def load(self, url: str, appkey: str, secretkey: str, log: bool) -> int:
        pass

    def uninit(self) -> None:
        pass

    def joinRoom(self, roomid: str, selfUserId: str, selfUserName: str) -> int:
        pass

    def leave(self, reason: int) -> int:
        pass

    def sendPrivateMessage(self, msgType: int, message: str, targetUserId: str) -> int:
        pass

    def sendPublicMessage(self, msgType: int, message: str) -> int:
        pass

    def publishAuditStream(self, data: bytes, len: int) -> int:
        pass

    def publishVedioStream(self, w: int, h: int, data: bytes, len: int) -> int:
        pass

    def subAudioStream(self, targetUserId: str) -> int:
        pass

    def getAudioStream(self, targetUserId: str, maxSize: int) -> int:
        pass