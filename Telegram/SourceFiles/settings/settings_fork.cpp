/*
Author: 23rd.
*/
#include "settings/settings_fork.h"

#include "app.h"
#include "base/qthelp_url.h"
#include "base/weak_ptr.h"
#include "boxes/abstract_box.h"
#include "boxes/confirm_box.h"
#include "core/file_utilities.h"
#include "facades.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "storage/localstorage.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/input_fields.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

namespace Settings {
namespace {

using langString = tr::phrase<>;
using SessionController = not_null<Window::SessionController*>;

class SettingBox : public Ui::BoxContent, public base::has_weak_ptr  {
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
		const auto weak = base::make_weak(this);
		getOrSetGlobal(linkUrl);
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

	addButton(tr::lng_box_ok(), submit);
	addButton(tr::lng_cancel(), [=] {
		_callback(!getOrSetGlobal(QString()).isEmpty());
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

class StickerSizeBox : public SettingBox {
	using SettingBox::SettingBox;

protected:
	QString getOrSetGlobal(QString value) override;
	bool isInvalidUrl(QString linkUrl) override;

private:
	int _startSize = 0;
};

QString StickerSizeBox::getOrSetGlobal(QString value) {
	if (value.isEmpty()) {
		if (!_startSize) {
			_startSize = Global::CustomStickerSize();
		} else if (_startSize == Global::CustomStickerSize()) {
			return QString();
		}
		return QString::number(Global::CustomStickerSize());
	}
	if (const auto number = value.toInt()) {
		Global::SetCustomStickerSize(number);
	}
	return QString();
}

bool StickerSizeBox::isInvalidUrl(QString linkUrl) {
	const auto number = linkUrl.toInt();
	return !number || number < 50 || number > 256;
}

//////


QWidget *parentWidget = nullptr;

void SetParentWidget(QWidget *parent) {
	parentWidget = parent;
}

void SetupForkContent(
	not_null<Ui::VerticalLayout*> container,
	SessionController controller) {

	const auto session = &controller->session();

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
	const auto useBlackTrayIcon = addCheckbox(
		tr::lng_settings_use_black_tray_icon(tr::now),
		session->settings().useBlackTrayIcon());

	const auto restartBox = [=](Fn<void()> ok, Fn<void()> cancel) {
		Ui::show(Box<ConfirmBox>(
			tr::lng_settings_need_restart(tr::now),
			tr::lng_settings_restart_now(tr::now),
			[=] {
				ok();
				Local::writeUserSettings();
				App::restart();
			},
			[=] {
				cancel();
			}));
	};

	AddButton(
		container,
		tr::lng_settings_custom_sticker_size(),
		st::settingsChatButton,
		&st::settingsIconStickers,
		st::settingsChatIconLeft
	)->addClickHandler([=] {
		Ui::show(Box<StickerSizeBox>([=](bool isSuccess) {
			if (isSuccess) {
				restartBox([] {}, [] {});
			}
		},
		tr::lng_settings_custom_sticker_size,
		tr::lng_settings_sticker_size_label));
	});

	squareAvatars->checkedChanges(
	) | rpl::filter([](bool checked) {
		return (checked != Global::SquareAvatars());
	}) | rpl::start_with_next([=](bool checked) {
		restartBox(
			[=] { Global::SetSquareAvatars(checked); },
			[=] { squareAvatars->setChecked(!checked); });
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
			Ui::LayerOption::KeepOther);
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
			Ui::LayerOption::KeepOther);
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

	useBlackTrayIcon->checkedChanges(
	) | rpl::filter([=](bool checked) {
		return (checked != session->settings().useBlackTrayIcon());
	}) | rpl::start_with_next([=](bool checked) {
		session->settings().setUseBlackTrayIcon(checked);
		Local::writeUserSettings();
		Global::RefUnreadCounterUpdate().notify(true);
	}, useBlackTrayIcon->lifetime());
}

void SetupFork(
	not_null<Ui::VerticalLayout*> container,
	SessionController controller) {
	AddSkip(container, st::settingsCheckboxesSkip);

	auto wrap = object_ptr<Ui::VerticalLayout>(container);
	SetupForkContent(wrap.data(), controller);

	container->add(object_ptr<Ui::OverrideMargins>(
		container,
		std::move(wrap)));

	AddSkip(container, st::settingsCheckboxesSkip);
}

} // namespace

Fork::Fork(QWidget *parent, SessionController controller)
: Section(parent) {
	SetParentWidget(parent);
	setupContent(controller);
}

void Fork::setupContent(SessionController controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	SetupFork(content, controller);

	Ui::ResizeFitChild(this, content);
}

} // namespace Settings
