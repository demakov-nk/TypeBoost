#include "KeyboardLayout.h"
#include <QVariantMap>

namespace {
KeyboardLayout::KeyInfo ki(const QString &id, int hand, int finger, int row, int col)
{
    return {id, finger, hand, row, col};
}
} // namespace

KeyboardLayout::KeyboardLayout(QObject *parent)
    : QObject(parent)
{
    loadLayout(QStringLiteral("qwerty"));
}

QString KeyboardLayout::currentLayoutId() const { return m_currentLayoutId; }

void KeyboardLayout::setCurrentLayoutId(const QString &id)
{
    if (m_currentLayoutId == id) return;
    loadLayout(id);
    emit layoutChanged();
}

QStringList KeyboardLayout::availableLayouts() const
{
    return {
        QStringLiteral("qwerty"),
        QStringLiteral("colemak"),
        QStringLiteral("workman"),
        QStringLiteral("dvorak"),
        QStringLiteral("йцукен"),
        QStringLiteral("русская_машинопись"),
        QStringLiteral("макинтош"),
        QStringLiteral("яверты"),
    };
}

int KeyboardLayout::fingerForChar(const QString &ch) const
{
    auto it = m_charToKeyIndex.find(ch.toLower());
    if (it == m_charToKeyIndex.end()) return -1;
    return m_keys.at(it.value()).finger;
}

int KeyboardLayout::handForChar(const QString &ch) const
{
    auto it = m_charToKeyIndex.find(ch.toLower());
    if (it == m_charToKeyIndex.end()) return -1;
    return m_keys.at(it.value()).hand;
}

// РЕАЛИЗАЦИЯ ДЛЯ QML: Превращаем внутренний QList<KeyInfo> в понятный для QML список объектов
QVariantList KeyboardLayout::keyInfoList() const
{
    QVariantList list;
    for (const auto &key : m_keys) {
        QVariantMap map;
        map[QStringLiteral("keyId")]  = key.keyId;
        map[QStringLiteral("finger")] = key.finger;
        map[QStringLiteral("hand")]   = key.hand;
        map[QStringLiteral("row")]    = key.row;
        map[QStringLiteral("col")]    = key.col;
        list.append(map);
    }
    return list;
}

int KeyboardLayout::shiftHandForChar(const QString &ch) const
{
    // Shift берётся противоположной рукой от руки символа
    auto it = m_charToKeyIndex.find(ch.toLower());
    if (it == m_charToKeyIndex.end()) return -1;
    int hand = m_keys.at(it.value()).hand;
    return hand == 0 ? 1 : 0; // инверсия
}

QString KeyboardLayout::baseKeyForChar(const QString &ch) const
{
    qDebug() << "baseKeyForChar called for" << ch 
             << "shiftMap size:" << m_shiftCharToBaseKey.size()
             << "contains:" << m_shiftCharToBaseKey.contains(ch);

    // Сначала проверяем shift-символы
    auto it = m_shiftCharToBaseKey.find(ch);
    if (it != m_shiftCharToBaseKey.end())
        return it.value();

    // Потом обычные символы
    if (m_charToKeyIndex.contains(ch.toLower()))
        return ch.toLower();

    return ch.toLower();
}

void KeyboardLayout::addKey(const QString &keyId, int row, int col, int finger, int hand)
{
    KeyInfo info;
    info.keyId = keyId;
    info.row = row;
    info.col = col;
    info.finger = finger;
    info.hand = hand;
    
    m_keys.append(info);
    
    // ВАЖНО: Заполняем обратный индекс символа для работы методов fingerForChar и handForChar
    int newIndex = m_keys.size() - 1;
    m_charToKeyIndex[keyId.toLower()] = newIndex;
}

void KeyboardLayout::buildQwertyRow0()
{
    addKey("`", 0,  0, 5, 0); addKey("1", 0,  1, 5, 0); addKey("2", 0,  2, 4, 0);
    addKey("3", 0,  3, 3, 0); addKey("4", 0,  4, 2, 0); addKey("5", 0,  5, 2, 0);
    addKey("6", 0,  6, 9, 1); addKey("7", 0,  7, 9, 1); addKey("8", 0,  8, 8, 1);
    addKey("9", 0,  9, 7, 1); addKey("0", 0, 10, 6, 1); addKey("-", 0, 11, 6, 1);
    addKey("=", 0, 12, 6, 1); addKey("backspace", 0, 13, 6, 1);
}

