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

#include "COToolBar.h"
#include "COFactory.h"
#include "COPlugin.h"

#include "../pics/cursor_arrow.xpm"
#include "../pics/cursorZoom.xpm"
#include "../pics/crosshair.xpm"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QButtonGroup>
#include <QToolButton>

COToolBar::COToolBar ()
{
  createCursorButtons();
  createCOButtons();
}

void COToolBar::createCursorButtons ()
{
  QButtonGroup *bg = new QButtonGroup(this);
  connect(bg, SIGNAL(buttonClicked(int)), this, SIGNAL(signalCursorButtonClicked(int)));

  // normal cursor button
  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(cursor_arrow_xpm));
  b->setToolTip(QString(tr("Select Cursor")));
  b->setCheckable(TRUE);
  b->setChecked(TRUE);
  addWidget(b);
  bg->addButton(b, 0);

  // zoom cursor button
  b = new QToolButton;
  b->setIcon(QIcon(cursorZoom_xpm));
  b->setToolTip(QString(tr("Zoom Cursor")));
  b->setCheckable(TRUE);
  addWidget(b);
  bg->addButton(b, 1);

  // crosshair button
  b = new QToolButton;
  b->setIcon(QIcon(crosshair));
  b->setToolTip(QString(tr("Crosshairs Cursor")));
  b->setCheckable(TRUE);
  addWidget(b);
  bg->addButton(b, 2);

  addSeparator();
}

void COToolBar::createCOButtons ()
{
  QButtonGroup *bg = new QButtonGroup(this);
//  connect(bg, SIGNAL(buttonClicked(int)), this, SIGNAL(signalCOButtonClicked(int)));
  connect(bg, SIGNAL(buttonClicked(int)), this, SLOT(COButtonClicked(int)));
  
  COFactory fac;
  QStringList l;
  fac.getList(l);
  
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    COPlugin *plug = fac.getCO(l[loop]);
    if (! plug)
      continue;
    
    QIcon icon;
    plug->getIcon(icon);
    QToolButton *b = new QToolButton;
    b->setIcon(icon);
    b->setToolTip(QString(l[loop]));
    b->setCheckable(FALSE);
    addWidget(b);
    bg->addButton(b, loop);
    
    delete plug;
  }
}

void COToolBar::COButtonClicked (int id)
{
  COFactory fac;
  QStringList l;
  fac.getList(l);
  emit signalCOButtonClicked(l[id]);
}

