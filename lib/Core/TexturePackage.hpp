#ifndef TEXTURE_PACKAGE_HPP
#define TEXTURE_PACKAGE_HPP

#include "Common.hpp"

class TexturePackage {
private:
  std ::unordered_map<std ::string, Texture2D> textures;

public:
  TexturePackage() = default;
  ~TexturePackage() = default;

  // Prevent copies — GPU texture handles would double-free
  TexturePackage(const TexturePackage &) = delete;
  TexturePackage &operator=(const TexturePackage &) = delete;

  void AddTexture(const std ::string &name, const std ::string &path);
  Texture2D *GetTexture(const std ::string &name);
};

#endif