void KeyboardLayout::buildStandardRow4()
{
    addKey("ctrll",  4, 0, 5, 0);
    addKey("winl",   4, 1, 5, 0);
    addKey("altl",   4, 2, 5, 0);
    addKey("space",  4, 3, 1, 0);  // большой палец — сторона определяется по контексту
    addKey("altr",   4, 4, 6, 1);
    addKey("winr",   4, 5, 6, 1);
    addKey("menur",  4, 6, 6, 1);
    addKey("ctrlr",  4, 7, 6, 1);
}

void KeyboardLayout::loadLayout(const QString &id)
{
    m_keys.clear();
    m_charToKeyIndex.clear();
    m_shiftCharToBaseKey.clear();
    m_currentLayoutId = id;

    if (id == QStringLiteral("qwerty")) buildQwertyLayout();
    else if (id == QStringLiteral("йцукен")) buildYcukenLayout();
    else if (id == QStringLiteral("colemak")) buildColemakLayout();
    else if (id == QStringLiteral("workman")) buildWorkmanLayout();
    else if (id == QStringLiteral("dvorak")) buildDvorakLayout();
    else if (id == QStringLiteral("русская_машинопись")) buildRuMachLayout();
    else if (id == QStringLiteral("макинтош")) buildRuMacLayout();
    else if (id == QStringLiteral("яверты")) buildYavertyLayout();
    else buildQwertyLayout();
}

void KeyboardLayout::buildQwertyLayout()
{
    buildQwertyRow0();
    // Ряд 1
    addKey("tab",    1,  0, 5, 0);
    addKey("q",      1,  1, 5, 0); addKey("w",  1,  2, 4, 0); addKey("e",  1,  3, 3, 0);
    addKey("r",      1,  4, 2, 0); addKey("t",  1,  5, 2, 0);
    addKey("y",      1,  6, 9, 1); addKey("u",  1,  7, 9, 1); addKey("i",  1,  8, 8, 1);
    addKey("o",      1,  9, 7, 1); addKey("p",  1, 10, 6, 1);
    addKey("[",      1, 11, 6, 1); addKey("]",  1, 12, 6, 1); addKey("\\", 1, 13, 6, 1);
    // Ряд 2
    addKey("caps",   2,  0, 5, 0);
    addKey("a",      2,  1, 5, 0); addKey("s",  2,  2, 4, 0); addKey("d",  2,  3, 3, 0);
    addKey("f",      2,  4, 2, 0); addKey("g",  2,  5, 2, 0);
    addKey("h",      2,  6, 9, 1); addKey("j",  2,  7, 9, 1); addKey("k",  2,  8, 8, 1);
    addKey("l",      2,  9, 7, 1); addKey(";",  2, 10, 6, 1);
    addKey("'",      2, 11, 6, 1); addKey("enter", 2, 12, 6, 1);
    // Ряд 3
    addKey("shiftl", 3,  0, 5, 0);
    addKey("z",      3,  1, 5, 0); addKey("x",  3,  2, 4, 0); addKey("c",  3,  3, 3, 0);
    addKey("v",      3,  4, 2, 0); addKey("b",  3,  5, 2, 0);
    addKey("n",      3,  6, 9, 1); addKey("m",  3,  7, 9, 1); addKey(",",  3,  8, 8, 1);
    addKey(".",      3,  9, 7, 1); addKey("/",  3, 10, 6, 1);
    addKey("shiftr", 3, 11, 6, 1);

    buildStandardRow4();
    buildQwertyShiftMap();
}

