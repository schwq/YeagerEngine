#include "InputHandle.h"
#include "Application.h"
#include "Common/Common.h"
#include "Engine/Editor/Camera.h"
using namespace Yeager;

std::vector<KeyMap> Yeager::KeyMapping = {KeyMap(GLFW_MOUSE_BUTTON_1), KeyMap(GLFW_MOUSE_BUTTON_2), KeyMap(GLFW_KEY_E)};
KeyMap* Yeager::FindKeyMap(uint8_t key)
{
  for (auto& keymap : KeyMapping) {
    if (key == keymap.GlfwMacro) {
      return &keymap;
    }
  }
  assert("Cannot find key associated to keymapping!");
}

float Input::m_LastMouseWidth = ygWindowWidth / 2.0f;
float Input::m_LastMouseHeight = ygWindowHeight / 2.0f;
bool Input::m_FirstMouse = true;
CameraCursorLastState Input::m_LastState;
bool Input::m_CursorShouldAppear = true;

void Input::KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (m_Application->GetMode() == ApplicationMode::AppEditor) {
    switch (key) {
      case GLFW_KEY_E:
        if (FindKeyMap(GLFW_KEY_E)->AddStateAwaitAction(action)) {
          if (m_Application->GetCamera()->GetShouldMove()) {
            m_Application->GetCamera()->SetShouldMove(false);
            m_FirstMouse = true;
            SetCursorAppear(true);
          } else {
            m_Application->GetCamera()->SetShouldMove(true);
            SetCursorAppear(false);
          }
        }
        break;
    }
  }
}

void Input::MouseKeyCallback(GLFWwindow* window, int button, int action, int mods)
{
  if (m_Application->GetMode() == ApplicationMode::AppEditor) {
    switch (button) {
      case GLFW_MOUSE_BUTTON_1:
        if (FindKeyMap(GLFW_MOUSE_BUTTON_1)->AddStateAwaitAction(action)) {
          double xpos, ypos;
          glfwGetCursorPos(window, &xpos, &ypos);
          m_Application->GetCamera()->RayCasting(xpos, ypos, m_Application->GetProjection(), m_Application->GetView());
        }
        break;
      case GLFW_MOUSE_BUTTON_2:
        if (FindKeyMap(GLFW_MOUSE_BUTTON_2)->AddStateAwaitAction(action)) {}
        break;
    }
  }
}

Yeager::ApplicationCore* Input::m_Application = YEAGER_NULLPTR;
Input::~Input()
{
  Yeager::Log(INFO, "Destrorying InputHandle!");
}

unsigned int Input::m_FramesCount = 0;
Input::Input(Yeager::ApplicationCore* app)
{
  m_Application = app;
  Yeager::Log(INFO, "Input created");
}

void Input::InitializeCallbacks()
{
  glfwSetMouseButtonCallback(m_Application->GetWindow()->getWindow(), MouseKeyCallback);
  glfwSetKeyCallback(m_Application->GetWindow()->getWindow(), KeyboardKeyCallback);
}

void Input::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (m_Application->GetCamera()->GetShouldMove()) {
    m_Application->GetCamera()->MouseCallback(m_FirstMouse, m_LastMouseWidth, m_LastMouseHeight, xpos, ypos);
  }
}

void Input::SetCursorCanDisappear(bool should) noexcept
{
  m_CursorCanDisappear = should;
  if (!should) {
    SetCursorAppear(true);
  }
}

void Input::SetCursorAppear(bool appear) noexcept
{
  m_CursorShouldAppear = appear;
  uint64_t cursor = appear ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
  glfwSetInputMode(m_Application->GetWindow()->getWindow(), GLFW_CURSOR, cursor);
}

void Input::SetCameraCursorToWindowState(bool state)
{
  if (state) {
    m_CursorCanDisappear = false;
    SetCursorAppear(true);
    m_Application->GetCamera()->SetShouldMove(false);
  } else {
    m_CursorCanDisappear = true;
    SetCursorAppear(true);
    m_Application->GetCamera()->SetShouldMove(false);
  }
}

const inline bool Input::GetKeyPressed(int key) noexcept
{
  return glfwGetKey(m_Application->GetWindow()->getWindow(), key) == GLFW_PRESS;
}

void Input::ProcessInputRender(Window* window, float delta)
{
  m_FramesCount++;
  Yeager::Interface* intr = m_Application->GetInterface();
  if (glfwGetKey(window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {

    if (!intr->GetExitProgramWindowOpen()) {
      intr->SetExitProgramWindowOpen(true);
    }
  }
  if (intr->GetUserChoiceExitProgram()) {
    glfwSetWindowShouldClose(window->getWindow(), true);
  }

  if (m_Application->GetMode() == Yeager::AppEditor && m_Application->GetCamera()->GetShouldMove()) {
    if (GetKeyPressed(GLFW_KEY_W)) {
      m_Application->GetCamera()->UpdatePosition(CameraPosition::kForward, delta);
    }
    if (GetKeyPressed(GLFW_KEY_D)) {
      m_Application->GetCamera()->UpdatePosition(CameraPosition::kRight, delta);
    }
    if (GetKeyPressed(GLFW_KEY_A)) {
      m_Application->GetCamera()->UpdatePosition(CameraPosition::kLeft, delta);
    }
    if (GetKeyPressed(GLFW_KEY_S)) {
      m_Application->GetCamera()->UpdatePosition(CameraPosition::kBackward, delta);
    }
  }
  if (m_Application->GetMode() == Yeager::AppEditor) {

    if (glfwGetKey(window->getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      if (glfwGetKey(window->getWindow(), GLFW_KEY_F) == GLFW_PRESS) {
        m_Application->GetInterface()->MakeScreenShotAppear(true);
      }
    }
  }
}
void Input::RestoreCameraCursorLastState() noexcept
{
  SetCursorAppear(m_LastState.CursorShouldAppear);
  m_Application->GetCamera()->SetShouldMove(m_LastState.CameraShouldMove);
  m_CursorCanDisappear = m_LastState.CursorCanDissapear;
}
void Input::WriteCameraCursorLastState() noexcept
{
  m_LastState.CursorShouldAppear = m_CursorShouldAppear;
  m_LastState.CameraShouldMove = m_Application->GetCamera()->GetShouldMove();
  m_LastState.CursorCanDissapear = m_CursorCanDisappear;
}