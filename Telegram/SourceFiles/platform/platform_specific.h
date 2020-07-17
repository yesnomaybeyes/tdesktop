/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

namespace Platform {

void start();
void finish();

enum class PermissionStatus {
	Granted,
	CanRequest,
	Denied,
};

enum class PermissionType {
	Microphone,
};

enum class SystemSettingsType {
	Audio,
};

void SetWatchingMediaKeys(bool watching);
void SetApplicationIcon(const QIcon &icon);
QString CurrentExecutablePath(int argc, char *argv[]);
QString SingleInstanceLocalServerName(const QString &hash);
void RegisterCustomScheme(bool force = false);
PermissionStatus GetPermissionStatus(PermissionType type);
void RequestPermission(PermissionType type, Fn<void(PermissionStatus)> resultCallback);
void OpenSystemSettingsForPermission(PermissionType type);
bool OpenSystemSettings(SystemSettingsType type);

[[nodiscard]] std::optional<crl::time> LastUserInputTime();
[[nodiscard]] inline bool LastUserInputTimeSupported() {
	return LastUserInputTime().has_value();
}

void IgnoreApplicationActivationRightNow();
bool AutostartSupported();
QImage GetImageFromClipboard();
bool StartSystemMove(QWindow *window);
bool StartSystemResize(QWindow *window, Qt::Edges edges);

namespace ThirdParty {

void start();
void finish();

} // namespace ThirdParty
} // namespace Platform

#ifdef Q_OS_MAC
#include "platform/mac/specific_mac.h"
#elif defined Q_OS_UNIX // Q_OS_MAC
#include "platform/linux/specific_linux.h"
#elif defined Q_OS_WIN // Q_OS_MAC || Q_OS_UNIX
#include "platform/win/specific_win.h"
#endif // Q_OS_MAC || Q_OS_UNIX || Q_OS_WIN
