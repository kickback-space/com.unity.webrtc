#include "pch.h"
#include "UnityVideoTrackSource.h"
#include "VideoFrameBufferCreatorInterface.h"

namespace unity
{
namespace webrtc
{

UnityVideoTrackSource::UnityVideoTrackSource(
    IGraphicsDevice* device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format,
    uint32_t memoryType, bool is_screencast, absl::optional<bool> needs_denoising)
    : AdaptedVideoTrackSource(/*required_alignment=*/1)
    , is_screencast_(is_screencast)
    , needs_denoising_(needs_denoising)
    , m_bufferCreator(VideoFrameBufferCreatorInterface::Create(
        device, ptr, device->GetGfxRenderer(), format, memoryType))
{
//  DETACH_FROM_THREAD(thread_checker_);
}

void UnityVideoTrackSource::Init()
{
    // todo::(kazuki) change compiler vc to clang
#if defined(__clang__)
    // DETACH_FROM_THREAD(thread_checker_);
#endif
    std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
    m_bufferCreator->Init();
}

UnityVideoTrackSource::~UnityVideoTrackSource()
{
    {
        std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
    }
}

UnityVideoTrackSource::SourceState UnityVideoTrackSource::state() const
{
    // TODO(nisse): What's supposed to change this state?
    return MediaSourceInterface::SourceState::kLive;
}

bool UnityVideoTrackSource::remote() const
{
    return false;
}

bool UnityVideoTrackSource::is_screencast() const
{
    return is_screencast_;
}

absl::optional<bool> UnityVideoTrackSource::needs_denoising() const
{
    return needs_denoising_;
}

void UnityVideoTrackSource::OnFrameCaptured(
    scoped_refptr<VideoFrame> frame)
{
    // todo::(kazuki) change compiler vc to clang
#if defined(__clang__)
    // DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
#endif
    const std::unique_lock<std::shared_timed_mutex> lock(m_mutex, std::try_to_lock);
    if (!lock)
    {
        // currently encoding
        return;
    }

    //const rtc::scoped_refptr<VideoFrameBuffer> buffer =
    //    m_bufferCreator->CreateBuffer(m_mutex);

    const int64_t now_us = rtc::TimeMicros();
    const int64_t translated_camera_time_us =
        timestamp_aligner_.TranslateTimestamp(frame->timestamp().InMicroseconds(),
            now_us);

    rtc::scoped_refptr<VideoFrameAdapter> frame_adapter(
        new rtc::RefCountedObject<VideoFrameAdapter>(frame));

    ::webrtc::VideoFrame::Builder builder =
        ::webrtc::VideoFrame::Builder()
        .set_video_frame_buffer(frame_adapter)
        .set_timestamp_us(translated_camera_time_us);

    OnFrame(builder.build());
}

} // end namespace webrtc
} // end namespace unity
