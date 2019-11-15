#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "libcurluril.h"


INCLUDEPATH +=/usr/include/curl
LIBS +=-L/usr/lib/x86_64-linux-gnu/ -lcurl

INCLUDEPATH +=/usr/include/jsoncpp
LIBS +=-L/usr/lib/x86_64-linux-gnu/ -ljsoncpp




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{
//    while (1) {
//        struct RequestsInfo info;
//        info.type = GET;
//        info.url = "https:\/\/www.baidu.com";
//        info.params = NULL;
//        info.fun = NULL;
//        start_thread(&info);
//        usleep(500000);
//    }

    struct RequestsInfo info[3];
    info[0].type = GET;
    info[0].url = "http://curl.haxx.se/";
    info[0].params = NULL;
    info[0].fun = NULL;

    info[1].type = GET;
    info[1].url = "http://www.contactor.se/";
    info[1].params = NULL;
    info[1].fun = NULL;

    info[2].type = GET;
    info[2].url = "https:\/\/www.baidu.com";
    info[2].params = NULL;
    info[2].fun = NULL;

    http_multi((struct RequestsInfo *)&info,3);
}

#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
using namespace std;
using namespace Json;


struct info{
    int code1;
};

struct uniform{
    std::string code;
    std::string msg;
    std::string adviceOpt;
    struct info data;
};

void MainWindow::on_pushButton_2_clicked()
{
    struct uniform ret;
    Json::Reader reader;
    Json::Value value;

    //要解析的json文本
    string text = "{\"code\": \"0000\",\"msg\": \"成功\",\"adviceOpt\": [\"006\"],\"data\": {\"code1\":0}}";
    //解析
    if (reader.parse(text.c_str(), value)){
        //包含字段
        if (value.isMember("code") &&
            value.isMember("msg") &&
            value.isMember("data")){

            ret.code = value["code"].asString();
            ret.msg = value["msg"].asString();

            //多层嵌套
            ret.data.code1 = value["data"]["code1"].asInt();
            //Json::Value data = value["data"];
            //ret.data.code1 = data["code1"].asInt();

            //解析对象数组对象
            const Json::Value arrayObj = value["adviceOpt"];
            for (int i = 0; i < arrayObj.size(); ++i) {
                //arrayObj[i].asString();
            }

            printf("%s %s %d\n",ret.code.c_str(), ret.msg.c_str(), ret.data.code1);
        }

        //获取字段
        Json::Value::Members member = value.getMemberNames();
            for(Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter){
            string temp = *iter;
            printf("%s\n",temp.c_str());
        }

        //生成json
        Json::Value root;
        root["code"] = "0000";
        root["msg"] = "成功";

        Json::Value son;
        son["code1"] = 1;
        root["data"] = son;

        Json::Value arrayObj;   // 构建对象
        arrayObj[0] = "000";
        arrayObj[1] = "001";
        arrayObj[2] = "002";
        root["keylist"] = arrayObj;

        string str = root.toStyledString();
        printf("%s\n",str.c_str());

    }else{
        printf("pares error\n");
    }
}
