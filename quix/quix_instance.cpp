#ifndef _QUIX_INSTANCE_CPP
#define _QUIX_INSTANCE_CPP

#include "quix_instance.hpp"

#include "quix_command_list.hpp"
#include "quix_common.hpp"
#include "quix_descriptor.hpp"
#include "quix_device.hpp"
#include "quix_pipeline.hpp"
#include "quix_render_target.hpp"
#include "quix_swapchain.hpp"
#include "quix_window.hpp"

namespace quix {

instance::instance(const char* app_name,
    uint32_t app_version,
    int width,
    int height)
    : m_window(allocate_shared<window>(app_name, width, height))
    , m_device(allocate_shared<device>(
          m_window,
          app_name,
          app_version,
          "quix",
          VK_MAKE_VERSION(1, 0, 0)))
    , m_swapchain(nullptr)
    , m_pipeline_manager(nullptr)
    , m_descriptor_allocator(nullptr)
    , m_descriptor_layout_cache(nullptr)
{
}

instance::~instance() = default;

void instance::create_device(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features)
{
    m_device->init(std::move(requested_extensions), requested_features);

    m_descriptor_allocator = allocate_unique<descriptor::allocator>(m_device->get_logical_device());
    m_descriptor_layout_cache = allocate_unique<descriptor::layout_cache>(m_device->get_logical_device());
}

void instance::create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode)
{
    m_swapchain = allocate_shared<swapchain>(m_window, m_device, frames_in_flight, present_mode);
}

void instance::create_pipeline_manager()
{
    m_pipeline_manager = allocate_shared<graphics::pipeline_manager>(m_device);
}

NODISCARD std::shared_ptr<command_pool> instance::get_command_pool()
{
    return allocate_shared<command_pool>(m_device, m_device->get_command_pool());
}

NODISCARD std::shared_ptr<render_target> instance::create_render_target(const VkRenderPassCreateInfo&& render_pass_create_info) noexcept
{
    return allocate_shared<render_target>(m_window, m_device, m_swapchain, &render_pass_create_info);
}

NODISCARD std::shared_ptr<sync> instance::create_sync_objects() noexcept
{
    return allocate_shared<sync>(m_device, m_swapchain);
}

void instance::wait_idle()
{
    m_device->wait_idle();
}

NODISCARD std::shared_ptr<window>
instance::get_window() const noexcept
{
    return m_window;
}

NODISCARD VkDevice
instance::get_logical_device() const noexcept
{
    return m_device->get_logical_device();
}

NODISCARD VkSurfaceFormatKHR instance::get_surface_format() const noexcept
{
    return m_swapchain->get_surface_format();
}

NODISCARD std::shared_ptr<graphics::pipeline_manager> instance::get_pipeline_manager() const noexcept
{
    return m_pipeline_manager;
}

NODISCARD descriptor::allocator_pool instance::get_descriptor_allocator_pool() const noexcept
{
    return m_descriptor_allocator->getPool();
}

NODISCARD descriptor::builder instance::get_descriptor_builder(descriptor::allocator_pool* allocator_pool) const noexcept
{
    return descriptor::builder { m_descriptor_layout_cache.get(), allocator_pool };
}

NODISCARD std::shared_ptr<device> instance::get_device() const noexcept
{
    return m_device;
}

} // namespace quix

#endif // _QUIX_DEVICE_CPP