#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <atomic>
#include <memory>

std::vector<int> array;
int markerCount = 0;

class ThreadController {
public:
    ThreadController() : cannotContinue(false), shouldContinue(false),
        shouldTerminate(false), markedElements(0), active(true) {}

    std::thread thread;
    std::mutex mtx;
    std::condition_variable cv;
    bool cannotContinue;
    bool shouldContinue;
    bool shouldTerminate;
    int markedElements;
    bool active;

    ThreadController(const ThreadController&) = delete;
    ThreadController& operator=(const ThreadController&) = delete;

    ThreadController(ThreadController&&) = default;
    ThreadController& operator=(ThreadController&&) = default;
};

std::vector<std::unique_ptr<ThreadController>> threadControllers;

std::mutex arrayMutex;

std::mutex startMutex;
std::condition_variable startCV;
bool shouldStart = false;

std::mutex mainMutex;
std::condition_variable mainCV;
std::atomic<int> threadsWaiting(0);

void PrintArray() {
    std::lock_guard<std::mutex> lock(arrayMutex);
    std::cout << "Array content: ";
    for (int val : array) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

void MarkerThreadProc(int threadIdx) {
    int threadNum = threadIdx + 1;

    {
        std::unique_lock<std::mutex> lock(startMutex);
        startCV.wait(lock, [] { return shouldStart; });
    }

    std::mt19937 rng(static_cast<unsigned int>(threadNum));

    while (true) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(array.size() - 1));
        int randomIdx = dist(rng);

        bool canMark = false;
        {
            std::unique_lock<std::mutex> lock(arrayMutex);

            if (array[randomIdx] == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                array[randomIdx] = threadNum;
                threadControllers[threadIdx]->markedElements++;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                canMark = true;
            }
        }

        if (!canMark) {
            std::cout << "Thread " << threadNum << " info:" << std::endl;
            std::cout << "- Thread number: " << threadNum << std::endl;
            std::cout << "- Marked elements: " << threadControllers[threadIdx]->markedElements << std::endl;
            std::cout << "- Can't mark element at index: " << randomIdx << std::endl;

            {
                std::unique_lock<std::mutex> lock(threadControllers[threadIdx]->mtx);
                threadControllers[threadIdx]->cannotContinue = true;
                threadsWaiting++;

                if (threadsWaiting.load() == std::count_if(threadControllers.begin(), threadControllers.end(),
                    [](const std::unique_ptr<ThreadController>& tc) { return tc->active; })) {
                    mainCV.notify_one();
                }

                threadControllers[threadIdx]->cv.wait(lock, [threadIdx] {
                    return threadControllers[threadIdx]->shouldContinue ||
                        threadControllers[threadIdx]->shouldTerminate;
                    });

                if (threadControllers[threadIdx]->shouldContinue) {
                    threadControllers[threadIdx]->shouldContinue = false;
                    threadControllers[threadIdx]->cannotContinue = false;
                }

                if (threadControllers[threadIdx]->shouldTerminate) {
                    std::lock_guard<std::mutex> arrayLock(arrayMutex);
                    for (size_t i = 0; i < array.size(); i++) {
                        if (array[i] == threadNum) {
                            array[i] = 0;
                        }
                    }
                    return;
                }
            }
        }
    }
}

int main() {
    int arraySize;
    std::cout << "Enter array size: ";
    std::cin >> arraySize;

    array.resize(arraySize, 0);

    std::cout << "Enter number of marker threads: ";
    std::cin >> markerCount;

    for (int i = 0; i < markerCount; i++) {
        threadControllers.push_back(std::make_unique<ThreadController>());
    }

    for (int i = 0; i < markerCount; i++) {
        threadControllers[i]->thread = std::thread(MarkerThreadProc, i);
    }

    {
        std::lock_guard<std::mutex> lock(startMutex);
        shouldStart = true;
    }
    startCV.notify_all();

    int activeThreads = markerCount;
    while (activeThreads > 0) {
        {
            std::unique_lock<std::mutex> lock(mainMutex);
            mainCV.wait(lock, [&] {
                return threadsWaiting.load() ==
                    std::count_if(threadControllers.begin(), threadControllers.end(),
                        [](const std::unique_ptr<ThreadController>& tc) { return tc->active; });
                });
            threadsWaiting = 0;
        }

        PrintArray();

        int threadToTerminate;
        std::cout << "Enter thread number to terminate (1-" << markerCount << "): ";
        std::cin >> threadToTerminate;

        threadToTerminate--;

        if (threadToTerminate >= 0 && threadToTerminate < markerCount &&
            threadControllers[threadToTerminate]->active) {
                {
                    std::lock_guard<std::mutex> lock(threadControllers[threadToTerminate]->mtx);
                    threadControllers[threadToTerminate]->shouldTerminate = true;
                }
                threadControllers[threadToTerminate]->cv.notify_one();

                threadControllers[threadToTerminate]->thread.join();

                threadControllers[threadToTerminate]->active = false;
                activeThreads--;

                PrintArray();

                for (int i = 0; i < markerCount; i++) {
                    if (threadControllers[i]->active) {
                        {
                            std::lock_guard<std::mutex> lock(threadControllers[i]->mtx);
                            threadControllers[i]->shouldContinue = true;
                        }
                        threadControllers[i]->cv.notify_one();
                    }
                }
        }
        else {
            std::cout << "Invalid thread number or thread already terminated" << std::endl;
            exit(1);

            for (int i = 0; i < markerCount; i++) {
                if (threadControllers[i]->active) {
                    {
                        std::lock_guard<std::mutex> lock(threadControllers[i]->mtx);
                        threadControllers[i]->shouldContinue = true;
                    }
                    threadControllers[i]->cv.notify_one();
                }
            }
        }
    }

    std::cout << "All threads terminated. Program completed." << std::endl;
    return 0;
}