#include <iostream>
#include <future>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

using namespace std;

const int NR_THREADS = std::thread::hardware_concurrency();

std::future<void> compute(const vector<vector<int>>& first, const vector<vector<int>>& second, vector<vector<long long>>& res, int line_start, int col_start, int nr_elems_per_thread) {
    return std::async(std::launch::async, [&first, &second, &res](int line_start, int col_start, int nr_elems_per_thread) {
        long long s = 0;

        for (int i = line_start; i < first.size(); ++i) {
            if (nr_elems_per_thread > 0) {
                for (int j = col_start; j < first.size(); ++j) {
                    if (nr_elems_per_thread == 0 || i > first.size() - 1 || j > first.size() - 1) {
                        nr_elems_per_thread = 0;
                        break;
                    }

                    s = 0;

                    for (int k = 0; k < second.size(); ++k) {
                        s += first[i][k] * second[k][j];
                    }

                    res[i][j] = s;

                    nr_elems_per_thread -= 1;
                    col_start = 0;
                }
            }
        }
    }, line_start, col_start, nr_elems_per_thread);
}

void compute_pool(const vector<vector<int>>& first, const vector<vector<int>>& second, vector<vector<long long>>& res, int line_start, int col_start, int nr_elems_per_thread) {
    long long s = 0;

    for (int i = line_start; i < first.size(); ++i) {
        if (nr_elems_per_thread > 0) {
            for (int j = col_start; j < first.size(); ++j) {
                if (nr_elems_per_thread == 0 || i > first.size() - 1 || j > first.size() - 1) {
                    nr_elems_per_thread = 0;
                    break;
                }

                s = 0;

                for (int k = 0; k < second.size(); ++k) {
                    s += first[i][k] * second[k][j];
                }

                res[i][j] = s;

                nr_elems_per_thread -= 1;
                col_start = 0;
            }
        }
    }
}

std::future<void> compute_col(const vector<vector<int>>& first, const vector<vector<int>>& second, vector<vector<long long>>& res, int line_start, int col_start, int nr_elems_per_thread) {
    return std::async(std::launch::async, [&first, &second, &res](int line_start, int col_start, int nr_elems_per_thread) {
        long long s = 0;

        for (int i = col_start; i < first.size(); ++i) {
            if (nr_elems_per_thread > 0) {
                for (int j = line_start; j < first.size(); ++j) {
                    if (nr_elems_per_thread == 0 || i > first.size() - 1 || j > first.size() - 1) {
                        nr_elems_per_thread = 0;
                        break;
                    }

                    s = 0;

                    for (int k = 0; k < second.size(); ++k) {
                        s += first[j][k] * second[k][i];
                    }

                    res[j][i] = s;

                    nr_elems_per_thread -= 1;
                    line_start = 0;
                }
            }
        }
    }, line_start, col_start, nr_elems_per_thread);
}

void compute_col_pool(const vector<vector<int>>& first, const vector<vector<int>>& second, vector<vector<long long>>& res, int line_start, int col_start, int nr_elems_per_thread) {
    long long s = 0;

    for (int i = col_start; i < first.size(); ++i) {
        if (nr_elems_per_thread > 0) {
            for (int j = line_start; j < first.size(); ++j) {
                if (nr_elems_per_thread == 0 || i > first.size() - 1 || j > first.size() - 1) {
                    nr_elems_per_thread = 0;
                    break;
                }

                s = 0;

                for (int k = 0; k < second.size(); ++k) {
                    s += first[j][k] * second[k][i];
                }

                res[j][i] = s;

                nr_elems_per_thread -= 1;
                line_start = 0;
            }
        }
    }
}

std::future<void> compute_jump(const vector<vector<int>>& first, const vector<vector<int>>& second, vector<vector<long long>>& res, int line_start, int col_start, int nr_elems_per_thread) {
    return std::async(std::launch::async, [&first, &second, &res](int line_start, int col_start, int nr_elems_per_thread) {
        int current_line = line_start;
        int current_col = col_start;
        int s;

        while (current_line < first.size() && current_col < first.size()) {
            s = 0;

            for (int k = 0; k < second.size(); ++k) {
                s += first[current_line][k] * second[k][current_col];
            }
            
            res[current_line][current_col] = s;

            current_col += nr_elems_per_thread;
            
            if (current_col > first.size() - 1) {
                current_line += 1;
                current_col = current_col % first.size();
            }
        }
    }, line_start, col_start, nr_elems_per_thread);
}

