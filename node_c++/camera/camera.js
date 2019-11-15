/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-14 18:01:47
 * @LastEditTime: 2019-08-15 09:36:56
 * @LastEditors: Please set LastEditors
 */


//使用require加载插件
let cameraMoudle = require('./build/Release/camera');
let qrcode = require('../qrcode/qrcode.js');

//et res = cameraMoudle.open_camera(0)
//if(res == 0) {
    let buff = cameraMoudle.qrdecode_image(function(imageData) {
        // qrcode.qrdecode(imageData, function(res) {
        //     console.log(res);
        // });
        console.log("1111111");
    });

    console.log(buff);


//} else {
 //   console.log("open fail");
//}

//cameraMoudle.close_camera();
