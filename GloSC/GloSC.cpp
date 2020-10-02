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
#include "GloSC.h"
#include <memory>
#include "UpdateChecker.h"
#include <collection.h>
#include <appmodel.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <windows.h>
#include <appxpackaging.h>
#pragma comment(lib, "Shlwapi.lib")

using namespace Windows::Management::Deployment;
using namespace Windows::Foundation::Collections;


GloSC::GloSC(QWidget *parent)
	: QMainWindow(parent), updater_(this)
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	ui_.setupUi(this);

	this->setMaximumWidth(small_x_);

	updateEntryList();
	updateTargetsToNewVersion();

	if (first_launch_)
		showTutorial();

	updater_.checkUpdate(GLOSC_VERSION);
}

void GloSC::updateEntryList()
{
	ui_.lwInstances->clear();

	QDir dir("./targets");
	QStringList fileNames = dir.entryList(QDir::Files);

	if (fileNames.isEmpty())
		first_launch_ = true;
	

	for (auto &fileName : fileNames)
	{
		if (fileName.endsWith(".ini"))
			ui_.lwInstances->addItem(fileName.left(fileName.length() - 4));
	}


}

void GloSC::writeIni(QString entryName) const
{
	QSettings settings("./targets/" + entryName + ".ini", QSettings::IniFormat);

	settings.beginGroup("BaseConf");

	settings.setValue("bEnableOverlay", 0 + ui_.cbOverlay->isChecked());
	settings.setValue("bEnableControllers", 0 + ui_.cbControllers->isChecked());
	settings.setValue("bUseDesktopConfig", 0 + ui_.cbUseDesktop->isChecked());
	settings.setValue("bHookSteam", 0 + hook_steam_);
	settings.setValue("version", GLOSC_VERSION);

	settings.endGroup();


	settings.beginGroup("LaunchGame");

	settings.setValue("bLaunchGame", 0 + ui_.cbLaunchGame->isChecked());
	settings.setValue("Path", ui_.lePath->text());
	settings.setValue("Args", ui_.leArguments->text());
	if (ui_.lePath->text().contains("\\") || ui_.lePath->text().contains("/"))
		settings.setValue("Type", "Win32");
	else
		settings.setValue("Type", "UWP");

	settings.setValue("bCloseWhenDone", 0 + ui_.cbCloseWhenDone->isChecked());

	settings.endGroup();

}

void GloSC::updateTargetsToNewVersion()
{
	//incredible lazy way to update to this next version but eh...
	for (int i = 0; i < ui_.lwInstances->count(); i++)
	{
		on_lwInstances_currentRowChanged(i);
		const QString name = ui_.leName->text();

		QSettings settings("./targets/" + name + ".ini", QSettings::IniFormat);
		settings.beginGroup("BaseConf");
		const unsigned int version = settings.value("version").toInt();
		hook_steam_ = settings.value("bHookSteam").toBool();
		settings.endGroup();

		if (version < GLOSC_VERSION || version >= 0x500)
		{
			if (version < GLOSC_VERSION && GLOSC_VERSION > 0x203)
				hook_steam_ = false;
			on_pbSave_clicked();
		}
	}
}

void GloSC::check360ControllerRebinding()
{
	QSettings settings(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam)", QSettings::NativeFormat);
	const QString steamPath = settings.value("SteamPath").toString();
	const QString activeUser = settings.value("ActiveProcess/ActiveUser").toString();

	QFile configFile(steamPath + "/userdata/" + activeUser + "/config/localconfig.vdf");

	if (!configFile.exists())
	{
		return;
	}
	if (!configFile.open(QFile::ReadWrite))
	{
		return;
	}

	//just detect already present paths the easy way and hardcode the actual shortcut structure
	//will prob. come back to bite me, but for now it should be enough
	QByteArray configFileBytes = configFile.readAll();

	QString searchString = "\"SteamController_XBoxSupport\"";
	int idx = configFileBytes.indexOf(searchString);

	if (idx < 0)
	{
		configFile.close();
		return;
	}

	int c_idx = configFileBytes.indexOf("\"0\"", idx + searchString.length());
	if (c_idx < 0)
	{
		configFile.close();
		return;
	}


	if (c_idx >= (idx + searchString.length() + 4))
	{
		configFile.close();
		return;
	}

	configFileBytes = configFileBytes.replace((c_idx + 1), 1, QString("1").toStdString().c_str());

	if (QMessageBox::information(this, "GloSC", 
		"For GloSC to function correctly, you have to enable XBox configuration support in Steam!\nEnable now?",
		QMessageBox::Yes | QMessageBox::No) 
		
		== QMessageBox::Yes)
	{
		configFile.close();
		if (!configFile.open(QFile::ReadWrite | QIODevice::Truncate))
		{
			return;
		}

		configFile.write(configFileBytes);

		configFile.close();
	}

}


