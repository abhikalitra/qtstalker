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

#include "ConfigureDialog.h"
#include "Globals.h"
#include "Doc.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>
#include <QFormLayout>
#include <QApplication>

ConfigureDialog::ConfigureDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";
  _modified = 0;

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Configure");
  setWindowTitle(l.join(" "));

  createGUI();
  createGeneralPage();

  loadSettings();

  buttonStatus();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void ConfigureDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);
  
  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void ConfigureDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  QSettings settings(g_settingsFile);
  
  // background color
  QColor color(settings.value("plot_background_color", "black").toString());
  _background = new ColorButton(w, color);
  _background->setToolTip(tr("Plot Background"));
  connect(_background, SIGNAL(valueChanged()), this, SLOT(backgroundChanged()));
  form->addRow(tr("Plot Background"), _background);
  
  _flags.insert("background", 0);

  // app font
  QFont font;
  QStringList l = settings.value("app_font").toString().split(",", QString::SkipEmptyParts);
  if (l.count())
    font.fromString(l.join(","));
  
  _appFont = new FontButton(w, font);
  _appFont->setToolTip(tr("App Font"));
  connect(_appFont, SIGNAL(valueChanged()), this, SLOT(appFontChanged()));
  form->addRow(tr("App Font"), _appFont);
  
  _flags.insert("appFont", 0);

  // tab position
  l.clear();
  l << tr("North") << tr("South") << tr("West") << tr("East");
  
  _tabPosition = new QComboBox;
  _tabPosition->addItems(l);
  _tabPosition->setCurrentIndex(settings.value("plot_tab_position", 3).toInt());
  connect(_tabPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(tabPositionChanged()));
  form->addRow(tr("Tab Position"), _tabPosition);
  
  _flags.insert("plotTabPosition", 0);

  _tabs->addTab(w, tr("General"));
}

void ConfigureDialog::buttonStatus ()
{
  if (_modified)
    _okButton->setEnabled(TRUE);
  else
    _okButton->setEnabled(FALSE);
}

void ConfigureDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ConfigureDialog::done ()
{
  QSettings settings(g_settingsFile);

  if (_flags.value("background"))
  {
    QColor color = _background->color();
    settings.setValue("plot_background_color", color.name());
    g_middleMan->plotBackgroundColor(color);
  }
  
  if (_flags.value("appFont"))
  {
    QFont font = _appFont->font();
    settings.setValue("app_font", font.toString());
    QApplication::setFont(font);
  }

  if (_flags.value("plotTabPosition"))
  {
    settings.setValue("plot_tab_position", _tabPosition->currentIndex());
    g_middleMan->plotTabPosition(_tabPosition->currentIndex());
  }

  _command->setReturnCode("0");

  saveSettings();

  accept();
}

void ConfigureDialog::cancel ()
{
  saveSettings();
  reject();
}

void ConfigureDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("configure_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("configure_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void ConfigureDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("configure_dialog_window_size", size());
  settings.setValue("configure_dialog_window_position", pos());
  settings.sync();
}

void ConfigureDialog::backgroundChanged ()
{
  _modified++;
  _flags.insert("background", 1);
  buttonStatus();
}

void ConfigureDialog::appFontChanged ()
{
  _modified++;
  _flags.insert("appFont", 1);
  buttonStatus();
}

void ConfigureDialog::tabPositionChanged ()
{
  _modified++;
  _flags.insert("plotTabPosition", 1);
  buttonStatus();
}
