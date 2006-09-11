/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include "../pics/next.xpm"
#include "../pics/home.xpm"
#include "../pics/previous.xpm"
#include "../pics/disable.xpm"
#include <qlayout.h>
#include <qdir.h>

HelpWindow::HelpWindow (QWidget *w, QString &fn) : QDialog (w, "HelpWindow", FALSE, WDestructiveClose)
{
  Config config;
  config.getData(Config::HelpFilePath, homePath);
  homePath.append("/");
  tocPath = homePath + "toc.html";

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setSpacing(5);
  vbox->setMargin(5);
  
  toolbar = new Toolbar(this, 30, 30, FALSE);
  vbox->addWidget(toolbar);

  QString s("home");
  QString s2(tr("Home"));
  toolbar->addButton(s, home, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(goHome()));
  
  s = "previous";
  s2 = tr("Previous");
  toolbar->addButton(s, previous, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(goPrevious()));

  s = "next";
  s2 = tr("Next");
  toolbar->addButton(s, next, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(goNext()));
  
  s = "exit";
  s2 = tr("Close");
  toolbar->addButton(s, disable, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(exit()));
  
  text = new QTextBrowser(this);
  QObject::connect(text, SIGNAL(backwardAvailable(bool)), this, SLOT(previousStatus(bool)));
  QObject::connect(text, SIGNAL(forwardAvailable(bool)), this, SLOT(nextStatus(bool)));
  vbox->addWidget (text);
  
  s = homePath + fn;
  QDir dir;
  if (dir.exists(s) && s.contains(".html"))
    text->setSource(s);
  else
    text->setSource(tocPath);
  
  setCaption(text->documentTitle());

  resize(350, 350);
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
  QString s("previous");
  toolbar->setButtonStatus(s, d);
}

void HelpWindow::nextStatus (bool d)
{
  QString s("next");
  toolbar->setButtonStatus(s, d);
}

void HelpWindow::exit ()
{
  done(0);
}


