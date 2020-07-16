// File: redefinemnemonicsdialog.cpp
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
#include "redefinemnemonicsdialog.h"
#include "ui_redefinemnemonicsdialog.h"

#include <QRegExpValidator>

#include "style/fonts.h"

#include "isadefs.h"

RedefineMnemonicsDialog::RedefineMnemonicsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::RedefineMnemonicsDialog), nop0IsTrap(true)
{
    ui->setupUi(this);
    restoreDefaults();
    connect(this, &RedefineMnemonicsDialog::closed, this, &RedefineMnemonicsDialog::onDone);

    ui->unaryOpCodeLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->unaryOpCode0Label->setFont(QFont(PepCore::codeFont));
    ui->unaryOpCode1Label->setFont(QFont(PepCore::codeFont));

    ui->unaryMnemonicLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->unaryMnemonic0LineEdit->setFont(QFont(PepCore::codeFont));
    ui->unaryMnemonic1LineEdit->setFont(QFont(PepCore::codeFont));

    ui->nonunaryMnemonicLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->iLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->dLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->nLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->sLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->sfLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->xLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->sxLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));
    ui->sfxLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));

    ui->nonunaryOpCodeLabel->setFont(QFont(PepCore::labelFont, PepCore::labelFontSize, QFont::Bold));

    ui->nonunaryOpCode1Label->setFont(QFont(PepCore::codeFont));
    ui->nonunaryOpCode2Label->setFont(QFont(PepCore::codeFont));
    ui->nonunaryOpCode3Label->setFont(QFont(PepCore::codeFont));
    ui->nonunaryOpCode4Label->setFont(QFont(PepCore::codeFont));
    ui->nonunaryOpCode5Label->setFont(QFont(PepCore::codeFont));

    ui->nonUnaryMnemonic0LineEdit->setFont(QFont(PepCore::codeFont));
    ui->nonUnaryMnemonic1LineEdit->setFont(QFont(PepCore::codeFont));
    ui->nonUnaryMnemonic2LineEdit->setFont(QFont(PepCore::codeFont));
    ui->nonUnaryMnemonic3LineEdit->setFont(QFont(PepCore::codeFont));
    ui->nonUnaryMnemonic4LineEdit->setFont(QFont(PepCore::codeFont));

    connect(ui->defaultMnemonicsButton, SIGNAL(clicked()), this, SLOT(restoreDefaults()));

    QRegExp rx("^[A-Za-z][A-Za-z0-9]{0,7}");
    QValidator *validator = new QRegExpValidator(rx, this);

    ui->nonUnaryMnemonic0LineEdit->setValidator(validator);
    ui->nonUnaryMnemonic1LineEdit->setValidator(validator);
    ui->nonUnaryMnemonic2LineEdit->setValidator(validator);
    ui->nonUnaryMnemonic3LineEdit->setValidator(validator);
    ui->nonUnaryMnemonic4LineEdit->setValidator(validator);

    ui->unaryMnemonic0LineEdit->setValidator(validator);
    ui->unaryMnemonic1LineEdit->setValidator(validator);

    connect(ui->nonUnaryMnemonic0LineEdit, &QLineEdit::textEdited, this, &RedefineMnemonicsDialog::redefineNonUnaryMnemonic0);
    connect(ui->nonUnaryMnemonic1LineEdit, &QLineEdit::textEdited, this, &RedefineMnemonicsDialog::redefineNonUnaryMnemonic1);
    connect(ui->nonUnaryMnemonic2LineEdit, &QLineEdit::textEdited, this, &RedefineMnemonicsDialog::redefineNonUnaryMnemonic2);
    connect(ui->nonUnaryMnemonic3LineEdit, &QLineEdit::textEdited, this, &RedefineMnemonicsDialog::redefineNonUnaryMnemonic3);
    connect(ui->nonUnaryMnemonic4LineEdit, &QLineEdit::textEdited, this, &RedefineMnemonicsDialog::redefineNonUnaryMnemonic3);

    connect(ui->unaryMnemonic0LineEdit, &QLineEdit::textEdited, this, &RedefineMnemonicsDialog::redefineUnaryMnemonic0);
    connect(ui->unaryMnemonic1LineEdit, &QLineEdit::textEdited, this, &RedefineMnemonicsDialog::redefineUnaryMnemonic1);

    connect(ui->mnemon0iCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon0dCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon0nCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon0sCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon0sfCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon0xCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon0sxCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon0sfxCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));

    connect(ui->mnemon1iCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon1dCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon1nCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon1sCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon1sfCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon1xCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon1sxCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));
    connect(ui->mnemon1sfxCheckBox, SIGNAL(clicked()), this, SLOT(setNonUnaryAllowedModes()));

    connect(ui->mnemon2iCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon2dCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon2nCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon2sCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon2sfCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon2xCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon2sxCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon2sfxCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);

    connect(ui->mnemon3iCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon3dCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon3nCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon3sCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon3sfCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon3xCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon3sxCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon3sfxCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);

    connect(ui->mnemon4iCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon4dCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon4nCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon4sCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon4sfCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon4xCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon4sxCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);
    connect(ui->mnemon4sfxCheckBox, &QAbstractButton::clicked, this, &RedefineMnemonicsDialog::setNonUnaryAllowedModes);

    setWindowFlag(Qt::WindowStaysOnTopHint);
}

