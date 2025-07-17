#include "RHI/GraphicsDevice.h"

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include "VkInit.h"
#include "VkTypes.h"
#include "VkDescriptors.h"

#include "Graphics/FlyCamera.h"

#include "Core/Containers/DeletionQueue.h"

namespace QE
{
	struct FrameData
	{
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;

		VkSemaphore SwapchainSemaphore;
		VkFence RenderFence;

		DeletionQueue CleanupQueue;
		DescriptorAllocatorGrowable FrameDescriptors{};

		RHIStats Stats;
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

	constexpr unsigned int MAX_FRAMES_IN_FLIGHT = 1;

	class VkGraphicsDevice : public GraphicsDevice
	{
	public: 
		VkGraphicsDevice(Window* window);
		~VkGraphicsDevice() = default;

		void ShutdownAndCleanup() override;

		void BeginFrame() override;
		void EndFrame() override;
		void PresentFrame() override;

		void RecompileShaders() override;
		void RemakePipelinesWhenShadersChange(std::vector<std::string>& compiledShaders);
		RHIStats GetStats() override;

		void UpdateWindowSize(uint32_t width, uint32_t height) override;

		std::unique_ptr<GraphicsContext> CreateGraphicsContext() override;
		void WaitForDeviceIdle() override;

		BufferHandle CreateBuffer(BufferDescription desc) override;
		TextureHandle CreateTexture(TextureDescription desc) override;
		MeshHandle CreateMesh(std::span<Vertex> vertices,  std::span<uint32_t> indices) override;
		ShaderHandle CreateShader(ShaderDescription desc) override;
		PipelineHandle CreatePipeline(PipelineDescription desc) override;

		// Wrappers
		PipelineHandle CreatePipelineWrapper(PipelineDescription desc, const PipelineHandle* prevPipeline = nullptr);

		void BeginRenderPass(RenderpassDescription desc) override;
		void EndRenderPass(RenderpassDescription desc) override;

		void DrawMesh(MeshHandle mesh, TextureHandle* texture = nullptr) override;
		void SetCamera(FlyCamera* camera) override;

		VkInstance GetVkInstance() const { return m_Instance; }
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_Device.PhysicalDevice; }
		VkDevice GetVkDevice() const { return m_Device.Device; }
		VkQueue GetVkGraphicsQueue() const { return m_Device.GraphicsQueue; }
		VkSurfaceKHR GetVkSurface() const { return m_Swapchain.Surface; }

		uint32_t GetCurrentFrameNumber() const { return m_CurrentFrameNumber; }
		FrameData& GetCurrentFrameData();

		AllocatedBuffer GetBufferFromHandle(BufferHandle handle);
		AllocatedImage GetTextureFromHandle(TextureHandle handle);
		GPUMeshBuffer GetMeshFromHandle(MeshHandle handle);
		VulkanShader GetShaderFromHandle(ShaderHandle handle);
		VulkanPipeline GetPipelineFromHandle(PipelineHandle handle);

	private:
		Window* m_Window;
		FlyCamera* m_Camera;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VulkanDevice m_Device;
		VulkanSwapchain m_Swapchain;

		VkExtent2D m_WindowExtent; // window size
		bool m_ResizeRequested = false;

		// Frame data
		FrameData m_FrameData[MAX_FRAMES_IN_FLIGHT];
		uint32_t m_CurrentFrameNumber = 0;

		VmaAllocator m_Allocator;
		DeletionQueue m_CleanupQueue;

		// Drawing resources
		AllocatedImage m_DrawImage;
		AllocatedImage m_DepthImage;
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

		// Current renderpass pipeline and layout
		VkPipeline m_CurrentPipeline = nullptr;
		VkPipelineLayout m_CurrentPipelineLayout = nullptr;

		VkDescriptorSetLayout m_SingleImageDescriptorLayout;

		AllocatedImage m_WhiteImage;
		AllocatedImage m_BlackImage;
		AllocatedImage m_GreyImage;
		AllocatedImage m_ErrorCheckerboardImage;

		VkSampler m_DefaultSamplerLinear;
		VkSampler m_DefaultSamplerNearest;

		// Initialize Vulkan Resources
		void InitSwapchain(VkExtent2D windowExtent);
		void CreateSwapchain(VkExtent2D windowExtent);
		void RecreateSwapchain();
		void DestroySwapchain();
		void InitializeFrameData();
		void InitializeDescriptors();
		void InitializeBackgroundPipelines();
		void InitializeImGui();
		void InitializeDefaultData();

		void TutorialSetupStuff();

		// REFACTOR LATER
		void DrawBackground(VkCommandBuffer cmd);
		void ImmediateCommandSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
		void DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView);
		AllocatedBuffer AllocateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void UploadDataToBuffer(AllocatedBuffer& buffer, void* data, size_t dataSize);
		void DestroyBuffer(const AllocatedBuffer& buffer);
		AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		AllocatedImage CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		void DestroyImage(const AllocatedImage& image);
	};
}