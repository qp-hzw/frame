#!/bin/bash

###################################################
##                    main
###################################################

g_FrameDependence='依赖项'
g_SubDependence='子依赖项'

g_Frame_kernel='内核引擎'
g_Frame_service='服务核心'
g_Frame_global='全局定义'
g_Frame_lib='链接库'

g_Sub_kernel='内核引擎'
g_Sub_service='服务核心'
g_Sub_global='全局定义'
g_Sub_gameser='游戏服务'
g_Sub_message='消息定义'

g_ReadMe=REAME.txt

## if exist, remove error file && temp file
if [ -d ${g_SubDependence} ]; then
rm -rf ${g_SubDependence}
fi

if [ -f ${g_SubDependence}/${g_ReadMe} ]; then
rm ${g_SubDependence}/${g_ReadMe}
fi

mkdir ${g_SubDependence}
cd ${g_SubDependence}
mkdir ${g_Sub_kernel}
mkdir ${g_Sub_service}
mkdir ${g_Sub_gameser}
mkdir ${g_Sub_global}
mkdir ${g_Sub_message}
cd ../

##全局定义
cp -r ${g_FrameDependence}/${g_Frame_global}/*.h ${g_SubDependence}//${g_Sub_global}
## - -删除文件最后三行
sed -i '$d' ${g_SubDependence}/${g_Sub_global}/Version.h
echo -e '\n#define SUB_INCLUDE_IMPORT_FILES\n\n#endif' >> ${g_SubDependence}/${g_Sub_global}/Version.h
cp -r 全局定义/Struct.h ${g_SubDependence}//${g_Sub_global}/Struct_Frame.h

## 消息定义
cp -r 消息定义/*.h ${g_SubDependence}/${g_Sub_message}

## 内核引擎
cp ${g_FrameDependence}/${g_Frame_kernel}/KernelEngineHead.h ${g_SubDependence}/${g_Sub_kernel}/
cp ${g_FrameDependence}/${g_Frame_kernel}/MSADO15.dll ${g_SubDependence}/${g_Sub_kernel}/

## 服务核心
cp ${g_FrameDependence}/${g_Frame_service}/ServiceCoreHead.h ${g_SubDependence}/${g_Sub_service}/
cp ${g_FrameDependence}/${g_Frame_service}/WHDataQueue.h ${g_SubDependence}/${g_Sub_service}/

## 游戏服务
cp 服务器组件/游戏服务/GameServiceHead.h ${g_SubDependence}/${g_Sub_gameser}/

# 说明文件
cat ${g_FrameDependence}/${g_ReadMe} > ${g_SubDependence}/${g_ReadMe}

echo -e "\n\nSubGame依赖项 自动化脚本" >> ${g_SubDependence}/${g_ReadMe}
sed -n '/CODE_VERSION:/p' ${g_SubDependence}/${g_Sub_global}/Version.h >> ${g_SubDependence}/${g_ReadMe}
sed -n '/KERNEL_VERSION:/p' ${g_SubDependence}/${g_Sub_global}/Version.h >> ${g_SubDependence}/${g_ReadMe}
sed -n '/PLATFORM_VERSION:/p' ${g_SubDependence}/${g_Sub_global}/Version.h >> ${g_SubDependence}/${g_ReadMe}
echo "author:  " >> ${g_SubDependence}/${g_ReadMe}
echo "WangChengQing    " >>  ${g_SubDependence}/${g_ReadMe}
echo "date:  " >> ${g_SubDependence}/${g_ReadMe}
echo `date`  >> ${g_SubDependence}/${g_ReadMe}

## msg
echo -e "Everything is ready, enjoy it!\n"
echo `date`