void KeyboardLayout::buildYcukenLayout()
{
    // Ряд 0
    addKey("ё",  0,  0, 5, 0); addKey("1",  0,  1, 5, 0); addKey("2",  0,  2, 4, 0);
    addKey("3",  0,  3, 3, 0); addKey("4",  0,  4, 2, 0); addKey("5",  0,  5, 2, 0);
    addKey("6",  0,  6, 9, 1); addKey("7",  0,  7, 9, 1); addKey("8",  0,  8, 8, 1);
    addKey("9",  0,  9, 7, 1); addKey("0",  0, 10, 6, 1); addKey("-",  0, 11, 6, 1);
    addKey("=",  0, 12, 6, 1); addKey("backspace", 0, 13, 6, 1);
    // Ряд 1
    addKey("tab",    1,  0, 5, 0);
    addKey("й",      1,  1, 5, 0); addKey("ц",  1,  2, 4, 0); addKey("у",  1,  3, 3, 0);
    addKey("к",      1,  4, 2, 0); addKey("е",  1,  5, 2, 0);
    addKey("н",      1,  6, 9, 1); addKey("г",  1,  7, 9, 1); addKey("ш",  1,  8, 8, 1);
    addKey("щ",      1,  9, 7, 1); addKey("з",  1, 10, 6, 1);
    addKey("х",      1, 11, 6, 1); addKey("ъ",  1, 12, 6, 1); addKey("\\", 1, 13, 6, 1);
    // Ряд 2
    addKey("caps",   2,  0, 5, 0);
    addKey("ф",      2,  1, 5, 0); addKey("ы",  2,  2, 4, 0); addKey("в",  2,  3, 3, 0);
    addKey("а",      2,  4, 2, 0); addKey("п",  2,  5, 2, 0);
    addKey("р",      2,  6, 9, 1); addKey("о",  2,  7, 9, 1); addKey("л",  2,  8, 8, 1);
    addKey("д",      2,  9, 7, 1); addKey("ж",  2, 10, 6, 1);
    addKey("э",      2, 11, 6, 1); addKey("enter", 2, 12, 6, 1);
    // Ряд 3
    addKey("shiftl", 3,  0, 5, 0);
    addKey("я",      3,  1, 5, 0); addKey("ч",  3,  2, 4, 0); addKey("с",  3,  3, 3, 0);
    addKey("м",      3,  4, 2, 0); addKey("и",  3,  5, 2, 0);
    addKey("т",      3,  6, 9, 1); addKey("ь",  3,  7, 9, 1); addKey("б",  3,  8, 8, 1);
    addKey("ю",      3,  9, 7, 1); addKey(".",  3, 10, 6, 1);
    addKey("shiftr", 3, 11, 6, 1);
    // Ряд 4
    addKey("ctrll",  4, 0, 5, 0);
    addKey("winl",   4, 1, 5, 0);
    addKey("altl",   4, 2, 5, 0);
    addKey("space",  4, 3, 1, 0);
    addKey("altr",   4, 4, 6, 1);
    addKey("winr",   4, 5, 6, 1);
    addKey("menur",  4, 6, 6, 1);
    addKey("ctrlr",  4, 7, 6, 1);

    buildYcukenShiftMap();
}

void KeyboardLayout::buildColemakLayout()
{
    buildQwertyRow0();
    // Ряд 1
    addKey("tab",  1, 0, 5, 0);
    addKey("q",    1, 1, 5, 0); addKey("w",  1, 2, 4, 0); addKey("f",  1, 3, 3, 0);
    addKey("p",    1, 4, 2, 0); addKey("g",  1, 5, 2, 0);
    addKey("j",    1, 6, 2, 1); addKey("l",  1, 7, 2, 1); addKey("u",  1, 8, 3, 1);
    addKey("y",    1, 9, 4, 1); addKey(";",  1,10, 5, 1);
    addKey("[",    1,11, 5, 1); addKey("]",  1,12, 5, 1); addKey("\\", 1,13, 5, 1);
    // Ряд 2
    addKey("caps", 2, 0, 5, 0);
    addKey("a",    2, 1, 5, 0); addKey("r",  2, 2, 4, 0); addKey("s",  2, 3, 3, 0);
    addKey("t",    2, 4, 2, 0); addKey("d",  2, 5, 2, 0);
    addKey("h",    2, 6, 2, 1); addKey("n",  2, 7, 2, 1); addKey("e",  2, 8, 3, 1);
    addKey("i",    2, 9, 4, 1); addKey("o",  2,10, 5, 1);
    addKey("'",    2,11, 5, 1); addKey("enter", 2,12, 5, 1);
    // Ряд 3
    addKey("shiftl", 3, 0, 5, 0);
    addKey("z",    3, 1, 5, 0); addKey("x",  3, 2, 4, 0); addKey("c",  3, 3, 3, 0);
    addKey("v",    3, 4, 2, 0); addKey("b",  3, 5, 2, 0);
    addKey("k",    3, 6, 2, 1); addKey("m",  3, 7, 2, 1); addKey(",",  3, 8, 3, 1);
    addKey(".",    3, 9, 4, 1); addKey("/",  3,10, 5, 1);
    addKey("shiftr", 3,11, 5, 1);

    buildStandardRow4();
    buildQwertyShiftMap();
}

