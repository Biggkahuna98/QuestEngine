#include "VkGraphicsDevice.h"
#include "Core/Log.h"

#include "VkInit.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include "GLFW/glfw3.h"

#include "VKGraphicsContext.h"

// ImGui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace QE
{
	VkGraphicsDevice::VkGraphicsDevice(Window* window)
		: GraphicsDevice(window), m_Window(window) // refactor to stored in graphicsdevice
	{
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

		if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create VMA allocator");
		}
		LOG_DEBUG_TAG("VkGraphicsDevice", "VMA Allocator created");

		m_CleanupQueue.PushFunction([&]() {
			vmaDestroyAllocator(m_Allocator);
		});

		// Set the queue family indices
		m_QueueFamilyIndices = VkInit::FindQueueFamilies(m_PhysicalDevice, m_Surface);

		// Swapchain
		InitSwapchain(m_WindowExtent);

		// Command pools and buffers
		InitializeFrameData();
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan command pools and buffers created");

		// Descriptors
		InitializeDescriptors();
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan descriptors created");

		// Graphics pipeline
		//VkInit::CreateGraphicsPipeline(m_Device, &m_PipelineLayout);
		InitializePipelines();

		InitializeImGui();
	}

	VkGraphicsDevice::~VkGraphicsDevice()
	{
		WaitForDeviceIdle();
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
		LOG_DEBUG_TAG("VkGraphicsDevice", "Beginning frame: {0}", m_CurrentFrameNumber);
		// Imgui
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Wait for the previous frame to finish
		vkWaitForFences(m_Device, 1, &GetCurrentFrameData().RenderFence, VK_TRUE, UINT64_MAX);

		// See if there is a better place later
		GetCurrentFrameData().CleanupQueue.Flush();

		// Request the image from the swapchain
		vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, GetCurrentFrameData().SwapchainSemaphore, VK_NULL_HANDLE, &m_CurrentSwapchainImageIndex);

		vkResetFences(m_Device, 1, &GetCurrentFrameData().RenderFence);

		// Reset command buffer
		vkResetCommandBuffer(GetCurrentFrameData().CommandBuffer, 0);

		// Begin the buffer for recording
		VkCommandBufferBeginInfo beginInfo = VkInit::BuildCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		if (vkBeginCommandBuffer(GetCurrentFrameData().CommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to begin command buffer recording");
		}

		// transition our main draw image into general layout so we can write into it
		// we will overwrite it all so we dont care about what was the older layout
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_DrawImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		// move later
		ImGui::ShowDemoWindow();
	}

	void VkGraphicsDevice::EndFrame()
	{
		ImGui::Render();
		LOG_DEBUG_TAG("VkGraphicsDevice", "Ending frame: {0}", m_CurrentFrameNumber);

		// Come back to this later
		DrawBackground(GetCurrentFrameData().CommandBuffer);

		// Transition the draw image and the swapchain image into their correct transfer layouts
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_SwapchainImages[m_CurrentSwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Execute a copy from the draw image into the swapchain
		VkInit::CopyImageToImage(GetCurrentFrameData().CommandBuffer, m_DrawImage.Image, m_SwapchainImages[m_CurrentSwapchainImageIndex], m_DrawExtent, m_SwapchainExtent);

		// Set swapchain image layout to Present so we can show it on the screen
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_SwapchainImages[m_CurrentSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		// Draw imgui
		DrawImGui(GetCurrentFrameData().CommandBuffer, m_SwapchainImageViews[m_CurrentSwapchainImageIndex]);

		// End command buffer recording
		if (vkEndCommandBuffer(GetCurrentFrameData().CommandBuffer) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to end command buffer recording");
		}

		// Submit command buffer to the graphics queue
		VkCommandBufferSubmitInfo cmdSubmitInfo = VkInit::BuildCommandBufferSubmitInfo(GetCurrentFrameData().CommandBuffer);

		VkSemaphoreSubmitInfo waitInfo = VkInit::BuildSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,GetCurrentFrameData().SwapchainSemaphore);
		VkSemaphoreSubmitInfo signalInfo = VkInit::BuildSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrameData().RenderSemaphore);	
	
		VkSubmitInfo2 submitInfo = VkInit::BuildSubmitInfo2(&cmdSubmitInfo, &signalInfo, &waitInfo);
		if (vkQueueSubmit2(m_GraphicsQueue, 1, &submitInfo, GetCurrentFrameData().RenderFence) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to submit command buffer to the graphics queue");
			throw std::runtime_error("Failed to submit command buffer to the graphics queue");
		}

		LOG_DEBUG_TAG("VkGraphicsDevice", "Command buffer submitted to the graphics queue");
	}

	void VkGraphicsDevice::PresentFrame()
	{
		LOG_DEBUG_TAG("VkGraphicsDevice", "Presenting frame: {0}", m_CurrentFrameNumber);
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &GetCurrentFrameData().SwapchainSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &m_CurrentSwapchainImageIndex;

		// Change to present queue later
		if (vkQueuePresentKHR(m_GraphicsQueue, &presentInfo) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to present swapchain image");
		}

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

	FrameData& VkGraphicsDevice::GetCurrentFrameData()
	{
		return m_FrameData[m_CurrentFrameNumber % MAX_FRAMES_IN_FLIGHT];
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
		ring_allocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

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
		LOG_DEBUG_TAG("VkGraphicsDevice", "Creating Vulkan swapchain");
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

		// REFACTOR LATER - IMGUI stuff
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
		InitializeBackgroundPipelines();
	}

	void VkGraphicsDevice::InitializeBackgroundPipelines()
	{
		VkPipelineLayoutCreateInfo computeLayout{};
		computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout.pNext = nullptr;
		computeLayout.pSetLayouts = &m_DrawImageDescriptorSetLayout;
		computeLayout.setLayoutCount = 1;

		VK_CHECK(vkCreatePipelineLayout(m_Device, &computeLayout, nullptr, &m_GradientPipelineLayout));

		//layout code
		VkShaderModule computeDrawShader = VkInit::CreateShaderModule(m_Device, "gradient-comp.spv");

		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageinfo.module = computeDrawShader;
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = m_GradientPipelineLayout;
		computePipelineCreateInfo.stage = stageinfo;

		VK_CHECK(vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_GradientPipeline));

		// Cleanup
		vkDestroyShaderModule(m_Device, computeDrawShader, nullptr);

		m_CleanupQueue.PushFunction([&]() {
			vkDestroyPipelineLayout(m_Device, m_GradientPipelineLayout, nullptr);
			vkDestroyPipeline(m_Device, m_GradientPipeline, nullptr);
		});
	}

	void VkGraphicsDevice::InitializeImGui()
	{
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
			vkDestroyDescriptorPool(m_Device, imguiPool, nullptr);
		});
	}

	void VkGraphicsDevice::DrawBackground(VkCommandBuffer commandBuffer)
	{
		//make a clear-color from frame number. This will flash with a 120 frame period.
		//VkClearColorValue clearValue;
		//clearValue = { 0.3f, 0.3f, 0.3f, 1.0f };

		//VkImageSubresourceRange clearRange = VkInit::GetImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		//clear image
		//vkCmdClearColorImage(commandBuffer, m_DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
		// bind the gradient drawing compute pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_GradientPipeline);

		// bind the descriptor set containing the draw image for the compute pipeline
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_GradientPipelineLayout, 0, 1, &m_DrawImageDescriptors, 0, nullptr);

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
}