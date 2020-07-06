// File: typetags.h
/*
    Pep9 is a virtual machine for writing machine language and assembly
    language programs.

    Copyright (C) 2019  J. Stanley Warford & Matthew McRaven, Pepperdine University

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "typetags.h"

#include "symbol/symbolentry.h"

AType::~AType()
{

}

PrimitiveType::PrimitiveType(QSharedPointer<const SymbolEntry> symbol, Enu::ESymbolFormat format):
    symbol(symbol), format(format)
{

}

PrimitiveType::~PrimitiveType()
{

}

QList<QPair<Enu::ESymbolFormat, QString> > PrimitiveType::toPrimitives(QString prefix) const
{
    return QList<QPair<Enu::ESymbolFormat, QString>>{{format, prefix+symbol->getName()}};
}

QString PrimitiveType::toString(QString prefix) const
{
    return QString("%1%2")
            .arg(prefix)
            .arg(QString(symbol->getName()), 0);
}

quint16 PrimitiveType::size() const
{
    return Enu::tagNumBytes(format);
}

PrimitiveType::operator QString() const
{
    return toString();
}

StructType::StructType(QSharedPointer<const SymbolEntry> symbol, QList<QSharedPointer<AType> > members):
   symbol(symbol),  members(members)

{

}

StructType::~StructType()
{

}

QList<QPair<Enu::ESymbolFormat, QString> > StructType::toPrimitives(QString prefix) const
{
    auto out = QList<QPair<Enu::ESymbolFormat, QString>>();
    QString runningPrefix = QString("%1%2.")
            .arg(prefix)
            .arg(symbol->getName());
    for(auto member : members) {
        out.append(member->toPrimitives(runningPrefix));
    }
    return out;
}

QString StructType::toString(QString prefix) const
{
    QStringList entries;
    for (QSharedPointer<AType> ent : members) {
        entries.append(QString("%1")
                       .arg(ent->toString(prefix+symbol->getName()+".")));
    }
    return QString("struct %1%2{"+entries.join(", ")+"}")
            .arg(prefix)
            .arg(symbol->getName());
}

quint16 StructType::size() const
{
    quint16 size = 0;
    for(auto member : members) {
        size += member->size();
    }
    return size;
}

StructType::operator QString() const
{
    return toString("");
}

ArrayType::ArrayType(QSharedPointer<const SymbolEntry> symbol, Enu::ESymbolFormat format, quint16 len):
    symbol(symbol), format(format), len(len)
{

}

ArrayType::~ArrayType()
{

}

QList<QPair<Enu::ESymbolFormat, QString> > ArrayType::toPrimitives(QString prefix) const
{
    auto out = QList<QPair<Enu::ESymbolFormat, QString>>();
    QString runningPrefix = QString("%1%2")
            .arg(prefix)
            .arg(symbol->getName());
    for(int it=0; it< this->len; it++) {
        out.append({{format,QString("%1[%2]").arg(runningPrefix).arg(it)}});
    }
    return out;
}

QString ArrayType::toString(QString prefix) const
{
    return QString("%1%2[%3]")
            .arg(prefix)
            .arg(symbol->getName())
            .arg(len);
}

quint16 ArrayType::size() const
{
    return static_cast<quint16>(Enu::tagNumBytes(format) * len);
}

ArrayType::operator QString() const
{
    return toString();
}

LiteralArrayType::LiteralArrayType(Enu::ESymbolFormat format, quint16 len):
    format(format), len(len)
{

}

LiteralArrayType::~LiteralArrayType()
{

}

QList<QPair<Enu::ESymbolFormat, QString> > LiteralArrayType::toPrimitives(QString prefix) const
{
    auto out = QList<QPair<Enu::ESymbolFormat, QString>>();
    QString runningPrefix = QString("%1")
            .arg(prefix);
    for(int it=0; it< this->len; it++) {
        out.append({{format,QString("%1[%2]").arg(runningPrefix).arg(it)}});
    }
    return out;
}

QString LiteralArrayType::toString(QString prefix) const
{
    return QString("%1[%3]")
            .arg(prefix)
            .arg(len);
}

quint16 LiteralArrayType::size() const
{
    return static_cast<quint16>(Enu::tagNumBytes(format) * len);
}

LiteralArrayType::operator QString() const
{
    return toString();
}

LiteralPrimitiveType::LiteralPrimitiveType(QString name, Enu::ESymbolFormat format): name(name), format(format)
{

}

LiteralPrimitiveType::~LiteralPrimitiveType()
{

}

QList<QPair<Enu::ESymbolFormat, QString> > LiteralPrimitiveType::toPrimitives(QString prefix) const
{
    return QList<QPair<Enu::ESymbolFormat, QString>>{{format, prefix+name}};
}

QString LiteralPrimitiveType::toString(QString prefix) const
{
    return QString("%1%2")
            .arg(prefix)
            .arg(QString(name), 0);
}

quint16 LiteralPrimitiveType::size() const
{
    return Enu::tagNumBytes(format);
}

LiteralPrimitiveType::operator QString() const
{
    return toString();
}


QDebug operator<<(QDebug os, const QSharedPointer<AType> &item)
{
    return os.noquote().nospace()<< *item.get();
}

QDebug operator<<(QDebug os, const QSharedPointer<const AType> &item)
{
    return os.noquote().nospace()<< *item.get();
}
