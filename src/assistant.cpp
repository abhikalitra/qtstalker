/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

/*
This file is from Qt4.5.2:
http://qt.nokia.com/doc/4.5/help-simpletextviewer-assistant-cpp.html

Qtstalker changes are minor configuration and are noted below, and also in the
project's revision control system.
CVS $Revision: 1.2 $ $Date: 2009/10/20 04:22:06 $
*/

#include <QtCore/QByteArray>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QProcess>

#include <QtGui/QMessageBox>

#include "assistant.h"
// Qtstalker start changes
#include "../lib/qtstalker_defines.h"
// Qtstalker end changes

Assistant::Assistant()
    : proc(0)
{
}

//! [0]
Assistant::~Assistant()
{
    if (proc && proc->state() == QProcess::Running) {
        proc->terminate();
        proc->waitForFinished(3000);
    }
    delete proc;
}
//! [0]

//! [1]
void Assistant::showDocumentation(const QString &page)
{
    if (!startAssistant())
        return;

    QByteArray ba("SetSource ");
// Qtstalker start changes
/*
    ba.append("qthelp://com.trolltech.examples.simpletextviewer/doc/");
*/
    ba.append("qthelp://qtstalker/doc/");
// Qtstalker end changes
    
    proc->write(ba + page.toLocal8Bit() + '\0');
}
//! [1]

//! [2]
bool Assistant::startAssistant()
{
    if (!proc)
        proc = new QProcess();

    if (proc->state() != QProcess::Running) {
        QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#if !defined(Q_OS_MAC)
        app += QLatin1String("assistant");
#else
        app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");    
#endif

        QStringList args;
// Qtstalker start changes
/*
        args << QLatin1String("-collectionFile")
            << QLibraryInfo::location(QLibraryInfo::ExamplesPath)
            + QLatin1String("/help/simpletextviewer/documentation/simpletextviewer.qhc")
            << QLatin1String("-enableRemoteControl");
*/
        QString collectionFile = QString("%1/qtstalker/html/doc.qhc").arg(INSTALL_DOCS_DIR);
        args << QLatin1String("-collectionFile")
            << collectionFile
            << QLatin1String("-enableRemoteControl");
// Qtstalker end changes

        proc->start(app, args);

        if (!proc->waitForStarted()) {
// Qtstalker start changes
/*
            QMessageBox::critical(0, QObject::tr("Simple Text Viewer"),
                QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
*/
            QMessageBox::critical(0, QObject::tr("Qtstalker"),
                QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
// Qtstalker end changes
            return false;
        }    
    }
    return true;
}
//! [2]
