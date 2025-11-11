# MyAsyncFramework Tests

This directory contains tests for the MyAsyncFramework library.

## Building and Running Tests

### Building the Library Without Tests (default)

```bash
cd my-async-framework
cmake -S . -B build
cmake --build build
```

### Building the Library With Tests

```bash
cd my-async-framework
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

### Running Tests

After building with tests enabled:

```bash
cd build

# Run all tests
ctest

# Run with verbose output
ctest --verbose

# Run a specific test
ctest -R MutexTest

# Run tests in parallel
ctest -j 4
```

### Running Tests Manually

```bash
cd build
./test-mutex
./test-condition-variable
./test-unbounded-mpmc-queue
./test-thread-pool
```

### Build Options

#### Log Level

```bash
cmake -S . -B build -DBUILD_TESTING=ON -DLOG_LEVEL=DEBUG
cmake --build build
```

Available levels: `DEBUG`, `WARNING`, `INFO`, `ERROR`

#### Sanitizers

To enable sanitizers (AddressSanitizer, UndefinedBehaviorSanitizer):

```bash
cmake -S . -B build -DBUILD_TESTING=ON -DSANITIZER=address,undefined
cmake --build build
```

### Complete Example

```bash
# Build library with tests and sanitizers
cd my-async-framework
cmake -S . -B build -DBUILD_TESTING=ON -DLOG_LEVEL=DEBUG -DSANITIZER=address,undefined
cmake --build build

# Run tests
cd build
ctest --verbose
```

## Test Structure

- `sync/test_mutex.cpp` - tests for mutex
- `sync/test_condition_variable.cpp` - tests for condition variable
- `sync/queues/unbounded_mpmc_queue_test.cpp` - tests for queue
- `scheduling/test_thread_pool.cpp` - tests for thread pool

## Dependencies

Tests use GoogleTest (v1.14.0), which is automatically downloaded via CMake FetchContent when `BUILD_TESTING=ON` is enabled.
