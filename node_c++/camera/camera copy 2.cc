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
         
            unsigned int i = 0;  // 循环读取摄像头
            int fd = -1;
            struct v4l2_buffer v4lbuf;

            const int nbuf = 3;// 设置摄像头缓存的个数
            // 定义struct v4l2_buffer，每个v4l2_buffer对应内核驱动中的一个缓存
            struct v4l2_buffer buffer[3];
            unsigned char *start[3];
                            
            /**
             * @description: 打开摄像头
             * @param {type} 
             * @return: 
             */
            int _open_camera(unsigned int deviceId) {

                char deviceName[20];
                sprintf(deviceName, "/dev/video%d", deviceId);

                bzero(&v4lbuf, sizeof(v4lbuf));
                v4lbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                v4lbuf.memory = V4L2_MEMORY_MMAP;

                // 打开摄像头
                int cam_fd = open(deviceName, O_RDWR);
                if (cam_fd < 0) {
                    return -1;
                }

                struct v4l2_format *fmt = (struct v4l2_format *)calloc(1, sizeof(*fmt));
                fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                
                // 获取摄像头当前的采集格式
                ioctl(cam_fd, VIDIOC_G_FMT,fmt);
                printf("%d\n", fmt->fmt.pix.width);
                printf("%d\n", fmt->fmt.pix.height);

                // 设置摄像头的采集格式JPEG
                bzero(fmt, sizeof(*fmt));
                fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                fmt->fmt.pix.width = 1024;
                fmt->fmt.pix.height = 768;
                fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;// 格式
                fmt->fmt.pix.field = V4L2_FIELD_INTERLACED; // 采样区域,交错方式捕获
                ioctl(cam_fd, VIDIOC_S_FMT, fmt);
                //释放
                free(fmt);
            
                // 向内核申请缓存（内核）
                struct v4l2_requestbuffers reqbuf;
                bzero(&reqbuf, sizeof(reqbuf));
                reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                reqbuf.memory = V4L2_MEMORY_MMAP;
                reqbuf.count = nbuf;
                ioctl(cam_fd, VIDIOC_REQBUFS, &reqbuf);
                //int length[nbuf];

                for(int i=0; i<nbuf; i++) {
                    bzero(&buffer[i], sizeof(buffer[i]));
                    buffer[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // 缓冲帧数据格式
                    buffer[i].memory = V4L2_MEMORY_MMAP;          // 内存映射方式
                    buffer[i].index = i;
                    // 取内核的buffer的数据，获取缓冲帧的地址，长度
                    ioctl(cam_fd, VIDIOC_QUERYBUF, &buffer[i]);

                    //length[i] = buffer[i].length;
                    // 内核缓冲区通过mmap映射到用户空间的内存，我们在用户层获取摄像头数据
                    start[i] = (unsigned char *)mmap(NULL, buffer[i].length, PROT_READ | PROT_WRITE, MAP_SHARED, cam_fd, buffer[i].m.offset);
                    // 入队，准备放入数据
                    ioctl(cam_fd, VIDIOC_QBUF, &buffer[i]);
                }

                // 采集数据
                enum v4l2_buf_type vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                // 开启IO流
                ioctl(cam_fd, VIDIOC_STREAMON, &vtype);
                return cam_fd;
            }


            /**
             * @description: 关闭摄像头
             * @param {type} 
             * @return: 
             */
            void _close_camera(int cam_fd) {
                if(cam_fd > 0) {
                    // 关闭IO流（丢掉缓冲区数据）
                    enum v4l2_buf_type vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    ioctl(cam_fd, VIDIOC_STREAMOFF, &vtype);
                    // 关闭摄像头
                    close(cam_fd);
                }
            }


            /**
             * @description: 打开摄像头
             * @param {type} 
             * @return: 
             */
            void open_camera (const FunctionCallbackInfo<Value>& args) {
                Isolate* isolate = args.GetIsolate();
                //int num = args[0]->NumberValue();

                Local<Number> value1 = Local<Number>::Cast(args[0]);
                int cam_fd = _open_camera(value1->IntegerValue());
                fd = cam_fd > 0 ? cam_fd : -1;

                cam_fd = cam_fd > 0 ? 0:1;
                args.GetReturnValue().Set(Number::New(isolate, cam_fd));
            }


            /**
             * @description: 关闭摄像头
             * @param {type} 
             * @return: 
             */
            void close_camera (const FunctionCallbackInfo<Value>& args) {
                _close_camera(fd);
                fd = -1;
            }


            /* 该类的成员方法，增加value的值 */
            void qrdecode_image (const FunctionCallbackInfo<Value>& args) {
                Isolate* isolate = args.GetIsolate();
                if(fd > 0) {
                    // 从队列中取出数据填满缓存
                    v4lbuf.index = i%nbuf;
                    // VIDIOC_DQBUF出队，摄像头没数据时阻塞
                    ioctl(fd, VIDIOC_DQBUF, &v4lbuf);

                    // callback, 使用Cast方法来转换
                    Local<Function> callback = Local<Function>::Cast(args[1]);
                    char *temp = (char *)start[(unsigned int)(v4lbuf.index)];
                    // Local<Value> bmpData = Nan::NewBuffer(temp, v4lbuf.bytesused).ToLocalChecked();
                    // Local<Value> argv[] = { bmpData };
                    // printf("callback\n");
                    // 调用回调, 参数: 当前上下文，参数个数，参数列表
                    // callback->Call(isolate->GetCurrentContext()->Global(), 1, argv);

                    // 将已经读到数据的缓存块重新置入队列中
                    v4lbuf.index=i%nbuf;
                    ioctl(fd, VIDIOC_QBUF, &v4lbuf);

                    i++;

                    args.GetReturnValue().Set(Nan::NewBuffer(temp, v4lbuf.bytesused).ToLocalChecked());
                } else {
                    printf("fd < 0\n");
                }
                args.GetReturnValue().Set(Number::New(isolate, 0));
            }

//模块导出

void init(Handle<Object> exports) {
    //把hello和Method这个方法挂钩,相当于在 exports 对象中添加 { hello: hello }
    NODE_SET_METHOD(exports, "open_camera", open_camera);
    NODE_SET_METHOD(exports, "close_camera", close_camera);
    NODE_SET_METHOD(exports, "qrdecode_image", qrdecode_image);
}


// 将 export 对象暴露出去
// 原型 `NODE_MODULE(module_name, Initialize)`
NODE_MODULE(camera, init)


};  // namespace demo