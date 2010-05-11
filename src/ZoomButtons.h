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

#ifndef ZOOM_BUTTONS_HPP
#define ZOOM_BUTTONS_HPP

#include <QObject>
#include <QList>
#include <QToolButton>
#include <QToolBar>

class ZoomButtons : public QObject
{
  Q_OBJECT
  
  signals:
    void signalZoom (int, int); // pixelSpace, index
    void signalPixelSpace (int); // pixelSpace

  public:
    ZoomButtons (QToolBar *);
    void createButtons (QToolBar *);
    void psButtonClicked (int);
    int getPixelSpace ();
    
  public slots:
    void addZoom (int index, int pixelSpace);
    void zoomIn ();
    void zoomOut ();
    void ps1ButtonClicked ();
    void ps2ButtonClicked ();
    
  protected:
    QToolButton *zoomInButton;
    QToolButton *zoomOutButton;
    int pixelSpace;
    QToolButton *ps1Button;
    QToolButton *ps2Button;
};

#endif