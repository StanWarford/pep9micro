// File: memorydumppane.cpp
/*
    Pep9 is a virtual machine for writing machine language and assembly
    language programs.
    
    Copyright (C) 2018  J. Stanley Warford, Pepperdine University

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
#include <QDebug>
#include <QFontDialog>
#include <QTextCharFormat>
#include <QAbstractTextDocumentLayout>
#include "memorydumppane.h"
#include "ui_memorydumppane.h"
#include "pep.h"
#include "enu.h"
#include <QStyle>
#include "cpucontrolsection.h"
#include "cpudatasection.h"
#include "cpumemoizer.h"
#include "memorysection.h"
#include "colors.h"
MemoryDumpPane::MemoryDumpPane(QWidget *parent) :
    QWidget(parent), data(new QStandardItemModel(this)), lineSize(320), colors(&PepColors::lightMode),
    ui(new Ui::MemoryDumpPane), inSimulation(false)
{
    ui->setupUi(this);
    if (Pep::getSystem() != "Mac") {
        ui->label->setFont(QFont(Pep::labelFont, Pep::labelFontSize));
        ui->tableView->setFont(QFont(Pep::codeFont, Pep::codeFontSize));
    }
    // Insert 1 column for address, 8 for memory bytes, and 1 for character dump
    data->insertColumns(0, 1+8+1);
    // Insert enough rows to hold 64k of memory
    data->insertRows(0, (1<<16)/8);
    // Set the addresses of every row now, as they will not change during execution of the program.
    for(int it = 0; it < (1<<16) /8; it++) {
        data->setData(data->index(it, 0), QString("0x%1").arg(it*8, 4, 16, QChar('0')));
    }

    // Hook the table view into the model, and size everything correctly
    ui->tableView->setModel(data);
    ui->tableView->resizeRowsToContents();

    // Connect scrolling events
    connect(ui->pcPushButton, &QAbstractButton::clicked, this, &MemoryDumpPane::scrollToPC);
    connect(ui->spPushButton, &QAbstractButton::clicked, this, &MemoryDumpPane::scrollToSP);
    connect(ui->scrollToLineEdit, &QLineEdit::textChanged, this, &MemoryDumpPane::scrollToAddress);
}

void MemoryDumpPane::init(MemorySection *memorySection, CPUDataSection *dataSection, CPUControlSection *controlSection)
{
    this->memorySection = memorySection;
    this->dataSection = dataSection;
    this->controlSection = controlSection;
    delegate = new MemoryDumpDelegate(memorySection, ui->tableView);
    ui->tableView->setItemDelegate(delegate);
    refreshMemory();
}

MemoryDumpPane::~MemoryDumpPane()
{
    delete ui;
    delete data;
    delete delegate;
}

void MemoryDumpPane::refreshMemory()
{
    quint8 tempData;
    QChar ch;
    QString memoryDumpLine;
    // Disable screen updates while re-writing all data fields to save execution time.
    bool updates = ui->tableView->updatesEnabled();
    ui->tableView->setUpdatesEnabled(false);
    for(int row = 0; row < (1<<16)/8; row++) {
        memoryDumpLine.clear();
        for(int col = 0; col < 8; col++) {
            // Use the data in the memory section to set the value in the model.
            tempData = memorySection->getMemoryByte(row*8 + col, false);
            data->setData(data->index(row, col + 1), QString("%1").arg(tempData, 2, 16, QChar('0')));
            ch = QChar(tempData);
            if (ch.isPrint()) {
                memoryDumpLine.append(ch);
            } else {
                memoryDumpLine.append(".");
            }
        }
        data->setData(data->index(row, 1+8), memoryDumpLine);
    }

    ui->tableView->setUpdatesEnabled(updates);
    clearHighlight();
    ui->tableView->resizeColumnsToContents();
    lineSize = 0;
    for(int it = 0; it<data->columnCount() ; it++) {
        lineSize += ui->tableView->columnWidth(it);
    }
}

void MemoryDumpPane::refreshMemoryLines(quint16 firstByte, quint16 lastByte)
{
    quint16 firstLine = firstByte / 8;
    quint16 lastLine = lastByte / 8;
    quint8 tempData;
    QChar ch;
    QString memoryDumpLine;
    // Disable screen updates while re-writing all data fields to save execution time.
    bool updates = ui->tableView->updatesEnabled();
    ui->tableView->setUpdatesEnabled(false);
    // Use <= comparison, so when firstLine == lastLine that the line is stil refreshed
    for(int row = firstLine; row <= lastLine; row++) {
        memoryDumpLine .clear();
        for(int col = 0; col < 8; col++) {
            // Use the data in the memory section to set the value in the model.
            tempData = memorySection->getMemoryByte(row*8 + col, false);
            data->setData(data->index(row, col + 1), QString("%1").arg(tempData, 2, 16, QChar('0')));
            ch = QChar(tempData);
            if (ch.isPrint()) {
                memoryDumpLine.append(ch);
            } else {
                memoryDumpLine.append(".");
            }
        }
        data->setData(data->index(row, 1+8), memoryDumpLine);
    }
    lineSize = 0;
    for(int it = 0; it< data->columnCount(); it++) {
        lineSize += ui->tableView->columnWidth(it);
    }

    ui->tableView->setUpdatesEnabled(updates);
}

void MemoryDumpPane::clearHighlight()
{
    // Don't to remove BackgroundRole & ForegroundRole from data->itemData(...) map,
    // followed by a data->setItemData(...) call.
    // Even though items were removed from the map and both calls were successful, the tableView would not remove the old highlighting.
    // Explicitly setting the field to QVariant (nothing) reutrns the field to default styling.
    while (!highlightedData.isEmpty()) {
        quint16 address = highlightedData.takeFirst();
        QStandardItem *item = data->item(address/8, address%8 +1);
        item->setData(QVariant(), Qt::BackgroundRole);
        item->setData(QVariant(), Qt::ForegroundRole);
    }
}

void MemoryDumpPane::highlight()
{
    highlightByte(dataSection->getRegisterBankWord(CPURegisters::SP), colors->altTextHighlight, colors->memoryHighlightSP);
    highlightedData.append(dataSection->getRegisterBankWord(CPURegisters::SP));
    quint16 programCounter;
    if(controlSection->getLineNumber() == 0) {
        // When µPC == 0, memoizer has not yet cached CPU's state, so it would return the previous instructions starting PC.
        // However, no µInstructions have yet affected the PC, so use the value of PC from the register bank
        programCounter = dataSection->getRegisterBankWord(CPURegisters::PC);
    }
    else {
        // If the µPC is not 0, then the memoizer has had a chance to cache the CPU's inital program counter value.
        const CPUMemoizer* memoizer = controlSection ->getCPUMemoizer();
        programCounter = memoizer->getRegisterStart(CPURegisters::PC);
    }
    if(!Pep::isUnaryMap[Pep::decodeMnemonic[memorySection->getMemoryByte(programCounter,false)]]) {
        for(int it = 0; it < 3; it++) {
            highlightByte(programCounter+it, colors->altTextHighlight, colors->memoryHighlightPC);
            highlightedData.append(programCounter+it);
        }
    }
    else {
        highlightByte(programCounter, colors->altTextHighlight, colors->memoryHighlightPC);
        highlightedData.append(programCounter);
    }

    for(quint16 byte : lastModifiedBytes) {
        highlightByte(byte, colors->altTextHighlight, colors->memoryHighlightChanged);
        highlightedData.append(byte);
    }

}

void MemoryDumpPane::cacheModifiedBytes()
{
    QSet<quint16> tempConstruct, tempDestruct = memorySection->modifiedBytes();
    modifiedBytes = memorySection->modifiedBytes();
    memorySection->clearModifiedBytes();
    lastModifiedBytes = memorySection->writtenLastCycle();
    for(quint16 val : tempDestruct) {
        if(tempConstruct.contains(val)) continue;
        for(quint16 temp = val - val%8;temp%8<=7;temp++) {
            tempConstruct.insert(temp);
        }
        refreshMemoryLines(val - val%8,val - val%8 + 1);
    }
    /*modifiedBytes.unite(Sim::modifiedBytes);
    if (Sim::tracingTraps) {
        bytesWrittenLastStep.clear();
        bytesWrittenLastStep = Sim::modifiedBytes.toList();
    }
    else if (Sim::trapped) {
        delayLastStepClear = true;
        bytesWrittenLastStep.append(Sim::modifiedBytes.toList());
    }
    else if (delayLastStepClear) {
        delayLastStepClear = false;
    }
    else {
        bytesWrittenLastStep.clear();
        bytesWrittenLastStep = Sim::modifiedBytes.toList();
    }*/
}

