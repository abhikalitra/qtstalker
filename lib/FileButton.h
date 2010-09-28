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

// *************************************************************************************************
// Custom pushbutton that displays a selected file on the button or if more than 1 file, displays
// the total number of files selected. When pressed, opens the file selection dialog. Used by PrefDialog
// *************************************************************************************************

#ifndef FILE_BUTTON_HPP
#define FILE_BUTTON_HPP

#include <QWidget>
#include <QPushButton>
#include <QStringList>

class FileButton : public QPushButton
{
  Q_OBJECT

  signals:
    void signalFileChanged ();

  public:
    FileButton (QWidget *, QString);
    QString & getFile ();
    void setFile (QString);
    void updateButtonText ();

  public slots:
    void fileDialog ();
        
  private:
    QString file;
};

#endif
