/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "TesterDialog.h"
#include "TesterConfig.h"
#include "Globals.h"
#include "TesterNewDialog.h"
#include "TesterOpenDialog.h"
#include "TesterDeleteDialog.h"

#include "../../../pics/edit.xpm"
#include "../../../pics/newchart.xpm"
#include "../../../pics/delete.xpm"

#include <QLayout>
#include <QDialogButtonBox>
#include <QPushButton>

TesterDialog::TesterDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": Tester ");

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);

  QPushButton *b = bbox->addButton(QDialogButtonBox::Close);
  connect(b, SIGNAL(clicked()), this, SLOT(closeDialog()));
  vbox->addWidget(bbox);

  createMainPage();

  loadSettings();
}

void TesterDialog::createMainPage ()
{
  QWidget *w = new QWidget;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  w->setLayout(vbox);

  QPushButton *b = new QPushButton;
  b->setText(tr("&New"));
  b->setIcon(QPixmap(newchart_xpm));
  b->setToolTip(tr("Create a new back tester"));
  connect(b, SIGNAL(clicked()), this, SLOT(newTest()));
  vbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Open"));
  b->setToolTip(tr("Open back tester"));
  b->setIcon(QPixmap(edit_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(openTest()));
  vbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Delete"));
  b->setToolTip(tr("Delete back tester"));
  b->setIcon(QPixmap(delete_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(deleteTest()));
  vbox->addWidget(b);

  vbox->addStretch();

  _tabs->addTab(w, tr("Tester"));
}

void TesterDialog::loadSettings ()
{
  TesterConfig config;

  // restore the size of the window
  QSize sz;
  config.getData(TesterConfig::Size, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(TesterConfig::Pos, p);
  if (! p.isNull())
    move(p);
}

void TesterDialog::saveSettings ()
{
  TesterConfig config;
  config.transaction();
  
  // save app size and position
  QSize sz = size();
  config.setData(TesterConfig::Size, sz);

  QPoint pt = pos();
  config.setData(TesterConfig::Pos, pt);

  config.commit();
}

void TesterDialog::newTest ()
{
  TesterNewDialog *dialog = new TesterNewDialog;
  dialog->show();
}

void TesterDialog::openTest ()
{
  TesterOpenDialog *dialog = new TesterOpenDialog;
  dialog->show();
}

void TesterDialog::deleteTest ()
{
  TesterDeleteDialog *dialog = new TesterDeleteDialog;
  dialog->show();
}

void TesterDialog::closeDialog ()
{
  saveSettings();
  hide();
}

