/*
Author: 23rd.
*/
#include "settings/settings_fork.h"

#include "settings/settings_common.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/widgets/checkbox.h"
#include "lang/lang_keys.h"
#include "storage/localstorage.h"
#include "styles/style_settings.h"
#include "core/file_utilities.h"
#include "boxes/confirm_box.h"

namespace Settings {
namespace {


// Dunno how to set this variable without method.
QWidget *parentWidget = nullptr;

void SetParentWidget(QWidget *parent) {
	parentWidget = parent;
}

void OnChooseVideoPlayer(Ui::Checkbox *externalPlayer) {
	auto filters = QStringList(qsl("Video Player (*.exe)")); 
	filters.push_back(FileDialog::AllFilesFilter());
	const auto callback = [=](const FileDialog::OpenResult &result) {
		if (result.paths.isEmpty() && result.remoteContent.isEmpty()) {
			externalPlayer->setChecked(false);
			return;
		}

		if (!result.paths.isEmpty()) {
			auto filePath = result.paths.front();
			Global::SetExternalPlayerPath(filePath);
			Local::writeUserSettings();
		}
	};

	const auto callbackFail = [=]() {
		externalPlayer->setChecked(false);
	};

	FileDialog::GetOpenPath(
		parentWidget,
		"Choose video player",
		filters.join(qsl(";;")),
		crl::guard(parentWidget, callback),
		crl::guard(parentWidget, callbackFail));
};

void SetupForkContent(not_null<Ui::VerticalLayout*> container) {
	const auto checkbox = [&](LangKey label, bool checked) {
		return object_ptr<Ui::Checkbox>(
			container,
			lang(label),
			checked,
			st::settingsCheckbox);
	};
	const auto addCheckbox = [&](LangKey label, bool checked) {
		return container->add(
			checkbox(label, checked),
			st::settingsCheckboxPadding);
	};
	const auto addSlidingCheckbox = [&](LangKey label, bool checked) {
		return container->add(
			object_ptr<Ui::SlideWrap<Ui::Checkbox>>(
				container,
				checkbox(label, checked),
				st::settingsCheckboxPadding));
	};
	const auto squareAvatars = addCheckbox(
		lng_settings_square_avatats,
		Global::SquareAvatars());
	const auto audioFade = addCheckbox(
		lng_settings_audio_fade,
		Global::AudioFade());
	const auto externalPlayer = addCheckbox(
		lng_settings_external_player,
		Global::AskExternalPlayerPath());

	squareAvatars->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::SquareAvatars());
	}) | rpl::start_with_next([=](bool checked) {
		Ui::show(Box<ConfirmBox>(
				lang(lng_settings_need_restart),
				lang(lng_settings_restart_now),
				[=] { 
					Global::SetSquareAvatars(checked);
					Local::writeUserSettings();
					App::restart();
				},
				[=] { 
					squareAvatars->setChecked(!checked);
				}));
	}, squareAvatars->lifetime());
	
	audioFade->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::AudioFade());
	}) | rpl::start_with_next([=](bool checked) {
		Global::SetAudioFade(checked);
		Local::writeUserSettings();
	}, audioFade->lifetime());

	externalPlayer->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::AskExternalPlayerPath());
	}) | rpl::start_with_next([=](bool checked) {
		Global::SetAskExternalPlayerPath(checked);
		if (checked) {
			OnChooseVideoPlayer(externalPlayer);
		}
		Local::writeUserSettings();
	}, externalPlayer->lifetime());

}

void SetupFork(not_null<Ui::VerticalLayout*> container) {
	AddSkip(container, st::settingsCheckboxesSkip);

	auto wrap = object_ptr<Ui::VerticalLayout>(container);
	SetupForkContent(wrap.data());

	container->add(object_ptr<Ui::OverrideMargins>(
		container,
		std::move(wrap)));

	AddSkip(container, st::settingsCheckboxesSkip);
}

} // namespace

Fork::Fork(QWidget *parent, not_null<UserData*> self)
: Section(parent)
, _self(self) {
	SetParentWidget(parent);
	setupContent();
}

void Fork::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	SetupFork(content);

	Ui::ResizeFitChild(this, content);
}

} // namespace Settings
