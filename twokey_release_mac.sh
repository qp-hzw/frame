#!/bin/bash

export LC_COLLATE='C'
export LC_CTYPE='C'
###################################################
##                    main
###################################################

g_FrameDependence='/Users/clay/share/依赖项'
g_ReleasePath='/Users/clay/share/quyou-test'
g_CodePath='/Users/clay/Desktop/server/frame_server/'

g_Frame_dll='运行依赖'

g_ReadMe=REAME.txt

## if exist, remove error file && temp file
if [ -d ${g_ReleasePath} ]; then
rm -rf ${g_ReleasePath}
fi

if [ -f ${g_ReleasePath}/${g_ReadMe} ]; then
rm ${g_ReleasePath}/${g_ReadMe}
fi

mkdir ${g_ReleasePath}

## kernel中的运行依赖
cp -r ${g_FrameDependence}/${g_Frame_dll}/ ${g_ReleasePath}


## frame的release项目
cp -r ${g_CodePath}/发布组件/Release/ ${g_ReleasePath}

# 说明文件
echo -e "release发布 自动化脚本\n" >> ${g_ReleasePath}/${g_ReadMe}
echo -e `git log -1` >>  ${g_ReleasePath}/${g_ReadMe}
echo -e "\nauthor:  " >> ${g_ReleasePath}/${g_ReadMe}
echo "WangChengQing    " >>  ${g_ReleasePath}/${g_ReadMe}
echo -e "\ndate:  " >> ${g_ReleasePath}/${g_ReadMe}
echo `date`  >> ${g_ReleasePath}/${g_ReadMe}
