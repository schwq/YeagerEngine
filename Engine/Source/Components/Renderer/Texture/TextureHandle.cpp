#include "TextureHandle.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Yeager;

MaterialBase::MaterialBase(const EntityBuilder& builder, const MaterialType::Enum type,
                           const MaterialSurfaceType::Enum surface)
    : EditorEntity(EntityBuilder(builder.Application, builder.Name, EntityObjectType::TEXTURE, builder.UUID)),
      m_MaterialType(type),
      m_SurfaceType(surface)
{
  Yeager::LogDebug(INFO, "Created Material Base {} UUID {}!", mName, uuids::to_string(mEntityUUID));
}

MaterialBase::~MaterialBase()
{
  Yeager::LogDebug(INFO, "Destrorying Material Base {} UUID {}!", mName, uuids::to_string(mEntityUUID));
}

void MaterialBase::ApplyToShader(Yeager::Shader* shader)
{
  shader->UseShader();
  shader->SetVec4("Color", m_Color);
}

MaterialTexture2D::MaterialTexture2D(const EntityBuilder& builder, const MaterialTextureType::Enum texture)
    : MaterialBase(builder, MaterialType::eTEXTURE2D, MaterialSurfaceType::eTEXTURED), m_TextureType(texture)
{}

MaterialTexture2D::~MaterialTexture2D()
{
  if (m_TextureHandle.Generated)
    glDeleteTextures(1, &m_TextureHandle.Texture);
}

GLenum Yeager::ChannelsToFormat(const int channels)
{
  switch (channels) {
    case 1:
      return GL_RED;
    case 3:
      return GL_RGB;
    case 4:
      return GL_RGBA;
    default:
      return GL_RGB;
  }
}

std::optional<Uint> Yeager::FormatToChannels(GLenum format)
{

  switch (format) {
    case GL_RED:
      return 1;
    case GL_RGB:
      return 3;
    case GL_RGBA:
      return 4;
    default:
      Yeager::LogDebug(ERROR, "FormatToChannels(), invalid format requested! {}", format);
      return std::nullopt;
  }
}

void Yeager::DisplayImageImGui(MaterialTexture2D* texture, Uint resize)
{

  ImDrawList* draw = ImGui::GetWindowDrawList();
  draw->AddCallback(ImDrawCallback_ResetRenderState, NULL);
  if (resize == 0)
    resize = 1;
  ImGui::Image((ImTextureID)(intptr_t)texture->GetTextureID(),
               ImVec2(texture->GetWidth() / resize, texture->GetHeight() / resize));
}

void MaterialTexture2D::Unbind2DTextures()
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, NULL);
}

void MaterialTexture2D::BindTexture()
{
  glBindTexture(m_TextureHandle.BindTarget, m_TextureHandle.Texture);
}

void MaterialTexture2D::GenerateFromData(STBIDataOutput* output, const MateriaTextureParameterGL parameteri)
{
  if (m_TextureHandle.Generated)
    Yeager::LogDebug(WARNING, "Material texture already generated! Overrided by a texture 2d!");

  stbi_set_flip_vertically_on_load(output->Flip);

  m_MaterialType = MaterialType::eTEXTURE2D;
  m_TextureHandle.Parameter = parameteri;

  GenerateTextureParameter(parameteri);
  m_TextureHandle.Height = output->Height;
  m_TextureHandle.Width = output->Width;
  m_TextureHandle.Path = output->OriginalPath;
  m_TextureHandle.Generated = true;
  m_TextureHandle.Format = ChannelsToFormat(output->NrComponents);
  m_TextureHandle.BindTarget = parameteri.BindTarget;
  m_TextureHandle.Flipped = output->Flip;

  glTexImage2D(parameteri.BindTarget, 0, m_TextureHandle.Format, output->Width, output->Height, 0,
               m_TextureHandle.Format, GL_UNSIGNED_BYTE, output->Data);
  glGenerateMipmap(parameteri.BindTarget);

  stbi_image_free(output->Data);

  glBindTexture(parameteri.BindTarget, 0);

  Yeager::LogDebug(INFO, "Created Material texture2D {} UUID {}", mName, uuids::to_string(mEntityUUID));
}

void MaterialTexture2D::GenerateTextureParameter(const MateriaTextureParameterGL& parameter)
{
  if (m_TextureHandle.Generated)
    glDeleteTextures(1, &m_TextureHandle.Texture);  // kill the current texture

  glGenTextures(1, &m_TextureHandle.Texture);
  glBindTexture(parameter.BindTarget, m_TextureHandle.Texture);

  glTexParameteri(parameter.BindTarget, GL_TEXTURE_WRAP_S, parameter.WRAP_S);
  glTexParameteri(parameter.BindTarget, GL_TEXTURE_WRAP_T, parameter.WRAP_T);
  glTexParameteri(parameter.BindTarget, GL_TEXTURE_WRAP_R, parameter.WRAP_R);
  glTexParameterf(parameter.BindTarget, GL_TEXTURE_BASE_LEVEL, parameter.BASE_LEVEL);
  glTexParameteri(parameter.BindTarget, GL_TEXTURE_MIN_FILTER, parameter.MIN_FILTER);
  glTexParameteri(parameter.BindTarget, GL_TEXTURE_MAG_FILTER, parameter.MAG_FILTER);
}