RedefineMnemonicsDialog::~RedefineMnemonicsDialog()
{
    delete ui;
}

void RedefineMnemonicsDialog::init(bool nop0IsTrap)
{
    this->nop0IsTrap = nop0IsTrap;
    if(!nop0IsTrap) {
        ui->unaryOpCode0Label->setHidden(true);
        ui->unaryOpCode0Label->setEnabled(false);
        ui->unaryMnemonic0LineEdit->setHidden(true);
        ui->unaryMnemonic0LineEdit->setEnabled(false);
    }
}

void RedefineMnemonicsDialog::reject()
{
    QDialog::reject();
    emit closed();
}

void RedefineMnemonicsDialog::restoreDefaults()
{
    using namespace Pep9::ISA;
    ui->unaryMnemonic0LineEdit->setText(defaultUnaryMnemonic0);
    ui->unaryMnemonic1LineEdit->setText(defaultUnaryMnemonic1);
    ui->nonUnaryMnemonic0LineEdit->setText(defaultNonUnaryMnemonic0);
    ui->mnemon0iCheckBox->setChecked(defaultMnemon0AddrModes & static_cast<int>(EAddrMode::I));
    ui->mnemon0dCheckBox->setChecked(defaultMnemon0AddrModes &static_cast<int>(EAddrMode::D));
    ui->mnemon0nCheckBox->setChecked(defaultMnemon0AddrModes &static_cast<int>(EAddrMode:: N));
    ui->mnemon0sCheckBox->setChecked(defaultMnemon0AddrModes & static_cast<int>(EAddrMode::S));
    ui->mnemon0sfCheckBox->setChecked(defaultMnemon0AddrModes & static_cast<int>(EAddrMode::SF));
    ui->mnemon0xCheckBox->setChecked(defaultMnemon0AddrModes & static_cast<int>(EAddrMode::X));
    ui->mnemon0sxCheckBox->setChecked(defaultMnemon0AddrModes & static_cast<int>(EAddrMode::SX));
    ui->mnemon0sfxCheckBox->setChecked(defaultMnemon0AddrModes & static_cast<int>(EAddrMode::SFX));
    ui->nonUnaryMnemonic1LineEdit->setText(defaultNonUnaryMnemonic1);
    ui->mnemon1iCheckBox->setChecked(defaultMnemon1AddrModes & static_cast<int>(EAddrMode::I));
    ui->mnemon1dCheckBox->setChecked(defaultMnemon1AddrModes & static_cast<int>(EAddrMode::D));
    ui->mnemon1nCheckBox->setChecked(defaultMnemon1AddrModes & static_cast<int>(EAddrMode::N));
    ui->mnemon1sCheckBox->setChecked(defaultMnemon1AddrModes & static_cast<int>(EAddrMode::S));
    ui->mnemon1sfCheckBox->setChecked(defaultMnemon1AddrModes & static_cast<int>(EAddrMode::SF));
    ui->mnemon1xCheckBox->setChecked(defaultMnemon1AddrModes & static_cast<int>(EAddrMode::X));
    ui->mnemon1sxCheckBox->setChecked(defaultMnemon1AddrModes & static_cast<int>(EAddrMode::SX));
    ui->mnemon1sfxCheckBox->setChecked(defaultMnemon1AddrModes & static_cast<int>(EAddrMode::SFX));
    ui->nonUnaryMnemonic2LineEdit->setText(defaultNonUnaryMnemonic2);
    ui->mnemon2iCheckBox->setChecked(defaultMnemon2AddrModes & static_cast<int>(EAddrMode::I));
    ui->mnemon2dCheckBox->setChecked(defaultMnemon2AddrModes & static_cast<int>(EAddrMode::D));
    ui->mnemon2nCheckBox->setChecked(defaultMnemon2AddrModes & static_cast<int>(EAddrMode::N));
    ui->mnemon2sCheckBox->setChecked(defaultMnemon2AddrModes & static_cast<int>(EAddrMode::S));
    ui->mnemon2sfCheckBox->setChecked(defaultMnemon2AddrModes & static_cast<int>(EAddrMode::SF));
    ui->mnemon2xCheckBox->setChecked(defaultMnemon2AddrModes & static_cast<int>(EAddrMode::X));
    ui->mnemon2sxCheckBox->setChecked(defaultMnemon2AddrModes & static_cast<int>(EAddrMode::SX));
    ui->mnemon2sfxCheckBox->setChecked(defaultMnemon2AddrModes & static_cast<int>(EAddrMode::SFX));
    ui->nonUnaryMnemonic3LineEdit->setText(defaultNonUnaryMnemonic3);
    ui->mnemon3iCheckBox->setChecked(defaultMnemon3AddrModes & static_cast<int>(EAddrMode::I));
    ui->mnemon3dCheckBox->setChecked(defaultMnemon3AddrModes & static_cast<int>(EAddrMode::D));
    ui->mnemon3nCheckBox->setChecked(defaultMnemon3AddrModes & static_cast<int>(EAddrMode::N));
    ui->mnemon3sCheckBox->setChecked(defaultMnemon3AddrModes & static_cast<int>(EAddrMode::S));
    ui->mnemon3sfCheckBox->setChecked(defaultMnemon3AddrModes & static_cast<int>(EAddrMode::SF));
    ui->mnemon3xCheckBox->setChecked(defaultMnemon3AddrModes & static_cast<int>(EAddrMode::X));
    ui->mnemon3sxCheckBox->setChecked(defaultMnemon3AddrModes & static_cast<int>(EAddrMode::SX));
    ui->mnemon3sfxCheckBox->setChecked(defaultMnemon3AddrModes & static_cast<int>(EAddrMode::SFX));
    ui->nonUnaryMnemonic4LineEdit->setText(defaultNonUnaryMnemonic4);
    ui->mnemon4iCheckBox->setChecked(defaultMnemon4AddrModes & static_cast<int>(EAddrMode::I));
    ui->mnemon4dCheckBox->setChecked(defaultMnemon4AddrModes & static_cast<int>(EAddrMode::D));
    ui->mnemon4nCheckBox->setChecked(defaultMnemon4AddrModes & static_cast<int>(EAddrMode::N));
    ui->mnemon4sCheckBox->setChecked(defaultMnemon4AddrModes & static_cast<int>(EAddrMode::S));
    ui->mnemon4sfCheckBox->setChecked(defaultMnemon4AddrModes& static_cast<int>(EAddrMode::SF));
    ui->mnemon4xCheckBox->setChecked(defaultMnemon4AddrModes & static_cast<int>(EAddrMode::X));
    ui->mnemon4sxCheckBox->setChecked(defaultMnemon4AddrModes & static_cast<int>(EAddrMode::SX));
    ui->mnemon4sfxCheckBox->setChecked(defaultMnemon4AddrModes & static_cast<int>(EAddrMode::SFX));

    addrModesMap.insert(EMnemonic::NOP, defaultMnemon0AddrModes);
    addrModesMap.insert(EMnemonic::DECI, defaultMnemon1AddrModes);
    addrModesMap.insert(EMnemonic::DECO, defaultMnemon2AddrModes);
    addrModesMap.insert(EMnemonic::HEXO, defaultMnemon3AddrModes);
    addrModesMap.insert(EMnemonic::STRO, defaultMnemon4AddrModes);

    initEnumMnemonMaps();
}

