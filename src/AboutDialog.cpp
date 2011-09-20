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

#include "AboutDialog.h"
#include "Globals.h"

#include "../pics/qtstalker.xpm"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QString>
#include <QLabel>
#include <QPushButton>

AboutDialog::AboutDialog ()
{
  createGUI();
  createQTSPage();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  QStringList wt;
  wt << "QtStalker" + g_session + ":" << tr("About QtStalker");
  setWindowTitle(wt.join(" "));
}

void AboutDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok);
  vbox->addWidget(bbox);

  QPushButton *b = bbox->button(QDialogButtonBox::Ok);
  connect(b, SIGNAL(clicked()), this, SLOT(accept()));
}

void AboutDialog::createQTSPage ()
{
  QWidget *w = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  w->setLayout(hbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  hbox->addLayout(vbox);

  QLabel *label = new QLabel;
  label->setPixmap(qtstalker_xpm);
  vbox->addWidget(label);

  vbox->addStretch(1);

  QString s = tr("QtStalker version 0.37-dev (working title)");
  s.append(tr("\nBuilt using Qt "));
  s += QT_VERSION_STR;
  s.append(tr("\n(C) 2001-2011 by Stefan Stratigakos\nqtstalker.sourceforge.net"));
  s.append(tr("\n\nQtstalker is licensed with GNU General Public License (GPL) version 2."));
  s.append(tr("\nQt Assistant is licensed with GNU General Public License (GPL) version 3."));

  label = new QLabel(s);
  hbox->addWidget(label);

  _tabs->addTab(w, tr("About"));
}
