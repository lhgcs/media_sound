/*
 * @Description: WIFI
 * @Author: your name
 * @Date: 2019-06-13 11:03:07
 * @LastEditTime: 2019-06-22 09:30:41
 * @LastEditors: Please set LastEditors
 */

var exec = require('child_process').exec;
var http = require("http");  

var opt = {
    //不能加http
    host:'www.utdimensions.com',  
    port:'5005',  
    method:'GET',  
    path:'/index/',  
    // headers:{  
    //     "Content-Type": 'application/json',  
    //     "Content-Length": data.length  
    // }  
}

var data = {username:"hello",password:"123456"};  
data = JSON.stringify(data);  
//data = require('querystring').stringify(data);  


/**
 * @description: 回调处理数据
 * @param {type} 
 * @return: 
 */
function callback_fun(res){
    res = 1;
    var body = ''; 
    console.log("response: " + res.statusCode + JSON.stringify(res.headers));
    if (res.statusCode == 200){ //响应正常
        res.setEncoding('utf8'); 
        res.on('data',function(data){  
            body += data;  
        }).on('end', function(){ 
            // 数据接收完成 
            console.log(body)  
        });
        res = 0;
    }
    return res;
}


/**
 * @description: 是否能连接上服务器
 * @param {type} 
 * @return: 
 */
// function test_server() {
//     res = 1;
//     // let req = http.request(opt, callback_fun).on('error', function(e) {  
//     //     console.log("error: " + e.message);  
//     // });
//     // //req.write(data);  
//     // req.end(); 
// }

const request = require('request');
console.log(request)

function test() {
    options = {
        url     : "http://www.baidu.com/" 
    };
    
    pro = new Promise(function(resolve, reject){
        // 向服务端发送请求
        request.get(options , function(error,response,body){
            if(error){
                //失败,异常错误抛出
                reject(error);
            }else{
                //完成
                resolve(body);
            }
        });
    });
    return pro;
    
    // pro.then(function(res){
    //     // 如果promise对象调用了resolve方法，则进入该函数逻辑
    //     console.log(res)
    // }, function(err){
    //     // 如果promise对象调用了reject方法，则进入该函数逻辑
    //     console.log(err)
    // })
}

function test_server() {
    url = "http://www.baidu.com/";
    body = await test();
    console.log(body);
}

// test_server()

/**
 * @description: 连接WIFI（使用子进程执行命令）
 * @param {type} 
 * @return: 
 */
// function connect_to_wifi() {
//     res = 1
//     exec('python3 ../qrcode-and-wifi/qrcode_and_wifi/main.py', function (error, stdout, stderr) {
//         if(error){
//           console.log(error.stack);
//           console.log('Error code: '+error.code);
//           console.log('Signal received: '+error.signal);
//           console.error('error: ' + error);
//         }
//         var jsonStr = stderr.toString();
//         console.log('receive: ' + jsonStr);
//         try {
//             var json = JSON.parse(jsonStr);
//             if(0 == json.wifi){
//                 console.log("wifi connected");
//                 res = 0;
//             }
//             console.log(json.time);
//         } catch(e){

//         }finally {
//             return res;
//         }
//     });
// }


/**
 * @description: shell
 * @param {type} 
 * @return: 
 */
function shell(sstr) {
    pro = new Promise(function(resolve, reject){
        exec(sstr, function (error, stdout, stderr) {
            if(error){
                //失败,异常错误抛出
                reject(error, stdout, stderr);
            }else{
                // 逻辑处理完毕且能正常退出
                resolve(error, stdout, stderr);
            }
        });
    });
    return pro;
}


/**
 * @description: 启动main
 * @param {type} 
 * @return: 
 */
async function run_mian() {
    res = await shell('cd ../main-server/build && ./main');
    console.log(res);
}

run_mian()


/**
 * @description: wifi
 * @param {type} 
 * @return: 
 */
async function run_wifi() {
    let {error, stdout, stderr} = await shell('python3 ../qrcode-and-wifi/qrcode_and_wifi/main.py');
    console.log(res);

    if(error){
        console.log(error.stack);
        console.log('Error code: '+error.code);
        console.log('Signal received: '+error.signal);
        console.error('error: ' + error);
    } else {
        var jsonStr = stderr.toString();
        console.log('receive: ' + jsonStr);
        try {
            var json = JSON.parse(jsonStr);
            if(0 == json.wifi){
                console.log("wifi connected");
                res = 0;
            }
            console.log(json.time);
        } catch(e){

        }finally {
            return res;
        }
    }
}


