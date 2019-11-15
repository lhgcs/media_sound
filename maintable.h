#ifndef MAINTABLE_H
#define MAINTABLE_H

#include "tableinterface.h"
#include <string.h>

using namespace std;

/**
 * @brief The MainTable class
 * 表格
 * 2019/04/18
 */

class MainTable : public TableInterface
{
public:
    MainTable();

    /**
     * @brief create
     * @param db
     * @return
     * 创建表
     */
    int create(sqlite3 *_db) {
        char temp[1024];
        int ret = -1;
        memset(temp, 0, sizeof(temp));
        sprintf(temp,
"CREATE TABLE IF NOT EXISTS %s(\
id         INTEGER PRIMARY KEY AUTOINCREMENT,\
currentTime  INTEGER NOT NULL DEFAULT 0,\
peopleNo   INTEGER NOT NULL DEFAULT 0,\
x          REAL    NOT NULL DEFAULT 0,\
y          REAL    NOT NULL DEFAULT 0,\
area       INTEGER NOT NULL DEFAULT 0,\
pose       INTEGER NOT NULL DEFAULT 0,\
insertTime INTEGER NOT NULL DEFAULT 0);",name);
        ret = sql_run(_db, temp, NULL);
        return ret;
    }

//    unsigned int id;        //ID
//    unsigned int insertTime;//插入时间
//    unsigned int currentTime;//发生时间
//    unsigned int peopleNo;  //人员序号
//    double x;               //坐标
//    double y;               //坐标
//    unsigned char area;     //区域
//    unsigned char pose;     //姿态

    /**
     * @brief drop
     * @return
     * 删除表
     */
    int drop(sqlite3 *db) {
        char temp[128];
        memset(temp, 0, sizeof(temp));
        sprintf(temp,"DROP TABLE '%s;",name);
        return sql_run(db, temp, NULL);
    }

    /**
     * @brief clear
     * @return
     * 清空表
     */
    int clear(sqlite3 *db) {
        char temp[128];
        memset(temp, 0, sizeof(temp));
        sprintf(temp,"delete from '%s';update sqlite_sequence SET seq = 0 where name ='%s';",name,name);//自增长ID为0
        return sql_run(db, temp, NULL);
    }

    /**
     * @brief insert
     * @param dao
     * 增
     */
    int insert(sqlite3 *db, MainDao *dao) {
        char temp[256];
        memset(temp, 0, sizeof(temp));
        sprintf(temp,"INSERT INTO %s (currentTime,peopleNo,x,y,area,pose,insertTime) VALUES(%d,%d,%f,%f,%d,%d,%d);",
                name,
                dao->currentTime,
                dao->peopleNo,
                dao->x,
                dao->y,
                dao->area,
                dao->pose,
                dao->insertTime);
        printf("\n%s\n",temp);
        return sql_run(db, temp, NULL);
    }

    /**
     * @brief remove
     * @param id
     * 删elete
     */
    int remove(sqlite3 *db,unsigned int id) {
        char temp[128];
        memset(temp, 0, sizeof(temp));
        sprintf(temp,"DELETE FROM '%s' WHERE id = %d;",name,id);
        return sql_run(db, temp, NULL);
    }

    /**
     * @brief update
     * @param db
     * @param dao
     * @return
     * 改
     */
    int update(sqlite3 *db,MainDao *dao) {
        char temp[256];
        memset(temp, 0, sizeof(temp));
        sprintf(temp,"UPDATE '%s' SET currentTime = %d, peopleNo = %d, x = %f, y = %f, area = %d, pose = %d, insertTime = %d WHERE id = %d;",
                name,
                dao->currentTime,
                dao->peopleNo,
                dao->x,
                dao->y,
                dao->area,
                dao->pose,
                dao->insertTime,
                dao->id);
        return sql_run(db, temp, NULL);
    }

    /**
     * @brief select
     * @param db
     * @param li
     * @return
     * 查
     */
    int select(sqlite3 *db,list<MainDao> *li) {
        int ret = 0;
        int i;
        int row = 0;
        int col = 0;
        char **azResult = NULL; //二维数组存放结果
        char *errMsg = NULL;

        char temp[128];
        memset(temp, 0, sizeof(temp));
        sprintf(temp,"SELECT * from %s;",name);//LINIT 1000

        while (!li->empty()) {
            li->pop_back();//删除最后一个元素
            //li->clear();
        }

        ret = sqlite3_get_table( db , temp , &azResult , &row , &col , &errMsg );

        printf("row=%d col=%d\n",row,col);
        printf("the result is:\n");

        for(i=col; i<(row+1)*col; i+=8) {
            MainDao dao;
            dao.id          = atoi(azResult[i+0]);
            dao.currentTime = atoi(azResult[i+1]);
            dao.peopleNo    = atoi(azResult[i+2]);
            dao.x           = atof(azResult[i+3]);
            dao.y           = atof(azResult[i+4]);
            dao.area        = atoi(azResult[i+5]);
            dao.pose        = atoi(azResult[i+6]);
            dao.insertTime  = atoi(azResult[i+7]);
            li->push_back(dao);//在list的末尾添加一个元素
        }


        for(i=0;i<(row+1)*col;i++) {
            printf("azResult[%d]=%s\n",i,azResult[i]);
        }

        sqlite3_free_table(azResult);
        return ret;
    }

};

#endif // MAINTABLE_H
