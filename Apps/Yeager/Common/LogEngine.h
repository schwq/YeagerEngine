//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include "Common.h"
#include "Utilities.h"

typedef struct {
  YgString message = YEAGER_NULL_LITERAL;
  int verbosity = -1;
  ImVec4 text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
} ConsoleLogSender;

extern ImVec4 VerbosityToColor(int verbosity);

class EditorConsole {
 public:
  EditorConsole(){};
  ~EditorConsole(){};

  void SetLogString(ConsoleLogSender message) { m_logs.push_back(message); }

  void ReadLog();

 private:
  std::vector<ConsoleLogSender> m_logs;
};
extern EditorConsole kConsole;

namespace Yeager {

template <typename... T>
void Log(int verbosity, fmt::format_string<T...> fmt, T&&... args)
{
  auto str = fmt::format(fmt, std::forward<T>(args)...);
  YgString log(str);

  ConsoleLogSender console_message;
  console_message.text_color = VerbosityToColor(verbosity);
  console_message.verbosity = verbosity;
  console_message.message = log;
  kConsole.SetLogString(console_message);

  YgString terminal_prefix;
  if (verbosity == INFO) {
    terminal_prefix = "(-) ";
  } else if (verbosity == WARNING) {
    terminal_prefix = "(??) ";
  } else {
    terminal_prefix = "(!!) ";
  }

  YgTime_t time = CurrentTimeToTimeType();
  YgString time_str = "[ " + std::to_string(time.Time.Hours) + ":" + std::to_string(time.Time.Minutes) + ":" +
                      std::to_string(time.Time.Seconds) + " ]";
  std::cout << time_str << terminal_prefix << log << std::endl;
}
}  // namespace Yeager