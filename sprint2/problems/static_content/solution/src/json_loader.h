#pragma once

#include <filesystem>

#include "model.h"

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path, const std::filesystem::path& base_path);

}  // namespace json_loader
