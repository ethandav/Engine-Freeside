#include "..\..\include\RenderThread.h"
#include "..\..\include\IRendererBackend.h"

#include <windows.h>

void RenderThread::Start(IRendererBackend* backend)
{
    m_backend = backend;
    m_running = true;
    m_thread = std::thread(&RenderThread::ThreadMain, this);
    SetThreadDescription(GetCurrentThread(), L"Render Thread");
}

void RenderThread::Stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_running = false;
    }

    m_hasSpaceCv.notify_one();

    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void RenderThread::ThreadMain()
{
    while (true)
    {
        SceneRenderData sceneData;

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_hasWorkCv.wait(lock, [&]()
                {
                    return !m_running || !m_frameQueue.empty();
                });

            if (!m_running && m_frameQueue.empty())
                break;

            sceneData = std::move(m_frameQueue.front());
            m_frameQueue.pop_front();

            lock.unlock();
            m_hasSpaceCv.notify_one();
        }

        m_backend->Render(sceneData);
    }

    //m_backend->WaitForGPU();
}

void RenderThread::Submit(SceneRenderData sceneData)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_hasSpaceCv.wait(lock, [&]()
        {
            return !m_running || m_frameQueue.size() < MaxQueuedFrames;
        });

    if (!m_running)
        return;

    m_frameQueue.push_back(std::move(sceneData));

    lock.unlock();
    m_hasWorkCv.notify_one();
}