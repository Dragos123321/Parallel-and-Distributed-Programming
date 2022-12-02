//#include <iostream>
//#include <thread>
//#include <mutex>
//#include <chrono>
//#include <queue>
//
//int main() {
//    std::condition_variable cond_var;
//    std::mutex mx;
//
//    int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
//    int b[] = { -2, 5, 6, 10, 20, 2, 4, 5, 11, 10 };
//    int size = 10;
//
//    size_t index = 0;
//    long long s = 0;
//
//    std::queue<int> prods;
//
//    if (size == 0)
//        return 0;
//
//    std::thread producer([&]() {
//        while (index < size) {
//            std::unique_lock<std::mutex> lock{ mx };
//
//            prods.push(a[index] * b[index]);
//            ++index;
//
//            lock.unlock();
//            cond_var.notify_one();
//        }
//        });
//
//    std::thread consumer([&]() {
//        while (index < size) {
//            std::unique_lock<std::mutex> lock{ mx };
//
//            cond_var.wait(lock, [&]() {
//                return prods.size() > 0;
//                });
//
//            while (prods.size() > 0) {
//                s += prods.front();
//                prods.pop();
//            }
//
//            lock.unlock();
//        }
//        });
//
//    producer.join();
//    consumer.join();
//
//    std::cout << s << "\n";
//}