void MemoryDumpPane::updateMemory()
{
    QList<quint16> list;
    QSet<quint16> linesToBeUpdated;
    modifiedBytes.unite(memorySection->modifiedBytes());
    memorySection->clearModifiedBytes();
    lastModifiedBytes = memorySection->writtenLastCycle();
    list = modifiedBytes.toList();

    while(!list.isEmpty()) {
        linesToBeUpdated.insert(list.takeFirst() / 8);
    }
    list = linesToBeUpdated.toList();
    qSort(list.begin(), list.end());

    for(auto x: list)
    {
        //Multiply by 8 to convert from line # to address of first byte on a line.
        refreshMemoryLines(x*8, x*8);
    }

    modifiedBytes.clear();
    ui->tableView->resizeColumnsToContents();
}

void MemoryDumpPane::scrollToTop()
{
    ui->tableView->scrollToTop();
}

void MemoryDumpPane::highlightOnFocus()
{
    if (ui->tableView->hasFocus() || ui->scrollToLineEdit->hasFocus()) {
        ui->label->setAutoFillBackground(true);
    }
    else {
        ui->label->setAutoFillBackground(false);
    }
}

bool MemoryDumpPane::hasFocus()
{
    return ui->tableView->hasFocus() || ui->scrollToLineEdit->hasFocus();
}

void MemoryDumpPane::copy()
{
#pragma message("TODO: Copy mechanics on memory pane")
    //ui->textEdit->copy();
}

