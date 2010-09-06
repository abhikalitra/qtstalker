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

#include "ChartObjectDialog.h"
#include "ChartObjectDataBase.h"
#include "ChartObject.h"

#include <QtDebug>
#include <QLayout>
#include <QMessageBox>

ChartObjectDialog::ChartObjectDialog () : QDialog (0, 0)
{
  setAttribute(Qt::WA_DeleteOnClose);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(5);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  _buttonBox = new QDialogButtonBox;

  _deleteButton = _buttonBox->addButton(tr("Delete"), QDialogButtonBox::RejectRole);
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteChartObject()));
  
  _buttonBox->addButton(QDialogButtonBox::Help);
  _buttonBox->addButton(QDialogButtonBox::Ok);
  _buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(_buttonBox);

  vbox->addStretch(1);
}

ChartObjectDialog::~ChartObjectDialog ()
{
}

// virtual
void ChartObjectDialog::done ()
{
}

void ChartObjectDialog::deleteChartObject ()
{
  int rc = QMessageBox::warning(this,
                                tr("Qtstalker: Warning"),
                                tr("Are you sure you want to delete group this chart object?"),
                                QMessageBox::Yes,
                                QMessageBox::No,
                                QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;
  
  ChartObjectDataBase db;
  db.deleteChartObject(_settings.id);
  
  signalDelete(_settings);

  reject();
}

void ChartObjectDialog::enableDeleteButton (int d)
{
  if (d)
    _deleteButton->show();
  else
    _deleteButton->hide();
}

void ChartObjectDialog::setSettings (ChartObjectSettings &)
{
}