void GloSC::animate(int to)
{
	if (to == width())
		return;
	QPropertyAnimation* anim = new QPropertyAnimation(this, "size");
	if (to > width())
	{
		anim->setEasingCurve(QEasingCurve::InOutExpo);
		connect(anim, &QPropertyAnimation::finished, this, [this, to]()
		{
			this->setMinimumWidth(to);
		});
		this->setMaximumWidth(to);

		QPropertyAnimation* buttonAnim = new QPropertyAnimation(ui_.pbCreateNew, "size");
		buttonAnim->setEasingCurve(QEasingCurve::InOutExpo);
		buttonAnim->setDuration(360);
		buttonAnim->setStartValue(QSize(ui_.pbCreateNew->width(), ui_.pbCreateNew->height()));
		buttonAnim->setEndValue(QSize(wide_x_create_, ui_.pbCreateNew->height()));
		buttonAnim->start(QPropertyAnimation::DeleteWhenStopped);

	}
	else
	{
		anim->setEasingCurve(QEasingCurve::InExpo);
		connect(anim, &QPropertyAnimation::finished, this, [this, to]()
		{
			this->setMaximumWidth(to);
		});
		this->setMinimumWidth(to);

		QPropertyAnimation* buttonAnim = new QPropertyAnimation(ui_.pbCreateNew, "size");
		buttonAnim->setEasingCurve(QEasingCurve::InExpo);
		buttonAnim->setDuration(360);
		buttonAnim->setStartValue(QSize(ui_.pbCreateNew->width(), ui_.pbCreateNew->height()));
		buttonAnim->setEndValue(QSize(small_x_create_, ui_.pbCreateNew->height()));
		buttonAnim->start(QPropertyAnimation::DeleteWhenStopped);

	}
	anim->setDuration(360);
	anim->setStartValue(QSize(this->width(), this->height()));
	anim->setEndValue(QSize(to, this->height()));
	anim->start(QPropertyAnimation::DeleteWhenStopped);
}

void GloSC::on_cbUseDesktop_toggled(bool checked)
{
	hook_steam_ = !checked;
}

void GloSC::showTutorial()
{


	ui_.pbTuorialCreate->setVisible(false);
	ui_.pbTuorialCreate->setEnabled(false);
	ui_.tutorialFrame->setGeometry(ui_.tutorialFrame->x(), ui_.tutorialFrame->y(), ui_.tutorialFrame->width(), 386);

	for (int i = 1; i < 14; i++)
	{
		QLabel* label = ui_.tutorialFrame->findChild<QLabel *>(QString("lTutorialText" + QString::number(i)));
		if (label != nullptr)
			label->setVisible(false);
	}

	connect(ui_.pbTutorialNext, &QPushButton::clicked, [this]() {
		current_slide_++;

		if (current_slide_ >= 14)
		{
			ui_.tutorialFrame->setGeometry(ui_.tutorialFrame->x(), ui_.tutorialFrame->y(), ui_.tutorialFrame->width(), 0);
			ui_.pbTutorialNext->setVisible(false);
			ui_.pbTutorialNext->setEnabled(false);
			return;
		}

		ui_.tutorialFrame->findChild<QLabel *>(QString("lTutorialText" + QString::number(current_slide_ - 1)))->setVisible(false);
		ui_.tutorialFrame->findChild<QLabel *>(QString("lTutorialText" + QString::number(current_slide_)))->setVisible(true);

		QString test = QString(":/Tutorial/tut-assets/Tut" + QString::number(current_slide_) + ".png");
		ui_.lTutorialBackground->setPixmap(QPixmap(test));

		if (current_slide_ == 1)
		{
			ui_.pbTutorialNext->setVisible(false);
			ui_.pbTutorialNext->setEnabled(false);
			ui_.pbTuorialCreate->setVisible(true);
			ui_.pbTuorialCreate->setEnabled(true);
		}
		if (current_slide_ == 2)
			ui_.pbTutorialNext->setGeometry(600, ui_.pbTutorialNext->y(), ui_.pbTutorialNext->width(), ui_.pbTutorialNext->height());

		if (current_slide_ == 13)
		{
			animate(small_x_);
			ui_.pbTutorialNext->setText("Finish");
			ui_.pbTutorialNext->setGeometry(180, 45, ui_.pbTutorialNext->width(), ui_.pbTutorialNext->height());

		}

	});

	connect(ui_.pbTuorialCreate, &QPushButton::clicked, [this]()
	{
		ui_.pbTuorialCreate->setVisible(false);
		ui_.pbTuorialCreate->setEnabled(false);
		ui_.pbTutorialNext->setVisible(true);
		ui_.pbTutorialNext->setEnabled(true);
		on_pbCreateNew_clicked();
		ui_.pbTutorialNext->click();
	});

}

