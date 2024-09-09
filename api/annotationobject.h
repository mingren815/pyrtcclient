#ifndef RT_AnnotationObject_H_
#define RT_AnnotationObject_H_

#include <memory>
#include "api/common.h"
#include "api/annotationdefines.h"

////////////////////////// json 
//{
//"annotationbase": {
//	"annotation_id_": "1594fcf816c84d75a6677ff2565af1c4",
//		"annotation_rect_" : {
//		"bottom": 500,
//			"left" : 0,
//			"right" : 800,
//			"top" : 0
//	},
//		"display_rect_": {
//			"bottom": 500,
//				"left" : 0,
//				"right" : 800,
//				"top" : 0
//		},
//			"fill_color_": {
//				"a": 255,
//					"b" : 0,
//					"g" : 0,
//					"r" : 255
//			},
//				"line_width_": 2,
//					"line_arrow_" : 1,
//					"line_color_" : {
//					"a": 255,
//						"b" : 0,
//						"g" : 0,
//						"r" : 255
//				},
//					"object_id_": "{79c09a71-2f89-48f2-9f0b-de9ad6394443}",
//						"object_rect_" : {
//						"bottom": 462,
//							"left" : 43,
//							"right" : 119,
//							"top" : 227
//					},
//						"tool_type_": 2,
//							"user_id_" : "30",
//							"imginfo" : {
//							"imgid": 1,
//								"layer" : 2,
//								"has_select" : 1,
//								"ox" : 10,
//								"oy" : 10,
//								"w" : 300,
//								"h" : 300,
//								"rotate" : 180,
//								"url" : "xxxxxxxxxxxxxxxxxx"
//						}
//},
//"points": [{
//	"x": 43,
//		"y" : 227
//},
//		{
//			"x": 118,
//			"y" : 461
//		}
//]
//}
//////////////
using rtc::AnnotationToolType;
namespace rtc {
	class CriticalSection;
}
namespace room_pdu {
	class AnnotationObject;
	class Annotation2Point;
	class AnnotationMultiPoint;
	class AnnotationList;
	class AnnotationImg;
	class AnnotationTxt;
}
namespace rtcimp {

    class RT_API_EXPORT AnnotationObject {
    public:
        static AnnotationObject* Create(AnnotationToolType type);
        static AnnotationToolType checkToolType(std::string json);
        virtual ~AnnotationObject() {}

        //object info
        virtual void SetLineWidth(int width) { line_width_ = width; }
        virtual void SetLineColor(Color color) { line_color_ = color; }
        virtual void SetFillColor(Color color) { fill_color_ = color; }
        virtual void SetLineArrow(const LineArrowType& arrow) { line_arrow_ = arrow; }
        virtual void SetEnable(bool enable = true) { enable_ = enable; }
        virtual void SetDisplayRect(const Rect& rc) { display_rect_ = rc; }
        virtual void SetAnnotationRect(const Rect& rc) { annotation_rect_ = rc; }
        virtual void SetZoomMode(bool isZoom) { zoom_ = isZoom; }
        virtual bool IsEnable() { return enable_; }
        virtual bool Is(AnnotationToolType type) { return tool_type_ == type; };
        virtual AnnotationToolType ToolType() { return tool_type_; }

        virtual bool Draw(const AVDContext& context, bool paint) { return true; }
        virtual void CopyDataFrom(AnnotationObject* data) {}


        void CopyDataToPdu(room_pdu::AnnotationObject* pduobject);
        void CopyDataFromPdu(const room_pdu::AnnotationObject* pduobject);

        //json
        virtual bool CopyDataFromJson(std::string annotStr);
        virtual bool CopyDataToJson(std::string& annotStr);

        virtual bool HitTest(const Point& pt) { return true; }

        //id info
        void SetUserId(const std::string& id) { user_id_ = id; }
        void SetAnnotationId(const std::string& id) { annotation_id_ = id; }
        void SetObjectId(const std::string& id) { object_id_ = id; }

