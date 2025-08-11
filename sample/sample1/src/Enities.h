
namespace libWebrtc {
	struct DeviceInfo
	{
		char deviceNameUTF8[1024];
		uint32_t deviceNameLength;
		char deviceUniqueIdUTF8[1024];
		uint32_t deviceUniqueIdUTF8Length;
		char productUniqueIdUTF8[1024];
		uint32_t productUniqueIdUTF8Length;
	};
}