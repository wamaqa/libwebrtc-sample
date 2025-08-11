#include <httplib.h>
#include <libwebrtc.h>
#include <rtc_peerconnection.h>
#include <rtc_mediaconstraints.h>
#include <rtc_desktop_capturer.h>
#include <rtc_desktop_media_list.h>

int main(int argc, char** argv) {
	libwebrtc::LibWebRTC::Initialize();
	libwebrtc::scoped_refptr<libwebrtc::RTCPeerConnectionFactory> factory = libwebrtc::LibWebRTC::CreateRTCPeerConnectionFactory();
	libwebrtc::RTCConfiguration configuration;
	libwebrtc::scoped_refptr<libwebrtc::RTCMediaConstraints> pcConstraints = libwebrtc::RTCMediaConstraints::Create();

	configuration.offer_to_receive_audio = false;
	configuration.offer_to_receive_video = false;
	configuration.local_video_bandwidth = 51200;
	auto pc = factory->Create(configuration, pcConstraints);

	auto videoDevice = factory->GetDesktopDevice();

	libwebrtc::scoped_refptr<libwebrtc::RTCRtpCapabilities> senderCapabilities = factory->GetRtpSenderCapabilities(libwebrtc::RTCMediaType::VIDEO);
	auto& codes = senderCapabilities->codecs();
	std::cout << "---------------------------------------------" << std::endl;

	for (size_t i = 0; i < codes.size(); i++)
	{
		auto& code = codes[i];
		std::cout << "mime_type :" << code->mime_type().std_string() << std::endl;
		std::cout << "clock_rate:" << code->clock_rate() << std::endl;
		std::cout << "sdp_fmtp_line:" << code->sdp_fmtp_line().std_string() << std::endl;
	}
	std::cout << "---------------------------------------------" << std::endl;


	libwebrtc::scoped_refptr<libwebrtc::RTCDesktopMediaList> mediaList = videoDevice->GetDesktopMediaList(libwebrtc::DesktopType::kScreen);
	mediaList->UpdateSourceList(true, true);
	int sourceCount = mediaList->GetSourceCount();
	std::cout << "---------------------------------------------" << std::endl;
	for (size_t i = 0; i < sourceCount; i++)
	{
		auto source = mediaList->GetSource(i);
		std::cout << "id :" << source->id().std_string() << std::endl;
		std::cout << "name:" << source->name().std_string() << std::endl;
	}
	std::cout << "---------------------------------------------" << std::endl;
	libwebrtc::scoped_refptr<libwebrtc::MediaSource> videoMediaSource = mediaList->GetSource(0);
	libwebrtc::scoped_refptr<libwebrtc::RTCDesktopCapturer> videoCapturer = videoDevice->CreateDesktopCapturer(videoMediaSource);
	videoCapturer->Start(60, 0, 0, 1920, 1080);
	auto videoSource = factory->CreateDesktopSource(videoCapturer, "video_source", libwebrtc::RTCMediaConstraints::Create());

	auto videoTrack = factory->CreateVideoTrack(videoSource, "video_track");

	std::vector<std::string> streamIds = { "stream_id" };
	pc->AddTrack(videoTrack, streamIds);

	auto audioSource = factory->CreateAudioSource("audio_source", libwebrtc::RTCAudioSource::SourceType::kMicrophone);
	auto audioTrack = factory->CreateAudioTrack(audioSource, "audio_track");
	pc->AddTrack(videoTrack, streamIds);

	httplib::Headers headers = {
	{"Content-Type", "application/sdp"},
	{"Accept", "*"},
	{"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3"} };

	httplib::Client* httpClient = new httplib::Client("127.0.0.1", 1985);

	libwebrtc::OnSetSdpSuccess  remoteSuccess = [&pc, &httpClient, &headers]() {

		std::cout << "---------------------------------------------" << std::endl;
		};

	libwebrtc::OnGetSdpSuccess localGetSuccess = [&pc, &httpClient, &headers, &remoteSuccess](const char* sdp, const char* type) {

		auto result = httpClient->Post("/rtc/v1/whip/?app=live&stream=livestream4.flv", headers, std::string(sdp), "application/sdp");//

		if (!result) {
			std::cerr << "Failed to send offer: " << result.error() << std::endl;
			return;
		}
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << result.value().body << std::endl;
		libwebrtc::string remoteSdp = result.value().body.c_str();
		pc->SetRemoteDescription(remoteSdp, "answer", remoteSuccess, [](const char* error) {
			std::cerr << "Failed to create offer: " << error << std::endl;
			});
		};
	libwebrtc::OnSetSdpSuccess localSuccess = [&pc, &httpClient, &headers, &localGetSuccess]() {
		std::cout << "local description set: ------------------------------------------\n" << std::endl;

		pc->GetLocalDescription(localGetSuccess,
			[](const char* error) {
				std::cerr << "Failed to get local description: " << error << std::endl;
			});
		};

	libwebrtc::OnSdpCreateSuccess offerSuccess = [&pc, &localSuccess](const libwebrtc::string sdp, const libwebrtc::string type) {
		std::cout << "Offer created: ------------------------------------------\n" << sdp.std_string() << std::endl;
		pc->SetLocalDescription(sdp, type, localSuccess,
			[](const char* error) {
				std::cerr << "Failed to set local description: " << error << std::endl;
			});
		};
	libwebrtc::OnSdpCreateFailure offerError = [](const char* error) {
		std::cerr << "Failed to create offer: " << error << std::endl;
		};

	libwebrtc::scoped_refptr<libwebrtc::RTCMediaConstraints> offerConstraints = libwebrtc::RTCMediaConstraints::Create();

	pc->CreateOffer(offerSuccess, offerError, libwebrtc::RTCMediaConstraints::Create());


   
	getchar();
	//libwebrtc::LibWebRTC::Terminate();
	return 0;
}