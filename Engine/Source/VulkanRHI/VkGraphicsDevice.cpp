#include "VkGraphicsDevice.h"
#include "Core/Log.h"

#include "VkInit.h"
#include "VkPipelines.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include "GLFW/glfw3.h"

#include "VKGraphicsContext.h"

#include "VkRHISettings.h"

#include <array>
#include <unordered_map>

// ImGui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace QE
{
	// Resource mappings
	std::uint32_t s_BufferCount = 0; // starting handle
	std::unordered_map<BufferHandle, AllocatedBuffer> s_BufferMap;

	std::uint32_t s_TextureCount = 0; // starting handle
	std::unordered_map<TextureHandle, AllocatedImage> s_TextureMap;

	VkGraphicsDevice::VkGraphicsDevice(Window* window)
		: GraphicsDevice(window), m_Window(window) // refactor to stored in graphicsdevice
	{
		// Handle maps
		s_BufferMap.reserve(1000);
		s_TextureMap.reserve(1000);

		// Set real window size
		int width, height;
		glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeWindow()), &width, &height);

		m_WindowExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		// Initialize Vulkan and all needed resources
		// Setup the instance
		m_Instance = VkInit::CreateInstance();
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan Instance created");

		// Print extension count
		LOG_DEBUG_TAG("VkGraphicsDevice", "{} Vulkan Instance extensions supported", VkInit::GetVulkanExtensionCount());
		// Print supported extensions
		std::vector<VkExtensionProperties> extensions = VkInit::GetSupportedExtensions();
		for (const auto& extension : extensions)
		{
			LOG_DEBUG_TAG("VkGraphicsDevice", "\t{}", extension.extensionName);
		}

		// Setup debug messenger
		if (VkInit::s_EnableValidationLayers)
		{
			auto debugMessengerCreateInfo = VkInit::BuildDebugMessengerCreateInfo();
			m_DebugMessenger = VkInit::CreateDebugMessenger(m_Instance, debugMessengerCreateInfo);
		}

		// Setup the surface
		// Only using GLFW for now, change this later to detect window backend later if needed
		glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(window->GetNativeWindow()), nullptr, &m_Surface);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan surface created and linked to GLFWwindow");

		// Pick a GPU
		m_PhysicalDevice = VkInit::PickPhysicalDevice(m_Instance, m_Surface);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan device created");

		// Logical device creation
		m_Device = VkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface, &m_GraphicsQueue, &m_PresentQueue);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan physical device created");

		// VMA Allocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_Device;
		allocatorInfo.instance = m_Instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		//allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;

		VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_Allocator));
		LOG_DEBUG_TAG("VkGraphicsDevice", "VMA Allocator created");

		m_CleanupQueue.PushFunction([&]() {
			vmaDestroyAllocator(m_Allocator);
		});

		// Set the queue family indices
		m_QueueFamilyIndices = VkInit::FindQueueFamilies(m_PhysicalDevice, m_Surface);

		// Swapchain
		InitSwapchain(m_WindowExtent);

		// Frame semaphores and fences
		InitializeFrameData();
		// Command pools and buffers
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan command pools and buffers created");

		// Descriptors
		//InitializeDescriptors();

		// Graphics pipeline
		//InitializeTrianglePipeline();
		//VkInit::CreateGraphicsPipeline(m_Device, m_SwapchainImageFormat, &m_PipelineLayout);

		InitializeImGui();

		// Stuff for tutorial setup before refactoring
		TutorialSetupStuff();
	}

	VkGraphicsDevice::~VkGraphicsDevice()
	{
		VkGraphicsDevice::WaitForDeviceIdle();

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkFreeCommandBuffers(m_Device, m_FrameData[i].CommandPool, 1, &m_FrameData[i].CommandBuffer);
			vkDestroyCommandPool(m_Device, m_FrameData[i].CommandPool, nullptr);

			vkDestroyFence(m_Device, m_FrameData[i].RenderFence, nullptr);
			vkDestroySemaphore(m_Device, m_FrameData[i].RenderSemaphore, nullptr);
			vkDestroySemaphore(m_Device, m_FrameData[i].SwapchainSemaphore, nullptr);
		}

		// Flush global lifetime deletion queue
		m_CleanupQueue.Flush();

		// Buffers
		for (auto& [handle, buffer] : s_BufferMap)
			DestroyBuffer(buffer);

		// Cleanup all resources
		//vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

		DestroySwapchain();

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		if (VkInit::s_EnableValidationLayers)
			VkInit::DestroyDebugMessenger(&m_Instance, &m_DebugMessenger);

		vkDestroyInstance(m_Instance, nullptr);
	}

	void VkGraphicsDevice::BeginFrame()
	{
		// Wait for the previous frame to finish
		vkWaitForFences(m_Device, 1, &GetCurrentFrameData().RenderFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device, 1, &GetCurrentFrameData().RenderFence);

		// See if there is a better place later
		GetCurrentFrameData().CleanupQueue.Flush();

		// Request the image from the swapchain
		vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, GetCurrentFrameData().SwapchainSemaphore, VK_NULL_HANDLE, &m_CurrentSwapchainImageIndex);

		// Reset command buffer
		vkResetCommandBuffer(GetCurrentFrameData().CommandBuffer, 0);

		// Begin the buffer for recording
		VkCommandBufferBeginInfo beginInfo = VkInit::BuildCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK(vkBeginCommandBuffer(GetCurrentFrameData().CommandBuffer, &beginInfo));

		// transition our main draw image into general layout so we can write into it
		// we will overwrite it all so we dont care about what was the older layout
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_DrawImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		// Imgui
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Draw triangle
		//DrawTriangle(GetCurrentFrameData().CommandBuffer);
	}

	void VkGraphicsDevice::EndFrame()
	{
		// Transition the draw image and the swapchain image into their correct transfer layouts
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_SwapchainImages[m_CurrentSwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Execute a copy from the draw image into the swapchain
		VkInit::CopyImageToImage(GetCurrentFrameData().CommandBuffer, m_DrawImage.Image, m_SwapchainImages[m_CurrentSwapchainImageIndex], m_DrawExtent, m_SwapchainExtent);

		// Set swapchain image layout to Present so we can show it on the screen
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_SwapchainImages[m_CurrentSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		// Render ImGui
		ImGui::Render();
		// Draw imgui
		DrawImGui(GetCurrentFrameData().CommandBuffer, m_SwapchainImageViews[m_CurrentSwapchainImageIndex]);

		// End command buffer recording
		VK_CHECK(vkEndCommandBuffer(GetCurrentFrameData().CommandBuffer));

		// Submit command buffer to the graphics queue
		VkCommandBufferSubmitInfo cmdSubmitInfo = VkInit::BuildCommandBufferSubmitInfo(GetCurrentFrameData().CommandBuffer);

		VkSemaphoreSubmitInfo waitInfo = VkInit::BuildSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,GetCurrentFrameData().SwapchainSemaphore);
		VkSemaphoreSubmitInfo signalInfo = VkInit::BuildSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrameData().RenderSemaphore);

		VkSubmitInfo2 submitInfo = VkInit::BuildSubmitInfo2(&cmdSubmitInfo, &signalInfo, &waitInfo);
		VK_CHECK(vkQueueSubmit2(m_GraphicsQueue, 1, &submitInfo, GetCurrentFrameData().RenderFence));
	}

	void VkGraphicsDevice::PresentFrame()
	{
		//LOG_DEBUG_TAG("VkGraphicsDevice", "Presenting frame: {0}", m_CurrentFrameNumber);
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &GetCurrentFrameData().RenderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &m_CurrentSwapchainImageIndex;

		// Present the image
		VK_CHECK(vkQueuePresentKHR(m_PresentQueue, &presentInfo));

		m_CurrentFrameNumber++;
	}

	void VkGraphicsDevice::UpdateWindowSize(uint32_t width, uint32_t height)
	{
		LOG_DEBUG_TAG("VkGraphicsDevice", "Updating window size: {0}x{1}", width, height);
		m_WindowExtent = { width, height };
		RecreateSwapchain();
	}

	std::unique_ptr<GraphicsContext> VkGraphicsDevice::CreateGraphicsContext()
	{
		return std::make_unique<VkGraphicsContext>(this);
	}

	void VkGraphicsDevice::WaitForDeviceIdle()
	{
		vkDeviceWaitIdle(m_Device);
	}

	BufferHandle VkGraphicsDevice::CreateBuffer(BufferDescription desc)
	{
		LOG_DEBUG("Creating Buffer");
		BufferHandle handle = { s_BufferCount++ };

		// Abstract these fields later
		//VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		VkBufferUsageFlags usageFlagsConverted = BufferTypeFlagsFromRHI(desc.Type) | BufferUsageFlagsFromRHI(desc.Usage);
		//VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		AllocatedBuffer allocatedBuffer = AllocateBuffer(desc.Data->Size, usageFlagsConverted, memoryUsage);
		allocatedBuffer.Size = desc.Data->DataCount;
		UploadDataToBuffer(allocatedBuffer, desc.Data->Data, desc.Data->Size);

		s_BufferMap[handle] = allocatedBuffer;

		return handle;
	}

	void VkGraphicsDevice::DrawVertexBuffer(BufferHandle vtx)
	{
		AllocatedBuffer allocatedBuffer = s_BufferMap[vtx];

		//begin a render pass  connected to our draw image
		VkClearValue clearValue = {0.27f, 0.3f, 0.32f, 1.0f};
		VkRenderingAttachmentInfo colorAttachment = VkInit::BuildRenderingAttachmentInfo(m_DrawImage.ImageView, &clearValue, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingInfo renderInfo = VkInit::BuildRenderingInfo(m_DrawExtent, &colorAttachment, nullptr);
		// Get the current command buffer
		VkCommandBuffer cmd = GetCurrentFrameData().CommandBuffer;
		vkCmdBeginRendering(cmd, &renderInfo);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Mesh2DPipeline);

		//set dynamic viewport and scissor
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = m_DrawExtent.width;
		viewport.height = m_DrawExtent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_DrawExtent.width;
		scissor.extent.height = m_DrawExtent.height;

		vkCmdSetScissor(cmd, 0, 1, &scissor);

		// Bind vertex buffer
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(cmd, 0, 1, &allocatedBuffer.Buffer, offsets);

		vkCmdDraw(cmd, allocatedBuffer.Size, 1, 0, 0);

		vkCmdEndRendering(cmd);
	}

	void VkGraphicsDevice::DrawMesh(Mesh mesh)
	{
		AllocatedBuffer vertexBuffer = s_BufferMap[mesh.VertexBuffer];
		AllocatedBuffer indexBuffer = s_BufferMap[mesh.IndexBuffer];

		//begin a render pass  connected to our draw image
		VkClearValue clearValue = {0.27f, 0.3f, 0.32f, 1.0f};
		VkRenderingAttachmentInfo colorAttachment = VkInit::BuildRenderingAttachmentInfo(m_DrawImage.ImageView, &clearValue, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingInfo renderInfo = VkInit::BuildRenderingInfo(m_DrawExtent, &colorAttachment, nullptr);
		// Get the current command buffer
		VkCommandBuffer cmd = GetCurrentFrameData().CommandBuffer;
		vkCmdBeginRendering(cmd, &renderInfo);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Mesh2DPipeline);

		//set dynamic viewport and scissor
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = m_DrawExtent.width;
		viewport.height = m_DrawExtent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_DrawExtent.width;
		scissor.extent.height = m_DrawExtent.height;

		vkCmdSetScissor(cmd, 0, 1, &scissor);

		// Bind vertex buffer
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer.Buffer, offsets);

		// Bind index buffer
		vkCmdBindIndexBuffer(cmd, indexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

		//vkCmdDraw(cmd, allocatedBuffer.Size, 1, 0, 0);
		vkCmdDrawIndexed(cmd, indexBuffer.Size, 1, 0, 0, 0);

		vkCmdEndRendering(cmd);
	}

	FrameData& VkGraphicsDevice::GetCurrentFrameData()
	{
		return m_FrameData[m_CurrentFrameNumber % MAX_FRAMES_IN_FLIGHT];
	}

	AllocatedBuffer VkGraphicsDevice::GetBufferFromHandle(BufferHandle handle)
	{
		return s_BufferMap[handle];
	}

	// PRIVATE FUNCTIONS
	void VkGraphicsDevice::InitSwapchain(VkExtent2D windowExtent)
	{
		CreateSwapchain(windowExtent);

		// Setup draw image
		VkExtent3D drawImageExtent = {
			windowExtent.width,
			windowExtent.height,
			1
		};

		m_DrawExtent = windowExtent;

		m_DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		m_DrawImage.ImageExtent = drawImageExtent;

		VkImageUsageFlags drawImageUsages{};
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VkImageCreateInfo ring_info = VkInit::BuildImageCreateInfo(m_DrawImage.ImageFormat, drawImageUsages, drawImageExtent);

		VmaAllocationCreateInfo ring_allocInfo = {};
		ring_allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		ring_allocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// Allocate and create the image
		vmaCreateImage(m_Allocator, &ring_info, &ring_allocInfo, &m_DrawImage.Image, &m_DrawImage.Allocation, nullptr);

		// Build the image view
		VkImageViewCreateInfo imageViewInfo = VkInit::BuildImageViewCreateInfo(m_DrawImage.ImageFormat, m_DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);
		if (vkCreateImageView(m_Device, &imageViewInfo, nullptr, &m_DrawImage.ImageView) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to create image view for draw image");
		}

		//add to deletion queues
		m_CleanupQueue.PushFunction([=]() {
			vkDestroyImageView(m_Device, m_DrawImage.ImageView, nullptr);
			vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);
		});
	}

	void VkGraphicsDevice::CreateSwapchain(VkExtent2D windowExtent)
	{
		m_Swapchain = VkInit::CreateSwapchain(m_PhysicalDevice, m_Device, m_Surface, windowExtent, &m_SwapchainImages, &m_SwapchainImageFormat, &m_SwapchainExtent);
		VkInit::CreateSwapchainImageViews(m_Device, &m_SwapchainImages, m_SwapchainImageFormat, &m_SwapchainImageViews);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan Swapchain and views created");
	}

	void VkGraphicsDevice::RecreateSwapchain()
	{
		LOG_DEBUG_TAG("VkGraphicsDevice", "Recreating Vulkan Swapchain");
		WaitForDeviceIdle();

		DestroySwapchain();
		CreateSwapchain(m_WindowExtent);
	}

	void VkGraphicsDevice::DestroySwapchain()
	{
		// Destroy swapchain resources
		for (int i = 0; i < m_SwapchainImageViews.size(); i++)
		{
			vkDestroyImageView(m_Device, m_SwapchainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
	}

	void VkGraphicsDevice::InitializeTrianglePipeline()
	{
		VkShaderModule triangleVertexShader = VkInit::CreateShaderModule(m_Device, "triangle-vert.spv");
		VkShaderModule triangleFragmentShader = VkInit::CreateShaderModule(m_Device, "triangle-frag.spv");

		// build the pipeline layout that controls the inputs/outputs of the shader
		// we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
		VkPipelineLayoutCreateInfo pipeline_layout_info = VkInit::BuildPipelineCreateInfo();
		VK_CHECK(vkCreatePipelineLayout(m_Device, &pipeline_layout_info, nullptr, &m_TrianglePipelineLayout));

		PipelineBuilder pipelineBuilder;

		//use the triangle layout we created
		pipelineBuilder.PipelineLayout = m_TrianglePipelineLayout;
		//connecting the vertex and pixel shaders to the pipeline
		pipelineBuilder.SetShaders(triangleVertexShader, triangleFragmentShader);
		//it will draw triangles
		pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		//filled triangles
		pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
		//no backface culling
		pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		//no multisampling
		pipelineBuilder.SetMultisamplingMode();
		//no blending
		pipelineBuilder.DisableBlending();
		//no depth testing
		pipelineBuilder.DisableDepthTest();

		//connect the image format we will draw into, from draw image
		pipelineBuilder.SetColorAttachmentFormat(m_DrawImage.ImageFormat);
		pipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

		//finally build the pipeline
		m_TrianglePipeline = pipelineBuilder.BuildPipeline(m_Device);

		//clean structures
		vkDestroyShaderModule(m_Device, triangleFragmentShader, nullptr);
		vkDestroyShaderModule(m_Device, triangleVertexShader, nullptr);

		m_CleanupQueue.PushFunction([&]() {
			vkDestroyPipelineLayout(m_Device, m_TrianglePipelineLayout, nullptr);
			vkDestroyPipeline(m_Device, m_TrianglePipeline, nullptr);
		});
	}

	void VkGraphicsDevice::InitializeMesh2DPipeline()
	{
		VkShaderModule vertexShader = VkInit::CreateShaderModule(m_Device, "general_input-vert.spv");
		VkShaderModule fragmentShader = VkInit::CreateShaderModule(m_Device, "general_input-frag.spv");

		// build the pipeline layout that controls the inputs/outputs of the shader
		// we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
		VkPipelineLayoutCreateInfo pipeline_layout_info = VkInit::BuildPipelineCreateInfo();
		VK_CHECK(vkCreatePipelineLayout(m_Device, &pipeline_layout_info, nullptr, &m_Mesh2DPipelineLayout));

		PipelineBuilder pipelineBuilder;

		//use the triangle layout we created
		pipelineBuilder.PipelineLayout = m_Mesh2DPipelineLayout;
		//connecting the vertex and pixel shaders to the pipeline
		pipelineBuilder.SetShaders(vertexShader, fragmentShader);
		//it will draw triangles
		pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		//filled triangles
		pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
		//no backface culling
		pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		//no multisampling
		pipelineBuilder.SetMultisamplingMode();
		//no blending
		pipelineBuilder.DisableBlending();
		//no depth testing
		pipelineBuilder.DisableDepthTest();
		// Use vertex input description thing
		pipelineBuilder.UseVertexInput();

		//connect the image format we will draw into, from draw image
		pipelineBuilder.SetColorAttachmentFormat(m_DrawImage.ImageFormat);
		pipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

		//finally build the pipeline
		m_Mesh2DPipeline = pipelineBuilder.BuildPipeline(m_Device);

		//clean structures
		vkDestroyShaderModule(m_Device, vertexShader, nullptr);
		vkDestroyShaderModule(m_Device, fragmentShader, nullptr);

		m_CleanupQueue.PushFunction([&]() {
			vkDestroyPipelineLayout(m_Device, m_Mesh2DPipelineLayout, nullptr);
			vkDestroyPipeline(m_Device, m_Mesh2DPipeline, nullptr);
		});
	}

	void VkGraphicsDevice::InitializeFrameData()
	{
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			// Command pools and buffers
			m_FrameData[i].CommandPool = VkInit::CreateCommandPool(m_Device, m_QueueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
			m_FrameData[i].CommandBuffer = VkInit::CreateCommandBuffer(m_Device, m_FrameData[i].CommandPool);

			// Semaphores
			m_FrameData[i].SwapchainSemaphore = VkInit::CreateSemaphore(m_Device);
			m_FrameData[i].RenderSemaphore = VkInit::CreateSemaphore(m_Device);

			// Fences
			m_FrameData[i].RenderFence = VkInit::CreateFence(m_Device, VK_FENCE_CREATE_SIGNALED_BIT);
		}
	}

	void VkGraphicsDevice::InitializeDescriptors()
	{
		// Create a descriptor pool that will hold 10 sets with 1 image each
		std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
		{
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
		};

		m_DescriptorAllocator.InitPool(m_Device, 10, sizes);

		//make the descriptor set layout for our compute draw
		{
			DescriptorLayoutBuilder builder;
			builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			m_DrawImageDescriptorSetLayout = builder.Build(m_Device, VK_SHADER_STAGE_COMPUTE_BIT);
		}

		//allocate a descriptor set for our draw image
		m_DrawImageDescriptors = m_DescriptorAllocator.Allocate(m_Device, m_DrawImageDescriptorSetLayout);

		VkDescriptorImageInfo imgInfo{};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imgInfo.imageView = m_DrawImage.ImageView;

		VkWriteDescriptorSet drawImageWrite = {};
		drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		drawImageWrite.pNext = nullptr;

		drawImageWrite.dstBinding = 0;
		drawImageWrite.dstSet = m_DrawImageDescriptors;
		drawImageWrite.descriptorCount = 1;
		drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		drawImageWrite.pImageInfo = &imgInfo;

		vkUpdateDescriptorSets(m_Device, 1, &drawImageWrite, 0, nullptr);

		//make sure both the descriptor allocator and the new layout get cleaned up properly
		m_CleanupQueue.PushFunction([&]() {
			m_DescriptorAllocator.DestroyPool(m_Device);

			vkDestroyDescriptorSetLayout(m_Device, m_DrawImageDescriptorSetLayout, nullptr);
		});
	}

	void VkGraphicsDevice::InitializePipelines()
	{
		// Compute
		InitializeBackgroundPipelines();

		// Graphics
		InitializeMeshPipeline();
	}

	void VkGraphicsDevice::InitializeBackgroundPipelines()
	{
		// Pipeline layout
		VkPipelineLayoutCreateInfo computeLayout{};
		computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout.pNext = nullptr;
		computeLayout.pSetLayouts = &m_DrawImageDescriptorSetLayout;
		computeLayout.setLayoutCount = 1;

		VK_CHECK(vkCreatePipelineLayout(m_Device, &computeLayout, nullptr, &m_GradientPipelineLayout));

		VkShaderModule gradientShader = VkInit::CreateShaderModule(m_Device, "gradient-comp.spv");
		VkShaderModule skyShader = VkInit::CreateShaderModule(m_Device, "sky-comp.spv");

		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageinfo.module = gradientShader;
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = m_GradientPipelineLayout;
		computePipelineCreateInfo.stage = stageinfo;

		ComputeEffect gradient;
		gradient.PipelineLayout = m_GradientPipelineLayout;
		gradient.Name = "gradient";
		gradient.Data = {};

		//default colors
		gradient.Data.Data1 = glm::vec4(1, 0, 0, 1);
		gradient.Data.Data2 = glm::vec4(0, 0, 1, 1);

		VK_CHECK(vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradient.Pipeline));

		//change the shader module only to create the sky shader
		computePipelineCreateInfo.stage.module = skyShader;

		ComputeEffect sky;
		sky.PipelineLayout = m_GradientPipelineLayout;
		sky.Name = "sky";
		sky.Data = {};
		//default sky parameters
		sky.Data.Data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

		VK_CHECK(vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &sky.Pipeline));

		//add the 2 background effects into the array
		m_BackgroundEffects.push_back(gradient);
		m_BackgroundEffects.push_back(sky);

		//destroy structures properly
		vkDestroyShaderModule(m_Device, gradientShader, nullptr);
		vkDestroyShaderModule(m_Device, skyShader, nullptr);
		m_CleanupQueue.PushFunction([=]() {
			vkDestroyPipelineLayout(m_Device, m_GradientPipelineLayout, nullptr);
			vkDestroyPipeline(m_Device, sky.Pipeline, nullptr);
			vkDestroyPipeline(m_Device, gradient.Pipeline, nullptr);
		});
	}

	void VkGraphicsDevice::InitializeMeshPipeline()
	{
		VkShaderModule triangleFragShader = VkInit::CreateShaderModule(m_Device, "colored_triangle-frag.spv");
		VkShaderModule triangleVertexShader = VkInit::CreateShaderModule(m_Device, "colored_triangle_mesh-vert.spv");

		VkPushConstantRange bufferRange{};
		bufferRange.offset = 0;
		bufferRange.size = sizeof(GPUDrawPushConstants);
		bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipeline_layout_info = VkInit::BuildPipelineCreateInfo(); // rename this function, i was confused
		pipeline_layout_info.pPushConstantRanges = &bufferRange;
		pipeline_layout_info.pushConstantRangeCount = 1;

		VK_CHECK(vkCreatePipelineLayout(m_Device, &pipeline_layout_info, nullptr, &m_MeshPipelineLayout));

		PipelineBuilder pipelineBuilder;

		//use the triangle layout we created
		pipelineBuilder.PipelineLayout = m_MeshPipelineLayout;
		//connecting the vertex and pixel shaders to the pipeline
		pipelineBuilder.SetShaders(triangleVertexShader, triangleFragShader);
		//it will draw triangles
		pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		//filled triangles
		pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
		//no backface culling
		pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		//no multisampling
		pipelineBuilder.SetMultisamplingMode();
		//no blending
		pipelineBuilder.DisableBlending();

		pipelineBuilder.DisableDepthTest();

		//connect the image format we will draw into, from draw image
		pipelineBuilder.SetColorAttachmentFormat(m_DrawImage.ImageFormat);
		pipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

		//finally build the pipeline
		m_MeshPipeline = pipelineBuilder.BuildPipeline(m_Device);

		//clean structures
		vkDestroyShaderModule(m_Device, triangleFragShader, nullptr);
		vkDestroyShaderModule(m_Device, triangleVertexShader, nullptr);

		m_CleanupQueue.PushFunction([&]() {
			vkDestroyPipelineLayout(m_Device, m_MeshPipelineLayout, nullptr);
			vkDestroyPipeline(m_Device, m_MeshPipeline, nullptr);
		});
	}

	void VkGraphicsDevice::InitializeImGui()
	{
		// ImGui command pools
		VkCommandPoolCreateInfo commandPoolInfo = VkInit::BuildCommandPoolCreateInfo(m_QueueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_ImGuiCommandPool));

		// allocate the command buffer for immediate submits
		VkCommandBufferAllocateInfo cmdAllocInfo = VkInit::BuildCommandBufferAllocateInfo(m_ImGuiCommandPool);

		VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_ImGuiCommandBuffer));

		m_CleanupQueue.PushFunction([=]() {
			vkDestroyCommandPool(m_Device, m_ImGuiCommandPool, nullptr);
		});

		VkFenceCreateInfo fenceCreateInfo = VkInit::BuildFenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_ImGuiFence));
		m_CleanupQueue.PushFunction([=]() {
			vkDestroyFence(m_Device, m_ImGuiFence, nullptr);
		});

		// 1: create descriptor pool for IMGUI
		//  the size of the pool is very oversize, but it's copied from imgui demo
		//  itself.
		VkDescriptorPoolSize pool_sizes[] = { 
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } 
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		VkDescriptorPool imguiPool;
		VK_CHECK(vkCreateDescriptorPool(m_Device, &pool_info, nullptr, &imguiPool));

		// 2: initialize imgui library

		// this initializes the core structures of imgui
		ImGui::CreateContext();

		// this initializes imgui for SDL
		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), true);

		// this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_Instance;
		init_info.PhysicalDevice = m_PhysicalDevice;
		init_info.Device = m_Device;
		init_info.Queue = m_GraphicsQueue;
		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.UseDynamicRendering = true;

		//dynamic rendering parameters for imgui to use
		init_info.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
		init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &m_SwapchainImageFormat;


		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&init_info);

		ImGui_ImplVulkan_CreateFontsTexture();

		// add the destroy the imgui created structures
		m_CleanupQueue.PushFunction([=]() {
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
			vkDestroyDescriptorPool(m_Device, imguiPool, nullptr);
		});
	}

	void VkGraphicsDevice::InitializeDefaultData()
	{
		/*std::array<Vertex, 4> rect_vertices;

		rect_vertices[0].Position = { 0.5,-0.5, 0 };
		rect_vertices[1].Position = { 0.5,0.5, 0 };
		rect_vertices[2].Position = { -0.5,-0.5, 0 };
		rect_vertices[3].Position = { -0.5,0.5, 0 };

		rect_vertices[0].Color = { 0,0, 0,1 };
		rect_vertices[1].Color = { 0.5,0.5,0.5 ,1 };
		rect_vertices[2].Color = { 1,0, 0,1 };
		rect_vertices[3].Color = { 0,1, 0,1 };

		std::array<uint32_t, 6> rect_indices;

		rect_indices[0] = 0;
		rect_indices[1] = 1;
		rect_indices[2] = 2;

		rect_indices[3] = 2;
		rect_indices[4] = 1;
		rect_indices[5] = 3;

		//m_Rectangle = UploadMesh(rect_indices, rect_vertices);

		//delete the rectangle data on engine shutdown
		m_CleanupQueue.PushFunction([&]() {
			//DestroyBuffer(m_Rectangle.IndexBuffer);
			//DestroyBuffer(m_Rectangle.VertexBuffer);
		});*/
	}

	void VkGraphicsDevice::TutorialSetupStuff()
	{
		InitializeMesh2DPipeline();
	}

	void VkGraphicsDevice::DrawBackground(VkCommandBuffer commandBuffer)
	{
		//make a clear-color from frame number. This will flash with a 120 frame period.
		//VkClearColorValue clearValue;
		//clearValue = { 0.3f, 0.3f, 0.3f, 1.0f };

		//VkImageSubresourceRange clearRange = VkInit::GetImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		//clear image
		//vkCmdClearColorImage(commandBuffer, m_DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
		ComputeEffect& effect = m_BackgroundEffects[m_CurrentBackgroundEffect];

		// bind the gradient drawing compute pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, effect.Pipeline);

		// bind the descriptor set containing the draw image for the compute pipeline
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_GradientPipelineLayout, 0, 1, &m_DrawImageDescriptors, 0, nullptr);

		vkCmdPushConstants(commandBuffer, m_GradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &effect.Data);

		// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
		vkCmdDispatch(commandBuffer, std::ceil(m_DrawExtent.width / 16.0), std::ceil(m_DrawExtent.height / 16.0), 1);
	}

	void VkGraphicsDevice::ImmediateCommandSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
	{
		VK_CHECK(vkResetFences(m_Device, 1, &m_ImGuiFence));
		VK_CHECK(vkResetCommandBuffer(m_ImGuiCommandBuffer, 0));

		VkCommandBuffer cmd = m_ImGuiCommandBuffer;

		VkCommandBufferBeginInfo cmdBeginInfo = VkInit::BuildCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		function(cmd);

		VK_CHECK(vkEndCommandBuffer(cmd));

		VkCommandBufferSubmitInfo cmdinfo = VkInit::BuildCommandBufferSubmitInfo(cmd);
		VkSubmitInfo2 submit = VkInit::BuildSubmitInfo2(&cmdinfo, nullptr, nullptr);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK(vkQueueSubmit2(m_GraphicsQueue, 1, &submit, m_ImGuiFence));

		VK_CHECK(vkWaitForFences(m_Device, 1, &m_ImGuiFence, true, 9999999999));
	}

	void VkGraphicsDevice::DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView)
	{
		VkRenderingAttachmentInfo colorAttachment = VkInit::BuildRenderingAttachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		VkRenderingInfo renderInfo = VkInit::BuildRenderingInfo(m_SwapchainExtent, &colorAttachment, nullptr);

		vkCmdBeginRendering(cmd, &renderInfo);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		vkCmdEndRendering(cmd);
	}

	void VkGraphicsDevice::DrawGeometry(VkCommandBuffer cmd)
	{
		//begin a render pass  connected to our draw image
		VkRenderingAttachmentInfo colorAttachment = VkInit::BuildRenderingAttachmentInfo(m_DrawImage.ImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingInfo renderInfo = VkInit::BuildRenderingInfo(m_DrawExtent, &colorAttachment, nullptr);
		vkCmdBeginRendering(cmd, &renderInfo);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline);

		//set dynamic viewport and scissor
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = m_DrawExtent.width;
		viewport.height = m_DrawExtent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_DrawExtent.width;
		scissor.extent.height = m_DrawExtent.height;

		vkCmdSetScissor(cmd, 0, 1, &scissor);

		//launch a draw command to draw 3 vertices
		//vkCmdDraw(cmd, 3, 1, 0, 0);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_MeshPipeline);

		GPUDrawPushConstants push_constants;
		push_constants.WorldMatrix = glm::mat4{ 1.0f };
		//push_constants.MeshBufferAddress = m_Rectangle.VertexBufferAddress;

		vkCmdPushConstants(cmd, m_MeshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
		//vkCmdBindIndexBuffer(cmd, m_Rectangle.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);

		vkCmdEndRendering(cmd);
	}

	void VkGraphicsDevice::DrawTriangle(VkCommandBuffer cmd)
	{
		//begin a render pass  connected to our draw image
		VkClearValue clearValue = {0.27f, 0.3f, 0.32f, 1.0f};
		VkRenderingAttachmentInfo colorAttachment = VkInit::BuildRenderingAttachmentInfo(m_DrawImage.ImageView, &clearValue, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingInfo renderInfo = VkInit::BuildRenderingInfo(m_DrawExtent, &colorAttachment, nullptr);
		vkCmdBeginRendering(cmd, &renderInfo);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline);

		//set dynamic viewport and scissor
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = m_DrawExtent.width;
		viewport.height = m_DrawExtent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_DrawExtent.width;
		scissor.extent.height = m_DrawExtent.height;

		vkCmdSetScissor(cmd, 0, 1, &scissor);

		vkCmdDraw(cmd, 3, 1, 0, 0);

		vkCmdEndRendering(cmd);
	}

	AllocatedBuffer VkGraphicsDevice::AllocateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
	{
		// allocate buffer
		VkBufferCreateInfo bufferInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.pNext = nullptr;
		bufferInfo.size = allocSize;

		bufferInfo.usage = usage;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = memoryUsage;
		vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		AllocatedBuffer newBuffer;
		newBuffer.Size = allocSize;

		// allocate the buffer
		VK_CHECK(vmaCreateBuffer(m_Allocator, &bufferInfo, &vmaallocInfo, &newBuffer.Buffer, &newBuffer.Allocation, &newBuffer.AllocationInfo));

		return newBuffer;
	}

	void VkGraphicsDevice::UploadDataToBuffer(AllocatedBuffer &buffer, void *data, size_t dataSize)
	{
		AllocatedBuffer staging = AllocateBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* mappedData;
		vmaMapMemory(m_Allocator, staging.Allocation, &mappedData);

		// copy data into the staging buffer
		memcpy(mappedData, data, dataSize);

		ImmediateCommandSubmit([&](VkCommandBuffer cmd) {
			VkBufferCopy buffCopy{ 0 };
			buffCopy.dstOffset = 0;
			buffCopy.srcOffset = 0;
			buffCopy.size = dataSize;

			vkCmdCopyBuffer(cmd, staging.Buffer, buffer.Buffer, 1, &buffCopy);
		});

		DestroyBuffer(staging);
	}

	void VkGraphicsDevice::DestroyBuffer(const AllocatedBuffer& buffer)
	{
		vmaDestroyBuffer(m_Allocator, buffer.Buffer, buffer.Allocation);
	}

	GPUMeshBuffer VkGraphicsDevice::UploadMeshOld(std::span<uint32_t> indices, std::span<VertexOld> vertices)
	{
		const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
		const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

		GPUMeshBuffer newSurface;

		//create vertex buffer
		newSurface.VertexBuffer = AllocateBuffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY);

		//find the adress of the vertex buffer
		VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = newSurface.VertexBuffer.Buffer };
		newSurface.VertexBufferAddress = vkGetBufferDeviceAddress(m_Device, &deviceAdressInfo);

		//create index buffer
		newSurface.IndexBuffer = AllocateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		AllocatedBuffer staging = AllocateBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);


		// https://www.reddit.com/r/vulkan/comments/1f4alg0/i_keep_getting_this_c2027_error_about/
		//void* data = staging.Allocation->GetMappedData();
		void* data;
		vmaMapMemory(m_Allocator, staging.Allocation, &data);

		// copy vertex buffer
		memcpy(data, vertices.data(), vertexBufferSize);
		// copy index buffer
		memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

		ImmediateCommandSubmit([&](VkCommandBuffer cmd) {
			VkBufferCopy vertexCopy{ 0 };
			vertexCopy.dstOffset = 0;
			vertexCopy.srcOffset = 0;
			vertexCopy.size = vertexBufferSize;

			vkCmdCopyBuffer(cmd, staging.Buffer, newSurface.VertexBuffer.Buffer, 1, &vertexCopy);

			VkBufferCopy indexCopy{ 0 };
			indexCopy.dstOffset = 0;
			indexCopy.srcOffset = vertexBufferSize;
			indexCopy.size = indexBufferSize;

			vkCmdCopyBuffer(cmd, staging.Buffer, newSurface.IndexBuffer.Buffer, 1, &indexCopy);
		});

		DestroyBuffer(staging);

		return newSurface;
	}
}