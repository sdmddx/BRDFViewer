// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� COMMON_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// COMMON_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef COMMON_EXPORTS
#define COMMON_API __declspec(dllexport)
#else
#define COMMON_API __declspec(dllimport)
#endif

#include "Math\Hash.h"
#include "Math\MathHelper.h"

#include "AppMain\WindowCreator.h"

#include "DXDevice\DirectXHelper.h"
#include "DXDevice\DXResources.h"

#include "Utilities\StepTimer.h"
#include "Utilities\ShaderStructure.h"
#include "Utilities\ShaderLoader.h"
#include "Utilities\SdmCamera.h"
#include "Utilities\DXBufferCreator.h"

#include "Scene\SceneBase.h"


#include "Graphics\AssetHelper.h"
#include "Graphics\BufMesh.h"
#include "Graphics\DataBuf.h"
#include "Graphics\SphereGen.h"



#include "AppMain\AppResources.h"
#include "AppMain\AppMain.h"

