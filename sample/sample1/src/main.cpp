#include <modules/video_capture/video_capture_options.h>

#include <api/peer_connection_interface.h>
#include <api/peer_connection_interface.h>
#include <api/create_peerconnection_factory.h>
#include <api/task_queue/default_task_queue_factory.h>
#include <api/environment/environment_factory.h>
#include <api/audio/create_audio_device_module.h>
#include <api/packet_socket_factory.h>
#include <rtc_base/win32_socket_init.h>
#include <rtc_base/physical_socket_server.h>
#include <rtc_base/ssl_adapter.h>
#include <api/audio/audio_mixer.h>
#include <api/audio/builtin_audio_processing_builder.h>
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <api/video_codecs/video_encoder_factory_template.h>
#include <api/video_codecs/video_decoder_factory_template.h>
#include <api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h>
#include <api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h>
#include <api/video_codecs/video_decoder_factory_template_open_h264_adapter.h>
#include <api/video_codecs/h265_profile_tier_level.h>
#include <api/video_codecs/video_decoder_factory_template_dav1d_adapter.h>
#include <api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h>
#include <api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h>
#include <api/video_codecs/video_encoder_factory_template_open_h264_adapter.h>
#include <api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h>
#include <api/video_codecs/video_encoder_factory_template.h>
#include <api/enable_media.h>
#include <api/enable_media_with_defaults.h>
#include <iostream>
#include <pc/video_track_source.h>
#include <pc/media_stream.h>
#include <media/base/video_broadcaster.h>
#include <modules/video_capture/video_capture_factory.h>
#include <modules/desktop_capture/desktop_capturer.h>
#include <modules/desktop_capture/desktop_capture_options.h>

#include "Enities.h"
#include "LambaObserver.h"
#include <httplib.h>
class FakeVideoTrackSource : public webrtc::VideoTrackSource {
public:
	static webrtc::scoped_refptr<FakeVideoTrackSource> Create(bool is_screencast) {
		return webrtc::make_ref_counted<FakeVideoTrackSource>(is_screencast);
	}

	static webrtc::scoped_refptr<FakeVideoTrackSource> Create() { return Create(false); }

	bool is_screencast() const override { return is_screencast_; }

	void InjectFrame(const webrtc::VideoFrame& frame) {
		video_broadcaster_.OnFrame(frame);
	}

protected:
	explicit FakeVideoTrackSource(bool is_screencast)
		: webrtc::VideoTrackSource(false /* remote */), is_screencast_(is_screencast) {
	}
	~FakeVideoTrackSource() override = default;

	webrtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
		return &video_broadcaster_;
	}

private:
	const bool is_screencast_;
	webrtc::VideoBroadcaster video_broadcaster_;
};

