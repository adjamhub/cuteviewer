/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef STATUS_BAR_H
#define STATUS_BAR_H


#include <QWidget>

class QLabel;


class StatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);
    
    void setLanguage(const QString& lang);
    void setPosition(int row, int col);
    void setCodec(const QString& codec);
    void setZoom(const QString& zoom);

private:
    QLabel* _langLabel;
    QLabel* _posLabel;
    QLabel* _codecLabel;
    QLabel* _zoomLabel;
};

#endif