int MemoryDumpPane::memoryDumpWidth()
{
    quint32 tableSize = lineSize;
    quint32 extraPad = 35;
    return  tableSize + extraPad;
}

void MemoryDumpPane::onFontChanged(QFont font)
{
    ui->tableView->setFont(font);
    ui->scrollToLineEdit->setFont(font);
    ui->tableView->resizeColumnsToContents();
}

void MemoryDumpPane::onDarkModeChanged(bool darkMode)
{
    if(darkMode) colors = &PepColors::darkMode;
    else colors = &PepColors::lightMode;
    // Explicitly rehighlight if in simulation, otherwise old highlighting colors will still be used until the next cycle.
    if(inSimulation) {
        clearHighlight();
        highlight();
    }
}

void MemoryDumpPane::onMemoryChanged(quint16 address, quint8, quint8)
{
    // Do not use address+1 or address-1, as an address at the end of a line
    // would incorrectly trigger a refresh of an adjacent line.
    // Refresh memoryLines(...) will work correctly if both start and end addresses are the same.
    this->refreshMemoryLines(address, address);
}

void MemoryDumpPane::onSimulationStarted()
{
    inSimulation = true;
}

void MemoryDumpPane::onSimulationFinished()
{
    inSimulation = false;
}

void MemoryDumpPane::highlightByte(quint16 memAddr, QColor foreground, QColor background)
{
    // Rows contain 8 bytes of memory.
    // The first column is an address, so the first byte in a row is in column one.
    QModelIndex index = data->index(memAddr/8, memAddr%8 +1);
    // Set style data of item from parameters.
    data->setData(index, foreground ,Qt::ForegroundRole);
    data->setData(index, background, Qt::BackgroundRole);
}

void MemoryDumpPane::mouseReleaseEvent(QMouseEvent *)
{
    ui->tableView->setFocus();
}

void MemoryDumpPane::scrollToByte(quint16 byte)
{
    // Rows contain 8 bytes of memory.
    // The first column is an address, so the first byte in a row is in column one.
    ui->tableView->scrollTo(data->index(byte/8, byte%8 + 1));
}

void MemoryDumpPane::scrollToPC()
{
    ui->scrollToLineEdit->setText(QString("0x") + QString("%1").arg(dataSection->getRegisterBankWord(CPURegisters::PC), 4, 16, QLatin1Char('0')).toUpper());
}

void MemoryDumpPane::scrollToSP()
{
    ui->scrollToLineEdit->setText(QString("0x") + QString("%1").arg(dataSection->getRegisterBankWord(CPURegisters::SP), 4, 16, QLatin1Char('0')).toUpper());
}

void MemoryDumpPane::scrollToAddress(QString string)
{
    bool ok;
    int byte;
    if (string.startsWith("0x", Qt::CaseInsensitive)) {
        byte = string.toInt(&ok, 16);
        if (ok) {
            if (byte > 65535) {
                ui->scrollToLineEdit->setText("0xFFFF");
            } else {
                scrollToByte(byte);
            }
        }
        else {
            ui->scrollToLineEdit->setText("0x");
        }
    }
    else {
        ui->scrollToLineEdit->setText("0x");
    }
}

MemoryDumpDelegate::MemoryDumpDelegate(MemorySection* memorySection, QObject *parent): QStyledItemDelegate(parent),
    memorySection(memorySection), canEdit(true)
{

}

MemoryDumpDelegate::~MemoryDumpDelegate()
{

}

QWidget *MemoryDumpDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // The first and last columns are not user editable, so do not create an editor.
    if(index.column() == 0 || index.column() == 1+8 || !canEdit) return 0;
    // Otherwise, defer to QStyledItemDelegate's implementation, which returns a LineEdit
    QLineEdit *line = qobject_cast<QLineEdit*>(QStyledItemDelegate::createEditor(parent, option, index));
    // Apply a validator, so that a user cannot input anything other than a one byte hexadecimal constant
    line->setValidator(new QRegExpValidator(QRegExp("[a-fA-F0-9][a-fA-F0-9]|[a-fA-F0-9]"), line));
    return line;
}

void MemoryDumpDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // The default value in the line edit should be the text currently in that cell.
    QString value = index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *line = static_cast<QLineEdit*>(editor);
        line->setText(value);
}

void MemoryDumpDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Pass geometry information to the editor.
    editor->setGeometry(option.rect);
}

void MemoryDumpDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Get text from editor and convert it to a integer
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    QString value = line->text();
    bool ok;
    quint64 hexValue = value.toInt(&ok, 16);
    // Use column-1 since the first column is the address
    quint16 addr = index.row()*8 + index.column() - 1;
    // Even though there is a regexp validator in place, validate data again.
    if(ok && hexValue< 1<<16) {
        // Instead of inserting data directly into the item model, notify the MemorySection of a change.
        // The memory section will signal back to the MemoryDump to update the value that changed.
        // This is done to avoid an infinite loop of signals between the memory section and the item model.
        memorySection->setMemoryByte(addr, hexValue);
    }
}


