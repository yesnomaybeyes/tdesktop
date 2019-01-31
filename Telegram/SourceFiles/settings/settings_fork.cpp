/*
Author: 23rd.
*/
#include "settings/settings_fork.h"

#include "base/qthelp_url.h"
#include "boxes/abstract_box.h"
#include "settings/settings_common.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/input_fields.h"
#include "lang/lang_keys.h"
#include "storage/localstorage.h"
#include "styles/style_settings.h"
#include "styles/style_boxes.h"
#include "core/file_utilities.h"
#include "boxes/confirm_box.h"

namespace Settings {
namespace {

class SearchEngineBox : public BoxContent {
public:
	SearchEngineBox(
		QWidget*,
		Fn<void(bool)> callback);

	void setInnerFocus() override;

protected:
	void prepare() override;

private:
	Fn<void(bool)> _callback;
	Fn<void()> _setInnerFocus;
};

SearchEngineBox::SearchEngineBox(
	QWidget*,
	Fn<void(bool)> callback)
: _callback(std::move(callback)) {
	Expects(_callback != nullptr);
}

void SearchEngineBox::setInnerFocus() {
	Expects(_setInnerFocus != nullptr);

	_setInnerFocus();
}

void SearchEngineBox::prepare() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	const auto url = content->add(
		object_ptr<Ui::InputField>(
			content,
			st::defaultInputField,
			langFactory(lng_settings_search_engine_field_label),
			Global::SearchEngineUrl()),
		st::markdownLinkFieldPadding);

	const auto submit = [=] {
		const auto linkUrl = qthelp::validate_url(url->getLastText());
		const auto isInvalid = linkUrl.isEmpty() || linkUrl.indexOf("%q") == -1;
		if (isInvalid) {
			url->showError();
			return;
		}
		const auto weak = make_weak(this);
		Global::SetSearchEngineUrl(linkUrl);
		Local::writeUserSettings();
		_callback(!isInvalid);
		if (weak) {
			closeBox();
		}
	};

	connect(url, &Ui::InputField::submitted, [=] {
		submit();
	});

	setTitle(langFactory(lng_settings_search_engine_box_title));

	addButton(langFactory(lng_formatting_link_create), submit);
	addButton(langFactory(lng_cancel), [=] { 
		_callback(!Global::SearchEngineUrl().isEmpty());
		closeBox();
	});

	content->resizeToWidth(st::boxWidth);
	content->moveToLeft(0, 0);
	setDimensions(st::boxWidth, content->height());

	_setInnerFocus = [=] {
		url->setFocusFast();
	};
}


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
	const auto lastSeenInDialogs = addCheckbox(
		lng_settings_last_seen_in_dialogs,
		Global::LastSeenInDialogs());
	const auto searchEngine = addCheckbox(
		lng_settings_search_engine,
		Global::SearchEngine());

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

	lastSeenInDialogs->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::LastSeenInDialogs());
	}) | rpl::start_with_next([=](bool checked) {
		Global::SetLastSeenInDialogs(checked);
		Local::writeUserSettings();
	}, lastSeenInDialogs->lifetime());

	searchEngine->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::SearchEngine());
	}) | rpl::start_with_next([=](bool checked) {
		if (checked) {
			Ui::show(Box<SearchEngineBox>([=](
				const bool &isSuccess) {
				searchEngine->setChecked(isSuccess);
				Global::SetSearchEngine(isSuccess);
				Local::writeUserSettings();
			}), LayerOption::KeepOther);
		} else {
			Global::SetSearchEngine(checked);
			Local::writeUserSettings();
		}
	}, searchEngine->lifetime());
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
