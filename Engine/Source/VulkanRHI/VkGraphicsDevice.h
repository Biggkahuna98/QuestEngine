#include "RHI/GraphicsDevice.h"

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>
#include "VkInit.h"

namespace QE
{
	struct FrameData
	{
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
	};

	constexpr unsigned int MAX_FRAMES_IN_FLIGHT = 2;

	class VkGraphicsDevice : public GraphicsDevice
	{
	public: 
		VkGraphicsDevice(Window* window);
		virtual ~VkGraphicsDevice() override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void PresentFrame() override;

		virtual void UpdateWindowSize(uint32_t width, uint32_t height) override;

		virtual std::unique_ptr<GraphicsContext> CreateGraphicsContext() override;

		virtual void WaitForDeviceIdle() override;
		
		VkInstance GetVkInstance() const { return m_Instance; }
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
		VkDevice GetVkDevice() const { return m_Device; }
		VkQueue GetVkGraphicsQueue() const { return m_GraphicsQueue; }
		VkSurfaceKHR GetVkSurface() const { return m_Surface; }

		int GetCurrentFrameNumber() const { return m_CurrentFrameNumber; }
		FrameData& GetCurrentFrameData();
	private:
		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;

		VkInit::QueueFamilyIndices m_QueueFamilyIndices;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		VkSurfaceKHR m_Surface;
		VkExtent2D m_WindowExtent; // window size

		VkSwapchainKHR m_Swapchain;
		VkExtent2D m_SwapchainExtent;
		VkFormat m_SwapchainImageFormat;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;

		VkPipelineLayout m_PipelineLayout;

		// Frame data
		FrameData m_FrameData[MAX_FRAMES_IN_FLIGHT];
		int m_CurrentFrameNumber = 0;

		// Initialize Vulkan Resources
		void CreateSwapchain(VkExtent2D windowExtent);
		void RecreateSwapchain();
		void DestroySwapchain();
		void InitializeFrameData();
	};
}