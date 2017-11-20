/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#include "ShaderLibrary.h"

LineShader::LineShader() : 
  Shader("Resource/Shader/line.vert", "Resource/Shader/line.frag")
{
  APosition = GetAttribLocation("APosition");
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  ULineColor = GetUniformLocation("ULineColor");
}

void LineShader::EnableAttributes()
{
  glVertexAttribPointer(APosition, 3, GL_FLOAT, GL_FALSE,
    3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(APosition);
}

void LineShader::DisableAttributes()
{
  glDisableVertexAttribArray(APosition);
}

SolidShader::SolidShader() :
  Shader("Resource/Shader/solid.vert", "Resource/Shader/solid.frag")
{
  APosition = GetAttribLocation("APosition");
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  UColor = GetUniformLocation("UColor");
}

void SolidShader::EnableAttributes()
{
  glVertexAttribPointer(APosition, 3, GL_FLOAT, GL_FALSE,
    3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(APosition);
}

void SolidShader::DisableAttributes()
{
  glDisableVertexAttribArray(APosition);
}

PhongShader::PhongShader() :
  Shader("Resource/Shader/phong.vert", "Resource/Shader/phong.frag")
{
  // finding attributes
  APosition = GetAttribLocation("APosition");
  ANormal = GetAttribLocation("ANormal");
  // finding uniforms
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  UCameraPosition = GetUniformLocation("UCameraPosition");
  UEmissiveColor = GetUniformLocation("UEmissiveColor");
  UGlobalAmbientColor = GetUniformLocation("UGlobalAmbientColor");
  // finding material uniforms
  UMaterial.UColor = GetUniformLocation("UMaterial.UColor");
  UMaterial.UAmbientFactor = GetUniformLocation("UMaterial.UAmbientFactor");
  UMaterial.UDiffuseFactor = GetUniformLocation("UMaterial.UDiffuseFactor");
  UMaterial.USpecularFactor = GetUniformLocation("UMaterial.USpecularFactor");
  UMaterial.USpecularExponent = GetUniformLocation("UMaterial.USpecularExponent");
  UMaterial.UTextureMapping = GetUniformLocation("UMaterial.UTextureMapping");
  UMaterial.UMappingType = GetUniformLocation("UMaterial.UMappingType");
  UMaterial.UDiffuseMap = GetUniformLocation("UMaterial.UDiffuseMap");
  UMaterial.USpecularMap = GetUniformLocation("UMaterial.USpecularMap");
  // finding light uniforms
  for (unsigned int i = 0; i < MAXLIGHTS; ++i) {
    std::string index(std::to_string(i));
    ULights[i].UType = GetUniformLocation(
      "ULights[" + index + "].UType");
    ULights[i].UPosition = GetUniformLocation(
      "ULights[" + index + "].UPosition");
    ULights[i].UDirection = GetUniformLocation(
      "ULights[" + index + "].UDirection");
    ULights[i].UInnerAngle = GetUniformLocation(
      "ULights[" + index + "].UInnerAngle");
    ULights[i].UOuterAngle = GetUniformLocation(
      "ULights[" + index + "].UOuterAngle");
    ULights[i].USpotExponent = GetUniformLocation(
      "ULights[" + index + "].USpotExponent");
    ULights[i].UAmbientColor = GetUniformLocation(
      "ULights[" + index + "].UAmbientColor");
    ULights[i].UDiffuseColor = GetUniformLocation(
      "ULights[" + index + "].UDiffuseColor");
    ULights[i].USpecularColor = GetUniformLocation(
      "ULights[" + index + "].USpecularColor");
    ULights[i].UAttenuationC0 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC0");
    ULights[i].UAttenuationC1 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC1");
    ULights[i].UAttenuationC2 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC2");
  }
  UActiveLights = GetUniformLocation("UActiveLights");
  // finding fog uniforms
  UFogColor = GetUniformLocation("UFogColor");
  UNearPlane = GetUniformLocation("UNearPlane");
  UFarPlane = GetUniformLocation("UFarPlane");
}
void PhongShader::EnableAttributes()
{
  glVertexAttribPointer(APosition, 3, GL_FLOAT, GL_FALSE,6 * sizeof(GLfloat),
    nullptr);
  glVertexAttribPointer(ANormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
    (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(APosition);
  glEnableVertexAttribArray(ANormal);
}
void PhongShader::DisableAttributes()
{
  glDisableVertexAttribArray(APosition);
  glDisableVertexAttribArray(ANormal);
}

GouraudShader::GouraudShader() :
  Shader("Resource/Shader/gouraud.vert", "Resource/Shader/gouraud.frag")
{
  // finding attributes
  APosition = GetAttribLocation("APosition");
  ANormal = GetAttribLocation("ANormal");
  // finding uniforms
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  UCameraPosition = GetUniformLocation("UCameraPosition");
  UEmissiveColor = GetUniformLocation("UEmissiveColor");
  UGlobalAmbientColor = GetUniformLocation("UGlobalAmbientColor");
  // finding material uniforms
  UMaterial.UColor = GetUniformLocation("UMaterial.UColor");
  UMaterial.UAmbientFactor = GetUniformLocation("UMaterial.UAmbientFactor");
  UMaterial.UDiffuseFactor = GetUniformLocation("UMaterial.UDiffuseFactor");
  UMaterial.USpecularFactor = GetUniformLocation("UMaterial.USpecularFactor");
  UMaterial.USpecularExponent = GetUniformLocation("UMaterial.USpecularExponent");
  // finding light uniforms
  for (unsigned int i = 0; i < MAXLIGHTS; ++i) {
    std::string index(std::to_string(i));
    ULights[i].UType = GetUniformLocation(
      "ULights[" + index + "].UType");
    ULights[i].UPosition = GetUniformLocation(
      "ULights[" + index + "].UPosition");
    ULights[i].UDirection = GetUniformLocation(
      "ULights[" + index + "].UDirection");
    ULights[i].UInnerAngle = GetUniformLocation(
      "ULights[" + index + "].UInnerAngle");
    ULights[i].UOuterAngle = GetUniformLocation(
      "ULights[" + index + "].UOuterAngle");
    ULights[i].USpotExponent = GetUniformLocation(
      "ULights[" + index + "].USpotExponent");
    ULights[i].UAmbientColor = GetUniformLocation(
      "ULights[" + index + "].UAmbientColor");
    ULights[i].UDiffuseColor = GetUniformLocation(
      "ULights[" + index + "].UDiffuseColor");
    ULights[i].USpecularColor = GetUniformLocation(
      "ULights[" + index + "].USpecularColor");
    ULights[i].UAttenuationC0 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC0");
    ULights[i].UAttenuationC1 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC1");
    ULights[i].UAttenuationC2 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC2");
  }
  UActiveLights = GetUniformLocation("UActiveLights");
  // finding fog uniforms
  UFogColor = GetUniformLocation("UFogColor");
  UNearPlane = GetUniformLocation("UNearPlane");
  UFarPlane = GetUniformLocation("UFarPlane");
}
void GouraudShader::EnableAttributes()
{
  glVertexAttribPointer(APosition, 3, GL_FLOAT, GL_FALSE,
    6 * sizeof(GLfloat), nullptr);
  glVertexAttribPointer(ANormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
    (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(APosition);
  glEnableVertexAttribArray(ANormal);
}
void GouraudShader::DisableAttributes()
{
  glDisableVertexAttribArray(APosition);
  glDisableVertexAttribArray(ANormal);
}

BlinnShader::BlinnShader() :
  Shader("Resource/Shader/blinn.vert", "Resource/Shader/blinn.frag")
{
  // finding attributes
  APosition = GetAttribLocation("APosition");
  ANormal = GetAttribLocation("ANormal");
  // finding uniforms
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  UCameraPosition = GetUniformLocation("UCameraPosition");
  UEmissiveColor = GetUniformLocation("UEmissiveColor");
  UGlobalAmbientColor = GetUniformLocation("UGlobalAmbientColor");
  // finding material uniforms
  UMaterial.UColor = GetUniformLocation("UMaterial.UColor");
  UMaterial.UAmbientFactor = GetUniformLocation("UMaterial.UAmbientFactor");
  UMaterial.UDiffuseFactor = GetUniformLocation("UMaterial.UDiffuseFactor");
  UMaterial.USpecularFactor = GetUniformLocation("UMaterial.USpecularFactor");
  UMaterial.USpecularExponent = GetUniformLocation("UMaterial.USpecularExponent");
  // finding light uniforms
  for (unsigned int i = 0; i < MAXLIGHTS; ++i) {
    std::string index(std::to_string(i));
    ULights[i].UType = GetUniformLocation(
      "ULights[" + index + "].UType");
    ULights[i].UPosition = GetUniformLocation(
      "ULights[" + index + "].UPosition");
    ULights[i].UDirection = GetUniformLocation(
      "ULights[" + index + "].UDirection");
    ULights[i].UInnerAngle = GetUniformLocation(
      "ULights[" + index + "].UInnerAngle");
    ULights[i].UOuterAngle = GetUniformLocation(
      "ULights[" + index + "].UOuterAngle");
    ULights[i].USpotExponent = GetUniformLocation(
      "ULights[" + index + "].USpotExponent");
    ULights[i].UAmbientColor = GetUniformLocation(
      "ULights[" + index + "].UAmbientColor");
    ULights[i].UDiffuseColor = GetUniformLocation(
      "ULights[" + index + "].UDiffuseColor");
    ULights[i].USpecularColor = GetUniformLocation(
      "ULights[" + index + "].USpecularColor");
    ULights[i].UAttenuationC0 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC0");
    ULights[i].UAttenuationC1 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC1");
    ULights[i].UAttenuationC2 = GetUniformLocation(
      "ULights[" + index + "].UAttenuationC2");
  }
  UActiveLights = GetUniformLocation("UActiveLights");
  // finding fog uniforms
  UFogColor = GetUniformLocation("UFogColor");
  UNearPlane = GetUniformLocation("UNearPlane");
  UFarPlane = GetUniformLocation("UFarPlane");
}

void BlinnShader::EnableAttributes()
{
  glVertexAttribPointer(APosition, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
    nullptr);
  glVertexAttribPointer(ANormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
    (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(APosition);
  glEnableVertexAttribArray(ANormal);
}
void BlinnShader::DisableAttributes()
{
  glDisableVertexAttribArray(APosition);
  glDisableVertexAttribArray(ANormal);
}

TextureShader::TextureShader() :
  Shader("Resource/Shader/texture.vert", "Resource/Shader/texture.frag")
{
  APosition = GetAttribLocation("APosition");
  ATexCoord = GetAttribLocation("ATexCoord");
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  UTexture = GetUniformLocation("UTexture");
}