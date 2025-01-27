#pragma once
#include <Rendering/Rendering.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace CrossCraft {

class TexturePackManager {
   public:
    TexturePackManager();
    ~TexturePackManager();

    auto write_config() -> void;

    auto scan_folder(std::string path) -> void;
    auto get_path() -> std::string;
    auto load_texture(std::string filename, u32 magFilter, u32 minFilter,
                      bool repeat, bool flip = false, bool vram = false) -> u32;
    auto get_file(std::string filename) -> std::string;
    auto add_layer(std::string name) -> void;

    auto convert_old_texturepacks() -> void;

    inline static auto get() -> TexturePackManager& {
        static TexturePackManager txm;
        return txm;
    }

    std::vector<std::string> path_names;
    std::vector<std::string> layers;

   private:
    auto extract_zip(std::string path) -> int;
};

}  // namespace CrossCraft
