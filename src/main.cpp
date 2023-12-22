#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <cmath>

using namespace std;

void processing(vector<vector<double > >& matrix, vector<vector<double > >& convolution_Matrix, vector<vector<double> >& rez_Matrix, int V, mutex& mtx, int step, int A, int B);

// Функция печати итоговых матриц
void print(const vector<vector<double > >& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

int main(int argc, char* argv[]) {
    int A = 0, B = 0;
    cout << "start" << endl;
    cout << "Максимально возможное количество потоков: " << thread::hardware_concurrency() << "\n";
    
    if ( atoi(argv[1]) > 12 || atoi(argv[1]) <= 0){
        cout << "Заданное число потоков не может быть использовано." << "\n";
        return 1;
    }

    const int Max = atoi(argv[1]);
    int rows, cols, p, q, K;
    vector<thread> th(Max);
    
    cout << "Введите количество раз для наложения фильтров: ";
    cin >> K;
    cout << "\n";

    cout << "Введите i & j: ";
    cin >> rows >> cols;
    cout << "Введите p & q: ";
    cin >> p >> q;
    //A = rows; B = cols;

    vector<vector<double > > matrix(rows, vector<double>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
    print(matrix);

    vector<vector<double > > convolution_Matrix(p, vector<double>(q));
    for (int i = 0; i < p; i++) {
        for (int j = 0; j < q; j++) {
            convolution_Matrix[i][j] = rand() % 3;
        }
    }
    print(convolution_Matrix);
    vector<vector<double> > rez_matrix(rows-p+1, vector<double>(cols-q+1));
    //print(rez_matrix);

    

// Начало отсчета времени
    chrono::steady_clock::time_point start_time = chrono::steady_clock::now();
    cout << "ID потока main: " << this_thread::get_id() << "\n";
    std::mutex mtx; // Мьютекс для синхронизации доступа к матрице

// Работа единственного потока
    if (Max == 1){
        for(int i = 0; i < K; i++){
        processing(matrix, convolution_Matrix, rez_matrix, 0, ref(mtx), 1, 0, 0);
        A = A + p - 1;
        B = B + q - 1;
        }
    }

    if (Max > 1){
        for(int i = 0; i < K; i++){
            for(int t = 0; t < Max; t++){
                th[t] = thread(processing, ref(matrix), ref(convolution_Matrix), ref(rez_matrix), t, ref(mtx), Max, A, B);
                cout << "ID потока: " << th[t].get_id() << "\n";
            }
            for(int t = 0; t < Max; t++){
                th[t].join();
            }
            //thread processing_thread_1(processing, ref(matrix), ref(convolution_Matrix), ref(rez_matrix), 0, ref(mtx), 2, A, B);
            //thread processing_thread_2(processing, ref(matrix), ref(convolution_Matrix), ref(rez_matrix), 1, ref(mtx), 2, A, B);

            // processing_thread_1.join();
            // processing_thread_2.join();
            A = A + p - 1;
            B = B + q - 1;
            //rez_matrix.resize(rows-p-1, vector<double>(cols-q-1));
            //cout << i << ' ' << A << ' ' <<  B << endl;
            //print(matrix);

        }
    }



    cout << "Матрица после наложения " << K << " раз матрицы свертки: " << "\n";
    for (int i = 0; i < rows - A; i++) {
        for (int j = 0; j < cols - B; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";

// Конец отсчета времени
    chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Время работы программы: " << duration.count()/Max << " микросекунд. " << "\n";

    return 0;
}

void processing(vector<vector<double> >& matrix, vector<vector<double> >& convolution_Matrix, 
                                        vector<vector<double> >& rez_matrix, int V, mutex& mtx, int step, int A, int B){
        int rows = matrix.size() - A;
        int cols = matrix[0].size() - B;
        int q = convolution_Matrix.size();
        int p = convolution_Matrix[0].size();
        //vector<vector<double> > new_matrix(rows-p+1, vector<double>(cols-q+1));
        for(int i = 0; i < (rows-p+1); i++){
            for(int j = V; j <= (cols-q); j+=step){
                int summ = 0;
                for(int a = 0; a < q; a++){
                    for(int b = 0; b < p; b++){
                        summ += fmod(convolution_Matrix[a][b] * matrix[a+i][b+j], 1000);
                    }
                }
                mtx.lock();
                rez_matrix[i][j] = summ;  
                //new_matrix[i][j] = summ;  
                mtx.unlock();            
            }
        }
        matrix = rez_matrix;
        
                                            
    // for (int k = 0; k < K; k++){
        
    //     // mtx.lock();
    //     // matrix = rez_matrix;
    //     // mtx.unlock(); 
    //     // cout << rows-p << this_thread::get_id() << endl;
    //     // r       
    // }
}