void KeyboardLayout::buildWorkmanLayout()
{
    buildQwertyRow0();
    // Ряд 1
    addKey("tab",  1, 0, 5, 0);
    addKey("q",    1, 1, 5, 0); addKey("d",  1, 2, 4, 0); addKey("r",  1, 3, 3, 0);
    addKey("w",    1, 4, 2, 0); addKey("b",  1, 5, 2, 0);
    addKey("j",    1, 6, 2, 1); addKey("f",  1, 7, 2, 1); addKey("u",  1, 8, 3, 1);
    addKey("p",    1, 9, 4, 1); addKey(";",  1,10, 5, 1);
    addKey("[",    1,11, 5, 1); addKey("]",  1,12, 5, 1); addKey("\\", 1,13, 5, 1);
    // Ряд 2
    addKey("caps", 2, 0, 5, 0);
    addKey("a",    2, 1, 5, 0); addKey("s",  2, 2, 4, 0); addKey("h",  2, 3, 3, 0);
    addKey("t",    2, 4, 2, 0); addKey("g",  2, 5, 2, 0);
    addKey("y",    2, 6, 2, 1); addKey("n",  2, 7, 2, 1); addKey("e",  2, 8, 3, 1);
    addKey("o",    2, 9, 4, 1); addKey("i",  2,10, 5, 1);
    addKey("'",    2,11, 5, 1); addKey("enter", 2,12, 5, 1);
    // Ряд 3
    addKey("shiftl", 3, 0, 5, 0);
    addKey("z",    3, 1, 5, 0); addKey("x",  3, 2, 4, 0); addKey("c",  3, 3, 3, 0);
    addKey("v",    3, 4, 2, 0); addKey("k",  3, 5, 2, 0);
    addKey("l",    3, 6, 2, 1); addKey("m",  3, 7, 2, 1); addKey(",",  3, 8, 3, 1);
    addKey(".",    3, 9, 4, 1); addKey("/",  3,10, 5, 1);
    addKey("shiftr", 3,11, 5, 1);

    buildStandardRow4();
    buildQwertyShiftMap();
}

void KeyboardLayout::buildDvorakLayout()
{
    buildQwertyRow0();
    // Ряд 1
    addKey("tab",  1, 0, 5, 0);
    addKey("'",    1, 1, 5, 0); addKey(",",  1, 2, 4, 0); addKey(".",  1, 3, 3, 0);
    addKey("p",    1, 4, 2, 0); addKey("y",  1, 5, 2, 0);
    addKey("f",    1, 6, 2, 1); addKey("g",  1, 7, 2, 1); addKey("c",  1, 8, 3, 1);
    addKey("r",    1, 9, 4, 1); addKey("l",  1,10, 5, 1);
    addKey("/",    1,11, 5, 1); addKey("=",  1,12, 5, 1); addKey("\\", 1,13, 5, 1);
    // Ряд 2
    addKey("caps", 2, 0, 5, 0);
    addKey("a",    2, 1, 5, 0); addKey("o",  2, 2, 4, 0); addKey("e",  2, 3, 3, 0);
    addKey("u",    2, 4, 2, 0); addKey("i",  2, 5, 2, 0);
    addKey("d",    2, 6, 2, 1); addKey("h",  2, 7, 2, 1); addKey("t",  2, 8, 3, 1);
    addKey("n",    2, 9, 4, 1); addKey("s",  2,10, 5, 1);
    addKey("-",    2,11, 5, 1); addKey("enter", 2,12, 5, 1);
    // Ряд 3
    addKey("shiftl", 3, 0, 5, 0);
    addKey(";",    3, 1, 5, 0); addKey("q",  3, 2, 4, 0); addKey("j",  3, 3, 3, 0);
    addKey("k",    3, 4, 2, 0); addKey("x",  3, 5, 2, 0);
    addKey("b",    3, 6, 2, 1); addKey("m",  3, 7, 2, 1); addKey("w",  3, 8, 3, 1);
    addKey("v",    3, 9, 4, 1); addKey("z",  3,10, 5, 1);
    addKey("shiftr", 3,11, 5, 1);

    buildStandardRow4();
    buildQwertyShiftMap();
}