int main(int argc, char **argv) {

	webrtc::WinsockInitializer winsock_init;
	webrtc::PhysicalSocketServer ss;
	webrtc::AutoSocketServerThread main_thread(&ss);

	webrtc::Environment env = webrtc::CreateEnvironment();
	webrtc::PeerConnectionFactoryDependencies dependencies;
	webrtc::InitializeSSL();
	auto signaling_thread_ = webrtc::Thread::CreateWithSocketServer();
	signaling_thread_->Start();
	dependencies.signaling_thread = signaling_thread_.get();
	webrtc::EnableMediaWithDefaults(dependencies);



	webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreateModularPeerConnectionFactory(std::move(dependencies));

	webrtc::PeerConnectionFactoryInterface::Options options;
	factory->SetOptions(options);
	webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream = factory->CreateLocalMediaStream("stream_id");

	auto s = stream->GetVideoTracks();

	webrtc::PeerConnectionInterface::RTCConfiguration configuration;
	configuration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
	//webrtc::PeerConnectionInterface::IceServer server;
	//server.uri = "stun:stun.l.google.com:19302";
	//configuration.servers.push_back(server);


	auto observer = libWebrtc::lambda::PeerConnection::Create();
	observer->SetOnIceCandidate([](const webrtc::IceCandidate* candidate) {
		std::string sdp;
		if (candidate->ToString(&sdp)) {
			std::cout << "New ICE candidate: " << sdp << std::endl;
		} else {
			std::cerr << "Failed to convert ICE candidate to string." << std::endl;
		}
	});

	webrtc::PeerConnectionDependencies pcDependencies(observer.get());

	auto result = factory->CreatePeerConnectionOrError(configuration, std::move(pcDependencies));

	if(!result.ok())
	{
		std::cerr << "Failed to create PeerConnection: " << result.error().message() << std::endl;
		return -1;
	}
	
	auto pc = result.MoveValue();

	auto deviceInfo = webrtc::VideoCaptureFactory::CreateDeviceInfo(); // This is just to ensure the factory is initialized, not used directly.

	int number = deviceInfo->NumberOfDevices(); // This will enumerate the available video capture devices.


	webrtc::DesktopCaptureOptions desktopCaptureOptions = webrtc::DesktopCaptureOptions::CreateDefault();





	std::unique_ptr<webrtc::DesktopCapturer> desktopCapture =  webrtc::DesktopCapturer::CreateWindowCapturer(desktopCaptureOptions);

	webrtc::DesktopCapturer::SourceList sources;

	desktopCapture->GetSourceList(&sources);
	desktopCapture->SelectSource(sources[0].id);
	desktopCapture->Start(new libWebrtc::lambda::DesktopCapturer(
		[](webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> desktopFrame) {
			std::cout << "Capture result: " << static_cast<int>(result) << std::endl;
		},
		[]() {
			std::cout << "Capture Start" << std::endl;
		}
	));


	desktopCapture->CaptureFrame();

	webrtc::AudioOptions audioOptions;
	audioOptions.highpass_filter = false;
	webrtc::scoped_refptr<webrtc::AudioSourceInterface> source = factory->CreateAudioSource(audioOptions);
	webrtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(factory->CreateAudioTrack("audio_local", source.get()));
	//stream->AddTrack(audio_track);
	webrtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack = factory->CreateVideoTrack(FakeVideoTrackSource::Create(), "video_local");
	//stream->AddTrack(videoTrack);



	pc->AddTrack(audio_track, { "stream_id" });
	pc->AddTrack(videoTrack, { "stream_id" });


	httplib::Headers headers = {
	{"Content-Type", "application/sdp"},
	{"Accept", "*"},
	{"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3"} };

	httplib::Client* httpClient = new httplib::Client("127.0.0.1", 1985);


	const auto remoteDescriptionObserverLambda = libWebrtc::lambda::SetRemoteDescription::Create(
		[](webrtc::RTCError error) {
			std::cerr << "Failed to set remote description: " << error.message() << std::endl;
	});

	const auto localDescriptionObserverLambda = libWebrtc::lambda::SetLocalDescription::Create(
		[&headers, &httpClient, &pc, &remoteDescriptionObserverLambda](webrtc::RTCError error)
		{
			if(error.ok())
			{
				std::string sdp;
				if (pc->local_description()->ToString(&sdp)) {
					auto result = httpClient->Post("/rtc/v1/whip/?app=live&stream=livestream4.flv", headers, std::string(sdp), "application/sdp");//

					if (!result) {
						std::cerr << "Failed to send offer: " << result.error() << std::endl;
						return;
					}
					std::cout << "---------------------------------------------" << std::endl;
					std::cout << result.value().body << std::endl;
					pc->SetRemoteDescription(webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, result.value().body), remoteDescriptionObserverLambda);
				}
			}
			else
			{
				std::cerr << "Failed to convert offer to string." << std::endl;
			}
		}
	);


	auto offerObserver = libWebrtc::lambda::CreateSessionDescription::Create(
		[&localDescriptionObserverLambda, & pc](webrtc::SessionDescriptionInterface* desc) {
			std::string sdp;
			if (desc->ToString(&sdp)) {
				std::cout << "Offer created successfully: " << sdp << std::endl;
				std::cout << "------------------------------------------------------------------" << std::endl;
				pc->SetLocalDescription(localDescriptionObserverLambda);
			} else {
				std::cerr << "Failed to convert offer to string." << std::endl;
			}
		},
		[](webrtc::RTCError error) {
			std::cerr << "Failed to create offer: " << error.message() << std::endl;
		}
	);

	webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offerOptions;
	offerOptions.offer_to_receive_audio = 0;
	offerOptions.offer_to_receive_video = 0;

	pc->CreateOffer(offerObserver.get(), offerOptions);


	getchar();
	webrtc::CleanupSSL();
	return 0;
}