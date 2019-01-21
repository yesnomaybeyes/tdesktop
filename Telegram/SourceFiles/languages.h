/*
	Author: github.com/23rd.
*/

#pragma once

static const QMap<QLocale::Language, QString> languages = {

	{QLocale::English,    "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./~QWERTYUIOP{}ASDFGHJKL:\" ZXCVBNM<>?"},
    {QLocale::Arabic,     "ذ1234567890-=ضصثقفغعهخحجدسيبلاتنمكط ؤر ىةوزظ  # إ`÷×؛<>|ٍِ]أـ،/:\"   ~ْ}',.؟ "},
	{QLocale::Chinese,    "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./~QWERTYUIOP{}ASDFGHJKL:\" ZXCVBNM<>?"},
	{QLocale::Croatian,   "¸1234567890'+qwertzuiopšđasdfghjklčćyxcvbnm,.-¨QWERTZUIOPŠĐASDFGHJKLČĆŽYXCVBNM;:_"},
	{QLocale::Danish,     "½1234567890+´qwertyuiopå¨asdfghjklæøzxcvbnm,.-½QWERTYUIOPÅ^ASDFGHJKLÆØ*ZXCVBNM;:_"},
	{QLocale::Esperanto,  "[1234567890-=',.pbfvdgh/yriaokjentlĝ;ŝscĉmuĵŭz{@<>PBFVDGH?YRIAOKJENTLĜW:ŜSCĈMUĴŬZ"},
	{QLocale::Finnish,    "§1234567890+´qwertyuiopå¨asdfghjklöäzxcvbnm,.-½QWERTYUIOPÅ^ASDFGHJKLÖÄ*ZXCVBNM;:_"},
	{QLocale::French,     " &é\"'(-è_çà)=azertyuiop^$qsdfghjklmùwxcvbn,;:! AZERTYUIOP\"£QSDFGHJKLM%µWXCVBN?./§"},
	{QLocale::Galician,   "º1234567890'¡qwertyuiop`+asdfghjklñ'zxcvbnm,.-ªQWERTYUIOP^*ASDFGHJKLÑ\"ÇZXCVBNM;:_"},
	{QLocale::German,     "°1234567890ß'qwertzuiopü+asdfghjklÖÄyxcvbnm,.-°QWERTZUIOPÜ*ASDFGHJKLÖÄ'YXCVBNM;:_"},
	{QLocale::Greek,      "`1234567890-=;ςερτυθιοπ[]ασδφγηξκλά'ζχψωβνμ,./~:ΣΕΡΤΥΘΙΟΠ{}ΑΣΔΦΓΗΞΚΛ¨\"|ΖΧΨΩΒΝΜ<>?"},
	{QLocale::Italian,    "\\1234567890'ìqwertyuiopè+asdfghjklòàzxcvbnm,.-|QWERTYUIOPé*ASDFGHJKLç°§ZXCVBNM;:_"},
	{QLocale::Korean,     "`1234567890-=ㅂㅈㄷㄱㅅㅛㅕㅑㅐㅔ[]ㅁㄴㅇㄹㅎㅗㅓㅏㅣ;'ㅋㅌㅊㅍㅠㅜㅡ,./~ㅃㅉㄸㄲㅆㅛㅕㅑㅒㅖ{}ㅁㄴㅇㄹㅎㅗㅓㅏㅣ:\" ㅋㅌㅊㅍㅠㅜㅡ<>?"},
	{QLocale::Norwegian,  "|1234567890+\\qwertyuiopå¨asdfghjkløæzxcvbnm,.-`QWERTYUIOPÅ^ASDFGHJKLØÆ*ZXCVBNM;:_"},
	{QLocale::Polish,     "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./~QWERTYUIOP{}ASDFGHJKL:\" ZXCVBNM<>?"},
	{QLocale::Portuguese, "º1234567890'¡qwertyuiop`+asdfghjklñ'zxcvbnm,.-ªQWERTYUIOP^*ASDFGHJKLÑ\"ÇZXCVBNM;:_"},
	{QLocale::Russian,    "ё1234567890-=йцукенгшщзхъфывапролджэячсмитьбю.ЁЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭ/ЯЧСМИТЬБЮ,"},
	{QLocale::Serbian,    "`1234567890'+љњертзуиопшђасдфгхјклчћsџцвбнм,.-~ЉЊЕРТЗУИОПШЂАСДФГХЈКЛЧЋ SЏЦВБНМ;:_"},
	{QLocale::Slovenian,  "¸1234567890'+qwertzuiopšđasdfghjklčćyxcvbnm,.-¨QWERTZUIOPŠĐASDFGHJKLČĆŽYXCVBNM;:_"},
	{QLocale::Spanish,    "º1234567890'¡qwertyuiop`+asdfghjklñ'zxcvbnm,.-ªQWERTYUIOP^*ASDFGHJKLÑ\"ÇZXCVBNM;:_"},
	{QLocale::Ukrainian,  "'1234567890-=йцукенгшщзхїфівапролджєячсмитьбю.'ЙЦУКЕНГШЩЗХЇФІВАПРОЛДЖЄ ЯЧСМИТЬБЮ,"},
};

const QString swapFromWrongKeyboardLayout(const QString &text, const bool &toEng) {
    const auto systemLang = QLocale::system().language();
    if (!languages.contains(systemLang)
    	|| systemLang == QLocale::English) {
    	return text;
    }

    QString sys = toEng ? languages[systemLang] : languages[QLocale::English];
    QString eng = toEng ? languages[QLocale::English] : languages[systemLang];
    QString newString = "";

    QString symbol = "";
    for (int i = 0; i < text.length(); i++) {
    	symbol = QString(text[i]);
        if (symbol == " ") {
            newString += symbol;
            continue;
        }
        int index = sys.indexOf(symbol);
        if (index >= 0) {
        	newString += QString(eng[index]);
        } else {
    		newString += symbol;
        }
    }

    return newString;
}