void KeyboardLayout::buildRuMachLayout()
{
    // Ряд 0
    addKey("ё",  0,  0, 5, 0); addKey("1",  0,  1, 5, 0); addKey("2",  0,  2, 4, 0);
    addKey("3",  0,  3, 3, 0); addKey("4",  0,  4, 2, 0); addKey("5",  0,  5, 2, 0);
    addKey("6",  0,  6, 9, 1); addKey("7",  0,  7, 9, 1); addKey("8",  0,  8, 8, 1);
    addKey("9",  0,  9, 7, 1); addKey("0",  0, 10, 6, 1); addKey("-",  0, 11, 6, 1);
    addKey("=",  0, 12, 6, 1); addKey("backspace", 0, 13, 6, 1);
    // Ряд 1
    addKey("tab",    1,  0, 5, 0);
    addKey("й",      1,  1, 5, 0); addKey("ц",  1,  2, 4, 0); addKey("у",  1,  3, 3, 0);
    addKey("к",      1,  4, 2, 0); addKey("е",  1,  5, 2, 0);
    addKey("н",      1,  6, 9, 1); addKey("г",  1,  7, 9, 1); addKey("ш",  1,  8, 8, 1);
    addKey("щ",      1,  9, 7, 1); addKey("з",  1, 10, 6, 1);
    addKey("х",      1, 11, 6, 1); addKey("ъ",  1, 12, 6, 1); addKey("\\", 1, 13, 6, 1);
    // Ряд 2
    addKey("caps",   2,  0, 5, 0);
    addKey("ф",      2,  1, 5, 0); addKey("ы",  2,  2, 4, 0); addKey("в",  2,  3, 3, 0);
    addKey("а",      2,  4, 2, 0); addKey("п",  2,  5, 2, 0);
    addKey("р",      2,  6, 9, 1); addKey("о",  2,  7, 9, 1); addKey("л",  2,  8, 8, 1);
    addKey("д",      2,  9, 7, 1); addKey("ж",  2, 10, 6, 1);
    addKey("э",      2, 11, 6, 1); addKey("enter", 2, 12, 6, 1);
    // Ряд 3
    addKey("shiftl", 3,  0, 5, 0);
    addKey("я",      3,  1, 5, 0); addKey("ч",  3,  2, 4, 0); addKey("с",  3,  3, 3, 0);
    addKey("м",      3,  4, 2, 0); addKey("и",  3,  5, 2, 0);
    addKey("т",      3,  6, 9, 1); addKey("ь",  3,  7, 9, 1); addKey("б",  3,  8, 8, 1);
    addKey("ю",      3,  9, 7, 1); addKey(".",  3, 10, 6, 1);
    addKey("shiftr", 3, 11, 6, 1);
    // Ряд 4
    addKey("ctrll",  4, 0, 5, 0);
    addKey("winl",   4, 1, 5, 0);
    addKey("altl",   4, 2, 5, 0);
    addKey("space",  4, 3, 1, 0);
    addKey("altr",   4, 4, 6, 1);
    addKey("winr",   4, 5, 6, 1);
    addKey("menur",  4, 6, 6, 1);
    addKey("ctrlr",  4, 7, 6, 1);
}

void KeyboardLayout::buildYavertyLayout()
{
    // Ряд 0 — цифры как у йцукен
    addKey("ё",  0,  0, 5, 0); addKey("1",  0,  1, 5, 0); addKey("2",  0,  2, 4, 0);
    addKey("3",  0,  3, 3, 0); addKey("4",  0,  4, 2, 0); addKey("5",  0,  5, 2, 0);
    addKey("6",  0,  6, 9, 1); addKey("7",  0,  7, 9, 1); addKey("8",  0,  8, 8, 1);
    addKey("9",  0,  9, 7, 1); addKey("0",  0, 10, 6, 1); addKey("-",  0, 11, 6, 1);
    addKey("=",  0, 12, 6, 1); addKey("backspace", 0, 13, 6, 1);
    // Ряд 1
    addKey("tab",    1,  0, 5, 0);
    addKey("я",      1,  1, 5, 0); addKey("в",  1,  2, 4, 0); addKey("е",  1,  3, 3, 0);
    addKey("р",      1,  4, 2, 0); addKey("т",  1,  5, 2, 0);
    addKey("ы",      1,  6, 9, 1); addKey("у",  1,  7, 9, 1); addKey("и",  1,  8, 8, 1);
    addKey("о",      1,  9, 7, 1); addKey("п",  1, 10, 6, 1);
    addKey("ш",      1, 11, 6, 1); addKey("щ",  1, 12, 6, 1); addKey("\\", 1, 13, 6, 1);
    // Ряд 2
    addKey("caps",   2,  0, 5, 0);
    addKey("а",      2,  1, 5, 0); addKey("ф",  2,  2, 4, 0); addKey("г",  2,  3, 3, 0);
    addKey("д",      2,  4, 2, 0); addKey("й",  2,  5, 2, 0);
    addKey("н",      2,  6, 9, 1); addKey("л",  2,  7, 9, 1); addKey("ж",  2,  8, 8, 1);
    addKey("к",      2,  9, 7, 1); addKey("з",  2, 10, 6, 1);
    addKey("э",      2, 11, 6, 1); addKey("enter", 2, 12, 6, 1);
    // Ряд 3
    addKey("shiftl", 3,  0, 5, 0);
    addKey("ю",      3,  1, 5, 0); addKey("х",  3,  2, 4, 0); addKey("ъ",  3,  3, 3, 0);
    addKey("с",      3,  4, 2, 0); addKey("м",  3,  5, 2, 0);
    addKey("ц",      3,  6, 9, 1); addKey("б",  3,  7, 9, 1); addKey("ч",  3,  8, 8, 1);
    addKey("ь",      3,  9, 7, 1); addKey(",",  3, 10, 6, 1);
    addKey("shiftr", 3, 11, 6, 1);
    // Ряд 4
    addKey("ctrll",  4, 0, 5, 0);
    addKey("winl",   4, 1, 5, 0);
    addKey("altl",   4, 2, 5, 0);
    addKey("space",  4, 3, 1, 0);
    addKey("altr",   4, 4, 6, 1);
    addKey("winr",   4, 5, 6, 1);
    addKey("menur",  4, 6, 6, 1);
    addKey("ctrlr",  4, 7, 6, 1);
}

