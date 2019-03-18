#To change the output file name, or adjust what files are included in the ouput, see the "XML Tester Defs.pri" file

#Clean path
defineReplace(cpl){
    #Adjust the input path so that the correct slashes are used for the host shell $$psc OS
    return($$system_path($$1))
}
#Clean path with force quote
defineReplace(cpq){
    return(\"$$cpl($$1)\")
}
#Platform Specific Command Seperator
win32{
    psc="&"
    TARGET_EXT=".exe"
}
macx{
    psc=";"
    TARGET_EXT=".app"
}


#Prevent Windows from trying to parse the project three times per build.
#This interferes with the deployment script, and makes debugging hard since Qt attempts to debug the optimized program.
CONFIG -= debug_and_release \
    debug_and_release_target

QtDir = $$clean_path($$[QT_INSTALL_LIBS]/..)
QtInstallerBin=$$clean_path($$QtDir/../../tools/Qtinstallerframework/3.0/bin)
EXECUTE_QTIFW=""
repoDir=""
PLATFORM_ICONS=""
PLATFORM_DATA=""
INSTALLER_CONFIG_FILE=""

!CONFIG(debug,debug|release):macx:!isEmpty(MAC_USE_DMG_INSTALLER){
    #For some reason, the release flag is set in both debug and release.
    #So, the above Config(...) makes it so a disk image is only built in release mode.

    #Create necessary directory structure for disk image.
    QMAKE_POST_LINK += $${QMAKE_MKDIR} $$cpq($$OUT_PWD/Installer/)$$psc
    #Copy over the executable and bundle it with its dependencies
    QMAKE_POST_LINK += $${QMAKE_COPY_DIR} $$cpq($$OUT_PWD/$$TARGET".app") $$cpq($$OUT_PWD/Installer/)$$psc
    QMAKE_POST_LINK += $$cpq($$QtDir/bin/macdeployqt) $$cpq($$OUT_PWD/Installer/$$TARGET".app")$$psc
    #Use HDIUtil to make a folder into a read/write image
    QMAKE_POST_LINK += hdiutil create -volname $$TARGET -srcfolder $$cpq($$OUT_PWD/Installer) -attach -ov -format UDRW $$TARGET"Temp.dmg"$$psc
    #Link from the read/write image to the machine's Applications folder
    QMAKE_POST_LINK += ln -s /Applications /Volumes/$$TARGET/Applications$$psc
    #Write all data files to image
    for(name,UNIVERSAL_DATA){
        QMAKE_POST_LINK += $${QMAKE_COPY} $$cpq($$PATH_PREFIX/$$name) /Volumes/$$TARGET $$psc
    }
    for(name,MAC_DATA){
        QMAKE_POST_LINK += $${QMAKE_COPY} $$cpq($$PATH_PREFIX/$$name) /Volumes/$$TARGET $$psc
    }
    #Unmount the image, and create a new compressed, readonly image.
    QMAKE_POST_LINK += hdiutil detach /Volumes/$$TARGET$$psc
    QMAKE_POST_LINK += $${QMAKE_COPY} $$cpq($$OUT_PWD/$$TARGET"Temp".dmg) $$cpq($$OUT_PWD/$$TARGET"Temp2".dmg)$$psc
    QMAKE_POST_LINK += hdiutil convert -format UDBZ -o $$cpq($$OUT_PWD/$$OUTPUT_INSTALLER_NAME".dmg") $$cpq($$OUT_PWD/$$TARGET"Temp2".dmg)$$psc
    #Remove the temporary read/write image.
    QMAKE_POST_LINK += $${QMAKE_DEL_FILE} $$cpq($$OUT_PWD/$$TARGET"Temp".dmg)$$psc
    QMAKE_POST_LINK += $${QMAKE_DEL_FILE} $$cpq($$OUT_PWD/$$TARGET"Temp2".dmg)$$psc
    #If QMAKE_POST_LINK stops working in a future version, QMAKE provides another way to add custom targets.
    #Use the method described in "Adding Custom Targets" on http://doc.qt.io/qt-5/qmake-advanced-usage.html.
    #Our deployment tool will be called anytime the application is sucessfully linked in release mode.
}
else:!CONFIG(debug,debug|release):macx:isEmpty(MAC_USE_DMG_INSTALLER) {
    !exists($$QtInstallerBin/Repogen){ #No tool to create new binary, so quit
        warning("Aborting installer creations, since QT Installer Framework 3.0 is not installed.")
        warning("Please run the QT maintence tool and install QT Installer Framework 3.0.")
    }
    else{
        repoDir=$$cpq($$OUT_PWD/Repository/macx)
        EXECUTE_QTIFW="true"
        PLATFORM_DATA=MAC_DATA
        PLATFORM_ICONS=MAC_ICONS
        INSTALLER_CONFIG_FILE=$$cpq($$PWD/../installer/packages/$$TARGET/configmacx.xml)
        DEPLOY_ARGS = ""
    }
}

#Otherwise if the target is windows, but no installer framework exists
else:!CONFIG(debug,debug|release):win32:!exists($$QtInstallerBin/repogen.exe){
    warning("Aborting installer creations, since QT Installer Framework 3.0 is not installed.")
    warning("Please run the QT maintence tool and install QT Installer Framework 3.0.")
}
    #Otherwise build the installer for windows as normal.
else:!CONFIG(debug,debug|release):win32{
    #Directory where the repogen tool will put its output
    repoDir=$$cpq($$OUT_PWD/Repository/win32)
    EXECUTE_QTIFW="true"
    PLATFORM_DATA = WINDOWS_DATA
    PLATFORM_ICONS = WINDOWS_ICONS
    INSTALLER_CONFIG_FILE = $$cpq($$PWD/../installer/packages/$$TARGET/configwin32.xml)
    DEPLOY_ARGS = "--no-translations --no-system-d3d-compiler"
}

#Since there is no native QT deploy tool for Linux, one must be added in the project configuration
#This condition is to make sure that a tool was provided as an argument to qmake
else:linux:isEmpty(LINUX_DEPLOY){
    warning("Attempting a Linux build, but no path to the build tool was provided")
}

#Then linuxdeployqt is available, and it should be used to make a working installer for linux.
else:linux{
    message("This is where the linux build code will go")
}

#Now that all confiugration flags have been set, execute
!isEmpty(EXECUTE_QTIFW){
    #Create installer directory structure
    #These will be ignored if the target already exists
    QMAKE_POST_LINK += $${QMAKE_MKDIR} $$cpq($$OUT_PWD/Installer) $$psc \
        $${QMAKE_MKDIR} $$cpq($$OUT_PWD/Installer/packages) $$psc \
        $${QMAKE_MKDIR} $$cpq($$OUT_PWD/Installer/packages/$$TARGET) $$psc \
        $${QMAKE_MKDIR} $$cpq($$OUT_PWD/Installer/packages/$$TARGET/meta) $$psc \
        $${QMAKE_MKDIR} $$cpq($$OUT_PWD/Installer/packages/$$TARGET/data) $$psc \
        $${QMAKE_MKDIR} $$cpq($$OUT_PWD/Installer/config) $$psc

    #Create a directory for update information
    !exists($$repoDir){
        QMAKE_POST_LINK += $${QMAKE_MKDIR} $$cpq($$repoDir) $$psc
    }

    #Copy over files needed to create installer
    QMAKE_POST_LINK += $${QMAKE_COPY} $$INSTALLER_CONFIG_FILE $$cpq($$OUT_PWD/Installer/config/config.xml) $$psc \ #Copy Platform dependant config file
        $${QMAKE_COPY} $$cpq($$PWD/../installer/common/control.js) $$cpq($$OUT_PWD/Installer/config) $$psc #Copy over installer control script

    for(PACKAGE, TARGET_PACKAGES) {
        #For each target package, copy it over into the installer
        NAME = $$eval($$PACKAGE"."NAME)
        meta_items = $$eval($$PACKAGE"."META_ITEMS)
        data_items = $$eval($$PACKAGE"."DATA_ITEMS)
        for(ITEM, meta_items) {
            QMAKE_POST_LINK += $${QMAKE_COPY} $$cpq($$ITEM) $$cpq($$OUT_PWD/Installer/packages/$$NAME/meta/) $$psc
        }
        for(ITEM, data_items) {
            QMAKE_POST_LINK += $${QMAKE_COPY} $$cpq($$ITEM) $$cpq($$OUT_PWD/Installer/packages/$$NAME/data/) $$psc
        }
    }


    #Copy over needed icons as set in defs file
    for(name,UNIVERSAL_ICONS){
        QMAKE_POST_LINK += $${QMAKE_COPY} $$cpq($$PATH_PREFIX/$$name) $$cpq($$OUT_PWD/Installer/config) $$psc
    }

    win32:{
        for(name, WINDOWS_ICONS){
            QMAKE_POST_LINK += $${QMAKE_COPY} $$cpq($$PATH_PREFIX/$$name) $$cpq($$OUT_PWD/Installer/config) $$psc
        }
        #Copy over executable to data directory
        QMAKE_POST_LINK +=  $${QMAKE_COPY} $$cpq($$OUT_PWD/$$TARGET$$TARGET_EXT) $$cpq($$OUT_PWD/Installer/packages/$$TARGET/data) $$psc
        #Execute windeployqt to copy needed binaries (dlls, etc).
        #See documentation here:
        #http://doc.qt.io/qt-5/windows-deployment.html
        QMAKE_POST_LINK += $$cpq($$QtDir/bin/windeployqt) $$DEPLOY_ARGS $$cpq($$OUT_PWD/Installer/packages/$$TARGET/data/$$TARGET$$TARGET_EXT) $$psc
    }

    else:macx{
        for(name, MAC_ICONS){
            QMAKE_POST_LINK += $${QMAKE_COPY} $$cpq($$PATH_PREFIX/$$name) $$cpq($$OUT_PWD/Installer/config) $$psc
        }
        #Copy over executable to data directory
        QMAKE_POST_LINK +=  $${QMAKE_COPY_DIR} $$cpq($$OUT_PWD/$$TARGET$$TARGET_EXT) $$cpq($$OUT_PWD/Installer/packages/$$TARGET/data) $$psc
        #Execute macdeployqt to copy needed binaries (dlls, etc).
        #See documentation here:
        #http://doc.qt.io/qt-5/windows-deployment.html
        QMAKE_POST_LINK += $$cpq($$QtDir/bin/macdeployqt) $$DEPLOY_ARGS $$cpq($$OUT_PWD/Installer/packages/$$TARGET/data/$$TARGET$$TARGET_EXT) $$psc
    }


    #The following two lines invoke QT Installer Framework executables. See the following link
    #for documentation on what the different comman line flags do.
    #http://doc.qt.io/qtinstallerframework/ifw-tools.html

    #Execute repository creator
    QMAKE_POST_LINK += $$cpq($$QtInstallerBin/repogen) --update-new-components -p $$cpq($$OUT_PWD/Installer/packages) $$repoDir $$psc

    #Create installer using the qt binary creator
    QMAKE_POST_LINK += $$cpq($$QtInstallerBin/binarycreator) -c $$cpq($$OUT_PWD/Installer/config/config.xml) -p $$cpq($$OUT_PWD/Installer/packages) \
    $$cpq($$OUT_PWD/Installer/$$OUTPUT_INSTALLER_NAME) $$psc
}

DISTFILES += \
    $$PWD/config/control.js \
    $$PWD/config/configlinux.xml \
    $$PWD/config/configwin32.xml \
    $$PWD/config/configmacx.xml \
    $$PWD/../installer/common/control.js \
    $$PWD/../installer/common/License.txt \
    $$PWD/../installer/common/regSetUninst.bat

FORMS += \
    $$PWD/../installer/common/ShortcutPage.ui \
    $$PWD/../installer/common/UserPage.ui
