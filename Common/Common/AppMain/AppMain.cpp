#include "stdafx.h"

using namespace App;

AppMain::AppMain(const std::shared_ptr<DX::DXResources>& dxResources) :
	m_dxResources(dxResources)
{

};

void AppMain::GetScene(const shared_ptr<Scene::SceneBase>& scene) 
{
	m_scene = scene;
}