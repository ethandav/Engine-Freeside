#pragma once

#include "types/FramePacket.h"

#include <thread>
#include <mutex>
#include <deque>

namespace efg
{
    namespace d3d12
    {
        class IRendererBackend;
    }

    class RenderThread
    {
    public:
        void Start(d3d12::IRendererBackend* backend);
        void Stop();
        void Submit(FramePacket sceneData);

    private:
        void ThreadMain();

    private:
        static constexpr uint32_t MaxQueuedFrames = 2;

        d3d12::IRendererBackend* m_backend = nullptr;

        std::thread m_thread;
        std::mutex m_mutex;
        std::condition_variable m_hasWorkCv;
        std::condition_variable m_hasSpaceCv;

        std::deque<FramePacket> m_frameQueue;

        bool m_running = false;
    };
}