bool MaterialTexture2D::RegenerateTextureFlipped(bool flip)
{
  if (!m_TextureHandle.Generated) {
    Yeager::Log(WARNING, "Trying to regenerate a unknown texture handle! Name: [{}] ID: [{}]", mName,
                uuids::to_string(mEntityUUID));
    return false;
  }

  m_TextureHandle.Flipped = flip;

  if (m_MaterialType == MaterialType::eTEXTURE2D) {
    return GenerateFromFile(m_TextureHandle.Path, flip, m_TextureHandle.Parameter);
  } else if (m_MaterialType == MaterialType::eCUBEMAP) {
    return GenerateCubeMapFromFile(m_TextureHandle.CubeMapsPaths, flip, m_TextureHandle.Parameter);
  } else {
    Yeager::Log(WARNING,
                "Texture material type does not support regeneration with the texture flipped!, This is bad code "
                "written! Name: [{}] ID: [{}]",
                mName, uuids::to_string(mEntityUUID));
    return false;
  }
}

bool MaterialTexture2D::GenerateFromFile(const String& path, bool flip, const MateriaTextureParameterGL parameteri)
{
  if (m_TextureHandle.Generated)
    Yeager::LogDebug(WARNING, "Material texture already generated! Overrided by a texture 2d!");

  /* Must reforce the correct type is set to the material */
  m_MaterialType = MaterialType::eTEXTURE2D;

  stbi_set_flip_vertically_on_load(flip);
  m_TextureHandle.Flipped = flip;
  m_TextureHandle.Parameter = parameteri;
  m_TextureHandle.BindTarget = parameteri.BindTarget;

  int channels = 0;
  unsigned char* data = stbi_load(path.c_str(), &m_TextureHandle.Width, &m_TextureHandle.Height, &channels, 0);

  if (data) {
    m_TextureHandle.Format = ChannelsToFormat(channels);

    GenerateTextureParameter(parameteri);

    glTexImage2D(parameteri.BindTarget, 0, m_TextureHandle.Format, m_TextureHandle.Width, m_TextureHandle.Height, 0,
                 m_TextureHandle.Format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(parameteri.BindTarget);

    m_TextureHandle.Path = path;
    m_TextureHandle.Generated = true;

    Yeager::LogDebug(INFO, "Created Material texture2D {} UUID {}", mName, uuids::to_string(mEntityUUID));

    stbi_image_free(data);

    glBindTexture(parameteri.BindTarget, 0);

    return true;

  } else {
    Yeager::LogDebug(ERROR, "Cannot read data from texture file {}, on {} UUID {}", path, mName,
                     uuids::to_string(mEntityUUID));
  }
  return false;
}

bool MaterialTexture2D::GenerateCubeMapFromFile(const std::vector<String>& paths, bool flip,
                                                const MateriaTextureParameterGL parameteri)
{
  if (m_TextureHandle.Generated)
    Yeager::LogDebug(WARNING, "Material texture already generated! Overrided by a cubemap texture!");

  /* Must reforce the correct type is set to the material */
  m_MaterialType = MaterialType::eCUBEMAP;

  stbi_set_flip_vertically_on_load(flip);

  m_TextureHandle.Flipped = flip;
  m_TextureHandle.Parameter = parameteri;
  m_TextureHandle.BindTarget = parameteri.BindTarget;

  int channels = 0;
  bool successed = true;
  for (Uint i = 0; i < paths.size(); i++) {
    unsigned char* data = stbi_load(paths[i].c_str(), &m_TextureHandle.Width, &m_TextureHandle.Height, &channels, 0);

    if (data) {
      m_TextureHandle.Format = ChannelsToFormat(channels);

      GenerateTextureParameter(parameteri);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_TextureHandle.Format, m_TextureHandle.Width,
                   m_TextureHandle.Height, 0, m_TextureHandle.Format, GL_UNSIGNED_BYTE, data);

      glGenerateMipmap(parameteri.BindTarget);

      m_TextureHandle.CubeMapsPaths = paths;
      m_TextureHandle.Generated = true;

      stbi_image_free(data);

      Yeager::LogDebug(INFO, "Created Material texture cube map {} UUID {}", mName, uuids::to_string(mEntityUUID));

    } else {
      Yeager::LogDebug(ERROR, "Cannot read data from texture file {}, on {} UUID {}", paths[i].c_str(), mName,
                       uuids::to_string(mEntityUUID));
      successed = false;
    }
  }

  glBindTexture(parameteri.BindTarget, 0);
  return successed;
}