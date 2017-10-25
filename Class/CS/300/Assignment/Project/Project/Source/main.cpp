/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file main.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 02/10/2017
\brief
  Entry point for the program. Implementation handles all editor 
  functionality, other interface code, and GL code from drawing.
*/
/*****************************************************************************/

#include "External\Imgui\imgui.h"
#include "External\Imgui\imgui_impl_sdl_gl3.h"

#include "Graphics\SDLContext.h"
#include <GL\glew.h>
#include "Utility\OpenGLError.h"

#include "Graphics\OpenGLContext.h"
#include "Core\Time.h"
#include "Math\Matrix4.h"
#include "Math\Vector3.h"
#include "Math\EulerAngles.h"
#include "Math\MathFunctions.h"

#include "Graphics\Mesh\Mesh.h"
#include "Graphics\Mesh\MeshRenderer.h"
#include "Graphics\ShaderLibrary.h"
#include "Graphics\Camera.h"

#include "Core\Input.h"
#include "Core\Framer.h"
#include "Graphics\Texture.h"


#define PI 3.141592653589f
#define FPS 60
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

struct Material
{
  Material() : _color(1.0f, 1.0f, 1.0f), _ambientFactor(0.0f), 
    _diffuseFactor(0.0f), _specularFactor(0.0f), _specularExponent(1.0f)
  {}
  void SetUniforms(PhongShader * phong_shader)
  {
    glUniform3f(phong_shader->UMaterial.UColor, 
      _color._r, _color._g, _color._b);
    glUniform1f(phong_shader->UMaterial.UAmbientFactor, _ambientFactor);
    glUniform1f(phong_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
    glUniform1f(phong_shader->UMaterial.USpecularFactor, _specularFactor);
    glUniform1f(phong_shader->UMaterial.USpecularExponent, _specularExponent);
  }
  Color _color;
  float _ambientFactor;
  float _diffuseFactor;
  float _specularFactor;
  float _specularExponent;
};


// defining a light
struct Light
{
  Light(): _type(0), _position(0.0f, 0.0f, 0.0f), _direction(0.0f, 0.0f, -1.0f),
    _ambientColor(0.0f, 0.0f, 0.0f), _diffuseColor(0.0f, 0.0f, 0.0f),
    _specularColor(0.0f, 0.0f, 0.0f)
  {}
  Light(unsigned int type, const Math::Vector3 & position, 
    const Math::Vector3 & direction, const Color & ambient_color,
    const Color & diffuse_color, const Color & specular_color) :
    _type(type), _position(position), _direction(direction),
    _ambientColor(ambient_color), _diffuseColor(diffuse_color),
    _specularColor(specular_color)
  {}
  int _type;
  Math::Vector3 _position;
  Math::Vector3 _direction;
  Color _ambientColor;
  Color _diffuseColor;
  Color _specularColor;

  static const int _typePoint;
  static const int _typeDirectional;
  static const int _typeSpot;

