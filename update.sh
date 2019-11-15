#!/bin/bash

###
 # @Description: 更新应用
 # @Author: your name
 # @Date: 2019-07-25 12:01:23
 # @LastEditTime: 2019-07-25 16:48:15
 # @LastEditors: Please set LastEditors
###

# 下载文件路径
DOWNLOAD_PATH="/home/rock/download"
# 备份文件路径
BACKUP_PATH="/home/rock/backup"
# 应用路径
PROGRAME_PATH="/home/rock/ud"

# 备份文件名称
BACKUP_NAME="backup.tar"
# 下载文件名称
DOWNLOAD_NAME="update.tar"


###
 # @description: 如果目录不存在，创建目录
 # @param {type} 
 # @return: 
###
function create_dir() {
    if [ ! -d $1 ]; then
        mkdir -p $1
    fi
}


###
 # @description: 清空目录下的所有文件（目录还在）
 # @param {type} 
 # @return: 
###
function clear_dir() {
    if [ -d $1 ]; then
        rm -rf $1/*
    fi
}


###
 # @description: 生成md5
 # @param {type} 
 # @return: 
###
function gennerate_md5() {
    if [ -f $1 ]; then
        md5sum $1 > $1.md5  #将MD5码写入md5文件
    fi
}


###
 # @description: 校验md5
 # @param {type} 
 # @return: 
###
function check_md5() {
    if [ -f $1 ]; then
        # 从指定文本中读取md5值，然后检测MD5值对应的文件是否完整
        res=$(md5sum --check $1.md5 | grep "OK")
        if [ ! -z "${res}" ]; then
            return 0
        fi
    fi
    return 1
}


###
 # @description: 备份
 # @param {type} 
 # @return: 
###
function backup() {
    temp="${BACKUP_PATH}/temp.tar"
    tar -cf ${temp} ${PROGRAME_PATH}
    if [ $? == 0 ]; then
        mv ${temp} ${BACKUP_PATH}/${BACKUP_NAME}
        gennerate_md5 ${BACKUP_PATH}/${BACKUP_NAME}
        return 0
    else
        rm -rf ${temp}
    fi
    return 1
}


###
 # @description: 更新程序
 # @param {type} 
 # @return: 
###
function update() {
    filePath="${DOWNLOAD_PATH}/${DOWNLOAD_NAME}"
    if [ -f ${filePath} ]; then

        check_md5 ${filePath}
        if [ $? == 0 ]; then

            # 删除旧程序
            clear_dir ${PROGRAME_PATH}
            # 解压
            tar -xf  ${DOWNLOAD_PATH}/${DOWNLOAD_NAME} -C ${PROGRAME_PATH}
            if [ $? == 0 ]; then
                return 0
            fi
        fi

    fi

    return 1
}


###
 # @description: 数据恢复
 # @param {type} 
 # @return: 
###
function recovery() {
    filePath="${DOWNLOAD_PATH}/${DOWNLOAD_NAME}"
    if [ -f ${filePath} ]; then

        check_md5 ${filePath}
        if [ $? == 0 ]; then

            # 删除程序
            clear_dir ${PROGRAME_PATH}
            # 解压
            tar -xf  ${BACKUP_PATH}/${BACKUP_NAME} -C ${PROGRAME_PATH}
            if [ $? == 0 ]; then
                return 0
            fi
            
        fi
        
    fi

    return 1
}



###################### run ######################

create_dir ${BACKUP_PATH}
create_dir ${DOWNLOAD_PATH}

downFile="${DOWNLOAD_PATH}/${DOWNLOAD_NAME}"
if [ -f ${downFile} ]; then
    backup
    if [ $? == 0 ]; then
        update
        if [ $? != 0 ]; then
            recovery
        fi
        clear_dir ${DOWNLOAD_PATH}
    fi
fi

exit 0