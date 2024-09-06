// Yeager Engine, free and open source 3D / 2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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

namespace Yeager
{
/**
	@brief Represents a human readable date, month, weekday, day and year
*/
struct DateType {
  Uint Month = 0;
  Uint WeekDay = 0;
  Uint Day = 0;
  Uint Year = 0;

  static String MonthNumberToString(int month);
  static String WeekDayNumberToString(int weekday);
};

/**
	@brief Represents a human readable clock time, with hours, minutes, seconds and milliseconds
*/
struct ClockTimeType {
  long long Millis = 0;
  Uint Seconds = 0;
  Uint Minutes = 0;
  Uint Hours = 0;
};

/**
	@brief Represents a specific human readable point in time, with a date and the clock time
*/
struct TimePointType {
  DateType Date;
  ClockTimeType Time;

  static String CurrentTimeFormatToString();
  static String CurrentTimeFormatToFileFormat();
  static TimePointType CurrentTimeToTimeType();
  static String FormatTimeToString(const TimePointType& time);
};

#if defined(YEAGER_SYSTEM_WINDOWS_x64)

/**
	@brief Converts the FILETIME format of time from windows files to the buildin structure and human readable TimePointType,
	that can be extracted information about the date and time
*/
YEAGER_NODISCARD extern TimePointType WindowsFILETIMEToTimePoint(const FILETIME& time);

#endif

}