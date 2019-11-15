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



#include <opencv2/opencv.hpp>

using namespace cv;

namespace camera {
   /*      
            unsigned int i = 0;  // 循环读取摄像头
            int fd = -1;
            struct v4l2_buffer v4lbuf;

            const int nbuf = 3;// 设置摄像头缓存的个数
            // 定义struct v4l2_buffer，每个v4l2_buffer对应内核驱动中的一个缓存
            struct v4l2_buffer buffer[3];
            unsigned char *start[3];
        */                    
            /**
             * @description: 打开摄像头
             * @param {type} 
             * @return: 
             */
     /*       int _open_camera(unsigned int deviceId) {

               
                return 0;
            }
*/

            /**
             * @description: 关闭摄像头
             * @param {type} 
             * @return: 
             */
     /*       void _close_camera(int cam_fd) {
                if(cam_fd > 0) {
                    // 关闭IO流（丢掉缓冲区数据）
                    enum v4l2_buf_type vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    ioctl(cam_fd, VIDIOC_STREAMOFF, &vtype);
                    // 关闭摄像头
                    close(cam_fd);
                }
            }

*/
            /**
             * @description: 打开摄像头
             * @param {type} 
             * @return: 
             */
    /*        void open_camera (const FunctionCallbackInfo<Value>& args) {
                Isolate* isolate = args.GetIsolate();
                //int num = args[0]->NumberValue();

                Local<Number> value1 = Local<Number>::Cast(args[0]);
                int cam_fd = _open_camera(value1->IntegerValue());
                fd = cam_fd > 0 ? cam_fd : -1;

                cam_fd = cam_fd > 0 ? 0:1;
                args.GetReturnValue().Set(Number::New(isolate, cam_fd));
            }
*/

            /**
             * @description: 关闭摄像头
             * @param {type} 
             * @return: 
             */
          /*  void close_camera (const FunctionCallbackInfo<Value>& args) {
                _close_camera(fd);
                fd = -1;
            }
*/

            /* 该类的成员方法，增加value的值 */
            void qrdecode_image (const FunctionCallbackInfo<Value>& args) {
                Isolate* isolate = args.GetIsolate();

// 先实例化再初始化

// VideoCapture capture;

// Capture.open("1.avi");

                VideoCapture cap(0);
                if(cap.isOpened()) {
                     UMat frame;
                cap >> frame;

                imwrite("/home/ubuntu/Desktop/1.png", frame);

                 // String 类型的声明 
                Local<v8::String> str = v8::String::NewFromUtf8(isolate, "/home/ubuntu/Desktop/1.png"); 



                Local<Value> argv[1] = {
                    str
                };
                    // callback, 使用Cast方法来转换
                    Local<Function> callback = Local<Function>::Cast(args[1]);
                    // 调用回调, 参数: 当前上下文，参数个数，参数列表
                    callback->Call(isolate->GetCurrentContext()->Global(), 1, argv);
                }
               


                //     char *temp = (char *)start[(unsigned int)(v4lbuf.index)];
                //     // Local<Value> bmpData = Nan::NewBuffer(temp, v4lbuf.bytesused).ToLocalChecked();
                //     // Local<Value> argv[] = { bmpData };

                 //   args.GetReturnValue().Set(Nan::NewBuffer(temp, v4lbuf.bytesused).ToLocalChecked());
                // } else {
                //     printf("fd < 0\n");
                // }
                // args.GetReturnValue().Set(Number::New(isolate, 0));
            }

//模块导出

void init(Handle<Object> exports) {
    //把hello和Method这个方法挂钩,相当于在 exports 对象中添加 { hello: hello }
 //   NODE_SET_METHOD(exports, "open_camera", open_camera);
 //   NODE_SET_METHOD(exports, "close_camera", close_camera);
    NODE_SET_METHOD(exports, "qrdecode_image", qrdecode_image);
}


// 将 export 对象暴露出去
// 原型 `NODE_MODULE(module_name, Initialize)`
NODE_MODULE(camera, init)


};  // namespace demo
