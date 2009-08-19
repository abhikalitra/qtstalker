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


#include "IndicatorDialog.h"
#include "DataBase.h"
#include "IndicatorParms.h"

#include <QList>
#include <QVBoxLayout>
#include <QMessageBox>



IndicatorDialog::IndicatorDialog (QWidget *w, QString &n) : QDialog (w, 0)
{
  name = n;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(10);
  vbox->setSpacing(5);
  setLayout(vbox);

  formula = new FormulaEdit;
  vbox->addWidget(formula);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(save()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(buttonBox);

  if (name.length())
    loadIndicator();
}

void IndicatorDialog::openRule ()
{
/*
  DataBase db;
  QStringList l;
  db.getIndicatorList(l);
  bool ok;
  QString selection = QInputDialog::getItem(this, tr("Select Indicator"), tr("Select indicator to open."), l, 0, FALSE, &ok, 0);
  if (! ok)
    return;

  QString s = name;
  name = selection;
  loadIndicator();
  name = s;
*/
}

void IndicatorDialog::loadIndicator ()
{
  Indicator i;
  i.setName(name);

  DataBase db;
  QString s, var;
  db.getIndicator(i);

  formula->setIndicator(i);
}

void IndicatorDialog::save ()
{
  Indicator i;
  i.setName(name);
  formula->getIndicator(i);
  if (! i.count())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Indicator has no parms."));
    return;
  }

  DataBase db;
  db.setIndicator(i);

  accept();
}

