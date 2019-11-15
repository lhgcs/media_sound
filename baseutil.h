#ifndef BASEUTIL_H
#define BASEUTIL_H

#include <map>

using std::map;

////用映射表存所有对象地址，每个对象保存自己的ID号
//class BaseUtil;  // 前置声明
//typedef map<unsigned int,BaseUtil*> PMap;

class BaseUtil
{
public:
    BaseUtil(){
//        //构造函数中将this指针赋给pThis，使得回调函数能通过pThis指针访问本对象
//        if(mPMap.empty()){
//            m_ID=1;
//        }else{
//            m_ID=mPMap.size()+1;
//        }
//        mPMap.insert(PMap::value_type(m_ID, this));
    }

//    //静态回调函数
//    static void callback(){
//        if (mPMap.empty()) {
//            return;
//        }
//        //find 返回迭代器指向当前查找元素的位置否则返回map::end()位置
//        PMap::iterator iter = mPMap.find(m_SID);
//        if(iter != mPMap.end()){
//            //回调函数中调用非静态函数
//            BaseUtil * pThis = iter->second;
//            pThis->thread_task();
//        }
//    }

//    //非静态函数BaseUtil
//    void *thread_task(void *){

//    }

//    unsigned int m_ID; //本对象在列表中的ID号
//    static int m_SID;  //静态当前对象ID        (需要时，将m_ID赋值给m_SID以起到调用本对象函数的功能)
//    static PMap mPMap; //静态对象映射表
};

//PMap BaseUtil::mPMap;
//int  BaseUtil::m_SID=0;

#endif // BASEUTIL_H