void GloSC::on_pbCreateNew_clicked()
{
	ui_.leName->setText("");

	ui_.cbOverlay->setChecked(true);
	ui_.cbControllers->setChecked(true);
	hook_steam_ = true;

	ui_.cbLaunchGame->setChecked(false);
	ui_.lePath->setText("");
	ui_.leArguments->setText("");
	ui_.cbCloseWhenDone->setChecked(false);

	animate(wide_x_);
}

void GloSC::on_pbSave_clicked()
{
	QString name = ui_.leName->text();
	name.remove("\\");
	name.remove("/");
	name.remove(":");
	name.remove(".");

	QString temp = name;
	if (temp.remove(" ") == "")
		return;

	QDir dir("targets");
	if (!dir.exists())
		dir.mkdir(".");

	writeIni(name);

	updateEntryList();

	animate(small_x_);
}


void GloSC::on_pbDelete_clicked()
{
	QString name = ui_.leName->text();

	QString temp = name;
	if (temp.remove(" ") == "")
		return;

	QFile file("./targets/" + name + ".ini");
	if (file.exists())
	{
		file.remove();
	}
	updateEntryList();

	animate(small_x_);
}

void GloSC::on_pbAddToSteam_clicked()
{
	if (ui_.lwInstances->count() <= 0)
	{
		QMessageBox::information(this, "GloSC", "No shortcuts! Create some shortcuts first for them to be added to Steam!", QMessageBox::Ok);
		return;
	}

	QSettings settings(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam)", QSettings::NativeFormat);
	const QString steamPath = settings.value("SteamPath").toString();
	const QString activeUser = settings.value("ActiveProcess/ActiveUser").toString();

	QFile shortcutsFile(steamPath + "/userdata/" + activeUser + "/config/shortcuts.vdf");

	//TODO: FIXME: If User has no shortcuts file, create one!
	if (!shortcutsFile.exists())
	{
		QMessageBox::information(this, "GloSC", QString("Couldn't detect Steam shortcuts file!\n")+
			"Make sure you have at least one non-Steam shortcut for the file to be present\n"+
			"Steam must be running for it to be detected", QMessageBox::Ok);
		return;
	}
	if (!shortcutsFile.open(QFile::ReadWrite))
	{
		QMessageBox::information(this, "GloSC", "Couldn't open Steam shortcuts file!", QMessageBox::Ok);
		return;
	}

	//just detect already present paths the easy way and hardcode the actual shortcut structure
	//will prob. come back to bite me, but for now it should be enough
	QByteArray shortcutsFileBytes = shortcutsFile.readAll();

	//get shortcutcount
	QByteArray temp = shortcutsFileBytes;
	temp.chop(9); //chop off last "tags"
	temp = temp.mid(temp.lastIndexOf("tags") + 8, temp.size() - 1);
	int shortcutCount = QString(temp).toInt();

	QDir appDir = QDir::current();
	for (int i = 0; i < ui_.lwInstances->count(); i++)
	{
		const QString itemName = ui_.lwInstances->item(i)->text();
		if (!shortcutsFileBytes.contains(("\"" + QDir::toNativeSeparators(appDir.absoluteFilePath("SteamTarget.exe")) + "\"" + " \"./targets/" + itemName + ".ini\"").toStdString().c_str()))
		{
			shortcutsFileBytes.chop(2); //chop of end bytes
			shortcutCount++;

			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append(QString::number(shortcutCount));
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x01');
			shortcutsFileBytes.append("appname");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append(itemName);
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x01');
			shortcutsFileBytes.append("exe");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append("\"" + QDir::toNativeSeparators(appDir.absoluteFilePath("SteamTarget.exe")) + "\"" + " \"./targets/" + itemName + ".ini\"");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x01');
			shortcutsFileBytes.append("StartDir");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append("\"" + QDir::toNativeSeparators(appDir.absolutePath()) + "\"");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x01');
			shortcutsFileBytes.append("icon");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x01');
			shortcutsFileBytes.append("ShortcutPath");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x02');
			shortcutsFileBytes.append("IsHidden");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x02');
			shortcutsFileBytes.append("AllowDesktopConfig");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x01');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x02');
			shortcutsFileBytes.append("OpenVR");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append("tags");
			shortcutsFileBytes.append('\x00');
			shortcutsFileBytes.append('\x08');
			shortcutsFileBytes.append('\x08');

			//append chopped of bytes
			shortcutsFileBytes.append('\x08');
			shortcutsFileBytes.append('\x08');
		}
	}

	shortcutsFile.close();
	if (!shortcutsFile.open(QFile::ReadWrite | QIODevice::Truncate)) 
	{
		QMessageBox::information(this, "GloSC", "Couldn't open Steam shortcuts file!", QMessageBox::Ok);
		return;
	}

	shortcutsFile.write(shortcutsFileBytes);

	shortcutsFile.close();

	animate(small_x_);

	if( QMessageBox::information(this, "GloSC",
		"Shortcuts were added!\nRestart Steam for changes to take effect!\nRestart Steam now?",
		QMessageBox::Yes | QMessageBox::No) 
		
		== QMessageBox::Yes)
	{
		
		QSettings settings(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam)", QSettings::NativeFormat);
		QString steamPath = settings.value("SteamPath").toString() + "/Steam.exe";
		steamPath = QDir::toNativeSeparators(steamPath);

		QProcess::execute("taskkill.exe /im Steam.exe /f");

		check360ControllerRebinding();

		QProcess::startDetached("explorer.exe", { steamPath });

	} 
	else
	{
		QMessageBox::warning(this, "GloSC", "For GloSC to function correctly, you have to enable XBox configuration support in Steam!", QMessageBox::Ok);
	}

	QMessageBox::information(this, "GloSC", "Don't forget to rebind you Controller in Steam!\nIf the controller randomly switches to desktop-configuration, run Steam as admin!\n\nPlease Enable \"Use the Big Picture Overlay when using a Steam Input enables controller from the Desktop\" to get working touch Menus when not in Big Picture", QMessageBox::Ok);

}

