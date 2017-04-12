// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
//#include <agile.h>
#include <concrt.h>
//#include <collection.h>

#include "Content\SphereScene.h"
#include "Content\Pref_IS_IBL.h"
#include "Content\Pref_PBR_DS_IS_IBL.h"
#include "Content\BP_DS_ML.h"
#include "Content\Origin_PBR_ML.h"
#include "Content\SphereAppMain.h"


// TODO: 在此处引用程序需要的其他头文件
