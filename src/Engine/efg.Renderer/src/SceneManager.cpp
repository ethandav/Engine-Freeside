#include "..\include\SceneManager.h"

void SceneManager::AddRenderObjectToScene(RenderObject object)
{
	m_renderObjects.push_back(object);
}