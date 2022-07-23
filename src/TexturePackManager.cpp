#include "TexturePackManager.hpp"
#include <Rendering/Rendering.hpp>
#include <contrib/minizip/unzip.h>
#include <fstream>

namespace CrossCraft {
using namespace Stardust_Celeste;

TexturePackManager::TexturePackManager() {
    path_names.clear();
    layers.push_back("default");
}
TexturePackManager::~TexturePackManager() {}
auto TexturePackManager::load_texture(std::string filename, u32 magFilter,
                                      u32 minFilter, bool repeat, bool flip)
    -> u32 {

    for (int i = layers.size() - 1; i >= 0; i--) {
        auto &l = layers[i];

        std::string path = "./texturepacks/" + l + "/" + filename;
        std::fstream file(path);

        if (file.is_open()) {
            file.close();

            return Rendering::TextureManager::get().load_texture(
                path, magFilter, minFilter, repeat, flip);
        }
    }

    return -1;
}

auto TexturePackManager::add_layer(std::string name) -> void {
    layers.push_back(name);
}

auto TexturePackManager::scan_folder(std::string path) -> void {
    std::filesystem::path p(path);

    std::filesystem::permissions(path,
        std::filesystem::perms::owner_all |
        std::filesystem::perms::group_all |
        std::filesystem::perms::others_all,
        std::filesystem::perm_options::add);

    for (const auto &entry : std::filesystem::directory_iterator(p)) {

        if (entry.is_directory()) {
            // Add to list
            path_names.push_back(entry.path().filename().string());
        } else if (entry.is_regular_file()) {
            auto filename = entry.path().filename().string();

            if (filename.find(".zip") != std::string::npos) {
                SC_APP_INFO("EXTRACT");
                extract_zip(path + filename);
                path_names.push_back(entry.path().filename().string().substr(
                    0, filename.find(".zip")));
            }
        }
    }

    std::sort(path_names.begin(), path_names.end());
    path_names.erase(std::unique(path_names.begin(), path_names.end()), path_names.end());

    SC_APP_INFO("TEXTURE PACK INFO: ");
    for (auto &str : path_names)
        SC_APP_INFO("Texture Pack Found: {}", str);
}

auto TexturePackManager::extract_zip(std::string path) -> int {
    unzFile zF = unzOpen(path.c_str());

    unz_global_info global_info;
    unzGetGlobalInfo(zF, &global_info);

    char read_buffer[8192];

    std::string dirname = path.substr(0, path.find(".zip")) + "/";
    std::filesystem::create_directory(dirname);
    std::filesystem::permissions(dirname,
                                 std::filesystem::perms::owner_all |
                                     std::filesystem::perms::group_all |
                                     std::filesystem::perms::others_all,
                                 std::filesystem::perm_options::add);

    for (int i = 0; i < global_info.number_entry; i++) {
        unz_file_info file_info;
        char filename[512];

        unzGetCurrentFileInfo(zF, &file_info, filename, 512, NULL, 0, NULL, 0);

        std::string prefix_name =
            path.substr(0, path.find(".zip")) + "/" + std::string(filename);

        const size_t filename_length = prefix_name.length();
        if (prefix_name[filename_length - 1] == '/') {
            std::filesystem::create_directory(prefix_name);
            std::filesystem::permissions(prefix_name,
                                         std::filesystem::perms::owner_all |
                                             std::filesystem::perms::group_all |
                                             std::filesystem::perms::others_all,
                                         std::filesystem::perm_options::add);

            SC_APP_INFO("CREATING: {}", prefix_name);
        } else {
            unzOpenCurrentFile(zF);
            FILE *out = fopen(prefix_name.c_str(), "wb");

            std::filesystem::permissions(prefix_name.c_str(),
                                         std::filesystem::perms::owner_all |
                                             std::filesystem::perms::group_all |
                                             std::filesystem::perms::others_all,
                                         std::filesystem::perm_options::add);

            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(zF, read_buffer, 8192);
                if (error > 0) {
                    fwrite(read_buffer, error, 1, out);
                }
            } while (error > 0);

            fclose(out);
        }

        unzCloseCurrentFile(zF);

        // Go the the next entry listed in the zip file.
        if ((i + 1) < global_info.number_entry)
            unzGoToNextFile(zF);
    }

    unzClose(zF);

    return 0;
}
} // namespace CrossCraft