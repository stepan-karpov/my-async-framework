#include <iostream>
#include <thread>
#include <csignal>   // For signal, SIGINT
#include <chrono>
#include <pthread.h> // For pthread_kill

static volatile sig_atomic_t keep_running = 1;

void sigint_handler(int signum) {
    std::cout << "\nSignal handler called in thread " << std::this_thread::get_id() << std::endl;
    keep_running = 0;
}

void thread_function() {
    std::cout << "Worker thread " << std::this_thread::get_id() << " started." << std::endl;
    while (keep_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << ".";
        std::cout.flush();
    }
    std::cout << "\nWorker thread " << std::this_thread::get_id() << " finished." << std::endl;
}

int main() {
    signal(SIGINT, sigint_handler);

    std::thread worker_thread(thread_function);

    pthread_t thread_handle = worker_thread.native_handle();

    std::cout << "Main thread. Waiting 3 seconds before sending SIGINT..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "\nSending SIGINT to worker thread..." << std::endl;
    int result = pthread_cancel(thread_handle);

    if (result != 0) {
        std::cerr << "Error sending signal: " << result << std::endl;
    }
    worker_thread.join();

    std::cout << "Main thread finished." << std::endl;

    return 0;
}
