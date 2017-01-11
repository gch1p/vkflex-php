<?php

/**
 * @param string $name
 * @param string $case
 * @param int $sex: 0 for male, 1 for female
 * @param int $type: 0 for names, 1 for surnames
 */
function flex($name, $case, $sex, $type) {
    global $lang;
    $name = iconv('utf-8', 'cp1251', $name);
    $result = vkflex($name, $case, $sex, $lang, $type);
    return iconv('cp1251', 'utf-8', $result);
}

dl('vkflex.so');
$names = ['0 Евгений Зиновьев', '0 Владимир Путин', '1 Катя Лебедева', '1 Анастасия Бабич'];
$lang = 0; // ru
$case = 'Gen';

foreach ($names as $name) {
    list($sex, $name, $surname) = explode(' ', $name);
    $sex = (int)$sex;
    echo sprintf("%s %s\n", flex($name, $case, $sex, 0), flex($surname, $case, $sex, 1));
}