void RedefineMnemonicsDialog::redefineNonUnaryMnemonic0(QString string)
{    
    using namespace Pep9::ISA;

    string = string.toUpper();
    if (string.isEmpty()) {
        ui->warningLabel->setText("Empty mnemonic not stored for 00101.");
    }
    else if (mnemonToEnumMap.contains(string)) {
        ui->warningLabel->setText("Duplicate not stored for 00101.");
    }
    else {
        ui->nonUnaryMnemonic0LineEdit->setText(string);
        mnemonToEnumMap.remove(enumToMnemonMap.value(EMnemonic::NOP));
        enumToMnemonMap.insert(EMnemonic::NOP, string);
        mnemonToEnumMap.insert(string, EMnemonic::NOP);
        ui->warningLabel->clear();
    }
}

void RedefineMnemonicsDialog::redefineNonUnaryMnemonic1(QString string)
{
    using namespace Pep9::ISA;

    string = string.toUpper();
    if (string.isEmpty()) {
        ui->warningLabel->setText("Empty mnemonic not stored for 00110.");
    }
    else if (mnemonToEnumMap.contains(string)) {
        ui->warningLabel->setText("Duplicate not stored for 00110.");
    }
    else {
        ui->nonUnaryMnemonic1LineEdit->setText(string);
        mnemonToEnumMap.remove(enumToMnemonMap.value(EMnemonic::DECI));
        enumToMnemonMap.insert(EMnemonic::DECI, string);
        mnemonToEnumMap.insert(string, EMnemonic::DECI);
        ui->warningLabel->clear();
    }
}

