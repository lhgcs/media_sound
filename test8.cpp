#include "test8.h"

void test8()
{



    //                    //【2】创建和原始图一样的效果图，x重映射图，y重映射图
    //                        dstImage.create( srcImage.size(), srcImage.type() );
    //                        map_x.create( srcImage.size(), CV_32FC1 );
    //                        map_y.create( srcImage.size(), CV_32FC1 );

    //                        //【3】双层循环，遍历每一个像素点，改变map_x & map_y的值
    //                        for( int j = 0; j < srcImage.rows;j++)
    //                        {
    //                            for( int i = 0; i < srcImage.cols;i++)
    //                            {
    //                                //改变map_x & map_y的值.
    //                                map_x.at<float>(j,i) = static_cast<float>(i);
    //                                map_y.at<float>(j,i) = static_cast<float>(srcImage.rows - j);
    //                            }
    //                        }

    //                        //【4】进行重映射操作
    //                        //此句代码的OpenCV2版为：
    //                        //remap( srcImage, dstImage, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0, 0) );
    //                        //此句代码的OpenCV3版为：
    //                        remap( srcImage, dstImage, map_x, map_y, INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0, 0) );

    //                        // 设置目标图像的大小和类型与源图像一致
    //                            dstImage_warp = Mat::zeros( srcImage.rows, srcImage.cols, srcImage.type() );

    //                            //【3】设置源图像和目标图像上的三组点以计算仿射变换
    //                            srcTriangle[0] = Point2f( 0,0 );
    //                            srcTriangle[1] = Point2f( static_cast<float>(srcImage.cols - 1), 0 );
    //                            srcTriangle[2] = Point2f( 0, static_cast<float>(srcImage.rows - 1 ));

    //                            dstTriangle[0] = Point2f( static_cast<float>(srcImage.cols*0.0), static_cast<float>(srcImage.rows*0.33));
    //                            dstTriangle[1] = Point2f( static_cast<float>(srcImage.cols*0.65), static_cast<float>(srcImage.rows*0.35));
    //                            dstTriangle[2] = Point2f( static_cast<float>(srcImage.cols*0.15), static_cast<float>(srcImage.rows*0.6));

    //                            //【4】求得仿射变换
    //                            warpMat = getAffineTransform( srcTriangle, dstTriangle );

    //                            //【5】对源图像应用刚刚求得的仿射变换
    //                            warpAffine( srcImage, dstImage_warp, warpMat, dstImage_warp.size() );

    //                            //【6】对图像进行缩放后再旋转
    //                            // 计算绕图像中点顺时针旋转50度缩放因子为0.6的旋转矩阵
    //                            Point center = Point( dstImage_warp.cols/2, dstImage_warp.rows/2 );
    //                            double angle = -50.0;
    //                            double scale = 0.6;
    //                            // 通过上面的旋转细节信息求得旋转矩阵
    //                            rotMat = getRotationMatrix2D( center, angle, scale );
    //                            // 旋转已缩放后的图像
    //                            warpAffine( dstImage_warp, dstImage_warp_rotate, rotMat, dstImage_warp.size() );


}

