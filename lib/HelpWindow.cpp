/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "HelpWindow.h"
#include "Config.h"
#include "../src/next.xpm"
#include "../src/home.xpm"
#include "../src/previous.xpm"
#include "../src/disable.xpm"
#include <qlayout.h>
#include <qdir.h>

HelpWindow::HelpWindow (QWidget *, QString fn) : QDialog (0, "HelpWindow", FALSE, WDestructiveClose)
{
  Config config;
  homePath = config.getData(Config::HelpFilePath) + "/";
  tocPath = homePath + "toc.html";

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setSpacing(5);
  vbox->setMargin(5);
  
  toolbar = new Toolbar(this, 30, 30, FALSE);
  vbox->addWidget(toolbar);
  
  toolbar->addButton("home", home, tr("Home"));
  QObject::connect(toolbar->getButton("home"), SIGNAL(clicked()), this, SLOT(goHome()));
  
  toolbar->addButton("previous", previous, tr("Previous"));
  QObject::connect(toolbar->getButton("previous"), SIGNAL(clicked()), this, SLOT(goPrevious()));

  toolbar->addButton("next", next, tr("Next"));
  QObject::connect(toolbar->getButton("next"), SIGNAL(clicked()), this, SLOT(goNext()));
  
  toolbar->addButton("exit", disable, tr("Close"));
  QObject::connect(toolbar->getButton("exit"), SIGNAL(clicked()), this, SLOT(exit()));
  
  text = new QTextBrowser(this);
  QObject::connect(text, SIGNAL(backwardAvailable(bool)), this, SLOT(previousStatus(bool)));
  QObject::connect(text, SIGNAL(forwardAvailable(bool)), this, SLOT(nextStatus(bool)));
  vbox->addWidget (text);
  
  QString s = homePath + fn;
  QDir dir;
  if (dir.exists(s) && s.contains(".html"))
    text->setSource(s);
  else
    text->setSource(tocPath);
  
  setCaption(text->documentTitle());

  resize(500, 350);
}

HelpWindow::~HelpWindow ()
{
}

void HelpWindow::goHome ()
{
  text->setSource(tocPath);
}

void HelpWindow::goPrevious ()
{
  text->backward();
}

void HelpWindow::goNext ()
{
  text->forward();
}

void HelpWindow::previousStatus (bool d)
{
  toolbar->setButtonStatus("previous", d);
}

void HelpWindow::nextStatus (bool d)
{
  toolbar->setButtonStatus("next", d);
}

void HelpWindow::exit ()
{
  done(0);
}


