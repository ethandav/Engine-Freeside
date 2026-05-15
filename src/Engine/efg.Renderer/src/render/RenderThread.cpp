#include "..\..\include\render\RenderThread.h"
#include "..\..\include\render\IRendererBackend.h"

#include <windows.h>

namespace efg
{
    void RenderThread::Start(d3d12::IRendererBackend* backend)
    {
        m_backend = backend;
        m_running = true;
        m_thread = std::thread(&RenderThread::ThreadMain, this);
    }

    void RenderThread::Stop()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_running = false;
        }

        m_hasWorkCv.notify_one();
        m_hasSpaceCv.notify_one();

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void RenderThread::ThreadMain()
    {
        SetThreadDescription(GetCurrentThread(), L"Render Thread");
        while (true)
        {
            efg::FramePacket sceneData;

            {
                std::unique_lock<std::mutex> lock(m_mutex);

                m_hasWorkCv.wait(lock, [&]()
                    {
                        return !m_running || !m_frameQueue.empty();
                    });

                if (!m_running && m_frameQueue.empty())
                    break;

                sceneData = std::move(m_frameQueue.back());
                m_frameQueue.clear();

                lock.unlock();
                m_hasSpaceCv.notify_one();
            }

            m_backend->Render(sceneData);
        }

        //m_backend->WaitForGPU();
    }

    void RenderThread::Submit(efg::FramePacket sceneData)
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
}