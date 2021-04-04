/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "settingsdialog.h"
#include "ui_settings.h"

#include <QColorDialog>
#include <QDebug>
#include <QFontDialog>
#include <QMessageBox>
#include <QSettings>


SettingsDialog::SettingsDialog(QWidget *parent) 
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle( tr("Cutepad Settings") );

    ui->spacesSpinBox->setRange(1,12);
        
    connect(ui->lineColorButton, &QPushButton::clicked, this, &SettingsDialog::chooseHighlightColor);
    connect(ui->fontButton, &QPushButton::clicked, this, &SettingsDialog::chooseFont);
    connect(ui->resetButton, &QPushButton::clicked, this, &SettingsDialog::resetSettings);

    loadSettings();

    connect(ui->lineNumbersComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::saveSettings);
    connect(ui->highlightCurrentLineCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::saveSettings);

    connect(ui->replaceTabsWithSpacesCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::saveSettings);
    connect(ui->spacesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::saveSettings);
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::loadSettings()
{
    // the settings object
    QSettings s;

    int lineNumbersMode = s.value( QStringLiteral("LineNumbers") , 0).toInt();
    ui->lineNumbersComboBox->setCurrentIndex(lineNumbersMode);

    bool highlight = s.value( QStringLiteral("CurrentLineHighlight") , false).toBool();
    ui->highlightCurrentLineCheckBox->setChecked(highlight);

    QColor highlightLineColor = s.value( QStringLiteral("HighlightLineColor") , QColor(Qt::yellow).lighter(160)).value<QColor>();
    QPalette p = ui->lineColorButton->palette();
    p.setColor(QPalette::Button, highlightLineColor);
    ui->lineColorButton->setPalette(p);
    ui->lineColorButton->setEnabled(highlight);

    bool tabReplace = s.value( QStringLiteral("TabReplace"), false).toBool();
    ui->replaceTabsWithSpacesCheckBox->setChecked(tabReplace);

    int tabsCount = s.value( QStringLiteral("TabsCount"), 4).toInt();
    ui->spacesSpinBox->setValue(tabsCount);
    
    // font
    QString fontFamily = s.value( QStringLiteral("fontFamily") , QStringLiteral("Monospace") ).toString();
    int fontSize = s.value( QStringLiteral("fontSize") , 12).toInt();
    int fontWeight = s.value( QStringLiteral("fontWeight"), 50).toInt();
    bool italic = s.value( QStringLiteral("fontItalic"), false).toBool();
    QFont font(fontFamily,fontSize, fontWeight);
    font.setItalic(italic);
    QString fontName = fontFamily + QLatin1String(", ") + QString::number(fontSize) + QLatin1String("pt");
    ui->fontLabel->setText(fontName);
    ui->fontLabel->setFont(font);
}


void SettingsDialog::saveSettings()
{
    // the settings object
    QSettings s;

    int lineNumbersMode = ui->lineNumbersComboBox->currentIndex();
    s.setValue( QStringLiteral("LineNumbers") , lineNumbersMode);

    bool highlight = ui->highlightCurrentLineCheckBox->isChecked();
    s.setValue( QStringLiteral("CurrentLineHighlight"), highlight);
    ui->lineColorButton->setEnabled(highlight);
    
    QPalette p = ui->lineColorButton->palette();
    QColor highlightLineColor = p.color(QPalette::Button);
    s.setValue( QStringLiteral("HighlightLineColor") , highlightLineColor);
    
    bool tabReplace = ui->replaceTabsWithSpacesCheckBox->isChecked();
    s.setValue( QStringLiteral("TabReplace") , tabReplace);
    
    int tabsCount = ui->spacesSpinBox->value();
    s.setValue( QStringLiteral("TabsCount") , tabsCount);

    // font
    QFont f = ui->fontLabel->font();
    QString fontFamily = f.family();
    int fontSize = f.pointSize();
    int fontWeight = f.weight();
    bool italic = f.italic();
    s.setValue( QStringLiteral("fontFamily") , fontFamily);
    s.setValue( QStringLiteral("fontSize")   , fontSize);
    s.setValue( QStringLiteral("fontWeight") , fontWeight);
    s.setValue( QStringLiteral("fontItalic") , italic);
}


void SettingsDialog::chooseHighlightColor()
{
    QPalette p = ui->lineColorButton->palette();
    QColor initialColor = p.color(QPalette::Button);
    
    QColor chosenColor = QColorDialog::getColor(initialColor, this, tr("Choose Highlight Line Color") );
    if (chosenColor.isValid()) {
        p.setColor(QPalette::Button, chosenColor);
        ui->lineColorButton->setPalette(p);
        
        saveSettings();
    }
}


void SettingsDialog::chooseFont()
{
    bool ok;
    // initialFont is the font initially shown in the dialog
    QFont initialFont = ui->fontLabel->font();
    QFont font = QFontDialog::getFont(&ok, initialFont, this, tr("Select font"), QFontDialog::MonospacedFonts);
    if (!ok) {
        return;
    }

    QString fontName = font.family() + QLatin1String(", ") + QString::number(font.pointSize()) + QLatin1String("pt");
    ui->fontLabel->setText(fontName);
    ui->fontLabel->setFont(font);

    saveSettings();
}


void SettingsDialog::resetSettings()
{
    int risp = QMessageBox::question(this,
                                     tr("Reset All Settings"),
                                     tr("Are you sure you want to reset all settings?"),
                                     QMessageBox::Reset | QMessageBox::Cancel);

    switch(risp) {
        case QMessageBox::Reset: {

            // the settings object
            QSettings s;
            s.clear();
            s.sync();
            loadSettings();
            break;
        }
        case QMessageBox::Cancel:
            return;

        default:
            // this should never happen
            break;
    }
}
