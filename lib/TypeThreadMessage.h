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
    enum Type
    {
      _CHART_CURVE,
      _CHART_DATE,
      _CHART_LOAD_OBJECT,
      _CHART_NEW,
      _CHART_OBJECT,
      _CHART_PANEL_REFRESH,
      _CHART_UPDATE,
      _DIALOG,
      _GROUP_PANEL_REFRESH
    };
};

#endif