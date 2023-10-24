#include "TextureHandle.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../../../Libraries/stb_image.h"

using namespace Yeager;

yg_uint Texture2D::m_texture_count = 0;

Texture2D::~Texture2D()
{
  glDeleteTextures(1, &m_id);
}

yg_uint LoadTextureFromFile(yg_string path, bool flip)
{
  stbi_set_flip_vertically_on_load(flip);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  yg_uint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  int width, height, nrComponents;
  unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format = GL_RGB;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {

    stbi_image_free(data);
  }
  glBindTexture(GL_TEXTURE_2D, NULL);
  return textureID;
}

bool LoadTextureFromFile(yg_cchar filename, GLuint* out_texture, int* out_width, int* out_height)
{
  // Load from file
  int image_width = 0;
  int image_height = 0;
  unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
  if (image_data == NULL)
    return false;

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE);                                    // This is required on WebGL for non
                                                                        // power-of-two textures
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  // Same

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  *out_texture = image_texture;
  *out_width = image_width;
  *out_height = image_height;

  return true;
}

void Texture2D::GenerateTexture()
{
  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_2D, m_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::ReadDataToTexture(yg_cchar path)
{
  int width, height, channels;
  yg_uint format;
  unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

  if (data) {
    if (channels == 1) {
      format = GL_RED;
    } else if (channels == 3) {
      format = GL_RGB;
    } else if (channels == 4) {
      format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    Yeager::Log(INFO, kSystem, "Create texture: {}, ID: {}", m_name.c_str(), m_texture_num);
    stbi_image_free(data);
  } else {
    Yeager::Log(ERROR, kSystem, "Cannot read data from texture file {}", path);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(yg_cchar texturePath, yg_string name) : m_name(name)
{
  m_texture_num = m_texture_count++;

  if (name == "DEFAULT") {
    name += std::to_string(m_texture_count);
  }

  stbi_set_flip_vertically_on_load(false);

  GenerateTexture();
  ReadDataToTexture(texturePath);
}

Skybox::~Skybox()
{
  glDeleteTextures(1, &m_id);
}

Skybox::Skybox(std::vector<yg_string> faces, yg_string name) : m_name(name)
{
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glBindVertexArray(0);
  stbi_set_flip_vertically_on_load(false);

  GenerateTexture();
  ReadDataToTexture(faces);
}

void Skybox::GenerateTexture()
{
  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::ReadDataToTexture(std::vector<yg_string> faces)
{
  int width, height, channels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

      stbi_image_free(data);
    } else {
      Yeager::Log(ERROR, kSystem, "Cannot read data from skybox file {}", faces[i].c_str());
      stbi_image_free(data);
    }
  }
  Yeager::Log(INFO, kSystem, "Create Skybox: {}", m_name.c_str());
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::Draw(Yeager::Shader* shader, yg_mat4 view, yg_mat4 projection)
{
  glDepthFunc(GL_LEQUAL);
  shader->UseShader();
  shader->SetMat4("view", view);
  shader->SetMat4("projection", projection);
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);
}