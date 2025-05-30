#include "RHI/GraphicsDevice.h"

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include "VkInit.h"
#include "VkTypes.h"
#include "VkDescriptors.h"
#include "VkLoader.h"

#include "../../Include/Core/Containers/DeletionQueue.h"

namespace QE
{
	struct FrameData
	{
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;

		VkSemaphore SwapchainSemaphore;
		VkSemaphore RenderSemaphore;
		VkFence RenderFence;

		DeletionQueue CleanupQueue;
	};

	struct ComputePushConstants
	{
		glm::vec4 Data1;
		glm::vec4 Data2;
		glm::vec4 Data3;
		glm::vec4 Data4;
	};

	struct ComputeEffect
	{
		const char* Name;

		VkPipeline Pipeline;
		VkPipelineLayout PipelineLayout;

		ComputePushConstants Data;
	};

	constexpr unsigned int MAX_FRAMES_IN_FLIGHT = 2;

	class VkGraphicsDevice : public GraphicsDevice
	{
	public: 
		VkGraphicsDevice(Window* window);
		~VkGraphicsDevice() override;

		void BeginFrame() override;
		void EndFrame() override;
		void PresentFrame() override;

		void UpdateWindowSize(uint32_t width, uint32_t height) override;

		std::unique_ptr<GraphicsContext> CreateGraphicsContext() override;

		void WaitForDeviceIdle() override;
		
		VkInstance GetVkInstance() const { return m_Instance; }
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
		VkDevice GetVkDevice() const { return m_Device; }
		VkQueue GetVkGraphicsQueue() const { return m_GraphicsQueue; }
		VkSurfaceKHR GetVkSurface() const { return m_Surface; }

		uint32_t GetCurrentFrameNumber() const { return m_CurrentFrameNumber; }
		FrameData& GetCurrentFrameData();

		GPUMeshBuffer UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);
	private:
		Window* m_Window;
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
		uint32_t m_CurrentSwapchainImageIndex;

		VkPipelineLayout m_PipelineLayout;

		// Frame data
		FrameData m_FrameData[MAX_FRAMES_IN_FLIGHT];
		uint32_t m_CurrentFrameNumber = 0;

		VmaAllocator m_Allocator;
		DeletionQueue m_CleanupQueue;

		// Drawing resources
		AllocatedImage m_DrawImage;
		VkExtent2D m_DrawExtent;

		DescriptorAllocator m_DescriptorAllocator;
		VkDescriptorSet m_DrawImageDescriptors;
		VkDescriptorSetLayout m_DrawImageDescriptorSetLayout;

		// Pipeline stuff
		VkPipeline m_GradientPipeline;
		VkPipelineLayout m_GradientPipelineLayout;

		// ImGui stuff (refactor later)
		VkFence m_ImGuiFence;
		VkCommandBuffer m_ImGuiCommandBuffer;
		VkCommandPool m_ImGuiCommandPool;

		// Compute effects
		std::vector<ComputeEffect> m_BackgroundEffects;
		int m_CurrentBackgroundEffect = 0;

		// Triangle
		VkPipelineLayout m_TrianglePipelineLayout;
		VkPipeline m_TrianglePipeline;

		// Mesh
		VkPipelineLayout m_MeshPipelineLayout;
		VkPipeline m_MeshPipeline;

		GPUMeshBuffer m_Rectangle;

		// Test meshes
		std::vector<std::shared_ptr<MeshAsset>> m_TestMeshes;

		// Initialize Vulkan Resources
		void InitSwapchain(VkExtent2D windowExtent);
		void CreateSwapchain(VkExtent2D windowExtent);
		void RecreateSwapchain();
		void DestroySwapchain();
		void InitializeFrameData();
		void InitializeDescriptors();
		void InitializePipelines();
		void InitializeBackgroundPipelines();
		void InitializeTrianglePipeline();
		void InitializeMeshPipeline();
		void InitializeImGui();
		void InitializeDefaultData();

		// REFACTOR LATER
		void DrawBackground(VkCommandBuffer cmd);
		void ImmediateCommandSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
		void DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView);
		void DrawGeometry(VkCommandBuffer cmd);
		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void DestroyBuffer(const AllocatedBuffer& buffer);
	};
}