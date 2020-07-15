// File: cpubuildhelper.cpp
/*
    Pep9Term is a  command line tool utility for assembling Pep/9 programs to
    object code and executing object code programs.

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
#include "cpubuildhelper.h"

#include <iostream>

#include "microassembler/microasm.h"
#include "microassembler/microcode.h"
#include "microassembler/microcodeprogram.h"
#include "symbol/symbolentry.h"
#include "symbol/symboltable.h"

#include "termhelper.h"

CPUBuildHelper::CPUBuildHelper(Enu::CPUType type,bool useExtendedFeatures,
                               const QString source, QFileInfo source_file_info,
                               QObject *parent):
    QObject(parent), QRunnable(), type(type), useExtendedFeatures(useExtendedFeatures),
    source(source)
{
    // Default error log name to the base name of the file with an _errLog.txt extension.
    this->error_log = source_file_info.absoluteDir().absoluteFilePath(source_file_info.baseName() + "_errLog.txt");
}

CPUBuildHelper::~CPUBuildHelper()
{
    // All of our memory is owned by sharedpointers, so we
    // should not attempt to delete anything ourselves.
}

bool CPUBuildHelper::buildMicroprogram()
{
    // Construct files that will be needed for assembly
    QFile errorLog(error_log.absoluteFilePath());

    auto result = buildMicroprogramHelper(type, useExtendedFeatures,
                                          source);
    // If there were errors, attempt to write all of them to the error file.
    // If the error file can't be opened, log that failure to standard output.
    if(!result.elist.isEmpty()) {
        if(!errorLog.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qDebug().noquote() << errLogOpenErr.arg(errorLog.fileName());
        }
        else {
            // Write all errors to the error log.
            QTextStream errAsStream(&errorLog);
            auto textList = source.split("\n");
            for(auto errorPair : result.elist) {
                // The first element of the error pair is the line number which
                // caused the error, allowing us to write the offending line
                // and error message to the console.
                errAsStream << textList[errorPair.first] << errorPair.second << "\n";
            }
            // Error log should be flushed automatically.
            errorLog.close();
        }
    }

    // Indicate to console if assembly was successful
    if(result.success) {
        // Program assembly can succeed despite the presence of errors in the
        // case of trace tag warnings. Must gaurd against this.
        if(result.elist.isEmpty()) {
            qDebug() << "Program assembled successfully.";
        }
        else {
            qDebug() << "Program assembled with warning(s).";
        }
    }
    else {
        qDebug() << "Error(s) generated. See error log.";
    }
    return result.success;
}

void CPUBuildHelper::run()
{
    // All set up work is done in build program, so all run needs to do is attempt
    if(buildMicroprogram()) {
       // Placeholder for potential work needing to be done after successful assembly.
    }

    // Application will live forever if we don't signal it to die.
    emit finished();
}

void CPUBuildHelper::set_error_file(QString error_file)
{
    this->error_log = error_file;
}

MicrocodeAssemblyResult buildMicroprogramHelper(Enu::CPUType type,
                                                bool useExtendedFeatures,
                                                const QString source)
{
    MicrocodeAssemblyResult result;
    // Returns true if object code is successfully generated (i.e. program is non-null).
    result.success = true;

    MicroAsm assembler(type, useExtendedFeatures);


    auto tempResult = assembler.assembleProgram(source);
    result.elist = tempResult.errorMessages;
    result.program = tempResult.program;
    result.success = tempResult.success;

    return result;
}
