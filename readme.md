一、文件说明
	mediainout.cpp ---测试代码
	Makefile ---测试代码makefile
	mediainout ---编出来的测试执行文件
                AudioInProxy.PCM ---测试导入原始音频文件
	VideoInput.h264 ---测试导入h264编码文件
	libcn_tee3_avd_so.so --- sdk
	AudioOutProxy.PCM ---导出音频文件，需要加入房间后才会有
	video_XXXX --- 导出的远端视频，是h264编码文件， XXX代表远端某一路设备id，需要程序跑起来才加入房间后才能产生出来
	data_xxx.yuv ---导出的是远端视频解码后的原始数据，XXX代表远端某一路设备id。
	
二、运行说明
   0，运行注意点
    运行时，需要mediainout 需要加执行权限 chmod +x mediainout
	修改vi ~/.bash_profile(或者 ~/.profile)在文件末尾加上两行： LD_LIBRARY_PATH=./ 和 export LD_LIBRARY_PATH
	source ~/.bash_profile
	需要mediainout 需要依赖libcn_tee3_avd_so.so ，需要在当前目录下链接该so
	
   1，运行命令：./mediainout --audio=2 --video=2 --roomid=12345678917
   --audio=0|1|2 ,0:导入音频,1:导出音频,2:导入和导出音频,不填默认值是2
   --video=0|1|2 ,0:导入视频,1:导出视频,2:导入和导出视频，不填默认值是2
   --roomid=12345678917, 加入的房间号，不填则创建新房价
   --url=xx
   当前测试代码设置的服务器地址默认是https://v.nice2meet.cn ，可以用chorme浏览器访问这个地址，加入和linux上相同房间号(比如12345678917)，可以观看linux发布的音视频
   要改地址，添加参数
	--url=xxxx
	--appkey=xxxx
	--secretkey=xxx
   --decodeable=0|1|2,0:只导出远端的编码数据，1:只导出解码后的原始数据，2:编码数据和解码数据都导出
   --inputfile=xxxx，导入264数据文件，默认为当前目录下的VideoInput_1080.h264
   
 三、在环境在centos7 和radhat7.9上都可以正常跑。房间里面发布视频编码必须使用h264的。


 python编译
 1. 安装pybind11

 2. 编译
 ```
 mkdir build 
 cd build
 cmake ..
 cmake --build . 
 ```
 cmake成功后得到rtc_client.cpython-310-x86_64-linux-gnu.so，