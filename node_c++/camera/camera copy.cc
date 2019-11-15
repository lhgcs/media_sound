/*
 * @Description: camera
 * @Author: your name
 * @Date: 2019-07-15 12:02:36
 * @LastEditTime: 2019-08-14 18:21:21
 * @LastEditors: Please set LastEditors
 */


#include <node_object_wrap.h>
#include <node.h>
#include <v8.h>
#include <nan.h>
using namespace v8;

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/time.h>		// for timestamp incomplete error in kernel 2.6.21
#include <sys/utsname.h>
#include <linux/videodev2.h>
#include <time.h>


namespace camera {
    /* 将C++类封装给JS使用，需要继承node::ObjectWrap */
    class Camera : public node::ObjectWrap {
        public:
            /* 初始化该类的JS构造函数，并返回JS构造函数 */
            static Local<Function> init (Isolate* isolate) {
                /* 利用函数模板，将一个C++函数包装成JS函数 */
                Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, CameraJS);
                tpl->SetClassName(String::NewFromUtf8(isolate, "Camera"));
                tpl->InstanceTemplate()->SetInternalFieldCount(1);
                /* 类方法定义在构造函数的prototype上 */
                // NODE_SET_PROTOTYPE_METHOD(tpl, "open_camera", open_camera);
                // NODE_SET_PROTOTYPE_METHOD(tpl, "close_camera", close_camera);
                // NODE_SET_PROTOTYPE_METHOD(tpl, "qrdecode_image", qrdecode_image);
                /* 获取类的JS构造函数 */
                Local<Function> fn = tpl->GetFunction();
                /* JS构造函数句柄存储于constructor上，后续还会使用到 */
                constructor.Reset(isolate, fn);
                return fn;
            }
        private:
            /* 成员变量 */
            static Persistent<Function> constructor;            
            static int fd;
            // static struct v4l2_buffer v4lbuf;

            // static const int nbuf = 3;// 设置摄像头缓存的个数
            // // 定义struct v4l2_buffer，每个v4l2_buffer对应内核驱动中的一个缓存
            // static struct v4l2_buffer buffer[3];
            // static unsigned char *start[3];
                            
            static unsigned int i;  // 循环读取摄像头

            /* 该类的C++构造函数，设置成员变量初始值 */
            explicit Camera() {
                this->fd = -1;
                this->i = 0;
            }


            /* 该类的JS构造函数，创建该类的对象，并包装成JS对象然后进行返回 */
            static void CameraJS (const FunctionCallbackInfo<Value>& args) {
                Isolate* isolate = args.GetIsolate();
                if (args.IsConstructCall()) {/* 通过 new Camera() 创建对象 */
                    /* 提取参数数值 */
                    //double val = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
                    /* 创建该类的实例对象 */
                    Camera* obj = new Camera();
                    /* 包装该对象 */
                    obj->Wrap(args.This());
                    /* 返回该对象 */
                    args.GetReturnValue().Set(args.This());
                } else {/* 通过直接调用函数 Camera() 创建对象，抛出异常 */
                    isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate, "Should use the new operator to create an instance.")
                    ));
                }
            }


//             /**
//              * @description: 打开摄像头
//              * @param {type} 
//              * @return: 
//              */
//             static int _open_camera(unsigned int deviceId) {

//                 char deviceName[20];
//                 sprintf(deviceName, "/dev/video%d", deviceId);

//                 bzero(&v4lbuf, sizeof(v4lbuf));
//                 v4lbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//                 v4lbuf.memory = V4L2_MEMORY_MMAP;

//                 // 打开摄像头
//                 int cam_fd = open(deviceName, O_RDWR);
//                 if (cam_fd < 0) {
//                     return -1;
//                 }

//                 struct v4l2_format *fmt = (struct v4l2_format *)calloc(1, sizeof(*fmt));
//                 fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                
//                 // 获取摄像头当前的采集格式
//                 ioctl(cam_fd, VIDIOC_G_FMT,fmt);
//                 printf("%d\n", fmt->fmt.pix.width);
//                 printf("%d\n", fmt->fmt.pix.height);

//                 // 设置摄像头的采集格式JPEG
//                 bzero(fmt, sizeof(*fmt));
//                 fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//                 fmt->fmt.pix.width = 1024;
//                 fmt->fmt.pix.height = 768;
//                 fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;// 格式
//                 fmt->fmt.pix.field = V4L2_FIELD_INTERLACED; // 采样区域,交错方式捕获
//                 ioctl(cam_fd, VIDIOC_S_FMT, fmt);
//                 //释放
//                 free(fmt);
            
//                 // 向内核申请缓存（内核）
//                 struct v4l2_requestbuffers reqbuf;
//                 bzero(&reqbuf, sizeof(reqbuf));
//                 reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//                 reqbuf.memory = V4L2_MEMORY_MMAP;
//                 reqbuf.count = nbuf;
//                 ioctl(cam_fd, VIDIOC_REQBUFS, &reqbuf);
//                 //int length[nbuf];

//                 for(int i=0; i<nbuf; i++) {
//                     bzero(&buffer[i], sizeof(buffer[i]));
//                     buffer[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // 缓冲帧数据格式
//                     buffer[i].memory = V4L2_MEMORY_MMAP;          // 内存映射方式
//                     buffer[i].index = i;
//                     // 取内核的buffer的数据，获取缓冲帧的地址，长度
//                     ioctl(cam_fd, VIDIOC_QUERYBUF, &buffer[i]);

