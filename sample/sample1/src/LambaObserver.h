
namespace libWebrtc {

	namespace lambda {

		class DesktopCapturer :public webrtc::DesktopCapturer::Callback
		{
		public:
			DesktopCapturer(std::function<void(webrtc::DesktopCapturer::Result, std::unique_ptr<webrtc::DesktopFrame>)> onCaptureResult,
				std::function<void()> onFrameCaptureStart)
				: onCaptureResult_(std::move(onCaptureResult)), onFrameCaptureStart_(std::move(onFrameCaptureStart)) {
			}
			~DesktopCapturer() {
				
			};
			virtual void OnFrameCaptureStart() override {
				if (onFrameCaptureStart_)
					onFrameCaptureStart_();
			}

			virtual void OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> frame) override
			{
				if (onCaptureResult_)
					onCaptureResult_(result, std::move(frame));
			}

			static std::unique_ptr<DesktopCapturer> Create(std::function<void(webrtc::DesktopCapturer::Result, std::unique_ptr<webrtc::DesktopFrame>)> onCaptureResult,
				std::function<void()> onFrameCaptureStart) {
				return std::make_unique<DesktopCapturer>(std::move(onCaptureResult), std::move(onFrameCaptureStart));
			}
		private:
			std::function<void(webrtc::DesktopCapturer::Result, std::unique_ptr<webrtc::DesktopFrame>)> onCaptureResult_;
			std::function<void()> onFrameCaptureStart_;
		};


		class SetLocalDescription : public webrtc::SetLocalDescriptionObserverInterface
		{
		public:

			SetLocalDescription(std::function<void(webrtc::RTCError)> onComplete)
				: onComplete_(std::move(onComplete)) {
			}

			virtual void OnSetLocalDescriptionComplete(webrtc::RTCError error) override {
				if(onComplete_)
					onComplete_(std::move(error));
			}

			static webrtc::scoped_refptr<webrtc::SetLocalDescriptionObserverInterface> Create(std::function<void(webrtc::RTCError)> onComplete) {
				return webrtc::make_ref_counted<SetLocalDescription>(std::move(onComplete));
			}

		private:
			std::function<void(webrtc::RTCError)> onComplete_;
		};



		class  SetRemoteDescription :public webrtc::SetRemoteDescriptionObserverInterface
		{
		public:
			SetRemoteDescription(std::function<void(webrtc::RTCError)> onComplete)
				: onComplete_(std::move(onComplete)) {
			}

			virtual void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override {
				if (onComplete_) {
					onComplete_(std::move(error));
				}
			}

			static webrtc::scoped_refptr<webrtc::SetRemoteDescriptionObserverInterface> Create(std::function<void(webrtc::RTCError)> onComplete) {
				return webrtc::make_ref_counted<SetRemoteDescription>(std::move(onComplete));
			}

		private:
			std::function<void(webrtc::RTCError)> onComplete_;

	
		};

	

		class LambdaRawVideoSinkInterface :public webrtc::RawVideoSinkInterface
		{
		public:
			LambdaRawVideoSinkInterface(std::function<int32_t(uint8_t*, size_t, const webrtc::VideoCaptureCapability&, webrtc::VideoRotation, int64_t)> onRawFrame)
				: onRawFrame_(std::move(onRawFrame)) {
			}

			virtual int32_t OnRawFrame(uint8_t* videoFrame,
				size_t videoFrameLength,
				const webrtc::VideoCaptureCapability& frameInfo,
				webrtc::VideoRotation rotation,
				int64_t captureTime) {
				if (onRawFrame_)
					return onRawFrame_(videoFrame, videoFrameLength, frameInfo, rotation, captureTime);
				return 0;
			}

			virtual ~LambdaRawVideoSinkInterface() = default;


		private:
			std::function<int32_t(uint8_t*, size_t, const webrtc::VideoCaptureCapability&, webrtc::VideoRotation, int64_t)> onRawFrame_;
		};


		static std::unique_ptr<webrtc::RawVideoSinkInterface> RawVideoSinkInterfaceLambda(std::function<int32_t(uint8_t*, size_t, const webrtc::VideoCaptureCapability&, webrtc::VideoRotation, int64_t)> onRawFrame) {
			return std::make_unique<LambdaRawVideoSinkInterface>(std::move(onRawFrame));
		}

		class LambdaVideoSinkInterface :public webrtc::VideoSinkInterface<webrtc::VideoFrame>
		{
		public:

			LambdaVideoSinkInterface(std::function<void(const webrtc::VideoFrame&)> onFrame)
				: onFrame_(std::move(onFrame)) {
			}

			virtual void OnFrame(const webrtc::VideoFrame& frame)
			{
				if (onFrame_)
					onFrame_(frame);
			};
		private:
			std::function<void(const webrtc::VideoFrame&)> onFrame_;
		};

		static std::unique_ptr<webrtc::VideoSinkInterface<webrtc::VideoFrame>> VideoSinkInterfaceLambda(std::function<void(const webrtc::VideoFrame&)> onFrame) {
			return std::make_unique<LambdaVideoSinkInterface>(std::move(onFrame));
		}

		class LambdaSetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
		{
		public:
			LambdaSetSessionDescriptionObserver(std::function<void()> onSuccess,
				std::function<void(webrtc::RTCError)> onFailure)
				: onSuccess_(std::move(onSuccess)), onFailure_(std::move(onFailure)) {
			}
			virtual void OnSuccess() override {
				if (onSuccess_) {
					onSuccess_();
				}
			};
			// The OnFailure callback takes an RTCError, which consists of an
			// error code and a string.
			// RTCError is non-copyable, so it must be passed using std::move.
			// Earlier versions of the API used a string argument. This version
			// is removed; its functionality was the same as passing
			// error.message.
			virtual void OnFailure(webrtc::RTCError error) override {
				if (onFailure_) {
					onFailure_(std::move(error));
				}
			};

