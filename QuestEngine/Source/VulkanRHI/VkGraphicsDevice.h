#include "RHI/GraphicsDevice.h"

#include <vector>
#include <cstdint>

#include "VkBootstrap.h"

namespace QE
{
	class VkGraphicsDevice : public GraphicsDevice
	{
	public: 
		VkGraphicsDevice(const Window& window);
		virtual ~VkGraphicsDevice() override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void PresentFrame() override;

		virtual void UpdateWindowSize(uint32_t width, uint32_t height) override;

		virtual void WaitForDeviceIdle() override;
		
		VkInstance GetVkInstance() const { return m_VkInstance; }
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
		VkDevice GetVkDevice() const { return m_VkDevice; }
		VkSurfaceKHR GetVkSurface() const { return m_VkSurface; }
	private:
		VkInstance m_VkInstance;
		VkDebugUtilsMessengerEXT m_VkDebugMessenger;
		VkPhysicalDevice m_VkPhysicalDevice;
		VkDevice m_VkDevice;
		VkSurfaceKHR m_VkSurface;
		VkExtent2D m_VkWindowExtent; // window size

		VkSwapchainKHR m_VkSwapchain;
		VkFormat m_VkSwapchainImageFormat;

		std::vector<VkImage> m_VkSwapchainImages;
		std::vector<VkImageView> m_VkSwapchainImageViews;
		VkExtent2D m_VkSwapchainExtent;


		// Initialize Vulkan Resources
		void InitVulkan(const Window& window);
		
		void InitSwapchain();
		void RecreateSwapchain();
		void CreateSwapchain(uint32_t width, uint32_t height);
		void DestroySwapchain();
	};
}