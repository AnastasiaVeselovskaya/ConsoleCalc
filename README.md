# ConsoleCalc

Консольный калькулятор с поддержкой базовых арифметических операций и проверкой переполнения.

## Возможности

- Сложение, вычитание, умножение, деление
- Возведение в степень (включая отрицательную)
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

С флагами:

```bash
build/bin/consolecalc --help
build/bin/consolecalc -l 2 -o ^ -r 8
build/bin/consolecalc -l 5 -o !
build/bin/consolecalc --left-number 10 --operation '/' --right-number 3
build/bin/consolecalc --left-number 9 --operation !
```

Позиционные аргументы:
```bash
build/bin/consolecalc 5 + 3
build/bin/consolecalc 10 / 3
build/bin/consolecalc 2 ^ 8
build/bin/consolecalc 5 !
```
