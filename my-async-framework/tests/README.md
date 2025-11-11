# Тесты MyAsyncFramework

Этот каталог содержит тесты для библиотеки MyAsyncFramework.

## Сборка и запуск тестов

### Сборка библиотеки без тестов (по умолчанию)

```bash
cd /Users/stepan-karpov/Desktop/my-server/my-async-framework
cmake -B build
cmake --build build
```

### Сборка библиотеки с тестами

```bash
cd /Users/stepan-karpov/Desktop/my-server/my-async-framework
cmake -B build -DBUILD_TESTING=ON
cmake --build build
```

### Запуск тестов

После сборки с включенными тестами:

```bash
cd build

# Запустить все тесты
ctest

# Запустить с подробным выводом
ctest --verbose

# Запустить конкретный тест
ctest -R MutexTest

# Запустить тесты параллельно
ctest -j 4
```

### Запуск тестов вручную

```bash
cd build
./test-mutex
./test-condition-variable
./test-unbounded-mpmc-queue
./test-thread-pool
```

### Опции сборки

#### Уровень логирования

```bash
cmake -B build -DBUILD_TESTING=ON -DLOG_LEVEL=DEBUG
cmake --build build
```

Доступные уровни: `DEBUG`, `WARNING`, `INFO`, `ERROR`

#### Санитайзеры

Для включения санитайзеров (AddressSanitizer, UndefinedBehaviorSanitizer):

```bash
cmake -B build -DBUILD_TESTING=ON -DSANITIZER=address,undefined
cmake --build build
```

### Полный пример

```bash
# Собрать библиотеку с тестами и санитайзерами
cd /Users/stepan-karpov/Desktop/my-server/my-async-framework
cmake -B build -DBUILD_TESTING=ON -DLOG_LEVEL=DEBUG -DSANITIZER=address,undefined
cmake --build build

# Запустить тесты
cd build
ctest --verbose
```

### Структура тестов

- `sync/test_mutex.cpp` - тесты для мьютекса
- `sync/test_condition_variable.cpp` - тесты для condition variable
- `sync/queues/unbounded_mpmc_queue_test.cpp` - тесты для очереди
- `scheduling/test_thread_pool.cpp` - тесты для thread pool
