/*
 * Copyright (C) Andrea Diamantini 2021 <adjam@protonmail.com>
 *
 * CuteViewer project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

class QCloseEvent;
class QKeyEvent;

class QPdfDocument;
class QPdfView;

class SearchBar;
class StatusBar;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    inline QString filePath() const { return _filePath; }

    void loadSettings();

    // needed to position next windows
    void tile(const QMainWindow *previous);

    // public functions to load and save the actual file
    // from the outside
    void loadFilePath(const QString &path);
    void saveFilePath(const QString &path);
    
    // ask user to save or not, eventually blocking exit action
    // returns true if window has to be closed, false otherwise
    bool exitAfterSaving();


protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupActions();

    void setCurrentFilePath(const QString& path);
    void addPathToRecentFiles(const QString& path);

private Q_SLOTS:
    void newWindow();
    void openFile();
    void saveFile();
    void saveFileAs();
    void printFile();

    void onZoomIn();
    void onZoomOut();
    void onZoomOriginal();
    void onFullscreen(bool on);

    void showSettings();

    void about();

    void updateStatusBar();

    void showSearchBar();

    void search(const QString & search,
                bool forward = true,
                bool casesensitive = false);

    void recentFileTriggered();

Q_SIGNALS:
    void searchMessage(const QString &);

private:
    QPdfView* _view;
    QPdfDocument* _document;
    
    SearchBar* _searchBar;
    StatusBar* _statusBar;

    QString _filePath;
    int _zoomRange;
    bool _canBeReloaded;
};

#endif // MAINWINDOW_H