void KeyboardLayout::buildRuMacLayout()
{
    buildYcukenLayout();
    m_keys.removeIf([](const KeyInfo &key) {
        return key.keyId == QStringLiteral("ё");
    });
}

void KeyboardLayout::addShiftChar(const QString &shiftChar, const QString &baseKeyId)
{
    // Регистрируем shift-символ → базовая клавиша
    m_shiftCharToBaseKey[shiftChar] = baseKeyId;
    // И добавляем в charToKeyIndex чтобы fingerForChar/handForChar работали
    auto it = m_charToKeyIndex.find(baseKeyId.toLower());
    if (it != m_charToKeyIndex.end())
        m_charToKeyIndex[shiftChar] = it.value();
}

void KeyboardLayout::buildQwertyShiftMap()
{
    // Ряд 0
    addShiftChar("~", "`");
    addShiftChar("!", "1"); addShiftChar("@", "2"); addShiftChar("#", "3");
    addShiftChar("$", "4"); addShiftChar("%", "5"); addShiftChar("^", "6");
    addShiftChar("&", "7"); addShiftChar("*", "8"); addShiftChar("(", "9");
    addShiftChar(")", "0"); addShiftChar("_", "-"); addShiftChar("+", "=");
    // Ряд 1
    addShiftChar("{", "["); addShiftChar("}", "]"); addShiftChar("|", "\\");
    // Ряд 2
    addShiftChar(":", ";"); addShiftChar("\"", "'");
    // Ряд 3
    addShiftChar("<", ","); addShiftChar(">", "."); addShiftChar("?", "/");

    // ВРЕМЕННО — дебаг
    qDebug() << "shiftMap check:";
    qDebug() << ": baseKey =" << baseKeyForChar(":");
    qDebug() << "? baseKey =" << baseKeyForChar("?");
    qDebug() << "! baseKey =" << baseKeyForChar("!");
    qDebug() << "; baseKey =" << baseKeyForChar(";");
    qDebug() << "charToKeyIndex has : ?" << m_charToKeyIndex.contains(":");
    qDebug() << "charToKeyIndex has ? ?" << m_charToKeyIndex.contains("?");
}

void KeyboardLayout::buildYcukenShiftMap()
{
    addShiftChar("!", "1");
    addShiftChar("\"", "2");
    addShiftChar("№", "3");
    addShiftChar(";", "4");   // ← ; это Shift+4 в русской
    addShiftChar("%", "5");
    addShiftChar(":", "6");   // ← : это Shift+6 в русской (не Shift+;!)
    addShiftChar("?", "7");   // ← ? это Shift+7 в русской
    addShiftChar("*", "8");
    addShiftChar("(", "9");
    addShiftChar(")", "0");
    addShiftChar("_", "-");
    addShiftChar("+", "=");
    // Буквенные ряды
    addShiftChar(",", ".");  // Shift+. = запятая
}
