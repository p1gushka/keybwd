# keybwd — клавиатурный тренажер для слепой печати

## Цели проекта
- Создать универсальный инструмент для обучения слепой печати
- Предоставить возможность соревноваться с другими пользователями
- Автоматизировать анализ ошибок для персонализированного обучения

## Используемые библиотеки
- POCO Libraries
- PostgreSQL
- nlohmann/json
- Qt Framework
- Google Test
- Cmake

## Архитектура проекта
- Клиент - Сервер - Бд 

## Функционал
- На время (10/15/30/60 сек)

<img src=screenshots/time15.png width=50%>

- По количеству слов (10/25/50/100)

<img src=screenshots/words10.png width=50%>

- Цитаты (короткие/средние/длинные)

<img src=screenshots/quotes_medium.png width=50%>

- Написание кода (C++/Python/Java/JavaScript)

<img src=screenshots/code_cpp.png width=50%>

- Пользовательский текст

## Сборка и установка (на Linux)
1. Склонируйте репозиторий

```bash
git clone git@github.com:p1gushka/keybwd.git
```

2. Перейдите в корневую папку провекта

```bash
cd keybwd
```

3. Соберите клиентскую часть с помощью Cmake

```
rm -rf build && mkdir build && cd build
cmake ..
cmake --build .
```


## Лицензия
MIT License — подробности в файле [LICENSE](LICENSE). 

## Авторы
- [`Баранов Александр`](https://github.com/p1gushka)
- [`Басков Сергей`](https://github.com/men229)
- [`Ведянин Даниил`](https://github.com/Kubirchik)
