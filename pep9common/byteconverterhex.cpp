// File: byteconverterhex.cpp
/*
    The Pep/9 suite of applications (Pep9, Pep9CPU, Pep9Micro) are
    simulators for the Pep/9 virtual machine, and allow users to
    create, simulate, and debug across various levels of abstraction.

    Copyright (C) 2010  J. Stanley Warford, Pepperdine University

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

#include "byteconverterhex.h"
#include "ui_byteconverterhex.h"
#include <QTextCursor>

ByteConverterHex::ByteConverterHex(QWidget *parent) :
        QWidget(parent),
        m_ui(new Ui::ByteConverterHex)
{
    m_ui->setupUi(this);
    // Regular expression to validate 0x00 - 0xff
    QRegExp hexRx("0x([0-9]|[a-f]|[A-F])([0-9]|[a-f]|[A-F])");
    hexValidator = new QRegExpValidator(hexRx, this);
    m_ui->lineEdit->setValidator(hexValidator);
    // Forward the textEdited() signal from m_ui->lineEdit up to the main window
    connect(m_ui->lineEdit, &QLineEdit::textEdited, this, &ByteConverterHex::textEdited);
    connect(m_ui->lineEdit, &QLineEdit::cursorPositionChanged, this, &ByteConverterHex::moveCursorAwayFromPrefix);

}

ByteConverterHex::~ByteConverterHex()
{
    delete m_ui;
}

void ByteConverterHex::setValue(int value)
{
    if (value == -1) {
        m_ui->lineEdit->setText("0x");
    }
    else {
        m_ui->lineEdit->setText(QString("0x%1").arg(value, 2, 16, QLatin1Char('0')));
    }
}

void ByteConverterHex::moveCursorAwayFromPrefix(int old, int current)
{
    if (current < 2) {
        m_ui->lineEdit->setCursorPosition(old);
    }
}
