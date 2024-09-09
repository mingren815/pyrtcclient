#ifndef RT_Annotation_H_
#define RT_Annotation_H_

#include "common.h"
#include "room.h"
#include "annotationobject.h"
#include "mannotationinternal.h"

namespace rtcimp {
	class AnnotationObject;
}
namespace cricket {
	class VideoCapturer;
}

namespace rtc {
		class IVideoRender;
		class IMAnnotation;
		class RT_API_EXPORT IToolbar {
		public:
			static IToolbar* create();
			virtual Result setAnnotation(IMAnnotation* atn) = 0;
			virtual Result getExcludeWindows(AvdWindowIds& windowFilter) = 0;
			virtual Result setPosition(int x, int y) = 0;
		protected:
			virtual ~IToolbar(){}
		};

		class RT_API_EXPORT IMAnnotation : public IModule {
		public:
			
			static IMAnnotation* create(bool defaultToolbar = true, bool toolbarimg = false, bool isboardCreator = true, IMAnnotationInternal* anotateMgr = NULL);
			static void destoryAnnotaion(IMAnnotation* annotaion);
			static bool   isSupported(AnnotationToolType type);
			//virtual bool SetAnnotationCapture(IAnnotationCapture* capture) = 0;
			virtual void SetBoardId(const String& boradId) = 0;
			virtual BoardId GetBoarId() = 0;
			virtual Result start() = 0;
			virtual Result stop() = 0;
			virtual AvdWindowId CreateWhiteBoardWnd(int x, int y, int w, int h, AvdWindowId parentId) = 0;
			virtual void SetBkground(rtcimp::Color color) = 0;
			virtual void SetBkgroud(const String& filepath,bool isflash=false) = 0;
			virtual Result setApplicationWindow(const AvdWindowId& id, bool isWhiteboard) = 0;
			virtual Result setZoomMode(bool isZoom = false) = 0;
			virtual Result setDesktop(const AvdDesktopId& id) = 0;
			virtual Result setCurrentType(AnnotationToolType type) = 0;
			virtual Result setLineWidth(int width) = 0;
			virtual Result setLineArrow(LineArrowType arrow) = 0;
			virtual Result setLineColor(uint8 r, uint8 g, uint8 b, uint8 a) = 0;
			virtual Result setFillColor(uint8 r, uint8 g, uint8 b, uint8 a) = 0;
			virtual Result undo() = 0;
			virtual Result redo() = 0;
			virtual Result save() = 0;
			virtual Result clear(ClearType type) = 0;
			virtual bool resetImgStatus() = 0;

        virtual Result uploadImgfile(std::string filename, std::string imgId) = 0;
        virtual Result downloadImgfile(std::string id, std::string url) = 0;
        virtual Result setNewfile(char* file) = 0;
        virtual Result selectImg() = 0;
        virtual Result rotateImg() = 0;
        virtual Result zoomin() = 0;
        virtual Result zoomout() = 0;

			//infos
			virtual AnnotationId   annotationId() = 0;
			virtual void   setAnnotationId(const AnnotationId& id) = 0;
			virtual UserId ownerUserId() = 0;
			virtual void   setOwnerUserId(const UserId& id) = 0;
			virtual bool   isMine() = 0;

        //rect
        virtual void   setAnnotationSize(int width, int height) = 0;
        virtual void   setVideoRender(IVideoRender* render) = 0;
        virtual Result  OnWhiteBoardClosed() = 0;
        virtual void OnSaveBmpResult(Result ret, std::string msg) = 0;
        //window
        //virtual void   messgeWindowId() = 0;
        //virtual void   displayWindowId() = 0;
        //virtual void   drawWindowId() = 0;
        virtual Result getMessageWindowId(AvdWindowId& winId) = 0;
        virtual Result getDisplayWindowId(AvdWindowId& winId) = 0;
        virtual IToolbar* getToolbar() = 0;
        virtual ~IMAnnotation() {}


    };

}
#endif
