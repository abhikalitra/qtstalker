/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#ifndef ALERT_CONFIG_HPP
#define ALERT_CONFIG_HPP

#include <QString>

#include "ConfigDataBase.h"

class AlertConfig : public ConfigDataBase
{
  public:
    enum Parm
    {
      _Size, // window size
      _Pos, // position
      _LastId, // next available alert id
      _TimerInterval, // timer interval
      _AlertsEnable, // enable / disable alert scans
      _SoundFile, // path to sound file to play on notification
      _MailAddress, // email address to send notifications
      _MailSubject, // subject header text for notifications
      _MailBody, // text body for notifications
      _EditDialogSize, // last edit dialog window size
      _EditDialogPos, // last edit dialog window position
      _ConfigureDialogSize, // last Configure dialog window size
      _ConfigureDialogPos // last Configure dialog window position
    };

    AlertConfig ();
};

#endif