		private:
			std::function<void()> onSuccess_;
			std::function<void(webrtc::RTCError)> onFailure_;
		};

		static webrtc::scoped_refptr<webrtc::SetSessionDescriptionObserver> CreateSetSessionDescriptionObserverLambda(std::function<void()>, std::function<void(webrtc::RTCError)>) {
			return webrtc::scoped_refptr<LambdaSetSessionDescriptionObserver>();
		}

		class CreateSessionDescription : public webrtc::CreateSessionDescriptionObserver {

		public:

			CreateSessionDescription(
				std::function<void(webrtc::SessionDescriptionInterface*)> onSuccess,
				std::function<void(webrtc::RTCError)> onFailure)
				: onSuccess_(std::move(onSuccess)), onFailure_(std::move(onFailure)) {
			}

			// around ownership.
			void OnSuccess(webrtc::SessionDescriptionInterface* desc) override {
				if(onSuccess_)
					onSuccess_(desc);
			};

			// The OnFailure callback takes an RTCError, which consists of an
			// error code and a string.
			// RTCError is non-copyable, so it must be passed using std::move.
			// Earlier versions of the API used a string argument. This version
			// is removed; its functionality was the same as passing
			// error.message.
			void OnFailure(webrtc::RTCError error) override {
				if(onFailure_)
					onFailure_(std::move(error));
			};
			static webrtc::scoped_refptr<webrtc::CreateSessionDescriptionObserver> Create(
				std::function<void(webrtc::SessionDescriptionInterface*)> onSuccess,
				std::function<void(webrtc::RTCError)> onFailure) {
				return webrtc::make_ref_counted<CreateSessionDescription>(std::move(onSuccess), std::move(onFailure));
			}
		private:
			std::function<void(webrtc::SessionDescriptionInterface*)> onSuccess_;
			std::function<void(webrtc::RTCError)> onFailure_;
		};




		class PeerConnection : public webrtc::PeerConnectionObserver {
		public:
			~PeerConnection() override = default;

			void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {
				if(onSignalingChange_)
					onSignalingChange_(new_state);
			}
			void OnAddStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {
				if(onAddStream_)
					onAddStream_(stream);
			}
			void OnRemoveStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {
				if(onRemoveStream_)
					onRemoveStream_(stream);
			}
			void OnDataChannel(webrtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override {
				if(onDataChannel_)
					onDataChannel_(data_channel);
			}
			void OnRenegotiationNeeded() override {
				if(onRenegotiationNeeded_)
					onRenegotiationNeeded_();
			}
			void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {
				if(onIceConnectionChange_)
					onIceConnectionChange_(new_state);
			}
			void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {
				if(onIceGatheringChange_)
					onIceGatheringChange_(new_state);
			}
			void OnIceCandidate(const webrtc::IceCandidate* candidate) override {
				if(onIceCandidate_)
					onIceCandidate_(candidate);
			}


			void SetOnSignalingChange(std::function<void(webrtc::PeerConnectionInterface::SignalingState)> onSignalingChange) {
				onSignalingChange_ = std::move(onSignalingChange);
			}
			void SetOnAddStream(std::function<void(webrtc::scoped_refptr<webrtc::MediaStreamInterface>)> onAddStream) {
				onAddStream_ = std::move(onAddStream);
			}
			void SetOnRemoveStream(std::function<void(webrtc::scoped_refptr<webrtc::MediaStreamInterface>)> onRemoveStream) {
				onRemoveStream_ = std::move(onRemoveStream);
			}
			void SetOnDataChannel(std::function<void(webrtc::scoped_refptr<webrtc::DataChannelInterface>)> onDataChannel) {
				onDataChannel_ = std::move(onDataChannel);
			}
			void SetOnRenegotiationNeeded(std::function<void()> onRenegotiationNeeded) {
				onRenegotiationNeeded_ = std::move(onRenegotiationNeeded);
			}
			void SetOnIceConnectionChange(std::function<void(webrtc::PeerConnectionInterface::IceConnectionState)> onIceConnectionChange) {
				onIceConnectionChange_ = std::move(onIceConnectionChange);
			}
			void SetOnIceGatheringChange(std::function<void(webrtc::PeerConnectionInterface::IceGatheringState)> onIceGatheringChange) {
				onIceGatheringChange_ = std::move(onIceGatheringChange);
			}
			void SetOnIceCandidate(std::function<void(const webrtc::IceCandidate*)> onIceCandidate) {
				onIceCandidate_ = std::move(onIceCandidate);
			}
			static std::shared_ptr<PeerConnection> Create() {
				return std::make_shared<PeerConnection>();
			}
		private:
			std::function<void(webrtc::PeerConnectionInterface::SignalingState)> onSignalingChange_;
			std::function<void(webrtc::scoped_refptr<webrtc::MediaStreamInterface>)> onAddStream_;
			std::function<void(webrtc::scoped_refptr<webrtc::MediaStreamInterface>)> onRemoveStream_;
			std::function<void(webrtc::scoped_refptr<webrtc::DataChannelInterface>)> onDataChannel_;
			std::function<void()> onRenegotiationNeeded_;
			std::function<void(webrtc::PeerConnectionInterface::IceConnectionState)> onIceConnectionChange_;
			std::function<void(webrtc::PeerConnectionInterface::IceGatheringState)> onIceGatheringChange_;
			std::function<void(const webrtc::IceCandidate*)> onIceCandidate_;

		};

	


	}

}