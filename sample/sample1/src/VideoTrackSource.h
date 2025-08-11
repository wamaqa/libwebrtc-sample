#pragma once
#include <api/video/video_source_interface.h>
#include <pc/video_track_source.h>

namespace libWebrtc {
	class DesktopCapturerVideoTrackSource : public webrtc::VideoTrackSourceInterface {
	public:
		virtual bool is_screencast()  const override {};


		virtual bool remote() const override {};

		// Indicates that the encoder should denoise video before encoding it.
		// If it is not set, the default configuration is used which is different
		// depending on video codec.
		// TODO(perkj): Remove this once denoising is done by the source, and not by
		// the encoder.
		virtual std::optional<bool> needs_denoising() const override {};

		// Returns false if no stats are available, e.g, for a remote source, or a
		// source which has not seen its first frame yet.
		//
		// Implementation should avoid blocking.
		virtual bool GetStats(Stats* stats) override {};

		// Returns true if encoded output can be enabled in the source.
		virtual bool SupportsEncodedOutput() const override {};

		// Reliably cause a key frame to be generated in encoded output.
		// TODO(bugs.webrtc.org/11115): find optimal naming.
		virtual void GenerateKeyFrame() override {};

		// Add an encoded video sink to the source and additionally cause
		// a key frame to be generated from the source. The sink will be
		// invoked from a decoder queue.
		virtual void AddEncodedSink(
			webrtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>* sink) override {
		};

		// Removes an encoded video sink from the source.
		virtual void RemoveEncodedSink(
			webrtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>* sink) override {
		};

		// Notify about constraints set on the source. The information eventually gets
		// routed to attached sinks via VideoSinkInterface<>::OnConstraintsChanged.
		// The call is expected to happen on the network thread.
		// TODO(crbug/1255737): make pure virtual once downstream project adapts.
		virtual void ProcessConstraints(
			const webrtc::VideoTrackSourceConstraints& /* constraints */) override {
		};


	};
} // namespace libWebrtc