void GloSC::on_pbSearchPath_clicked()
{
	QString filePath = QFileDialog::getOpenFileName(this, "Select Game", "", "*.exe *.url");
	if (!filePath.isEmpty())
	{
		QFileInfo fileInfo(filePath);
		ui_.lePath->setText(fileInfo.filePath());
		QString name = fileInfo.fileName();
		name.chop(4);
		ui_.leName->setText(name);
		ui_.cbLaunchGame->setChecked(true);
	}
}

void GloSC::on_pbUWP_clicked()
{

	PackageManager^ packageManager = ref new PackageManager();
	IIterable<Windows::ApplicationModel::Package^>^ packages = packageManager->FindPackages();

	int packageCount = 0;
	// Only way to get the count of packages is to iterate through the whole collection first
	std::for_each(Windows::Foundation::Collections::begin(packages), Windows::Foundation::Collections::end(packages),
		[&](Windows::ApplicationModel::Package^ package)
		{
			packageCount += 1;
		});

	QProgressDialog progDialog("Searching for UWP apps...", "Cancel", 0, packageCount, this);
	progDialog.setWindowModality(Qt::WindowModal);
	// Without this the progress dialog doesn't update in the lambda function.
	progDialog.show();
	QApplication::processEvents();
	
	QList<UWPPair> pairs;

	QStringList AppNames;
	QStringList AppUMIds;

	int currPackage = 0;
	// Iterate through all the packages
	std::for_each(Windows::Foundation::Collections::begin(packages), Windows::Foundation::Collections::end(packages),
		[&](Windows::ApplicationModel::Package^ package)
		{
			progDialog.setValue(currPackage);
			progDialog.update();
			

			if (progDialog.wasCanceled())
			{
				return;
			}

			HRESULT hr = S_OK;
			IStream* inputStream = NULL;
			UINT32 pathLen = 0;
			IAppxManifestReader* manifestReader = NULL;
			IAppxFactory* appxFactory = NULL;						
			LPWSTR appId = NULL;
			LPWSTR manifestAppName = NULL;
			
			// Get the package path on disk so we can load the manifest XML and get the PRAID
			GetPackagePathByFullName(package->Id->FullName->Data(), &pathLen, NULL);
            
			if (pathLen > 0) {

				// Length of the path + "\\AppxManifest.xml" that we'll be appending
				UINT32 manifestLen = pathLen + 20;
				PWSTR pathBuf = (PWSTR)malloc(manifestLen * sizeof(wchar_t));

				GetPackagePathByFullName(package->Id->FullName->Data(), &pathLen, pathBuf);
				PWSTR manifest_xml = L"\\AppxManifest.xml";

				hr = StringCchCatW(pathBuf, manifestLen, manifest_xml);

				// Let's ignore a bunch of built in apps and such
				if (wcsstr(pathBuf, L"SystemApps")) {
					hr = E_FAIL;
				}
				else if (wcsstr(pathBuf, L".NET.Native."))
					hr = E_FAIL;
				else if (wcsstr(pathBuf, L".VCLibs."))
					hr = E_FAIL;
				else if (wcsstr(pathBuf, L"Microsoft.UI"))
					hr = E_FAIL;
				else if (wcsstr(pathBuf, L"Microsoft.Advertising"))
					hr = E_FAIL;
				else if (wcsstr(pathBuf, L"Microsoft.Services.Store"))
					hr = E_FAIL;

				BOOL hasCurrent = FALSE;

				// Open the manifest XML
				if (SUCCEEDED(hr)) {

					hr = SHCreateStreamOnFileEx(
						pathBuf,
						STGM_READ,
						0, // default file attributes
						FALSE, // do not create new file
						NULL, // no template
						&inputStream);
				}
				if (SUCCEEDED(hr)) {

					hr = CoCreateInstance(
						__uuidof(AppxFactory),
						NULL,
						CLSCTX_INPROC_SERVER,
						__uuidof(IAppxFactory),
						(LPVOID*)(&appxFactory));
				}
				if (SUCCEEDED(hr)) {
					hr = appxFactory->CreateManifestReader(inputStream, &manifestReader);
				}

				// Grab application ID (PRAID) and DisplayName from the XML
				if (SUCCEEDED(hr)) {
					IAppxManifestApplicationsEnumerator* applications = NULL;
					manifestReader->GetApplications(&applications);
					if (SUCCEEDED(hr)) {
						hr = applications->GetHasCurrent(&hasCurrent);
						if (hasCurrent) {
							IAppxManifestApplication* application = NULL;
							hr = applications->GetCurrent(&application);
							if (SUCCEEDED(hr)) {
								application->GetStringValue(L"Id", &appId);
								application->GetStringValue(L"DisplayName", &manifestAppName);
								application->Release();
							}
						}
						else {
							hr = S_FALSE;
						}
						applications->Release();
					}
					manifestReader->Release();
					inputStream->Release();
					
				}

				if (SUCCEEDED(hr)) {
					PWSTR appNameBuf;
					QString AppUMId = QString::fromWCharArray(package->Id->FamilyName->Data());
					QString AppName;
					if (manifestAppName != NULL)
					{
						// If the display name is an indirect string, we'll try and load it using SHLoadIndirectString
						if (wcsstr(manifestAppName, L"ms-resource:"))
						{
							PWSTR res_name = wcsdup(&manifestAppName[12]);
							appNameBuf = (PWSTR)malloc(1026);
							LPCWSTR resource_str = L"@{";
							std::wstring reslookup = std::wstring(resource_str) + package->Id->FullName->Data() + L"?ms-resource://" + package->Id->Name->Data() + L"/resources/" + res_name + L"}";
							PCWSTR res_str = reslookup.c_str();
							hr = SHLoadIndirectString(res_str, appNameBuf, 512, NULL);
							// Try several resource paths
							if (!SUCCEEDED(hr)) {
								std::wstring reslookup = std::wstring(resource_str) + package->Id->FullName->Data() + L"?ms-resource://" + package->Id->Name->Data() + L"/Resources/" + res_name + L"}";
								PCWSTR res_str = reslookup.c_str();
								hr = SHLoadIndirectString(res_str, appNameBuf, 512, NULL);
								// If the third one doesn't work, we give up and use the package name from PackageManager
								if (!SUCCEEDED(hr)) {
									std::wstring reslookup = std::wstring(resource_str) + package->Id->FullName->Data() + L"?ms-resource://" + package->Id->Name->Data() + L"/" + res_name + L"}";
									PCWSTR res_str = reslookup.c_str();
									hr = SHLoadIndirectString(res_str, appNameBuf, 512, NULL);
								}
							}

							
							if (!SUCCEEDED(hr))
								AppName = QString::fromWCharArray(package->Id->Name->Data());
							else
								AppName = QString::fromWCharArray(appNameBuf);
							free(appNameBuf);
						}
						else
						{
							appNameBuf = manifestAppName;
							AppName = QString::fromWCharArray(appNameBuf);

						}
						
					}
					else {
						AppName = QString::fromWCharArray(package->Id->Name->Data());
					}
					
					QString PRAID = QString::fromWCharArray(appId);
					CoTaskMemFree(appId);
					if (!PRAID.isEmpty()) {
						AppUMId = AppUMId.append("!");
						AppUMId = AppUMId.append(PRAID);
					}
			
					const UWPPair uwpPair = {
						AppName,
						AppUMId,
					};

					
					free(pathBuf);

					pairs.push_back(uwpPair);
				}
			}
			currPackage += 1;
		});


	uwp_pairs_ = pairs;

	progDialog.close();
	UWPSelectDialog dialog(this);
	dialog.setUWPList(uwp_pairs_);
	int selection = dialog.exec();
	

	if (selection > -1)
	{
		ui_.lePath->setText(uwp_pairs_.at(selection).AppUMId);
		ui_.leName->setText(uwp_pairs_.at(selection).AppName);
		ui_.cbLaunchGame->setChecked(true);
	}

}

