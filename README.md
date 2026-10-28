# PS Vita Cube Demo

Простой пример для PS Vita: 3D-куб, который можно вращать.

## Что делает
- рисует куб в 3D
- вращает его вокруг осей X/Y
- управление через левый и правый стики
- собирается через `vitasdk`

## Требования
- `vitasdk`
- `vita2d`
- CMake

## Сборка

```bash
export VITASDK=/path/to/vitasdk
export PATH="$VITASDK/bin:$PATH"

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Для запуска на PS Vita используйте нормальный процесс сборки `vitasdk`/`vita2d` и затем установите полученный пакет через VitaShell или SD2Vita.

## Управление
- левый стик — вращение куба
- можно добавить кнопки `UP/DOWN/LEFT/RIGHT` при необходимости

## Файлы проекта
- `main.c` — основной код
- `CMakeLists.txt` — настройки сборки
- `.gitignore` — исключения для артефактов сборки
