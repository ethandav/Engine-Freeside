#pragma once

#include <thread>
#include <mutex>
#include <deque>
#include "RenderTypes.h"

class IRendererBackend;

class RenderThread
{
public:
    void Start(IRendererBackend* backend);
    void Stop();
    void Submit(efg::FramePacket sceneData);

private:
    void ThreadMain();

private:
    static constexpr uint32_t MaxQueuedFrames = 2;

    IRendererBackend* m_backend = nullptr;

    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_hasWorkCv;
    std::condition_variable m_hasSpaceCv;

    std::deque<efg::FramePacket> m_frameQueue;

    bool m_running = false;
};