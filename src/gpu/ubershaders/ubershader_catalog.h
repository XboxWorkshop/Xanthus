#pragma once

// Shared machinery for generated ubershader variant catalogs.
// - Each shader family supplies its generated tables plus hashing and
//   normalization rules
// - This header adds selection, membership tests and lazy shader creation
// - Everything is a template over the family, so generated code matches a
//   hand-written per-family implementation

#include "shader_variant.h"

#include <plume_render_interface.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fmt/format.h>
#include <iterator>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

// A family provides:
// - `States`: generated state table, sorted by `State::hash`; hashes are
//   unique per state, so hash equality identifies a state
// - `Programs`: generated program table keyed by `Program::key`, sorted by
//   `key` ascending
// - `Hash(state)`: stable 64-bit hash of a normalized state
// - `Normalize(state)`: maps a raw state to its table representation;
//   identity for families that match raw states
// - `Name`: prefix for created RenderShader names
template<typename Family>
class UbershaderCatalog
{
public:
    using StateType = std::remove_const_t<decltype(Family::States[0].state)>;

    // Stores the device used to create variant shaders.
    // Aborts when reinitialized with a different device or graphics API:
    // shaders cached so far belong to the old device.
    void Initialize(plume::RenderDevice* device, bool vulkan)
    {
        if (m_device != nullptr)
        {
            if (m_device != device || m_vulkan != vulkan)
                std::abort();
            return;
        }

        m_device = device;
        m_vulkan = vulkan;

#ifndef NDEBUG
        // The committed tables must stay self-consistent with selection.
        for (const auto& entry : Family::States)
        {
            assert(Family::Hash(entry.state) == entry.hash);
            assert(Select(entry.state) == entry.programKey);
        }
        for (size_t i = 1; i < std::size(Family::States); ++i)
            assert(Family::States[i - 1].hash < Family::States[i].hash);
        for (size_t i = 1; i < std::size(Family::Programs); ++i)
            assert(Family::Programs[i - 1].key < Family::Programs[i].key);
#endif
    }

    // Returns the program key selected by `state`, or GenericShaderVariant
    // when the state has no specialized program.
    ShaderVariantKey Select(const StateType& state)
    {
        // Families whose Normalize returns a reference skip the state copy
        // entirely; normalizing families pay one copy per lookup.
        if constexpr (std::is_lvalue_reference_v<decltype(Family::Normalize(std::declval<const StateType&>()))>)
        {
            return Lookup(Family::Normalize(state));
        }
        else
        {
            const StateType normalizedState = Family::Normalize(state);
            return Lookup(normalizedState);
        }
    }

    // Returns true when `key` is the generic fallback or a known program key.
    bool Contains(ShaderVariantKey key)
    {
        if (key == GenericShaderVariant)
            return true;

        return FindProgram(key) != ProgramNotFound;
    }

    // Returns the shader for `key`, creating it on first use.
    // Returns nullptr for the generic key, unknown keys and before
    // Initialize; callers bind their generic ubershader in that case.
    plume::RenderShader* Resolve(ShaderVariantKey key)
    {
        if (key == GenericShaderVariant || m_device == nullptr)
            return nullptr;

        const size_t index = FindProgram(key);
        if (index == ProgramNotFound)
            return nullptr;

        std::lock_guard lock(m_shaderMutex);
        auto& shader = m_shaders[index];
        if (shader == nullptr)
        {
            const auto& program = Family::Programs[index];
#ifdef XANTHUS_D3D12
            const void* data = m_vulkan ? static_cast<const void*>(program.spirv) : static_cast<const void*>(program.dxil);
            size_t size = m_vulkan ? program.spirvSize : program.dxilSize;
            auto format = m_vulkan ? plume::RenderShaderFormat::SPIRV : plume::RenderShaderFormat::DXIL;
#else
            const void* data = program.spirv;
            size_t size = program.spirvSize;
            auto format = plume::RenderShaderFormat::SPIRV;
#endif
            shader = m_device->createShader(data, size, "main", format);
            shader->setName(fmt::format("{}_{:016X}", Family::Name, key));
        }

        return shader.get();
    }

private:
    // Index returned by FindProgram for keys missing from the table.
    static constexpr size_t ProgramNotFound = static_cast<size_t>(-1);

    // Binary-searches the program table by its sorted keys.
    static size_t FindProgram(ShaderVariantKey key)
    {
        const auto first = std::lower_bound(std::begin(Family::Programs), std::end(Family::Programs), key,
            [](const auto& program, ShaderVariantKey value)
            {
                return program.key < value;
            });
        if (first != std::end(Family::Programs) && first->key == key)
            return static_cast<size_t>(first - std::begin(Family::Programs));
        return ProgramNotFound;
    }

    // Binary-searches the state table by hash.
    // 
    // WE DO NOT CHECK EQUALITY ON STATE ITSELF.
    //
    // At build time, we verify there's no duplicate hashes instead.
    // The 'states' are always stored in ascending order, so we can binary search here.
    static ShaderVariantKey Lookup(const StateType& state)
    {
        const uint64_t hash = Family::Hash(state);
        const auto first = std::lower_bound(std::begin(Family::States), std::end(Family::States), hash,
            [](const auto& entry, uint64_t value)
            {
                return entry.hash < value;
            });
        if (first != std::end(Family::States) && first->hash == hash)
            return first->programKey;
        return GenericShaderVariant;
    }

    plume::RenderDevice* m_device = nullptr;
    bool m_vulkan = false;
    std::vector<std::unique_ptr<plume::RenderShader>> m_shaders{ std::size(Family::Programs) };
    std::mutex m_shaderMutex;
};