void compute_jump_pool(const vector<vector<int>>& first, const vector<vector<int>>& second, vector<vector<long long>>& res, int line_start, int col_start, int nr_elems_per_thread) {
    int current_line = line_start;
    int current_col = col_start;
    int s;

    while (current_line < first.size() && current_col < first.size()) {
        s = 0;

        for (int k = 0; k < second.size(); ++k) {
            s += first[current_line][k] * second[k][current_col];
        }

        res[current_line][current_col] = s;

        current_col += nr_elems_per_thread;

        if (current_col > first.size() - 1) {
            current_line += 1;
            current_col = current_col % first.size();
        }
    }
}

int main() {
    vector<vector<int>> A{ { 1, 2, 1, 2 }, { 1, 2, 1, 2 }, { 1, 2, 1, 2 } };
    vector<vector<int>> B{ { 1, 2, 1 }, { 1, 2, 1 }, { 1, 2, 1 }, { 1, 2, 1 } };

    int test_type;

    cin >> test_type;

    if (test_type == 1) {
        A.clear();
        B.clear();

        int nr_cols_A = 1000;
        int nr_rows_A = 1000;
        int nr_rows_B = 1000;

        A.resize(nr_rows_A);

        for (auto& el : A) {
            el.resize(nr_cols_A);
        }

        B.resize(nr_rows_B);

        for (auto& el : B) {
            el.resize(nr_rows_A);
        }
    }

    vector<std::future<void>> futures;

    boost::asio::thread_pool pool_1(std::thread::hardware_concurrency());
    boost::asio::thread_pool pool_2(std::thread::hardware_concurrency());
    boost::asio::thread_pool pool_3(std::thread::hardware_concurrency());

    futures.reserve(NR_THREADS);

    int nr_elems = A.size() * B.size();

    vector<vector<long long>> C;
    C.resize(A.size());

    for (auto& el : C) {
        el.resize(A.size());
    }

    int nr_elems_per_thread = nr_elems / NR_THREADS + 1;
    int line_start = 0;
    int col_start = 0;

    int type;

    cin >> type;

    int res = 0;

    if (type == 0) {
#pragma region Compute product matrix row by row

        auto start = chrono::high_resolution_clock::now();

        for (;;) {
            futures.push_back(compute(A, B, C, line_start, col_start, nr_elems_per_thread));
            line_start += (col_start + nr_elems_per_thread) / A.size();
            col_start = (col_start + nr_elems_per_thread) % A.size();

            if (line_start > A.size() - 1) {
                break;
            }
        }

        for (auto& f : futures) {
            f.get();
        }

        auto end = chrono::high_resolution_clock::now();

        auto time_per_rows = chrono::duration_cast<chrono::milliseconds>(end - start).count();

       /*for (int i = 0; i < C.size(); ++i) {
            for (int j = 0; j < C[i].size(); ++j) {
                cout << C[i][j] << " ";
            }
            cout << '\n';
        }
        cout << '\n';*/

#pragma endregion

        futures.clear();
        C.clear();
        C.resize(A.size());

        for (auto& el : C) {
            el.resize(A.size());
        }

#pragma region Compute product matrix column by column

        line_start = 0;
        col_start = 0;

        start = chrono::high_resolution_clock::now();

        for (;;) {
            futures.push_back(compute_col(A, B, C, line_start, col_start, nr_elems_per_thread));
            col_start += (line_start + nr_elems_per_thread) / B.size();
            line_start = (line_start + nr_elems_per_thread) % B.size();

            if (col_start > B.size() - 1) {
                break;
            }
        }

        for (auto& f : futures) {
            f.get();
        }

        end = chrono::high_resolution_clock::now();

        auto time_per_cols = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        /*for (int i = 0; i < C.size(); ++i) {
            for (int j = 0; j < C[i].size(); ++j) {
                cout << C[i][j] << " ";
            }
            cout << '\n';
        }
        cout << '\n';*/

#pragma endregion

        futures.clear();
        C.clear();
        C.resize(A.size());

        for (auto& el : C) {
            el.resize(A.size());
        }

#pragma region Compute product matrix jumping

        line_start = 0;
        col_start = 0;

        start = chrono::high_resolution_clock::now();

        while (nr_elems_per_thread > 0) {
            if (col_start > A.size() - 1) {
                col_start = 0;
                line_start += 1;
            }

            futures.push_back(compute_jump(A, B, C, line_start, col_start, nr_elems_per_thread));

            col_start += 1;
            nr_elems_per_thread -= 1;
        }

        for (auto& f : futures) {
            f.get();
        }

        end = chrono::high_resolution_clock::now();

        auto time_per_jumps = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        /*for (int i = 0; i < C.size(); ++i) {
            for (int j = 0; j < C[i].size(); ++j) {
                cout << C[i][j] << " ";
            }
            cout << "\n";
        }*/

#pragma endregion

        cout << '\n' << "Iterating row by row time: " << time_per_rows << '\n';
        cout << '\n' << "Iterating column by column time: " << time_per_cols << '\n';
        cout << '\n' << "Iterating jumping time: " << time_per_jumps << "\n\n";

    }
    else {
        C.clear();
        C.resize(A.size());

        for (auto& el : C) {
            el.resize(A.size());
        }

#pragma region Compute product matrix row by row using thread_pool

        nr_elems_per_thread = nr_elems / NR_THREADS + 1;
        line_start = 0;
        col_start = 0;

        auto start = chrono::high_resolution_clock::now();

        for (;;) {
            boost::asio::post(pool_1, [&A, &B, &C, line_start, col_start, nr_elems_per_thread] { compute_pool(A, B, C, line_start, col_start, nr_elems_per_thread); });
            line_start += (col_start + nr_elems_per_thread) / A.size();
            col_start = (col_start + nr_elems_per_thread) % A.size();

            if (line_start > A.size() - 1) {
                break;
            }
        }

        pool_1.join();

        auto end = chrono::high_resolution_clock::now();

        auto time_per_rows = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        /*for (int i = 0; i < C.size(); ++i) {
            for (int j = 0; j < C[i].size(); ++j) {
                cout << C[i][j] << " ";
            }
            cout << '\n';
        }
        cout << '\n';*/

#pragma endregion

        C.clear();
        C.resize(A.size());

        for (auto& el : C) {
            el.resize(A.size());
        }

#pragma region Compute product matrix column by column using thread_pool

        line_start = 0;
        col_start = 0;

        start = chrono::high_resolution_clock::now();

        for (;;) {
            boost::asio::post(pool_2, [&A, &B, &C, line_start, col_start, nr_elems_per_thread] { compute_col_pool(A, B, C, line_start, col_start, nr_elems_per_thread); });
            col_start += (line_start + nr_elems_per_thread) / B.size();
            line_start = (line_start + nr_elems_per_thread) % B.size();

            if (col_start > B.size() - 1) {
                break;
            }
        }

        pool_2.wait();

        end = chrono::high_resolution_clock::now();

        auto time_per_cols = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        /*for (int i = 0; i < C.size(); ++i) {
            for (int j = 0; j < C[i].size(); ++j) {
                cout << C[i][j] << " ";
            }
            cout << '\n';
        }
        cout << '\n';*/

#pragma endregion

        C.clear();
        C.resize(A.size());

        for (auto& el : C) {
            el.resize(A.size());
        }

#pragma region Compute product matrix jumping

        line_start = 0;
        col_start = 0;

        start = chrono::high_resolution_clock::now();

        while (nr_elems_per_thread > 0) {
            if (col_start > A.size() - 1) {
                col_start = 0;
                line_start += 1;
            }

            boost::asio::post(pool_3, [&A, &B, &C, line_start, col_start, nr_elems_per_thread] { compute_jump_pool(A, B, C, line_start, col_start, nr_elems_per_thread); });

            col_start += 1;
            nr_elems_per_thread -= 1;
        }

        pool_3.join();

        end = chrono::high_resolution_clock::now();

        auto time_per_jumps = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        /*for (int i = 0; i < C.size(); ++i) {
            for (int j = 0; j < C[i].size(); ++j) {
                cout << C[i][j] << " ";
            }
            cout << "\n";
        }*/

#pragma endregion

        cout << '\n' << "Iterating row by row time: " << time_per_rows << '\n';
        cout << '\n' << "Iterating column by column time: " << time_per_cols << '\n';
        cout << '\n' << "Iterating jumping time: " << time_per_jumps << "\n\n";
    }

    return 0;
}