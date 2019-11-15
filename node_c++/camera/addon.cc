/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-15 10:02:30
 * @LastEditTime: 2019-08-15 10:02:41
 * @LastEditors: Please set LastEditors
 */
#include <node.h>
#include "myobject.h"

namespace demo {

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports) {
  MyObject::Init(exports);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, InitAll)

}  // namespace demo