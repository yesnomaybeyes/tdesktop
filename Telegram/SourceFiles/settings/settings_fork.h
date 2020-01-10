/*
Author: 23rd.
*/
#pragma once

#include "settings/settings_common.h"

namespace Settings {

class Fork : public Section {
public:
	Fork(QWidget *parent, not_null<Window::SessionController*> controller);

private:
	void setupContent(not_null<Window::SessionController*> controller);

};

} // namespace Settings
