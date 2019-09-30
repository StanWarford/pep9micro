#include "cpurunhelper.h"
#include "termhelper.h"
#include "amemorydevice.h"
#include "symboltable.h"
#include "symbolentry.h"
#include "memorychips.h"
#include "amemorychip.h"
#include "mainmemory.h"
#include "partialmicrocodedcpu.h"
#include "cpubuildhelper.h"
#include "microcode.h"
#include "cpudata.h"
#include "partialmicrocodedcpu.h"
#include "termhelper.h"
#include "amemorydevice.h"
CPURunHelper::~CPURunHelper()
{
    // All of our memory is owned by sharedpointers, so we should not attempt
    // to delete anything ourselves.
    if(outputFile != nullptr) {
        outputFile->flush();
        // It might seem like we should close the file here, but it causes read / write violations to do so.
        // Instead, delete it later under the assumption that the operating system will handle that for us.
        // Schedule the output file for deletion via the event loop.
        outputFile->deleteLater();
    }
}

void CPURunHelper::onSimulationFinished()
{
    // There migh be outstanding IO events. Give them a chance to finish
    // before initiating shutdown.
    QCoreApplication::processEvents();
    emit finished();
}

void CPURunHelper::runProgram()
{

    // Construct files that will be needed for assembly
    QFile errorLog(QFileInfo(microcodeProgramFile).absoluteDir().absoluteFilePath(
                       QFileInfo(microcodeProgramFile).baseName() + "_errLog.txt"));

    QVector<AMicroCode*> preconditionLines;
    auto programResult = buildMicroprogramHelper(type, false,
                                          microcodeProgram);
    BuildResult preconditionResult;
    // If there were errors, attempt to write all of them to the error file.
    // If the error file can't be opened, log that failure to standard output.
    if(!programResult.elist.isEmpty()) {
        if(!errorLog.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qDebug().noquote() << errLogOpenErr.arg(errorLog.fileName());
        }
        else {
            QTextStream errAsStream(&errorLog);
            auto textList = microcodeProgram.split("\n");
            for(auto errorPair : programResult.elist) {
                errAsStream << textList[errorPair.first] << errorPair.second << endl;
            }
            // Error log should be flushed automatically.
            errorLog.close();
        }
    }
    if(programResult.success && !programResult.program.isNull()) {
        // Program assembly can succeed despite the presence of errors in the
        // case of trace tag warnings. Must gaurd against this.
        if(programResult.elist.isEmpty()) {
            qDebug() << "Program assembled successfully.";
            cpu->setMicrocodeProgram(programResult.program);
        }
    }
    else {
        qDebug() << "Error(s) generated in microcode input. See error log.";
        emit finished();
        return;
    }

    if(!preconditionsProgram.isEmpty()) {
        preconditionResult = buildMicroprogramHelper(type, false,
                                              preconditionsProgram);
        // If there were errors, attempt to write all of them to the error file.
        // If the error file can't be opened, log that failure to standard output.
        if(!preconditionResult.elist.isEmpty()) {
            if(!errorLog.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                qDebug().noquote() << errLogOpenErr.arg(errorLog.fileName());
            }
            else {
                QTextStream errAsStream(&errorLog);
                auto textList = microcodeProgram.split("\n");
                for(auto errorPair : preconditionResult.elist) {
                    errAsStream << textList[errorPair.first] << errorPair.second << endl;
                }
                // Error log should be flushed automatically.
                errorLog.close();
            }
        }

        // Only open & write object code file if assembly was successful.
        if(preconditionResult.success) {
            // Program assembly can succeed despite the presence of errors in the
            // case of trace tag warnings. Must gaurd against this.
            if(preconditionResult.elist.isEmpty()) {
                qDebug() << "Preconditions assembled successfully.";
                preconditionLines = preconditionResult.program->getObjectCode();
            }
        }
        else {
            qDebug() << "Error(s) generated in precondition input. See error log.";
            emit finished();
            return;
        }
    }
    else {
        preconditionLines = programResult.program->getObjectCode();
    }

    // Having selected preconditons, apply them.
    CPUDataSection* data = cpu->getDataSection().get();
    AMemoryDevice* memory = this->memory.get();
    for(auto line : preconditionLines) {
        if(line->hasUnitPre()) {
            static_cast<UnitPreCode*>(line)->setUnitPre(data, memory);
        }
    }

    // Open up program output file if possible.
    // If output can't be opened up, abort.
    QFile *output = new QFile(programOutput.absoluteFilePath());
    if(!output->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug().noquote() << errLogOpenErr.arg(output->fileName());
        throw std::logic_error("Can't open output file.");
    } else {
        // If it could be opened, map charOut to the file.
        outputFile = output;
    }

    // Make sure to set up any last minute flags needed by CPU to perform simulation.
    cpu->onSimulationStarted();
    bool passed = true;
    QString errorString;

    if(!cpu->onRun()) {
        qDebug().noquote() << "The CPU failed for the following reason: "<<cpu->getErrorMessage();
        QTextStream (&*outputFile) << "[[" << cpu->getErrorMessage() << "]]";
    }
    else {
        CPUDataSection* data = cpu->getDataSection().get();
        AMemoryDevice* memory = this->memory.get();
        for (AMicroCode* x : preconditionLines) {
            if(x->hasUnitPost()) {
                UnitPostCode* code = dynamic_cast<UnitPostCode*>(x);
                if(!code->testPostcondition(data, memory, errorString)) {
                    qDebug().noquote() << errorString;
                    QTextStream (&*outputFile) << errorString;
                    passed = false;

                }
             }
        }
        if(passed) {
            QTextStream (&*outputFile) << "success";
            qDebug() << "Passed unit tests.";
        }

    }

}

CPURunHelper::CPURunHelper(Enu::CPUType type, const QString microcodeProgram,
                           QFileInfo microcodeProgramFile,
                           const QString preconditionsProgram,
                           QFileInfo programOutput, QObject *parent) :
    QObject(parent), QRunnable(), type(type), microcodeProgram(microcodeProgram),
    microcodeProgramFile(microcodeProgramFile),
    preconditionsProgram(preconditionsProgram), programOutput(programOutput),
    // Explicitly initialize both simulation objects to nullptr,
    // so that it is clear to that neither object has been allocated
    memory(nullptr), cpu(nullptr), outputFile(nullptr)

{


}

void CPURunHelper::run()
{

    // Construct all needed simulation objects in run, so that the owning
    // thread is the one doing the computation, not the main thread.
    if(memory.isNull()) {
        // Assume memory will always be 64k.
        memory = QSharedPointer<MainMemory>::create(nullptr);
        QSharedPointer<RAMChip> ramChip(new RAMChip(1<<16, 0, memory.get()));
        memory->insertChip(ramChip, 0);

        cpu = QSharedPointer<PartialMicrocodedCPU>::create(type, memory, nullptr);
    }

    // Clear & initialize all values in CPU before starting simulation.
    cpu->reset();
    cpu->initCPU();

    // Instead of directly allowing run() to kill itself, uses events to "schedule"
    // shutting down the application. This should ensure all IO completes. We were
    // having an error where closing IO streams directly after simulation completion would
    // cause a race condition with IO pending for the file. The overhead of the simulation events
    // seems to "serialize" writes / closing.
    connect(cpu.get(), &PartialMicrocodedCPU::simulationFinished,
            this, &CPURunHelper::onSimulationFinished);
    runProgram();

    // Make sure any outstanding events are handled.
    QCoreApplication::processEvents();
}