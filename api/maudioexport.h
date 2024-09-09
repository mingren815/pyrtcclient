#ifndef RT_MexportAudio_H_
#define RT_MexportAudio_H_

#include "common.h"
#include "room.h"

namespace rtc {

    /**
    * 音频导出接口
    * 导出房间内用户的音频数据
    */
    class RT_API_EXPORT IAudioExport : public IModule {
    public:

			class IListener {
			public:
				
				/** 音频数据导出回调通知,导出数据是pcm数据
				* @param[out] uId 用户Id。
				* @param[out] timestamp_ns 时间戳。
				* @param[out] sampleRate   采样频率。
				* @param[out] channels 通道数。
				* @param[out] data 音频数据。
				* @param[out] len 音频数据长度。
				* @return 返回错误代码。
				*/
				virtual void audioStreamOut(const String& uId, uint64 timestamp_ns, int sampleRate, int channels, const uint8 *data, unsigned int len) = 0;

			protected:
				virtual ~IListener() {}
			};
		public:

        static IAudioExport* getAudioExport(rtc::IRoom* room);
        /** 设置音频设备管理回调接口
        *
        * @param[in] *listener 音频数据导出回调。
        * @return 返回错误代码。
        * @sa IListener
        */
        virtual Result setListener(IListener* listener) = 0;


			/** 选择需要导出指定用户的音频
			* @param[in] uid 选择需要导出音频的用户id。
			* @param[in] freq 采样频率 :8000,16000,44100 ,如果不填，默认16000
			* @param[in] channels 通道数 :1,2  如果不填，默认是1
			*/
			virtual Result selectUser4Export(const UserId& uid, int freq = 16000,int channels = 1) = 0;

			/** 停止指定用户的音频导出
			*  @note 停止正在导出音频
			*/
			virtual Result stopExport() = 0;

    protected:
        virtual ~IAudioExport() {}
    };

} // namespace rtc
#endif//RT_MexportAudio_H_
