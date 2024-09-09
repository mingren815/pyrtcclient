#ifndef RTC_RtspClient_H_
#define RTC_RtspClient_H_

#include "common.h"

namespace rtc {
    /**
    * Rtsp客户端.
    *
    */
    class FakeAudioCapturer;
    class FakeVideoCapturer;
    class IVideoRender;

		class RT_API_EXPORT RtspClient {
		public:
			class IListenerStatus
			{
			public:
            virtual void OnStartResult(Result rv, rtc::String msg, int width, int height) = 0;

            virtual void OnStopResult(Result rv, rtc::String msg) = 0;
			};
		public:
			/** 创建一个Rtsp客户端
			*
			* @sa
			*/
			static	RtspClient* create();

			/** 销毁一个Rtsp客户端
			*
			* @sa
			*/
			static	void destroy(RtspClient*);

			/** 开始接收流
			*
			* @param[in] uri rtsp流地址。
			* @sa
			*/
			virtual Result start(std::string uri) = 0;
			/** 停止接收流
			*
			* @sa
			*/
			virtual Result stop() = 0;

			/** 是否已经启动
			*
			* @sa
			*/
			virtual Result isStarted() = 0;

			/** 设置rtsp连接状态回调
			*
			*/
			virtual Result setStatusListener(IListenerStatus* listner) = 0;
			/** 设置渲染接口指针,发布视频时可以用attach,这个接口只是针对预览时候使用
			*
			* @param[in] render 渲染接口指针。
			* @sa
			*/
        virtual Result setRender(rtc::IVideoRender* render, bool isattach = true) = 0;
			/** 查找并删除render
			*/
        virtual Result detachRender(rtc::IVideoRender* render) = 0;
			/** 删除所有的render
			*/
			virtual Result detachAllRender() = 0;
			/** 设置模拟视频采集接口指针
			*
			* @param[in] capture 模拟视频采集接口指针。
			* @note 由于默认回调Encoded数据，只能使用导入编码数据的VideoCapture，如果需要传入的是导入YUV数据的FakeVideoCapture需要调用enableVideoCallbackYUV，并且调用enableVideoCallbackEncoded(false)。
			* @sa enableVideoCallbackEncoded
			* @sa enableVideoCallbackYUV
			* @return 错误码。
			*/
			
        virtual Result setVideoCapture(rtc::FakeVideoCapturer* capture) = 0;
			/** 设置模拟音频采集接口指针
			*
			* @param[in] capture 模拟音频采集接口指针。
			* @note 由于音频导入只能导入一种数据，PCM或者编码后的数据，所以enableAudioCallbackEncoded enableAudioCallbackPCM只能有一种生效，或者将默认导入encoded数据。
			* @note PCM：enableAudioCallbackEncoded(false)  enableAudioCallbackPCM(true); 导入encoded数据：enableAudioCallbackEncoded(true)  enableAudioCallbackPCM(false);
			* @sa enableVideoCallbackEncoded
			* @sa enableVideoCallbackYUV
			* @return 错误码。
			*/
        virtual Result setAudioCapture(rtc::FakeAudioCapturer* capture) = 0;

			/** 获取错误信息
			*/
			virtual std::string getErrMsg(int rt) = 0;

			/** 检查是否启动rtsp连接
			*/
			virtual bool getRtspIsStarted() = 0;

		protected:
			virtual ~RtspClient() {};
		};

} // namespace rtc

#endif //RTC_RtspClient_H_
