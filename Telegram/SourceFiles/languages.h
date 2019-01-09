/*
	Author: github.com/23rd.
*/

#pragma once

static const QMap<QLocale::Language, QString> languages = {
	{QLocale::English, "`1234567890-= qwertyuiop[]\\ asdfghjkl;'    zxcvbnm,./   ~!@#$%^&*()_+ QWERTYUIOP{}| ASDFGHJKL:\"    ZXCVBNM<>?   "},
    {QLocale::Arabic, "ذ1234567890-= ضصثقفغعهخحجد\\شسيبلاتنمكط   ئءؤر ىةوزظ  ّ!@#$%^&*()_+ًٌَُ إ`÷×؛<>|ٍِ][ أـ،/:\"   ~ْ}{ آ',.؟ "},
	{QLocale::Chinese, "`1234567890-= qwertyuiop[]\\ asdfghjkl;'    zxcvbnm,./   ~!@#$%^&*()_+ QWERTYUIOP{}| ASDFGHJKL:\"    ZXCVBNM<>?   "},
	{QLocale::Croatian, "¸1234567890'+ qwertzuiopšđ  asdfghjklčćž  <yxcvbnm,.-   ¨!\"#$%&/()=?* QWERTZUIOPŠĐ  ASDFGHJKLČĆŽ  >YXCVBNM;:_   "},
	{QLocale::Danish, "½1234567890+´ qwertyuiopå¨  asdfghjklæø'  <zxcvbnm,.-   ½!\"#¤%&/()=?` QWERTYUIOPÅ^  ASDFGHJKLÆØ*  >ZXCVBNM;:_   "},
	{QLocale::Esperanto, "[1234567890-= ',.pbfvdgh/y  riaokjentlĝw  \\;ŝscĉmuĵŭz   {!\"£$%^&*()_+ @<>PBFVDGH?Y  RIAOKJENTLĜW  |:ŜSCĈMUĴŬZ   "},
	{QLocale::Finnish, "§1234567890+´ qwertyuiopå¨  asdfghjklöä'  <zxcvbnm,.-   ½!\"#¤%&/()=?` QWERTYUIOPÅ^  ASDFGHJKLÖÄ*  >ZXCVBNM;:_   "},
	{QLocale::French, " &é\"'(-è_çà)= azertyuiop^$  qsdfghjklmù*  <wxcvbn,;:!    1234567890°+ AZERTYUIOP\"£  QSDFGHJKLM%µ  >WXCVBN?./§   "},
	{QLocale::Galician, "º1234567890'¡ qwertyuiop`+  asdfghjklñ'ç  <zxcvbnm,.-   ª!\"·$%&/()=?¿ QWERTYUIOP^*  ASDFGHJKLÑ\"Ç  >ZXCVBNM;:_   "},
	{QLocale::German, "°1234567890ß' qwertzuiopü+  asdfghjklÖÄ#  <yxcvbnm,.-   °!\"§$%&/()=?` QWERTZUIOPÜ*  ASDFGHJKLÖÄ'  >YXCVBNM;:_   "},
	{QLocale::Greek, "`1234567890-= ;ςερτυθιοπ[]  ασδφγηξκλά'\\   ζχψωβνμ,./   ~!@#$%^&*()_+ :ΣΕΡΤΥΘΙΟΠ{}  ΑΣΔΦΓΗΞΚΛ¨\"|   ΖΧΨΩΒΝΜ<>?   "},
	{QLocale::Italian, "\\1234567890'ì qwertyuiopè+  asdfghjklòàù  <zxcvbnm,.-   |!\"£$%&/()=?^ QWERTYUIOPé*  ASDFGHJKLç°§  >ZXCVBNM;:_   "},
	{QLocale::Korean, "`1234567890-= ㅂㅈㄷㄱㅅㅛㅕㅑㅐㅔ[]\\ ㅁㄴㅇㄹㅎㅗㅓㅏㅣ;'    ㅋㅌㅊㅍㅠㅜㅡ,./   ~!@#$%^&*()_+ ㅃㅉㄸㄲㅆㅛㅕㅑㅒㅖ{}| ㅁㄴㅇㄹㅎㅗㅓㅏㅣ:\"    ㅋㅌㅊㅍㅠㅜㅡ<>?   "},
	{QLocale::Norwegian, "|1234567890+\\ qwertyuiopå¨  asdfghjkløæ'  <zxcvbnm,.-   `!\"#¤%&/()=?` QWERTYUIOPÅ^  ASDFGHJKLØÆ*  >ZXCVBNM;:_   "},
	{QLocale::Polish, "`1234567890-= qwertyuiop[]\\ asdfghjkl;'    zxcvbnm,./   ~!@#$%^&*()_+ QWERTYUIOP{}| ASDFGHJKL:\"    ZXCVBNM<>?   "},
	{QLocale::Portuguese, "º1234567890'¡ qwertyuiop`+  asdfghjklñ'ç  <zxcvbnm,.-   ª!\"·$%&/()=?¿ QWERTYUIOP^*  ASDFGHJKLÑ\"Ç  >ZXCVBNM;:_   "},
	{QLocale::Russian, "ё1234567890-= йцукенгшщзхъ  фывапролджэ\\   ячсмитьбю.   Ё!\"№;%:?*()_+ ЙЦУКЕНГШЩЗХЪ  ФЫВАПРОЛДЖЭ/   ЯЧСМИТЬБЮ,   "},
	{QLocale::Serbian, "`1234567890'+ љњертзуиопшђж асдфгхјклчћ    sџцвбнм,.-\\  ~!\"#$%&/()=?* ЉЊЕРТЗУИОПШЂЖ АСДФГХЈКЛЧЋ    SЏЦВБНМ;:_|  "},
	{QLocale::Slovenian, "¸1234567890'+ qwertzuiopšđ  asdfghjklčćž  <yxcvbnm,.-   ¨!\"#$%&/()=?* QWERTZUIOPŠĐ  ASDFGHJKLČĆŽ  >YXCVBNM;:_   "},
	{QLocale::Spanish, "º1234567890'¡ qwertyuiop`+  asdfghjklñ'ç  <zxcvbnm,.-   ª!\"·$%&/()=?¿ QWERTYUIOP^*  ASDFGHJKLÑ\"Ç  >ZXCVBNM;:_   "},
	{QLocale::Ukrainian, "'1234567890-= йцукенгшщзхїґ фівапролджє    ячсмитьбю.   '!\"№;%:?*()_+ ЙЦУКЕНГШЩЗХЇҐ ФІВАПРОЛДЖЄ    ЯЧСМИТЬБЮ,   "},
};

const QString swapFromWrongKeyboardLayout(const QString &text) {
    const auto systemLang = QLocale::system().language();
    if (!languages.contains(systemLang)
    	|| systemLang == QLocale::English) {
    	return text;
    }

    QString sys = languages[systemLang];
    QString eng = languages[QLocale::English];
    QString newString = "";

    QString symbol = "";
    for (int i = 0; i < text.length(); i++) {
    	symbol = QString(text[i]);
        int index = sys.indexOf(symbol);
        if (index >= 0) {
        	newString += QString(eng[index]);
        } else {
        	index = eng.indexOf(symbol);
        	if (index >= 0) {
        		newString += QString(sys[index]);
        	} else {
        		newString += symbol;
        	}
        }
    }

    return newString;
}