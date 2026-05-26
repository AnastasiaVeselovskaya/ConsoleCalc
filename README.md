# ConsoleCalc

Консольный калькулятор с поддержкой базовых арифметических операций и проверкой переполнения.

## Возможности

- Сложение, вычитание, умножение, деление
- Возведение в степень
- Факториал
- Проверка переполнения для всех операций

## Сборка

```bash
git clone git@github.com:AnastasiaVeselovskaya/ConsoleCalc.git
cd ConsoleCalc
cmake -B build
cmake --build build
sudo cmake --build build --target install
```

## Использование

Аргумент передаётся как JSON-строка:

```bash
build/bin/consolecalc '{"left operand":5, "operation":"+", "right operand":3}'
build/bin/consolecalc '{"left operand":10, "operation":"/", "right operand":3}'
build/bin/consolecalc '{"left operand":2, "operation":"^", "right operand":8}'
build/bin/consolecalc '{"left operand":5, "operation":"!"}'
```

Поддерживаемые операции: `+` `-` `*` `/` `^` `!`

Унарные операции (`!`) используют только левый операнд.

## Тесты

Сборка и запуск:

```bash
cmake -B build
cmake --build build --target consolecalc_tests
./build/bin/consolecalc_tests
```