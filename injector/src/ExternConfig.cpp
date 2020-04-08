#include "ExternConfig.h"

ExternConfig::ExternConfig() {
  memset(&config_, 0, sizeof(config_));
}

std::vector<char> ExternConfig::GetConfigBlob() const {
  const char* config = reinterpret_cast<const char*>(&config_);
  std::vector<char> blob(config, config + sizeof(config_));
  return blob;
}