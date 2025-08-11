//#pragma once
//
//#include <modules/video_capture/video_capture_factory.h>
//#include <modules/desktop_capture/desktop_capturer.h>
//#include <modules/desktop_capture/desktop_capture_options.h>
//
//namespace libWebrtc {
//	namespace internal {
//		class DesktopCapturerImpl : webrtc::DesktopCapturer::Callback
//		{
//		public:
//			static DesktopCapturerImpl* CreateScreenCapturer(webrtc::DesktopCaptureOptions &options)
//			{
//				DesktopCapturerImpl*  impl = new DesktopCapturerImpl();
//				impl->m_desktopCapturer = webrtc::DesktopCapturer::CreateWindowCapturer(options);
//				return impl;
//			}
//
//			void Start()
//			{
//				m_desktopCapturer->Start(this);
//			}
//
//			DesktopCapturerImpl(std::function<void(webrtc::DesktopCapturer::Result, std::unique_ptr<webrtc::DesktopFrame>)> onCaptureResult,
//				std::function<void()> onFrameCaptureStart)
//				: onCaptureResult_(std::move(onCaptureResult)), onFrameCaptureStart_(std::move(onFrameCaptureStart)) {
//			}
//			virtual void OnFrameCaptureStart() override {
//				if (onFrameCaptureStart_)
//					onFrameCaptureStart_();
//			}
//
//			virtual void OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> frame) override
//			{
//				if (onCaptureResult_)
//					onCaptureResult_(result, std::move(frame));
//			}
//
//
//
//		private:
//			DesktopCapturerImpl() {
//			}
//			std::function<void(webrtc::DesktopCapturer::Result, std::unique_ptr<webrtc::DesktopFrame>)> onCaptureResult_;
//			std::function<void()> onFrameCaptureStart_;
//
//			std::unique_ptr<webrtc::DesktopCapturer> m_desktopCapturer = nullptr;
//
//		};
//	} // namespace internal
//}