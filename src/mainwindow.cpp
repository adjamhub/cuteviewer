/*
 * Copyright (C) Andrea Diamantini 2021 <adjam@protonmail.com>
 *
 * CuteViewer project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "mainwindow.h"

#include "application.h"
#include "searchbar.h"
#include "settingsdialog.h"
#include "statusbar.h"

#include <QLinkedList>
#include <QImage>
#include <QPixmap>

#include <QCloseEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>

#include <QDebug>

#include <QPrinter>
#include <QPrintDialog>

#include <QPdfBookmarkModel>
#include <QPdfDocument>
#include <QPdfPageNavigation>
#include <QPdfView>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _view(new QPdfView(this))
    , _document(new QPdfDocument(this))
    , _searchBar(new SearchBar(this))
    , _statusBar(new StatusBar(this))
    , _zoomRange(0)
    , _canBeReloaded(true)
{
    setAttribute(Qt::WA_DeleteOnClose);

    _view->setDocument(_document);
    
    // The UI
    QWidget* w = new QWidget(this);
    auto layout = new QVBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (_view);
    layout->addWidget (_searchBar);
    w->setLayout (layout);
    setCentralWidget(w);

    // let's start with the hidden bar(s)
    _searchBar->setVisible(false);

    connect(_searchBar, &SearchBar::search, this, &MainWindow::search);
    connect(this, &MainWindow::searchMessage, _searchBar, &SearchBar::searchMessage);

    // restore geometry and state
    QSettings s;
    restoreGeometry( s.value( QStringLiteral("geometry") ).toByteArray() );
    restoreState( s.value( QStringLiteral("myWidget/windowState") ).toByteArray() );

    // we need to load settings BEFORE setup actions,
    // to SET initial states
    loadSettings();

    setupActions();

    // application icon and title
    QIcon appIcon = QIcon::fromTheme( QStringLiteral("document-viewer"), QIcon( QStringLiteral(":/icons/document-viewer.svg") ) );
    setWindowIcon(appIcon);

    setCurrentFilePath( QLatin1String("") );

    // take care of the statusbar
    statusBar()->addWidget(_statusBar);

    updateStatusBar();
}


void MainWindow::loadSettings()
{
    // the settings object
    QSettings s;
}


void MainWindow::tile(const QMainWindow *previous)
{
    if (!previous)
        return;

    // TODO: investigate about better positioning
    int topFrameWidth = previous->geometry().top() - previous->pos().y();
    if (!topFrameWidth) {
        topFrameWidth = 40;
    }

    const QPoint pos = previous->pos() + 2 * QPoint(topFrameWidth, topFrameWidth);
    if (screen()->availableGeometry().contains(rect().bottomRight() + pos)) {
        move(pos);
    }
}


void MainWindow::loadFilePath(const QString &path)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    _document->load(path);
    const auto documentTitle = _document->metaData(QPdfDocument::Title).toString();
    setWindowTitle(!documentTitle.isEmpty() ? documentTitle : QStringLiteral("PDF Viewer"));
            
    QGuiApplication::restoreOverrideCursor();

    setCurrentFilePath(path);
    updateStatusBar();
}


void MainWindow::saveFilePath(const QString &path)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    // TODO
    QGuiApplication::restoreOverrideCursor();

    setCurrentFilePath(path);
    updateStatusBar();
}


bool MainWindow::exitAfterSaving()
{
    if (isWindowModified()) {

        int risp = QMessageBox::question(this,
                                         tr("Save Changes"),
                                         tr("The file has unsaved changes"),
                                         QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);

        switch(risp) {

            case QMessageBox::Save:
                // save and exit
                saveFile();
                break;

            case QMessageBox::No:
                // don't save and exit
                break;

            case QMessageBox::Cancel:
                // don't exit
                return false;

            default:
                // this should never happen
                break;
        }
    }
    return true;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (exitAfterSaving()) {
        QSettings s;
        s.setValue( QStringLiteral("geometry") , saveGeometry());
        s.setValue( QStringLiteral("windowState") , saveState());

//        Application::instance()->removeWindowFromList(this);
        event->accept();
        return;
    }
    event->ignore();
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {

        if (_searchBar->isVisible()) {
            _searchBar->hide();
            event->accept();
            return;
        }
    }

    QMainWindow::keyPressEvent(event);
    return;
}


void MainWindow::setupActions()
{
    // ------------------------------------------------------------------------------------------------------------------------
    // Create and set ALL the needed actions

    // file actions -----------------------------------------------------------------------------------------------------------

    // OPEN
    QAction* actionOpen = new QAction( QIcon::fromTheme( QStringLiteral("document-open"), QIcon( QStringLiteral(":/icons/document-open.svg") ) ) , tr("Open"), this);
    actionOpen->setShortcut(QKeySequence::Open);
    connect(actionOpen, &QAction::triggered, this, &MainWindow::openFile);

    // RECENT FILES
    QMenu* menuRecentFiles = new QMenu( tr("Recent Files"), this);
    connect(menuRecentFiles, &QMenu::aboutToShow, this, [=] () {
            QSettings s;
            QStringList recentFiles = s.value( QStringLiteral("recentFiles") ).toStringList();
            if (recentFiles.count() == 0) {
                QAction* voidAction = new QAction( tr("no recent files"), this);
                menuRecentFiles->addAction(voidAction);
                return;
            }
            for (const QString &path : qAsConst(recentFiles)) {
                QAction* recentFileAction = new QAction(path, this);
                menuRecentFiles->addAction(recentFileAction);
                connect(recentFileAction, &QAction::triggered, this, &MainWindow::recentFileTriggered);
            }
        }
    );
    connect(menuRecentFiles, &QMenu::aboutToHide, menuRecentFiles, &QMenu::clear);

    // SAVE
    QAction* actionSave = new QAction( QIcon::fromTheme( QStringLiteral("document-save"), QIcon( QStringLiteral(":/icons/document-save.svg") ) ) , tr("Save"), this);
    actionSave->setShortcut(QKeySequence::Save);
    connect(actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    actionSave->setEnabled(false);

    // SAVE AS
    QAction* actionSaveAs = new QAction( QIcon::fromTheme( QStringLiteral("document-save-as"), QIcon( QStringLiteral(":/icons/document-save-as.svg") ) ) , tr("Save As"), this);
    connect(actionSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);

    // PRINT
    QAction* actionPrint = new QAction( QIcon::fromTheme( QStringLiteral("document-print"), QIcon( QStringLiteral(":/icons/document-print.svg") ) ) , tr("Print"), this);
    actionPrint->setShortcut(QKeySequence::Print);
    connect(actionPrint, &QAction::triggered, this, &MainWindow::printFile);

    // CLOSE
    QAction* actionClose = new QAction( QIcon::fromTheme( QStringLiteral("document-close"), QIcon( QStringLiteral(":/icons/document-close.svg") ) ) , tr("Close"), this);
    actionClose->setShortcut(QKeySequence::Close);
    connect(actionClose, &QAction::triggered, this, &MainWindow::close);

    // QUIT
    QAction* actionQuit = new QAction( QIcon::fromTheme( QStringLiteral("application-exit"), QIcon( QStringLiteral(":/icons/application-exit.svg") ) ) , tr("Exit"), this );
    actionQuit->setShortcut(QKeySequence::Quit);
    connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit, Qt::QueuedConnection);

    // view actions -----------------------------------------------------------------------------------------------------------
    // ZOOM IN
    QAction* actionZoomIn = new QAction( QIcon::fromTheme( QStringLiteral("zoom-in"), QIcon( QStringLiteral(":/icons/zoom-in.svg") ) ) , tr("Zoom In"), this );
    actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn );

    // ZOOM OUT
    QAction* actionZoomOut = new QAction( QIcon::fromTheme( QStringLiteral("zoom-out") , QIcon( QStringLiteral(":/icons/zoom-out.svg") ) ) , tr("Zoom Out"), this );
    actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(actionZoomOut, &QAction::triggered, this, &MainWindow::onZoomOut );

    // ZOOM ORIGINAL
    QAction* actionZoomOriginal = new QAction( QIcon::fromTheme( QStringLiteral("zoom-original") , QIcon( QStringLiteral(":/icons/zoom-original.svg") ) ) , tr("Zoom Original"), this );
    actionZoomOriginal->setShortcut(Qt::CTRL + Qt::Key_0);
    connect(actionZoomOriginal, &QAction::triggered, this, &MainWindow::onZoomOriginal );

    // FULL SCREEN
    QAction* actionFullScreen = new QAction( QIcon::fromTheme( QStringLiteral("view-fullscreen") , QIcon( QStringLiteral(":/icons/view-fullscreen.svg") ) ) , tr("FullScreen"), this );
    actionFullScreen->setShortcuts(QKeySequence::FullScreen);
    actionFullScreen->setCheckable(true);
    connect(actionFullScreen, &QAction::triggered, this, &MainWindow::onFullscreen );

    // find actions -----------------------------------------------------------------------------------------------------------
    // FIND
    QAction* actionFind = new QAction( QIcon::fromTheme( QStringLiteral("edit-find") , QIcon( QStringLiteral(":/icons/edit-find.svg") ) ) , tr("Find"), this );
    actionFind->setShortcut(QKeySequence::Find);
    connect(actionFind, &QAction::triggered, this, &MainWindow::showSearchBar );

    // option actions ----------------------------------------------------------------------------------------------------------- 
    // SETTINGS
    QAction* actionShowSettings = new QAction( QIcon::fromTheme( QStringLiteral("configure"), QIcon( QStringLiteral(":/icons/configure.svg") ) ) , tr("Settings"), this);
    connect(actionShowSettings, &QAction::triggered, this, &MainWindow::showSettings);

    // about actions -----------------------------------------------------------------------------------------------------------    
    // ABOUT Qt
    QAction* actionAboutQt = new QAction( QIcon::fromTheme( QStringLiteral("qt") , QIcon( QStringLiteral(":/icons/qt.svg") ) ) , tr("About Qt"), this );
    connect(actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // ABOUT
    QAction* actionAboutApp = new QAction( QIcon::fromTheme( QStringLiteral("help-about") , QIcon( QStringLiteral(":/icons/help-about.svg") ) ) , tr("About"), this );
    connect(actionAboutApp, &QAction::triggered, this, &MainWindow::about);

    // ------------------------------------------------------------------------------------------------------------------------
    // Create and set the MENUBAR

    QMenu* fileMenu = menuBar()->addMenu( tr("&File") );
    fileMenu->addAction(actionOpen);
    fileMenu->addMenu(menuRecentFiles);
    fileMenu->addAction(actionSave);
    fileMenu->addAction(actionSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(actionPrint);
    fileMenu->addSeparator();
    fileMenu->addAction(actionClose);
    fileMenu->addAction(actionQuit);

    QMenu* viewMenu = menuBar()->addMenu( tr("&View") );
    viewMenu->addAction(actionZoomIn);
    viewMenu->addAction(actionZoomOut);
    viewMenu->addAction(actionZoomOriginal);
    viewMenu->addSeparator();
    viewMenu->addAction(actionFullScreen);

    QMenu* searchMenu = menuBar()->addMenu( tr("&Search") );
    searchMenu->addAction(actionFind);

    QMenu* optionsMenu = menuBar()->addMenu( tr("&Options") );
    optionsMenu->addAction(actionShowSettings);

    QMenu* helpMenu = menuBar()->addMenu( tr("&Help") );
    helpMenu->addAction(actionAboutQt);
    helpMenu->addAction(actionAboutApp);

    // ------------------------------------------------------------------------------------------------------------------------
    // Create and set the MAIN TOOLBAR

    QToolBar* mainToolbar = addToolBar( QStringLiteral("Main Toolbar") );
    mainToolbar->setObjectName( QStringLiteral("Main Toolbar") );

    mainToolbar->addAction(actionOpen);
    mainToolbar->addAction(actionSave);
    mainToolbar->addSeparator();
    mainToolbar->addAction(actionPrint);
    mainToolbar->addSeparator();

    connect(actionFullScreen, &QAction::triggered, this, [=](bool on) {
            if (on) {
                mainToolbar->addAction(actionFullScreen);
                actionFullScreen->setText( tr("Exit FullScreen") );
            } else {
                mainToolbar->removeAction(actionFullScreen);
                actionFullScreen->setText( tr("FullScreen") );
            }
        }
    );

    // toolbar style and (position) lock
    mainToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mainToolbar->setMovable(false);
}


void MainWindow::setCurrentFilePath(const QString& path)
{
    QString curFile;
    if (path.isEmpty()) {
        curFile = tr("untitled");
        _filePath = QLatin1String("");
    } else {
        curFile = QFileInfo(path).canonicalFilePath();
        _filePath = path;
        addPathToRecentFiles(_filePath);
//        Application::instance()->addWatchedPath(_filePath);
    }

    setWindowModified(false);

    setWindowFilePath(curFile);
}


void MainWindow::newWindow()
{
//    Application::instance()->loadPath( QLatin1String("") );
}


void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this);
    if (path.isEmpty())
        return;
    
    if (_filePath.isEmpty() && !isWindowModified()) {
        loadFilePath(path);
        return;
    }

//    Application::instance()->loadPath(path);
}


void MainWindow::saveFile()
{
    if (_filePath.isEmpty()) {
        saveFileAs();
        return;
    }

    saveFilePath(_filePath);
}


void MainWindow::saveFileAs()
{
    // needed to catch document dir location (and it has to be writable, obviously...)
    QString documentDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString path = QFileDialog::getSaveFileName(this, tr("Save File"), documentDir);
    if (path.isEmpty())
        return;

    // try to add .txt extension in the end
    QFileInfo info(path);
    if (info.fileName() == info.baseName()) {
        path += QLatin1String(".txt");
    }

    saveFilePath(path);
}


void MainWindow::printFile()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
//        _textEdit->print(&printer);
    }
}


void MainWindow::onZoomIn()
{
    _zoomRange++;
    updateStatusBar();
}


void MainWindow::onZoomOut()
{
    _zoomRange--;
    updateStatusBar();
}


void MainWindow::onZoomOriginal()
{
    _zoomRange = 0;
    updateStatusBar();
}


void MainWindow::onFullscreen(bool on)
{
    if (on) {
        showFullScreen();
        menuBar()->hide();
    } else {
        showNormal();
        menuBar()->show();
    }
}


void MainWindow::about()
{
    QString version = qApp->applicationVersion();

    QString aboutText;
    aboutText += QStringLiteral("<h1>Cuteviewer ") + version + QStringLiteral("</h1>");
    aboutText += QStringLiteral("<p>");
    aboutText += QStringLiteral("The Qt viewer ;)<br>Just an easy PDF docuemnt viewer, based on Qt libraries");
    aboutText += QStringLiteral("</p><p>");
    aboutText += QStringLiteral("(c) 2021 <a href='mailto:adjam@protonmail.com'>Andrea Diamantini</a> (adjam)");
    aboutText += QStringLiteral("</p>");
    aboutText += QStringLiteral("<a href='https://github.com/adjamhub/cuteviewer'>https://github.com/adjamhub/cuteviewer</a>");
    aboutText += QStringLiteral("<br>");

    QMessageBox::about(this, QStringLiteral("About cuteviewer"), aboutText);
}


void MainWindow::updateStatusBar()
{
}


void MainWindow::showSettings()
{
    SettingsDialog* dialog = new SettingsDialog(this);
    dialog->exec();
    dialog->deleteLater();

//    Application::instance()->loadSettings();
}


void MainWindow::showSearchBar()
{
    if (_searchBar->isVisible()) {
        _searchBar->hide();
        return;
    }

    _searchBar->show();

    _searchBar->setFocus();
}


void MainWindow::search(const QString & search, bool forward, bool casesensitive)
{
}


void MainWindow::addPathToRecentFiles(const QString& path)
{
    QSettings s;
    QStringList recentFiles = s.value( QStringLiteral("recentFiles") ).toStringList();
    recentFiles.removeOne(path);
    recentFiles.prepend(path);
    if (recentFiles.count() > 10) {
        recentFiles.removeLast();
    }
    s.setValue( QStringLiteral("recentFiles") , recentFiles);
}


void MainWindow::recentFileTriggered()
{
    QAction* a = qobject_cast<QAction* >(sender());
    QString path = a->text();
    if (_filePath.isEmpty()) {
        loadFilePath(path);
        return;
    }
//    Application::instance()->loadPath(path);
}
