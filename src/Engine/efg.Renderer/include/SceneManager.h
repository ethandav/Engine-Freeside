#pragma once
#include "..\include\Renderer.h"

class SceneManager
{
public:
	void AddRenderObjectToScene(RenderObject obj);
private:
	std::vector<RenderObject> m_renderObjects;
};