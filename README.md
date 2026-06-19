# ConsoleCalc

Консольный калькулятор с поддержкой базовых арифметических операций, кешированием результатов в PostgreSQL и клиент-серверной архитектурой на основе Boost.Asio.

## Возможности

- Сложение, вычитание, умножение, деление
- Возведение в степень
- Факториал
- Проверка переполнения для всех операций
- Кеширование результатов в PostgreSQL и в памяти
- Работа как systemd-сервис
- Сетевой интерфейс: приём заданий по TCP, возврат результата в JSON

## Требования

- CMake 3.22+
- GCC 11+ или Clang 13+
- PostgreSQL + libpq-dev
- Boost (заголовочные файлы)

## Сборка

### Базовая сборка

```bash
git clone git@github.com:AnastasiaVeselovskaya/ConsoleCalc.git
cd ConsoleCalc
cmake -B build
cmake --build build
```

Результат: `build/bin/consolecalc` и `build/bin/consolecalc_tests`.

### С конкретным типом сборки

```bash
# Debug (с отладочными символами, без оптимизации)
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Release (с оптимизацией без символов отладки)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# RelWithDebInfo (используется по умолчанию, с оптимизацией, но без символов отладки)
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

### Установка

```bash
sudo cmake --build build --target install
```

## Использование

Сервис принимает задания по TCP в формате JSON и возвращает результат.

Запуск вручную:

```bash
build/bin/consolecalc --config /etc/consolecalc/consolecalc.conf
```

Отправка запроса (например, через netcat):

```bash
echo '{"left operand":5, "operation":"+", "right operand":3}' | nc -q 127.0.0.1 8080
```

Поддерживаемые операции: `+` `-` `*` `/` `^` `!`

Унарные операции (`!`) используют только левый операнд.

Формат ответа:

```json
{"result": 8}
{"error": "Division by zero"}
```

## Запуск как systemd-сервис

```bash
sudo systemctl start consolecalc
sudo systemctl stop consolecalc
sudo systemctl restart consolecalc
sudo systemctl status consolecalc
```

## Тесты

```bash
cmake --build build
./build/bin/consolecalc_tests

# Конкретный тест:
./build/bin/consolecalc_tests --gtest_filter=ApplicationTest.FactorialPrintsResult
```

### Valgrind

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
make -C build test_memcheck
```

### ThreadSanitizer

```bash
cmake --build build
make -C build test_tsan
```

TSan-цель собирает отдельный бинарник с флагами `-fsanitize=thread` и запускает его через `setarch -R` для отключения ASLR. Отсутствие предупреждений в выводе означает что гонок данных не обнаружено.

### Clang-format

```bash
# Проверка (не меняет файлы):
cmake -B build -DUSE_CLANG_FORMAT_CHECK=ON
cmake --build build --target clang-format

# Автоисправление:
cmake -B build -DUSE_CLANG_FORMAT_FIX=ON
cmake --build build --target clang-format-fix
```

### Clang-tidy

```bash
cmake -B build -DUSE_CLANG_TIDY=ON
cmake --build build
```

Замечания clang-tidy появятся во время сборки.

## deb-пакет

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build && cpack -G DEB
```

Результат: `consolecalc-1.0.0-Linux.deb`

```bash
# Установка:
sudo dpkg -i ConsoleCalc-1.0.0-Linux.deb

# Удаление:
sudo dpkg -r consolecalc
```

## Справочник команд

| Цель | Команда |
|---|---|
| Обычная сборка | `cmake -B build && cmake --build build` |
| Тесты | `sudo ./build/bin/consolecalc_tests` |
| Valgrind | `sudo make -C build test_memcheck` |
| TSan | `sudo make -C build test_tsan` |
| Clang-format проверка | `cmake -B build -DUSE_CLANG_FORMAT_CHECK=ON && cmake --build build --target clang-format` |
| Clang-format fix | `cmake -B build -DUSE_CLANG_FORMAT_FIX=ON && cmake --build build --target clang-format-fix` |
| Clang-tidy | `cmake -B build -DUSE_CLANG_TIDY=ON && cmake --build build` |
| deb-пакет | `cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build && cd build && cpack -G DEB` |