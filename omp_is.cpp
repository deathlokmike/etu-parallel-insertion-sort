#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

using namespace std;

// инициалазация массива псевдорандомными данными
vector<int> random_initialization(int size) {
    vector<int> A;
    for (int i = 0; i < size; i++) {
        A.push_back(rand() % 100 - 50);
    }
    return A;
}

void write_to_file(vector<int>& A) {
    ofstream output_file("result.txt", ios::out);

    if (!output_file.is_open()) {
        cerr << "Failed to open the file: result.txt" << endl;
        return;
    }

    for (int& value : A) {
        output_file << value << " ";
    }

    output_file.close();
    cout << "Data has been written to result.txt" << endl;
}

// сортировка вставками
void insertion_sort(vector<int>& A, int start, int end) {
    for (int i = start + 1; i <= end; i++) {
        int key = A[i];
        int j = i - 1;
        while (j >= start && A[j] > key) {
            A[j + 1] = A[j];
            j--;
        }
        A[j + 1] = key;
    }
}

// слияние параллельных результатов
void merge(vector<int>& A, int start, int middle, int end) {
    int n1 = middle - start + 1;
    int n2 = end - middle;

    vector<int> L(n1);
    vector<int> R(n2);

    for (int i = 0; i < n1; i++) {
        L[i] = A[start + i];
    }
    for (int i = 0; i < n2; i++) {
        R[i] = A[middle + 1 + i];
    }

    int i = 0;
    int j = 0;
    int k = start;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            A[k] = L[i];
            i++;
        } else {
            A[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        A[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        A[k] = R[j];
        j++;
        k++;
    }
}

// параллельная сортировка вставками
void parallel_merge_sort(vector<int>& A, int start, int end, int b) {
    if (end - start + 1 <= b) {
        insertion_sort(A, start, end);
    } else {
        int middle = (start + end) / 2;

        #pragma omp parallel sections
        {
            #pragma omp section
            {
                parallel_merge_sort(A, start, middle, b);
            }
            #pragma omp section
            {
                parallel_merge_sort(A, middle + 1, end, b);
            }
        }

        merge(A, start, middle, end);
    }
}

void print_array(vector<int>& A){
    for (int i = 0; i < A.size(); i++) 
        cout << A[i] << " ";
    cout << endl;
}

int main() {
    vector<int> A;    
    int b, num_threads, size;  // b - Порог для переключения на сортировку вставками
    double start, finish;
    setvbuf(stdout, 0, _IONBF, 0);

    cout << ("Enter threads number: ");
    cin >> num_threads;
    omp_set_num_threads(num_threads);
    cout << ("Enter size of the initial objects: ");
    cin >> size;
    A = random_initialization(size);
    b = size / num_threads;

    start=omp_get_wtime();
    parallel_merge_sort(A, 0, size - 1, b);
    finish=omp_get_wtime();

    cout << "Sorted array:\n";
    print_array(A);
    write_to_file(A);
    cout << "Time of execution = " << finish - start << endl;

    return 0;
}