void GloSC::on_lwInstances_currentRowChanged(int row)
{
	if (row < 0)
		return;
	const QString entryName = ui_.lwInstances->item(row)->text();
	ui_.leName->setText(entryName);

	QSettings settings("./targets/" + entryName + ".ini", QSettings::IniFormat);

	settings.beginGroup("BaseConf");

	ui_.cbOverlay->setChecked(settings.value("bEnableOverlay").toBool());
	ui_.cbControllers->setChecked(settings.value("bEnableControllers").toBool());
	ui_.cbUseDesktop->setChecked(settings.value("bUseDesktopConfig").toBool());
	if (ui_.cbUseDesktop->isChecked())
	{
		hook_steam_ = false;
	}
	else
	{
		hook_steam_ = true;
	}

	settings.endGroup();


	settings.beginGroup("LaunchGame");

	ui_.cbLaunchGame->setChecked(settings.value("bLaunchGame").toBool());
	ui_.lePath->setText(settings.value("Path").toString());
	ui_.leArguments->setText(settings.value("Args").toString());
	ui_.cbCloseWhenDone->setChecked(settings.value("bCloseWhenDone").toBool());

	settings.endGroup();

}

void GloSC::on_lwInstances_itemSelectionChanged()
{
	if (width() != wide_x_)
	{
		animate(wide_x_);
	} 
}
