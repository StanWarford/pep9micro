// File: memorycellgraphicsitem.cpp
/*
    Pep9 is a virtual machine for writing machine language and assembly
    language programs.

    Copyright (C) 2009  J. Stanley Warford, Pepperdine University

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

#include "memorycellgraphicsitem.h"

#include <QPainter>

#include "memory/amemorydevice.h"
#include "pep/pep.h"
#include "style/fonts.h"

const int MemoryCellGraphicsItem::boxHeight = 22;
const int MemoryCellGraphicsItem::boxWidth = 50;
const int MemoryCellGraphicsItem::addressWidth = 48;
const int MemoryCellGraphicsItem::symbolWidth = 96;
const int MemoryCellGraphicsItem::bufferWidth = 14;

MemoryCellGraphicsItem::MemoryCellGraphicsItem(const AMemoryDevice *memDevice, int addr, QString sym,
                                               ESymbolFormat eSymFrmt, int xLoc, int yLoc): memDevice(memDevice), x(xLoc), y(yLoc),
    address(quint16(addr)), eSymbolFormat(eSymFrmt), colors(&PepColors::lightMode), isModified(false)
{
    if (sym.length() > 0 && sym.at(0).isDigit()) {
        sym = "";
    }
    symbol = sym;
    setColorTheme(*colors);
}

MemoryCellGraphicsItem::~MemoryCellGraphicsItem()
{
    // We do not own the memory device, so we should not delete it.
}

QRectF MemoryCellGraphicsItem::boundingRect() const
{
    const int Margin = 4;
    return QRectF(QPointF(x - addressWidth - Margin, y - Margin),
                  QSizeF(addressWidth + bufferWidth * 2 + boxWidth + symbolWidth + Margin * 2, boxHeight + Margin * 2));
}

void MemoryCellGraphicsItem::updateContents(int newAddr, QString newSymbol, ESymbolFormat newFmt, int newY)
{
    this->address = quint16(newAddr);
    if (newSymbol.length() > 0 && newSymbol.at(0).isDigit()) {
        newSymbol = "";
    }
    this->symbol = newSymbol;
    this->eSymbolFormat = newFmt;
    this->y = newY;
}

void MemoryCellGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen pen(colors->textColor);
    pen.setWidth(2);
    painter->setPen(pen);
    if(isModified) {
        painter->setBrush(colors->muxCircuitRed);
    }
    else {
        painter->setBrush(backgroundColor);
    }
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(QRectF(x, y, boxWidth, boxHeight), 2, 2, Qt::RelativeSize);

    painter->setPen(colors->textColor);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setFont(QFont(PepCore::codeFont, PepCore::codeFontSize));

    painter->drawText(QRectF(x - addressWidth - bufferWidth, y, addressWidth, boxHeight), Qt::AlignVCenter | Qt::AlignRight, QString("%1").arg(address, 4, 16, QLatin1Char('0')).toUpper());

    painter->drawText(QRectF(x + bufferWidth + boxWidth, y, symbolWidth, boxHeight), Qt::AlignVCenter | Qt::AlignLeft, QString("%1").arg(symbol));

    painter->setPen(colors->textColor);
    painter->drawText(QRectF(x, y, boxWidth, boxHeight), Qt::AlignCenter, value);
}

void MemoryCellGraphicsItem::setModified(bool value)
{
    isModified = value;
}

void MemoryCellGraphicsItem::setColorTheme(const PepColors::Colors &newColors)
{
    this->colors = &newColors;
    backgroundColor = colors->backgroundFill;
}

void MemoryCellGraphicsItem::setBackgroundColor(QColor color)
{
    backgroundColor = color;
}

quint16 MemoryCellGraphicsItem::getValue() const
{
    return iValue;
}

void MemoryCellGraphicsItem::updateValue()
{
    quint8 byte;
    quint16 word;
    switch (eSymbolFormat) {
    case ESymbolFormat::F_1C:
        memDevice->getByte(address, byte);
        if(QChar::isPrint(byte)) {
            value = QChar(byte);
        }
        else value = QChar('.');
        iValue = byte;
        break;
    // 1 byte integers are to be displayed as unsigned.
    case ESymbolFormat::F_1D:
        memDevice->getByte(address, byte);
        value = QString("%1").arg(byte);
        iValue = byte;
        break;
    // 2 byte integers are to be displayed as signed.
    case ESymbolFormat::F_2D:
        memDevice->getWord(address, word);
        value = QString("%1").arg(static_cast<qint16>(word));
        iValue = word;
        break;
    case ESymbolFormat::F_1H:
        memDevice->getByte(address, byte);
        value = QString("%1").arg(byte, 2, 16, QLatin1Char('0')).toUpper();
        iValue = byte;
        break;
    case ESymbolFormat::F_2H:
        memDevice->getWord(address, word);
        value = QString("%1").arg(word, 4, 16, QLatin1Char('0')).toUpper();
        iValue = word;
        break;
    default:
        value = ""; // Should not occur
        iValue = 0;
        break;
    }

}

quint16 MemoryCellGraphicsItem::getAddress() const
{
    return address;
}

quint16 MemoryCellGraphicsItem::getNumBytes() const
{
    return cellSize(eSymbolFormat);
}

quint16 cellSize(ESymbolFormat symbolFormat)
{
    switch (symbolFormat) {
    case ESymbolFormat::F_1C:
        return 1;
    case ESymbolFormat::F_1D:
        return 1;
    case ESymbolFormat::F_2D:
        return 2;
    case ESymbolFormat::F_1H:
        return 1;
    case ESymbolFormat::F_2H:
        return 2;
    default:
        // Should not occur
        return 0;
    }
}
