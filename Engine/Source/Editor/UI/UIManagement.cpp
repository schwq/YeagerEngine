#include "UIManagement.h"
#include "Main/Core/Application.h"
using namespace ImGui;
using namespace Yeager;

WindowMenuBar Yeager::EngineEditorMenuBar;
bool Yeager::EngineEditorWindowShouldVanish = false;

InterfaceWindow::InterfaceWindow(Yeager::ApplicationCore* application, String title, ImVec2 size,
                                 InterfaceWindow* parent_window, ImVec2 parent_relative_pos,
                                 WindowRelativePos relative_pos)
    : m_Title(title),
      m_Application(application),
      Size(size),
      m_ParentWindow(parent_window),
      m_ParentRelativePosition(parent_relative_pos),
      m_RelativePosition(relative_pos)
{
  m_ScreenPosition = ImVec2(0.0f, 0.0f);
}
InterfaceWindow::InterfaceWindow(Yeager::ApplicationCore* application, String title, ImVec2 size, ImVec2 position,
                                 bool follow_glfw_window, WindowRelativePos glfw_relative_pos)
    : m_Application(application),
      m_Title(title),
      Size(size),
      m_ScreenPosition(position),
      m_FollowGlfwWindow(follow_glfw_window),
      m_GlfwRelativePosition(glfw_relative_pos)
{
  m_RootWindow = true;
}
InterfaceWindow::~InterfaceWindow() {}

void InterfaceWindow::Begin(ImGuiWindowFlags flags)
{

  m_TempSize = Size;
  ProcessPreRules();
  CalcScreenPos();
  ProcessPosRules();
  CheckIfOffWindow();
  SetNextWindowPos(m_ScreenPosition);
  SetNextWindowSize(Size);

  if (!m_WindowRules.RoundingCorner) {
    PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  }

  ImGui::Begin(m_Title.c_str(), NULL, flags);
}

void Yeager::InterfaceWindow::ProcessPreRules()
{
  // TODO
  return;
}

void Yeager::InterfaceWindow::ProcessPosRules()
{
  const Vector2 wndSize = m_Application->GetWindow()->GetWindowSize();
  if (m_WindowRules.Fullscreen) {
    Size.x = wndSize.x;
    Size.y = wndSize.y;
    m_ScreenPosition = ImVec2(0.0f, 0.0f);
  }

  if (m_WindowRules.FollowAbove) {

    ImVec2 new_size = ImVec2(0.0f, 0.0f);
    new_size.x = Size.x;
    new_size.y = Size.y + m_ScreenPosition.y;

    m_ScreenPosition.y = Yeager::EngineEditorMenuBar.MenuBarSize.y;
    Size = new_size;
  }

  if (m_WindowRules.FollowUnder) {
    ImVec2 new_size = ImVec2(0, 0);
    new_size.x = Size.x;
    new_size.y = Size.y + wndSize.y - (m_ScreenPosition.y + Size.y);
    Size = new_size;
  }

  if (!m_RootWindow) {
    if (m_WindowRules.SameSizeParent) {
      Size = m_ParentWindow->GetSize();
    }

    if (m_WindowRules.SameWidthParent) {
      Size.x = m_ParentWindow->GetSize().x;
    }

    if (m_WindowRules.SameHeightParent) {
      Size.y = m_ParentWindow->GetSize().y;
    }
  }
}

void InterfaceWindow::End()
{

  /// What is this doing here, well, if in the future, we want to alloc something, this is going to be used to desalloc
  ImGui::End();

  if (!m_WindowRules.RoundingCorner) {
    PopStyleVar();
  }

  Size = m_TempSize;
}

