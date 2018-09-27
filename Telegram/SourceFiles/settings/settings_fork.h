/*
Author: 23rd.
*/
#pragma once

#include "settings/settings_common.h"

namespace Settings {

class Fork : public Section {
public:
	Fork(QWidget *parent, not_null<UserData*> self);

private:
	void setupContent();

	not_null<UserData*> _self;

};

} // namespace Settings