void RedefineMnemonicsDialog::redefineNonUnaryMnemonic2(QString string)
{
    using namespace Pep9::ISA;

    string = string.toUpper();
    if (string.isEmpty()) {
        ui->warningLabel->setText("Empty mnemonic not stored for 00111.");
    }
    else if (mnemonToEnumMap.contains(string)) {
        ui->warningLabel->setText("Duplicate not stored for 00111.");
    }
    else {
        ui->nonUnaryMnemonic2LineEdit->setText(string);
        mnemonToEnumMap.remove(enumToMnemonMap.value(EMnemonic::DECO));
        enumToMnemonMap.insert(EMnemonic::DECO, string);
        mnemonToEnumMap.insert(string, EMnemonic::DECO);
        ui->warningLabel->clear();
    }
}

void RedefineMnemonicsDialog::redefineNonUnaryMnemonic3(QString string)
{
    using namespace Pep9::ISA;

    string = string.toUpper();
    if (string.isEmpty()) {
        ui->warningLabel->setText("Empty mnemonic not stored for 01000.");
    }
    else if (mnemonToEnumMap.contains(string)) {
        ui->warningLabel->setText("Duplicate not stored for 01000.");
    }
    else {
        ui->nonUnaryMnemonic3LineEdit->setText(string);
        mnemonToEnumMap.remove(enumToMnemonMap.value(EMnemonic::HEXO));
        enumToMnemonMap.insert(EMnemonic::STRO, string);
        mnemonToEnumMap.insert(string, EMnemonic::HEXO);
        ui->warningLabel->clear();
    }
}

void RedefineMnemonicsDialog::redefineNonUnaryMnemonic4(QString string)
{
    using namespace Pep9::ISA;

    string = string.toUpper();
    if (string.isEmpty()) {
        ui->warningLabel->setText("Empty mnemonic not stored for 01001.");
    }
    else if (mnemonToEnumMap.contains(string)) {
        ui->warningLabel->setText("Duplicate not stored for 01001.");
    }
    else {
        ui->nonUnaryMnemonic3LineEdit->setText(string);
        mnemonToEnumMap.remove(enumToMnemonMap.value(EMnemonic::STRO));
        enumToMnemonMap.insert(EMnemonic::STRO, string);
        mnemonToEnumMap.insert(string, EMnemonic::STRO);
        ui->warningLabel->clear();
    }
}

