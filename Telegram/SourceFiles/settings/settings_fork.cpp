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

using langString = tr::phrase<>;

class SettingBox : public BoxContent {
public:
	explicit SettingBox(
		QWidget*,
		Fn<void(bool)> callback,
		langString title,
		langString info);

	void setInnerFocus() override;

protected:
	void prepare() override;

	virtual QString getOrSetGlobal(QString value) = 0;
	virtual bool isInvalidUrl(QString linkUrl) = 0;

	Fn<void(bool)> _callback;
	Fn<void()> _setInnerFocus;
	langString _info;
	langString _title;
};

SettingBox::SettingBox(
	QWidget*,
	Fn<void(bool)> callback,
	langString title,
	langString info)
: _callback(std::move(callback))
, _info(info)
, _title(title) {
	Expects(_callback != nullptr);
}

void SettingBox::setInnerFocus() {
	Expects(_setInnerFocus != nullptr);

	_setInnerFocus();
}

void SettingBox::prepare() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	const auto url = content->add(
		object_ptr<Ui::InputField>(
			content,
			st::defaultInputField,
			_info(),
			getOrSetGlobal(QString())),
		st::markdownLinkFieldPadding);

	const auto submit = [=] {
		const auto linkUrl = url->getLastText();
		const auto isInvalid = isInvalidUrl(linkUrl);
		if (isInvalid) {
			url->showError();
			return;
		}
		const auto weak = make_weak(this);
		getOrSetGlobal(linkUrl);
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

	setTitle(_title());

	addButton(tr::lng_formatting_link_create(), submit);
	addButton(tr::lng_cancel(), [=] {
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

//////

class SearchEngineBox : public SettingBox {

	using SettingBox::SettingBox;

protected:
	QString getOrSetGlobal(QString value) override;
	bool isInvalidUrl(QString linkUrl) override;
};

QString SearchEngineBox::getOrSetGlobal(QString value) {
	if (value.isEmpty()) {
		return Global::SearchEngineUrl();
	}
	Global::SetSearchEngineUrl(value);
	return QString();
}

bool SearchEngineBox::isInvalidUrl(QString linkUrl) {
	linkUrl = qthelp::validate_url(linkUrl);
	return linkUrl.isEmpty() || linkUrl.indexOf("%q") == -1;
}


//////

class URISchemeBox : public SettingBox {

	using SettingBox::SettingBox;

protected:
	QString getOrSetGlobal(QString value) override;
	bool isInvalidUrl(QString linkUrl) override;
};

QString URISchemeBox::getOrSetGlobal(QString value) {
	if (value.isEmpty()) {
		return Global::UriScheme();
	}
	Global::SetUriScheme(value);
	return QString();
}

bool URISchemeBox::isInvalidUrl(QString linkUrl) {
	return linkUrl.indexOf("://") < 2;
}

//////


QWidget *parentWidget = nullptr;

void SetParentWidget(QWidget *parent) {
	parentWidget = parent;
}

void SetupForkContent(not_null<Ui::VerticalLayout*> container) {
	const auto checkbox = [&](const QString &label, bool checked) {
		return object_ptr<Ui::Checkbox>(
			container,
			label,
			checked,
			st::settingsCheckbox);
	};
	const auto addCheckbox = [&](const QString &label, bool checked) {
		return container->add(
			checkbox(label, checked),
			st::settingsCheckboxPadding);
	};
	const auto addSlidingCheckbox = [&](const QString &label, bool checked) {
		return container->add(
			object_ptr<Ui::SlideWrap<Ui::Checkbox>>(
				container,
				checkbox(label, checked),
				st::settingsCheckboxPadding));
	};
	const auto squareAvatars = addCheckbox(
		tr::lng_settings_square_avatats(tr::now),
		Global::SquareAvatars());
	const auto audioFade = addCheckbox(
		tr::lng_settings_audio_fade(tr::now),
		Global::AudioFade());
	const auto uriScheme = addCheckbox(
		tr::lng_settings_uri_scheme(tr::now),
		Global::AskUriScheme());
	const auto lastSeenInDialogs = addCheckbox(
		tr::lng_settings_last_seen_in_dialogs(tr::now),
		Global::LastSeenInDialogs());
	const auto searchEngine = addCheckbox(
		tr::lng_settings_search_engine(tr::now),
		Global::SearchEngine());
	const auto allRecentStickers = addCheckbox(
		tr::lng_settings_show_all_recent_stickers(tr::now),
		Global::AllRecentStickers());

	squareAvatars->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::SquareAvatars());
	}) | rpl::start_with_next([=](bool checked) {
		Ui::show(Box<ConfirmBox>(
				tr::lng_settings_need_restart(tr::now),
				tr::lng_settings_restart_now(tr::now),
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

	uriScheme->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::AskUriScheme());
	}) | rpl::start_with_next([=](bool checked) {
		if (checked) {
			Ui::show(Box<URISchemeBox>([=](
				const bool &isSuccess) {
				uriScheme->setChecked(isSuccess);
				Global::SetAskUriScheme(isSuccess);
				Local::writeUserSettings();
			},
				tr::lng_settings_uri_scheme_box_title,
				tr::lng_settings_uri_scheme_field_label),
			LayerOption::KeepOther);
		} else {
			Global::SetAskUriScheme(checked);
			Local::writeUserSettings();
		}
	}, uriScheme->lifetime());

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
			},
				tr::lng_settings_search_engine_box_title,
				tr::lng_settings_search_engine_field_label),
			LayerOption::KeepOther);
		} else {
			Global::SetSearchEngine(checked);
			Local::writeUserSettings();
		}
	}, searchEngine->lifetime());

	allRecentStickers->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::AllRecentStickers());
	}) | rpl::start_with_next([=](bool checked) {
		Global::SetAllRecentStickers(checked);
		Local::writeUserSettings();
	}, lastSeenInDialogs->lifetime());
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