        std::string GetUserId() { return user_id_; }
        std::string GetAnnotationId() { return annotation_id_; }
        std::string GetObjectId() { return object_id_; }
        Color		GetLineColor() { return line_color_; }
        void setSeqNum(uint32 i) { _seq_num = i; }
        uint32 getSeqNum() { return _seq_num; }
    protected:
        AnnotationObject();
        AnnotationToolType	tool_type_;
        LineArrowType		line_arrow_;
        bool				owned_cursor_;
        bool				touched_;
        bool				enable_;
        bool				zoom_;
        int					line_width_;

        //not need initialize params
        Color				line_color_;
        Color				fill_color_;
        Rect				object_rect_;
        Rect				display_rect_;
        Rect				annotation_rect_;

        //user info
        std::string			object_id_;
        std::string			annotation_id_;
        std::string			user_id_;
        uint32              _seq_num;
    };

    typedef std::vector <AnnotationObject* > AnnotationObjects;
    typedef std::vector <std::string > AnnotationObjectIds;
    class Annotation2Point : public AnnotationObject {
    public:
        void CopyDataFrom(AnnotationObject* data) override;
        void CopyDataFrom(const room_pdu::Annotation2Point* pdu2point);
        void CopyDataFromEx(const room_pdu::Annotation2Point* pdu2point);
        void CopyDataTo(room_pdu::Annotation2Point* pdu2point);
        void SetDisplayRect(const Rect& rc) override;
        bool CopyDataFromJson(std::string annotStr) override;
        bool CopyDataToJson(std::string& annotStr) override;
    public:
        Point	point_from_;
        Point	point_to_;
    };

    class AnnotationLine : public Annotation2Point {
    public:
        bool HitTest(const Point& pt) override;
        AnnotationLine();
    };
    class AnnotationRectangle : public Annotation2Point {
    public:
        AnnotationRectangle();
        bool HitTest(const Point& pt) override;
    protected:
    };
    class AnnotationEllipse : public Annotation2Point {
    public:
        AnnotationEllipse();
        bool HitTest(const Point& pt) override;
    protected:
    };
    class AnnotationRhomb : public Annotation2Point {
    public:
        AnnotationRhomb();
        bool HitTest(const Point& pt) override;
    protected:
    };

    class AnnotationArrow : public Annotation2Point {
    public:
        AnnotationArrow();
        bool HitTest(const Point& pt) override;
    protected:
    };

    class AnnotationSuccess : public Annotation2Point {
    public:
        AnnotationSuccess();
        bool HitTest(const Point& pt) override;
    protected:
    };

    class AnnotationFailure : public Annotation2Point {
    public:
        AnnotationFailure();
        bool HitTest(const Point& pt) override;
    protected:
    };

    class AnnotationMultiPoint : public AnnotationObject {
    public:
        AnnotationMultiPoint();
        ~AnnotationMultiPoint();
        void CopyDataFrom(AnnotationObject* data) override;
        void CopyDataFrom(const room_pdu::AnnotationMultiPoint* pdupoints);
        void CopyDataFromEx(const room_pdu::AnnotationMultiPoint* pdupoints);
        void CopyDataTo(room_pdu::AnnotationMultiPoint* pdupoints);
        void SetDisplayRect(const Rect& rc) override;
        bool CopyDataFromJson(std::string annotStr) override;
        bool CopyDataToJson(std::string& annotStr) override;
    protected:
        PointArray	points_;
        rtc::CriticalSection*	m_critSec;
    };

    class AnnotationPolyline : public AnnotationMultiPoint {
    public:
        AnnotationPolyline();
        virtual bool HitTest(const Point& pt) override;
    protected:
    };