void RedefineMnemonicsDialog::redefineUnaryMnemonic0(QString string)
{
    using namespace Pep9::ISA;

    string = string.toUpper();
    if (string.isEmpty()) {
        ui->warningLabel->setText("Empty mnemonic not stored for 0010 0010.");
    }
    else if (mnemonToEnumMap.contains(string)) {
        ui->warningLabel->setText("Duplicate not stored for 0010 0010.");
    }
    else {
        ui->unaryMnemonic0LineEdit->setText(string);
        mnemonToEnumMap.remove(enumToMnemonMap.value(EMnemonic::NOP0));
        enumToMnemonMap.insert(EMnemonic::NOP0, string);
        mnemonToEnumMap.insert(string, EMnemonic::NOP0);
        ui->warningLabel->clear();
    }
}

void RedefineMnemonicsDialog::redefineUnaryMnemonic1(QString string)
{
    using namespace Pep9::ISA;

    string = string.toUpper();
    if (string.isEmpty()) {
        ui->warningLabel->setText("Empty mnemonic not stored for 0010 0101.");
    }
    else if (mnemonToEnumMap.contains(string)) {
        ui->warningLabel->setText("Duplicate not stored for 0010 0101.");
    }
    else {
        ui->unaryMnemonic1LineEdit->setText(string);
        mnemonToEnumMap.remove(enumToMnemonMap.value(EMnemonic::NOP1));
        enumToMnemonMap.insert(EMnemonic::NOP1, string);
        mnemonToEnumMap.insert(string, EMnemonic::NOP1);
        ui->warningLabel->clear();
    }
}

void RedefineMnemonicsDialog::setNonUnaryAllowedModes()
{
    using namespace Pep9::ISA;

    int addrMode = 0;
    if (ui->mnemon0iCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::I);
    if (ui->mnemon0dCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::D);
    if (ui->mnemon0nCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::N);
    if (ui->mnemon0sCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::S);
    if (ui->mnemon0sfCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SF);
    if (ui->mnemon0xCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::X);
    if (ui->mnemon0sxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SX);
    if (ui->mnemon0sfxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SFX);
    addrModesMap.insert(EMnemonic::NOP, addrMode);
    addrMode = 0;
    if (ui->mnemon1iCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::I);
    if (ui->mnemon1dCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::D);
    if (ui->mnemon1nCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::N);
    if (ui->mnemon1sCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::S);
    if (ui->mnemon1sfCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SF);
    if (ui->mnemon1xCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::X);
    if (ui->mnemon1sxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SX);
    if (ui->mnemon1sfxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SFX);
    addrModesMap.insert(EMnemonic::DECI, addrMode);
    addrMode = 0;
    if (ui->mnemon2iCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::I);
    if (ui->mnemon2dCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::D);
    if (ui->mnemon2nCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::N);
    if (ui->mnemon2sCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::S);
    if (ui->mnemon2sfCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SF);
    if (ui->mnemon2xCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::X);
    if (ui->mnemon2sxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SX);
    if (ui->mnemon2sfxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SFX);
    addrModesMap.insert(EMnemonic::DECO, addrMode);
    addrMode = 0;
    if (ui->mnemon3iCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::I);
    if (ui->mnemon3dCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::D);
    if (ui->mnemon3nCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::N);
    if (ui->mnemon3sCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::S);
    if (ui->mnemon3sfCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SF);
    if (ui->mnemon3xCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::X);
    if (ui->mnemon3sxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SX);
    if (ui->mnemon3sfxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SFX);
    addrModesMap.insert(EMnemonic::HEXO, addrMode);
    addrMode = 0;
    if (ui->mnemon4iCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::I);
    if (ui->mnemon4dCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::D);
    if (ui->mnemon4nCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::N);
    if (ui->mnemon4sCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::S);
    if (ui->mnemon4sfCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SF);
    if (ui->mnemon4xCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::X);
    if (ui->mnemon4sxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SX);
    if (ui->mnemon4sfxCheckBox->isChecked()) addrMode |= static_cast<int>(EAddrMode::SFX);
    addrModesMap.insert(EMnemonic::STRO, addrMode);
}

void RedefineMnemonicsDialog::onDone()
{

}