//                     //length[i] = buffer[i].length;
//                     // 内核缓冲区通过mmap映射到用户空间的内存，我们在用户层获取摄像头数据
//                     start[i] = (unsigned char *)mmap(NULL, buffer[i].length, PROT_READ | PROT_WRITE, MAP_SHARED, cam_fd, buffer[i].m.offset);
//                     // 入队，准备放入数据
//                     ioctl(cam_fd, VIDIOC_QBUF, &buffer[i]);
//                 }

//                 // 采集数据
//                 enum v4l2_buf_type vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//                 // 开启IO流
//                 ioctl(cam_fd, VIDIOC_STREAMON, &vtype);
//                 return cam_fd;
//             }


//             /**
//              * @description: 关闭摄像头
//              * @param {type} 
//              * @return: 
//              */
//             static void _close_camera(int cam_fd) {
//                 if(cam_fd > 0) {
//                     // 关闭IO流（丢掉缓冲区数据）
//                     enum v4l2_buf_type vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//                     ioctl(cam_fd, VIDIOC_STREAMOFF, &vtype);
//                     // 关闭摄像头
//                     close(cam_fd);
//                 }
//             }


//             /**
//              * @description: 打开摄像头
//              * @param {type} 
//              * @return: 
//              */
//             static void open_camera (const FunctionCallbackInfo<Value>& args) {
//                 Isolate* isolate = args.GetIsolate();
//                 //int num = args[0]->NumberValue();

//                 Local<Number> value1 = Local<Number>::Cast(args[0]);
//                 int cam_fd = _open_camera(value1->IntegerValue());
//                 if(cam_fd > 0) {
//                     /* 将被包装的JS对象还原为C++对象 */
//                     Camera* obj = node::ObjectWrap::Unwrap<Camera>(args.Holder());
//                     /* 访问C++对象上的成员变量进行操作 */
//                     obj->fd = cam_fd;
//                 }

//                 cam_fd = cam_fd > 0 ? 0:1;
//                 args.GetReturnValue().Set(Number::New(isolate, cam_fd));
//             }


//             /**
//              * @description: 关闭摄像头
//              * @param {type} 
//              * @return: 
//              */
//             static void close_camera (const FunctionCallbackInfo<Value>& args) {
//                 //Isolate* isolate = args.GetIsolate();
//                 /* 将被包装的JS对象还原为C++对象 */
//                 Camera* obj = node::ObjectWrap::Unwrap<Camera>(args.Holder());
//                 /* 访问C++对象上的成员变量进行操作 */
//                 _close_camera(obj->fd);
//                 obj->fd = -1;
//             }


//             /* 该类的成员方法，增加value的值 */
//             static void qrdecode_image (const FunctionCallbackInfo<Value>& args) {
//                 Isolate* isolate = args.GetIsolate();
//                 /* 将被包装的JS对象还原为C++对象 */
//                 Camera* obj = node::ObjectWrap::Unwrap<Camera>(args.Holder());
//                 /* 访问C++对象上的成员变量进行操作 */
//                 if(obj->fd > 0) {
//                     // 从队列中取出数据填满缓存
//                     v4lbuf.index = i%nbuf;
//                     // VIDIOC_DQBUF出队，摄像头没数据时阻塞
//                     ioctl(obj->fd, VIDIOC_DQBUF, &v4lbuf);


//                     // callback, 使用Cast方法来转换
//                     Local<Function> callback = Local<Function>::Cast(args[1]);
//                     //char *temp = (char *)start[(unsigned int)(v4lbuf.index)];
//                     char *temp = (char *)"123";
//                     //Local<Object> bmpData =
//                     // Nan::NewBuffer(temp, 3);
//                     Local<Value> bmpData = Nan::NewBuffer(temp, v4lbuf.bytesused).ToLocalChecked();
//                     Local<Value> argv[] = { bmpData };
// //callback->Call(1, argv);
                    
//     callback->Call(isolate->GetCurrentContext()->Global(), 1, argv);

//                     // 调用回调, 参数: 当前上下文，参数个数，参数列表
//                    // callback->Call(isolate->GetCurrentContext()->Global(), 1, Nan::NewBuffer((char *)(start[v4lbuf.index]), v4lbuf.bytesused));

//                     // 将已经读到数据的缓存块重新置入队列中
//                     v4lbuf.index=i%nbuf;
//                     ioctl(obj->fd, VIDIOC_QBUF, &v4lbuf);

//                     i++;
//                 } else {

//                 }
//                 //args.GetReturnValue().Set(Number::New(isolate, 0));
//             }
    };
//模块导出


// Local<Object> bufferObj = args[0]->ToObject();
// char* bufferData = node::Buffer::Data(bufferObj);
// size_t bufferLength = node::Buffer::Length(bufferObj);

//  char* buffer = (char*) node::Buffer::Data(info[0]->ToObject());
//     unsigned int size = info[1]->Uint32Value();


    Persistent<Function> Camera::constructor;

    void init (Local<Object> exports) {
        Isolate* isolate = exports->GetIsolate();
        /* 初始化Camera类的JS构造函数 */
        Local<Function> _Camera = Camera::init(isolate);
        /* 将Camera类的JS构造函数暴露给JS使用 */
        /* 这里不能使用NODE_SET_METHOD，因为NODE_SET_METHOD是暴露一个C++函数给JS使用 */
        /* NODE_SET_METHOD(exports, "Camera", _Camera); */
        /* 此处是暴露一个JS函数，它在C++里面表示为一个Function对象，不是一个C++函数 */
        /* 要通过设置属性的方法将其挂到exports上 */
        exports->Set(String::NewFromUtf8(isolate, "Camera"), _Camera);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, init)

};  // namespace demo