void InterfaceWindow::CheckIfOffWindow()
{
  const Vector2 wndSize = m_Application->GetWindow()->GetWindowSize();
  if (m_ScreenPosition.y >= wndSize.y) {
    m_ScreenPosition.y = wndSize.y - 10;
  } else if (m_ScreenPosition.y < 0) {
    if (EngineEditorMenuBar.HasMenuBar) {
      m_ScreenPosition.y = EngineEditorMenuBar.MenuBarSize.y;
    } else {
      m_ScreenPosition.y = 0.0f;
    }
  }

  if (m_ScreenPosition.y + Size.y > wndSize.y) {
    Size.y = wndSize.y - m_ScreenPosition.y;
  }

  if (m_ScreenPosition.x >= wndSize.x) {
    m_ScreenPosition.x = wndSize.x - 10;
  } else if (m_ScreenPosition.x < 0) {
    m_ScreenPosition.x = 0.0f;
  }

  if (m_ScreenPosition.x + Size.x > wndSize.x) {
    Size.x = wndSize.x - m_ScreenPosition.x;
  }
}

void InterfaceWindow::CalcScreenPos()
{
  const Vector2 wndSize = m_Application->GetWindow()->GetWindowSize();

  if (m_RootWindow) {

    if (m_FollowGlfwWindow) {

      switch (m_GlfwRelativePosition) {
        case WindowRelativePos::LEFT_ABOVE: {
          ImVec2 pos = ImVec2(0, 0);
          pos.y += EngineEditorMenuBar.MenuBarSize.y;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::RIGHT_ABOVE: {
          ImVec2 pos = ImVec2(0, 0);
          pos.y += Yeager::EngineEditorMenuBar.MenuBarSize.y;
          pos.x = wndSize.x - Size.x;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::ABOVE_MIDDLE: {
          Uint middle_above_x = wndSize.x / 2;
          Uint window_middle_x = Size.x / 2;
          ImVec2 pos = ImVec2(middle_above_x - window_middle_x, 0);
          pos.y += Yeager::EngineEditorMenuBar.MenuBarSize.y;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::UNDER_MIDDLE: {
          Uint middle_above_x = wndSize.x / 2;
          Uint window_middle_x = Size.x / 2;
          ImVec2 pos = ImVec2(middle_above_x - window_middle_x, wndSize.y - Size.y);
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::LEFT_MIDDLE: {
          Uint middle_right_y = wndSize.y / 2;
          Uint window_middle_y = Size.y / 2;
          ImVec2 pos = ImVec2(0, middle_right_y - window_middle_y);
          pos.y += Yeager::EngineEditorMenuBar.MenuBarSize.y;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::MIDDLE: {
          Uint window_middle_x = Size.x / 2;
          Uint window_middle_y = Size.y / 2;
          Uint middle_window_x = wndSize.x / 2;
          Uint middle_window_y = wndSize.y / 2;
          ImVec2 pos = ImVec2(middle_window_x - window_middle_x, middle_window_y - window_middle_y);
          pos.y += Yeager::EngineEditorMenuBar.MenuBarSize.y;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::RIGHT_MIDDLE: {
          Uint middle_left_y = wndSize.y / 2;
          Uint window_middle_y = Size.y / 2;
          ImVec2 pos = ImVec2(wndSize.x, middle_left_y - window_middle_y);
          pos.y += Yeager::EngineEditorMenuBar.MenuBarSize.y;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::LEFT_UNDER: {
          ImVec2 pos = ImVec2(0, wndSize.y - Size.y);
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::RIGHT_UNDER: {
          ImVec2 pos = ImVec2(wndSize.x - Size.x, wndSize.y - Size.y);
          m_ScreenPosition = pos;
        } break;
      }
    }
  } else {
    if (m_ParentWindow) {
      ImVec2 parent_pos = m_ParentWindow->GetScreenPos();
      ImVec2 parent_size = m_ParentWindow->GetSize();
      ImVec2 pos = parent_pos;
      switch (m_RelativePosition) {
        case WindowRelativePos::ABOVE: {
          pos.y -= Size.x;
          pos.y -= m_ParentRelativePosition.y;
          pos.x -= m_ParentRelativePosition.x;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::UNDER: {
          pos.y += parent_size.y;
          pos.y += m_ParentRelativePosition.y;
          pos.x += m_ParentRelativePosition.x;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::RIGHT: {
          pos.x += parent_size.x;
          pos.y += m_ParentRelativePosition.y;
          pos.x += m_ParentRelativePosition.x;
          m_ScreenPosition = pos;
        } break;
        case WindowRelativePos::LEFT: {
          pos.x -= Size.x;
          pos.y += m_ParentRelativePosition.y;
          pos.x -= m_ParentRelativePosition.x;
          m_ScreenPosition = pos;
        } break;
      }
    }
  }
}

bool InterfaceWindow::GetIsRoot()
{
  return m_RootWindow;
}
bool InterfaceWindow::GetFollowGLFWWindow()
{
  return m_FollowGlfwWindow;
}
InterfaceWindow* InterfaceWindow::GetParent()
{
  return m_ParentWindow;
}
ImVec2 InterfaceWindow::GetParentRelativePos()
{
  return m_ParentRelativePosition;
}
WindowRelativePos InterfaceWindow::GetRelativePos()
{
  return m_RelativePosition;
}
ImVec2 InterfaceWindow::GetSize()
{
  return Size;
}
ImVec2 InterfaceWindow::GetScreenPos()
{
  return m_ScreenPosition;
}

void InterfaceWindow::SetSize(ImVec2 size)
{
  Size = size;
}
void InterfaceWindow::SetPos(ImVec2 pos)
{
  m_ScreenPosition = pos;
}

bool InterfaceWindow::GetCollision(Uint x, Uint y)
{
  bool collision_x = false;
  bool collision_y = false;
  if (m_ScreenPosition.x <= x && m_ScreenPosition.x + Size.x >= x) {
    collision_x = true;
  }
  if (m_ScreenPosition.y <= y && m_ScreenPosition.y + Size.y >= y) {
    collision_y = true;
  }
  return collision_x && collision_y;
}

void InterfaceWindow::SetRule(EYGWindowRule rule, bool value)
{
  switch (rule) {
    case EWindowFullscreen:
      m_WindowRules.Fullscreen = value;
      break;
    case EWindowFollowAbove:
      m_WindowRules.FollowAbove = value;
      break;
    case EWindowFollowUnder:
      m_WindowRules.FollowUnder = value;
      break;
    case EWindowSameSizeParent:
      m_WindowRules.SameSizeParent = value;
      break;
    case EWindowSameWidthParent:
      m_WindowRules.SameWidthParent = value;
      break;
    case EWindowSameHeightParent:
      m_WindowRules.SameHeightParent = value;
      break;
    case EWindowRoundingCorner:
      m_WindowRules.RoundingCorner = value;
      break;
    case EWindowKeepBottomOnMiddle:
      m_WindowRules.KeepBottomOnMiddle = value;
      break;
    default:
      Yeager::Log(ERROR, "Invalid window rule!");
  }
}

bool InterfaceWindow::GetRule(EYGWindowRule rule)
{
  switch (rule) {
    case EWindowFullscreen:
      return m_WindowRules.Fullscreen;
    case EWindowFollowAbove:
      return m_WindowRules.FollowAbove;
    case EWindowFollowUnder:
      return m_WindowRules.FollowUnder;
    case EWindowSameSizeParent:
      return m_WindowRules.SameSizeParent;
    case EWindowSameWidthParent:
      return m_WindowRules.SameWidthParent;
    case EWindowSameHeightParent:
      return m_WindowRules.SameHeightParent;
    case EWindowRoundingCorner:
      return m_WindowRules.RoundingCorner;
    case EWindowKeepBottomOnMiddle:
      return m_WindowRules.KeepBottomOnMiddle;
    default:
      Yeager::Log(ERROR, "Invalid window rule!");
      return false;
  }
}