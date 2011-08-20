#include "CommandAdaptor.h"

#include <QMetaObject>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

CommandAdaptor::CommandAdaptor (QObject *p) : QDBusAbstractAdaptor (p)
{
  setAutoRelaySignals(true);
}

CommandAdaptor::~CommandAdaptor()
{
}