  static int _activeLights;
  void SetUniforms(unsigned int light_index, PhongShader * phong_shader)
  {
    glUniform1i(phong_shader->ULights[light_index].UType, _type);
    glUniform3f(phong_shader->ULights[light_index].UPosition,
      _position.x, _position.y, _position.z);
    glUniform3f(phong_shader->ULights[light_index].UDirection,
      _direction.x, _direction.y, _direction.z);
    glUniform3f(phong_shader->ULights[light_index].UAmbientColor,
      _ambientColor._x, _ambientColor._y, _ambientColor._z);
    glUniform3f(phong_shader->ULights[light_index].UDiffuseColor,
      _diffuseColor._x, _diffuseColor._y, _diffuseColor._z);
    glUniform3f(phong_shader->ULights[light_index].USpecularColor,
      _specularColor._x, _specularColor._y, _specularColor._z);
  }
};
int Light::_activeLights = 1;
const int Light::_typePoint = 0;
const int Light::_typeDirectional = 1;
const int Light::_typeSpot = 2;



// GLOBAL

//editor
bool show_light_editor = false;
bool show_material_editor = false;

Mesh * mesh = nullptr;
unsigned int mesh_id = -1;
std::string current_mesh("cube.obj");
char next_mesh[FILENAME_BUFFERSIZE] = "cube.obj";
Light lights[MAXLIGHTS];
Material material;
unsigned int active_lights = 2;
MeshRenderer::ShaderType shader_in_use = MeshRenderer::PHONG;


Math::Vector3 trans(0.0f, 0.0f, -2.0f);
float cur_scale = 1.0f;
float scale_speed = 7.0f;
// initializing to pie so rotations make sense at the start
Math::EulerAngles rotate(0.0f, PI, 0.0f, Math::EulerOrders::XYZs);


Camera camera(Math::Vector3(0.0f, 1.0f, 0.0f));
float movespeed = 1.0f;
float mouse_sensitivity = 0.3f;
float mouse_wheel_sensitivity = 2.0f;

// GLOBAL

void LoadMesh(const std::string & model);
void Update();
void EditorUpdate();
void Render();


inline void InitialUpdate()
{
  Time::Update();
  ImGui_ImplSdlGL3_NewFrame(SDLContext::SDLWindow());
  SDLContext::CheckEvents();
}

int main(int argc, char * argv[])
{
  ErrorLog::Clean();
  SDLContext::Create("CS 300 - Assignment 1", true, OpenGLContext::AdjustViewport);
  OpenGLContext::Initialize();
  MeshRenderer::Initialize();
  ImGui_ImplSdlGL3_Init(SDLContext::SDLWindow());
  SDLContext::AddEventProcessor(ImGui_ImplSdlGL3_ProcessEvent);

  LoadMesh(current_mesh);
  glEnable(GL_DEPTH_TEST);
  Framer::Lock(FPS);
  while (SDLContext::KeepOpen())
  {
    // frame start
    Framer::Start();
    InitialUpdate();
    Update();
    EditorUpdate();
    // clearing and rendering new frame
    Color & fog_color = MeshRenderer::_fogColor;
    glClearColor(fog_color._r, fog_color._g, fog_color._b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Render();
    ImGui::Render();
    OpenGLContext::Swap();
    // frame end
    Framer::End();
  }

  MeshRenderer::Purge();
  OpenGLContext::Purge();
  SDLContext::Purge();
}

inline void MaterialEditor()
{
  ImGui::Begin("Material");
  ImGui::ColorEdit3("Color", material._color._values);
  ImGui::SliderFloat("Ambient Factor", &material._ambientFactor, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse Factor", &material._diffuseFactor, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular Factor", &material._specularFactor, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular Exponent", &material._specularExponent, 0.0f, 30.0f);
  ImGui::End();
}

inline void LightEditor()
{
  ImGui::Begin("Lights");
  ImGui::InputInt("Active Lights", &Light::_activeLights, 1, 1);
  if(Light::_activeLights > MAXLIGHTS)
    Light::_activeLights = MAXLIGHTS;
  else if(Light::_activeLights < 0)
    Light::_activeLights = 0;

  for(int i = 0; i < Light::_activeLights; ++i){
    std::string light_name("Light" + std::to_string(i));
    if(ImGui::TreeNode(light_name.c_str())){
      ImGui::Combo("Type", &lights[i]._type, "Point\0Directional\0Spot\0\0");
      int type = lights[i]._type;
      if(type == Light::_typePoint || type == Light::_typeSpot){
        if(ImGui::TreeNode("Position")) {
          ImGui::DragFloat("X", &lights[i]._position.x, 0.01f);
          ImGui::DragFloat("Y", &lights[i]._position.y, 0.01f);
          ImGui::DragFloat("Z", &lights[i]._position.z, 0.01f);
          ImGui::TreePop();
        }
      }
      if (type == Light::_typeDirectional || type == Light::_typeSpot) {
        if(ImGui::TreeNode("Direction")){
          ImGui::DragFloat("X", &lights[i]._direction.x, 0.01f);
          ImGui::DragFloat("Y", &lights[i]._direction.y, 0.01f);
          ImGui::DragFloat("Z", &lights[i]._direction.z, 0.01f);
          ImGui::TreePop();
        }
      }
      ImGui::Text("Light Colors");
      ImGui::ColorEdit3("Ambient Color", lights[i]._ambientColor._values);
      ImGui::ColorEdit3("Diffuse Color", lights[i]._diffuseColor._values);
      ImGui::ColorEdit3("Specular Color", lights[i]._specularColor._values);
      ImGui::TreePop();
     }
    ImGui::Separator();
  }
  ImGui::End();

}

inline void EditorUpdate()
{
  ImGui::Begin("Editor");
  if (ImGui::CollapsingHeader("Help"))
  {
    ImGui::TextWrapped("Hello! There are a few controls I would like to "
      "mention so you know how to use this application.");
    ImGui::Separator();
    if (ImGui::TreeNode("QOL Controls"))
    {
      ImGui::Separator();
      ImGui::TextWrapped("Click and drag anywhere outside the ImGui "
        "window to rotate the object.");
      ImGui::Separator();
      ImGui::TextWrapped("Use the mouse scroll wheel to perform a "
        "uniform scaling on the object.");
      ImGui::Separator();
      ImGui::TextWrapped("These will not work when the mouse is hovering over "
        "the ImGui window.");
      ImGui::Separator();
      ImGui::TreePop();
    }
    ImGui::Separator();
    if (ImGui::TreeNode("Editor Tabs")) {
      ImGui::Separator();
      ImGui::TextWrapped("The Debug tab only contains FPS at the moment.");
      ImGui::Separator();
      ImGui::TextWrapped("The Global window contains parameters for the "
        "Clear Color and Time Scale (not currently in use).");
      ImGui::Separator();
      ImGui::TextWrapped("The Mesh window allows you to edit mesh properites, "
        "display normal lines, and load new meshes.");
      ImGui::Separator();
      ImGui::TextWrapped("The Light window contains parameters for the single "
        "light that is currently being used.");
      ImGui::TreePop();
    }
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Text("Average FPS: %f", Framer::AverageFPS());
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Global")) {
    ImGui::ColorEdit3("Fog Color", MeshRenderer::_fogColor._values);
    ImGui::SliderFloat("Near Plane", &MeshRenderer::_nearPlane, 0.01f, 5.0f);
    ImGui::SliderFloat("Far Plane", &MeshRenderer::_farPlane, MeshRenderer::_nearPlane, 100.0f);
    ImGui::DragFloat("Time Scale", &Time::m_TimeScale, 0.01f);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Editor Windows")) {
    ImGui::Checkbox("Material Editor", &show_material_editor);
    ImGui::Checkbox("Light Editor", &show_light_editor);
  }
  if (ImGui::CollapsingHeader("Shaders")) {
    int shader_int = MeshRenderer::ShaderTypeToInt(shader_in_use);
    ImGui::Combo("Shader Type", &shader_int, "Phong\0Gouraud\0Blinn\0Toon\0\0");
    shader_in_use = MeshRenderer::IntToShaderType(shader_int);
    if(ImGui::Button("Reload Selected Shader"))
      MeshRenderer::ReloadShader(shader_in_use);
  }
  if (ImGui::CollapsingHeader("Mesh")) {

    MeshRenderer::MeshObject * mesh_object = MeshRenderer::GetMeshObject(mesh_id);
    ImGui::Text("Load Different Mesh");
    ImGui::InputText("", next_mesh, FILENAME_BUFFERSIZE);
    if (ImGui::Button("Load Mesh"))
      LoadMesh(next_mesh);
    ImGui::Separator();
    ImGui::Text("Mesh Stats");
    ImGui::Text("Current Mesh: %s", current_mesh.c_str());
    ImGui::Text("Vertex Count: %d", mesh->VertexCount());
    ImGui::Text("Face Count: %d", mesh->FaceCount());
    ImGui::Separator();
    ImGui::Text("Color");
    ImGui::ColorEdit3("Mesh Color", mesh_object->_color._values);
    ImGui::Separator();
    ImGui::Text("Translation");
    ImGui::DragFloat("TX", &trans.x, 0.01f);
    ImGui::DragFloat("TY", &trans.y, 0.01f);
    ImGui::DragFloat("TZ", &trans.z, 0.01f);
    ImGui::Separator();
    ImGui::Text("Rotation");
    ImGui::DragFloat("RX", &rotate.Angles.x, 0.01f);
    ImGui::DragFloat("RY", &rotate.Angles.y, 0.01f);
    ImGui::DragFloat("RZ", &rotate.Angles.z, 0.01f);
    ImGui::Separator();
    ImGui::Text("Scale");
    ImGui::DragFloat("s", &cur_scale, 0.01f);
    ImGui::Separator();
    ImGui::Text("Normals");
    ImGui::ColorEdit3("Vertex Normal Color", mesh_object->_vertexNormalColor._values);
    ImGui::ColorEdit3("Face Normal Color", mesh_object->_faceNormalColor._values);
    ImGui::Checkbox("Show Vertex Normals", &mesh_object->_showVertexNormals);
    ImGui::Checkbox("Show Face Normals", &mesh_object->_showFaceNormals);
    ImGui::Separator();
    ImGui::Text("Other");
    ImGui::Checkbox("Wireframe", &mesh_object->_showWireframe);
    ImGui::Separator();
  }
  ImGui::End();
  if (show_material_editor)
    MaterialEditor();
  if (show_light_editor)
    LightEditor();
}

void LoadMesh(const std::string & model)
{
  // loading new mesh
  Mesh * new_mesh;
  std::string mesh_path = MODEL_PATH + model;
  try {
    new_mesh = Mesh::Load(mesh_path, Mesh::OBJ);
  }
  catch (const Error & error) {
    return;
  }
  // deleting old mesh
  if (mesh){
    MeshRenderer::Purge(mesh_id);
    Mesh::Purge(mesh);
  }
  // uploading mesh data to gpu
  new_mesh->SetNormalLineLengthMeshRelative(0.1f);
  mesh_id = MeshRenderer::Upload(new_mesh);
  // new mesh loaded
  mesh = new_mesh;
  current_mesh = model;
  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "Initialize()", "During initialization", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }
}


inline void ManageInput()
{
  if (Input::MouseButtonDown(LEFT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();

    if (mouse_motion.first != 0)
      rotate.Angles.y += mouse_motion.first / 100.0f;
    if (mouse_motion.second != 0) {
      rotate.Angles.x -= mouse_motion.second / 100.0f;
    }
  }


  movespeed += movespeed * mouse_wheel_sensitivity * Time::DT() * Input::MouseWheelMotion();
  if (Input::KeyDown(W))
    camera.MoveBack(-Time::DT() * movespeed);
  if (Input::KeyDown(S))
    camera.MoveBack(Time::DT() * movespeed);
  if (Input::KeyDown(A))
    camera.MoveRight(-Time::DT() * movespeed);
  if (Input::KeyDown(D))
    camera.MoveRight(Time::DT() * movespeed);
  if (Input::KeyDown(Q))
    camera.MoveGlobalUp(-Time::DT() * movespeed);
  if (Input::KeyDown(E))
    camera.MoveGlobalUp(Time::DT() * movespeed);
  if (Input::MouseButtonDown(RIGHT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();
    camera.MoveYaw((float)mouse_motion.first * Time::DT() * mouse_sensitivity);
    camera.MovePitch(-(float)mouse_motion.second * Time::DT() * mouse_sensitivity);
  }
    
}

inline void Update()
{
  ImGuiIO & imgui_io = ImGui::GetIO();
  if(!imgui_io.WantCaptureMouse)
    ManageInput();
}

inline void Render()
{
  Math::Matrix4 projection;
  Math::Matrix4 model;

  Math::Matrix4 translation;
  translation.Translate(trans.x, trans.y, trans.z);
  Math::Matrix4 scale;
  scale.Scale(cur_scale, cur_scale, cur_scale);
  Math::Matrix4 rotation;
  Math::ToMatrix4(rotate, &rotation);

  projection = Math::Matrix4::Perspective(PI / 2.0f, 
    OpenGLContext::AspectRatio(), MeshRenderer::_nearPlane, 
    MeshRenderer::_farPlane);
  model = translation * rotation * scale;

  // prepping uniforms
  PhongShader * phong_shader = MeshRenderer::GetPhongShader();
  GouraudShader * gouraud_shader = MeshRenderer::GetGouraudShader();
  const Math::Vector3 & cpos = camera.Position();
  switch (shader_in_use)
  {
  //PHONG SHADER
  case MeshRenderer::ShaderType::PHONG:
    phong_shader->Use();
    glUniform3f(phong_shader->UCameraPosition, cpos.x, cpos.y, cpos.z);
    glUniform1i(phong_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      lights[i].SetUniforms(i, phong_shader);
    material.SetUniforms(phong_shader);
    // rendering mesh
    MeshRenderer::Render(mesh_id, shader_in_use, projection, camera.ViewMatrix(), model);
    break;
  // GOURAUD SHADER
  case MeshRenderer::ShaderType::GOURAUD:
    gouraud_shader->Use();
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      //lights[i].SetUniforms(i, gouraud_shader);
    // rendering mesh
    MeshRenderer::Render(mesh_id, shader_in_use, projection, camera.ViewMatrix(), model);
    break;
  case MeshRenderer::ShaderType::BLINN:
    break;
  case MeshRenderer::ShaderType::TOON:
    break;
  default:
    break;
  }
  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "Update()", "During Update", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }
}