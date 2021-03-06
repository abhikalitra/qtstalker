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

#ifndef TYPE_THREAD_MESSAGE_HPP
#define TYPE_THREAD_MESSAGE_HPP

class TypeThreadMessage
{
  public:
    enum Key
    {
      _CHART_CURVE,
      _CHART_DATE,
      _CHART_LOAD_OBJECT,
      _CHART_NEW,
      _CHART_OBJECT,
      _CHART_OBJECT_REMOVE,
      _CHART_PANEL_REFRESH,
      _CHART_PANEL_RELOAD,
      _CHART_PANEL_SELECT,
      _CHART_REMOVE,
      _CHART_UPDATE,
      _DIALOG,
      _DIALOG_CONFIRM,
      _DIALOG_MESSAGE,
      _DIALOG_NEW,
      _DIALOG_SCRIPT,
      _DIALOG_SELECT,
      _DIALOG_USER,
      _GROUP_PANEL_REFRESH,
      _GROUP_PANEL_SELECT,
      _SCRIPT_PANEL_SELECT,
      _SCRIPT_PANEL_RUN
    };
};

#endif
