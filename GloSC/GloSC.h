/*
Copyright 2018-2019 Peter Repukat - FlatspotSoftware

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GloSC.h"

#include <qdir.h>
#include <qfile.h>
#include <qsettings.h>
#include <QFileDialog>
#include <qlist.h>
#include <qmessagebox.h>
#include <qprocess.h>

#include <Windows.h>
#include <appmodel.h>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QProgressDialog>
#include <QDir>


#include "UWPPair.h"
#include "UWPSelectDialog.h"
#include "UpdateChecker.h"


class GloSC : public QMainWindow
{
	Q_OBJECT

public:
	explicit GloSC(QWidget *parent = Q_NULLPTR);

private:
	Ui::GloSCClass ui_;

	void updateEntryList();
	void writeIni(QString entryName) const;

	void updateTargetsToNewVersion();

	void check360ControllerRebinding();

	QList<UWPPair> uwp_pairs_;

	constexpr static const unsigned int GLOSC_VERSION = 0x00000208; //Version Number in as bytes, just remove the dots.

	int wide_x_ = 711;
	int small_x_ = 302;

	int wide_x_create_ = 261;
	int small_x_create_ = 131;

	QGraphicsOpacityEffect op_eff_;

	void animate(int to);

	bool hook_steam_ = false;

	bool first_launch_ = false;

	void showTutorial();

	int current_slide_ = 0;
	int last_slide_ = 13;

	UpdateChecker updater_;

private slots:
	void on_cbUseDesktop_toggled(bool checked);
	void on_pbCreateNew_clicked();
	void on_pbSave_clicked();
	void on_pbDelete_clicked();
	void on_pbAddToSteam_clicked();
	void on_pbSearchPath_clicked();
	void on_pbUWP_clicked();
	void on_lwInstances_currentRowChanged(int row);
	void on_lwInstances_itemSelectionChanged();


};
