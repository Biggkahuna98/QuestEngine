#include "RHI/GraphicsDevice.h"

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

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

		virtual std::unique_ptr<GraphicsContext> CreateGraphicsContext() override;

		virtual void WaitForDeviceIdle() override;
		
		VkInstance GetVkInstance() const { return m_VkInstance; }
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
		VkDevice GetVkDevice() const { return m_VkDevice; }
		VkQueue GetVkGraphicsQueue() const { return m_VkGraphicsQueue; }
		VkSurfaceKHR GetVkSurface() const { return m_VkSurface; }
	private:
		VkInstance m_VkInstance;
		VkDebugUtilsMessengerEXT m_VkDebugMessenger;
		VkPhysicalDevice m_VkPhysicalDevice;
		VkDevice m_VkDevice;

		VkQueue m_VkGraphicsQueue;
		VkQueue m_VkPresentQueue;

		VkSurfaceKHR m_VkSurface;
		VkExtent2D m_VkWindowExtent; // window size

		VkSwapchainKHR m_VkSwapchain;
		VkExtent2D m_VkSwapchainExtent;
		VkFormat m_VkSwapchainImageFormat;
		std::vector<VkImage> m_VkSwapchainImages;
		std::vector<VkImageView> m_VkSwapchainImageViews;


		// Initialize Vulkan Resources
		void CreateSwapchain(uint32_t width, uint32_t height);
		void RecreateSwapchain();
		void DestroySwapchain();
	};
}