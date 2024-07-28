
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_funcs.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_hpp_macros.hpp>
#include <vulkan/vulkan_handles.hpp>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

void CreateDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT& pCreateInfo, vk::Optional<const vk::AllocationCallbacks> pAllocator, vk::DebugUtilsMessengerEXT& debugMessenger) {
    //动态加载调用创建扩展函数
    vk::DispatchLoaderDynamic dld(instance, vkGetInstanceProcAddr);
    debugMessenger = instance.createDebugUtilsMessengerEXT(pCreateInfo, pAllocator, dld);
}

void DestroyDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerEXT& debugMessenger, vk::Optional<const vk::AllocationCallbacks> pAllocator) {
    //动态加载调用创建扩展函数
    vk::DispatchLoaderDynamic dld(instance, vkGetInstanceProcAddr);
    instance.destroyDebugUtilsMessengerEXT(debugMessenger, pAllocator, dld);
}

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window_;
    vk::Instance instance_;
    vk::DebugUtilsMessengerEXT debugMessenger_;
    void initWindow() {
        //初始化glfw
        glfwInit();
        //设置glfw不使用OpenGLAPI，不允许缩放
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        //创建glfw窗口
        window_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
    }

    void mainLoop() {
        //事件循环
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
        }

        //销毁glfw窗口
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    void createInstance()
    {
        //创建vulkan instance
        vk::ApplicationInfo app_info;
        app_info.setApiVersion(VK_API_VERSION_1_0);

        vk::InstanceCreateInfo instance_create_info;

        //配置需要的扩展
        uint32_t glfwExtensionCount = 0;
        auto extensions = getRequiredExtensions();
        instance_create_info.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
        instance_create_info.setPpEnabledExtensionNames(extensions.data());
        instance_create_info.setPApplicationInfo(&app_info);

        //配置ValidationLayer
        vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
        if (enableValidationLayers) {
            instance_create_info.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
            instance_create_info.setPpEnabledLayerNames(validationLayers.data());
            instance_create_info.setPNext(&debug_create_info);
            populateDebugMessengerCreateInfo(debug_create_info);
        }
        else {
            instance_create_info.setEnabledLayerCount(0);
            instance_create_info.setPNext(nullptr);
        }

        instance_ = vk::createInstance(instance_create_info);
    }

    void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info)
    {
        create_info.setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        );
        create_info.setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        );
        create_info.setPfnUserCallback(debugCallback);
    }

    void setupDebugMessenger()
    {
        if (!enableValidationLayers) return;

        vk::DebugUtilsMessengerCreateInfoEXT create_info;
        populateDebugMessengerCreateInfo(create_info);
        CreateDebugUtilsMessengerEXT(instance_, create_info, nullptr, debugMessenger_);
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