    class AnnotationEraser : public AnnotationObject {
    public:
        using AnnotationObject::CopyDataFrom;
        AnnotationEraser(AnnotationObjects& eraserObjects);
        AnnotationEraser(AnnotationObjectIds& eraserObjectIds);
        AnnotationEraser();
        ~AnnotationEraser();
        void Undo();
        void Redo();
        void CopyDataFrom(const room_pdu::AnnotationList* pdulist);
        void CopyDataTo(room_pdu::AnnotationList* pdulist);
        void SetEraserObjects(AnnotationObjects& eraserObjects);
        void GetEraserObjs(AnnotationObjects& eraserObjects);
        void GetEraserObjectIds(AnnotationObjectIds& eraserObjectIds);
        bool HitTest(const Point& pt) override { return false; }
        bool Draw(const AVDContext& context, bool paint) override { return false; }
        void MoveOnly(const Point& movePt) {};
    protected:
        AnnotationObjects eraser_objects_;
        AnnotationObjectIds eraser_object_ids_;
        rtc::CriticalSection*	map_crit_;
    };

    class AnnotationHighlightPoint : public Annotation2Point {
    public:
        AnnotationHighlightPoint();
        virtual bool HitTest(const Point& pt) override { return false; }
        virtual bool Draw(const AVDContext& context, bool paint) override { return false; }
    protected:
    };
    class AnnotationImg : public AnnotationRectangle {
    public:
        AnnotationImg();
        ~AnnotationImg();
        virtual void CopyDataFrom(AnnotationObject* data);
        virtual void CopyDataTo(room_pdu::AnnotationImg* pduimg);
        virtual void CopyDataFromEx(const room_pdu::AnnotationImg* pduimg);
        virtual bool CopyDataToJson(std::string& annotStr);
        virtual void SetDisplayRect(const Rect& rc);
        virtual void SetEnable(bool enable = true);
        virtual bool IsEnable();
        virtual void SetLocalFile(const std::string& localfile);
        bool HitTest(const Point& pt);
        std::string getId();
        int getLayerId();
        bool isUpdate();
        void resetupdate();
    public:
        std::shared_ptr<DrawImgInfo> _img_info;
        std::string _local_file;
    };
    class AnnotationHighlightCircle : public Annotation2Point {
    public:
        AnnotationHighlightCircle();
        virtual bool HitTest(const rtcimp::Point& pt) override { return false; }
        virtual bool Draw(const AVDContext& context, bool paint) override { return false; }
    protected:
        uint32 _radius;
    };

    struct TxtInfo {
        TxtInfo() :_font_size(0), _font_unit(0),
            _one_width(0), _one_height(0) {

        }
        int _font_size; ///required
        int _font_unit;
        int _one_width;
        int _one_height;
        Color _font_color; ///required
        std::vector<std::string> _contens; ///required
        std::string _font_family;
        std::string _font_style;
        std::string _font_weight;
        std::string _extend_data;

        void copyFrom(TxtInfo& ti) {
            _font_size = ti._font_size;
            _font_color = ti._font_color;
            _contens = ti._contens;
            _font_family = ti._font_family;
            _font_style = ti._font_style;
            _font_weight = ti._font_weight;
            _font_unit = ti._font_unit;
            _extend_data = ti._extend_data;
            _one_width = ti._one_width;
            _one_height = ti._one_height;
        }
        void clear() {
            _font_size = 0;
            _font_unit = 0;
            _one_width = 0;
            _one_height = 0;
            _contens.clear(); ///required
            _font_family.clear();
            _font_style.clear();
            _font_weight.clear();
            _extend_data.clear();
        }
    };
    class AnnotationTxt :public AnnotationRectangle {
    public:
        AnnotationTxt();
        ~AnnotationTxt();

        virtual void CopyDataFrom(AnnotationObject* data);
        virtual void CopyDataTo(room_pdu::AnnotationTxt* pdutxt);
        virtual void CopyDataFromEx(const room_pdu::AnnotationTxt* pdutxt);
        virtual bool CopyDataToJson(std::string& annotStr);
        virtual bool CopyDataFromJson(std::string annotStr);
        bool HitTest(const Point& pt);


        TxtInfo _txt;
    };
}
#endif//RT_AnnotationObject_H_
