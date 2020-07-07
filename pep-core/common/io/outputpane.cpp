// File: outputpane.cpp
/*
    The Pep/9 suite of applications (Pep9, Pep9CPU, Pep9Micro) are
    simulators for the Pep/9 virtual machine, and allow users to
    create, simulate, and debug across various levels of abstraction.

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
#include "outputpane.h"
#include "ui_outputpane.h"

#include <QFontDialog>
#include <QScrollBar>

#include "style/fonts.h"

OutputPane::OutputPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputPane)
{
    ui->setupUi(this);

    ui->label->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize));
    ui->plainTextEdit->setFont(QFont(PepCore::codeFont, PepCore::ioFontSize));
}

OutputPane::~OutputPane()
{
    delete ui;
}

void OutputPane::appendOutput(QString str)
{
    ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText().append(str));
    ui->plainTextEdit->verticalScrollBar()->setValue(ui->plainTextEdit->verticalScrollBar()->maximum());
}

void OutputPane::clearOutput()
{
    ui->plainTextEdit->clear();
}

void OutputPane::highlightOnFocus()
{
    if (ui->plainTextEdit->hasFocus()) {
        ui->label->setAutoFillBackground(true);
    }
    else {
        ui->label->setAutoFillBackground(false);
    }
}

bool OutputPane::hasFocus()
{
    return ui->plainTextEdit->hasFocus();
}

void OutputPane::copy()
{
    ui->plainTextEdit->copy();
}

void OutputPane::clearText()
{
    ui->plainTextEdit->clear();
}

void OutputPane::onFontChanged(QFont font)
{
    ui->plainTextEdit->setFont(font);
}

void OutputPane::mouseReleaseEvent(QMouseEvent *)
{
    ui->plainTextEdit->setFocus();
}
