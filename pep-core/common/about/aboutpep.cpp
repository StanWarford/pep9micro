// File: aboutpep.cpp
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

#include "aboutpep.h"
#include "ui_aboutpep.h"

#include "pep/pep.h"

AboutPep::AboutPep(QString aboutText, QPixmap pixmap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutPep), str(aboutText), icon(pixmap)
{
    ui->setupUi(this);
    QPalette p = ui->aboutText->palette();
    p.setColor(QPalette::Base, QColor(0,0,0,0)); // r,g,b,A
    ui->aboutText->setPalette(p);
    ui->aboutText->setText(str);
    ui->label->setPixmap(pixmap);
    setMinimumHeight(460);
    this->resize(this->contentsRect().x(),450);
    bool hide=true;
    QString commitText;

    #ifdef GIT_SHA
    commitText = QString(GIT_SHA);
    if(commitText.isEmpty()) {
        hide = true;
    } else {
        hide = false;
    }
    #endif
    ui->commit_label->setHidden(hide);
    ui->commit_title->setHidden(hide);
    if(!hide){
        ui->commit_label->setText(commitText);
    }
}

AboutPep::~AboutPep()
{
    delete ui;
}
