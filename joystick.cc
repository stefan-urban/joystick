// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>

#include "joystick.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sstream>
#include "unistd.h"
#include <linux/joystick.h>
#include <linux/ioctl.h>

Joystick::Joystick()
{
  devicePath_ = std::string("/dev/input/js0");
}

Joystick::Joystick(int joystickNumber)
{
  std::stringstream sstm;
  sstm << "/dev/input/js" << joystickNumber;
  devicePath_ = sstm.str();
}

Joystick::Joystick(std::string devicePath)
  : devicePath_(devicePath)
{
}

void Joystick::connect()
{
  // Close if file is still open
  if (_fd >= 0)
  {
    close(_fd);
    _fd = -1;
  }

  // Connect to joystick file
  _fd = open(devicePath_.c_str(), O_RDONLY | O_NONBLOCK);
}

bool Joystick::sample(JoystickEvent* event)
{
  int bytes = read(_fd, event, sizeof(*event));

  if (bytes == -1)
    return false;

  // NOTE if this condition is not met, we're probably out of sync and this
  // Joystick instance is likely unusable
  return bytes == sizeof(*event);
}

bool Joystick::isFound()
{
  return _fd >= 0 && fcntl(_fd, F_GETFD) != -1 && errno != ENODEV;
}

char Joystick::numberOfButtons()
{
  char number_of_buttons;
  ioctl(_fd, JSIOCGBUTTONS, &number_of_buttons);

  return number_of_buttons;
}

char Joystick::numberOfAxis()
{
  char number_of_axis;
  ioctl(_fd, JSIOCGAXES, &number_of_axis);

  return number_of_axis;
}


Joystick::~Joystick()
{
  close(_fd);
}
