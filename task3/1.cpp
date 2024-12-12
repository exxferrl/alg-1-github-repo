// Подключаем библиотеку iostream для управления выводом и вводом
#include <iostream> 
#include <fstream> // Для работы с файлами. Комбинация ofstream и ifstream: создание, чтение и запись в файлы.
#include <chrono> // Для работы с датой
#include <ctime>  // Для работы с временем
#include <iomanip> //Используется для управления форматированием ввода и вывода.
#include <string> //Для использования getline
#include <sstream> // Подключаем библиотеку для работы с потоками строк (istringstream и ostringstream)
#include <vector> // Для работы с векторами

// Подключаем пространство имен std 
using namespace std; 

//Класс для сохранения веременных данных при работе с матрицами
template<typename T = double>
class MatrixDense {
    T* __data; // Указатель на массив данных типа T
    unsigned _m, _n; // Размеры матрицы: количество строк (_m) и столбцов (_n)
    //unsigned — это модификатор типа, который указывает, что переменные будут хранить только неотрицательные целые числа

public:
    // Конструктор, принимающий размеры матрицы
    MatrixDense(unsigned m, unsigned n) : _m(m), _n(n) {
        // Выделение памяти для хранения элементов матрицы
        __data = new T[_m * _n]; // Создаем одномерный массив для хранения элементов матрицы
    }

    // Деструктор, вызываемый при уничтожении объекта
    ~MatrixDense() {
        // Освобождение выделенной памяти для массива данных
        if (__data!=nullptr)
        {
            delete[] __data; // Удаляем массив, чтобы предотвратить утечку памяти
            __data= nullptr;
        }
    }

    // Метод для доступа к элементам матрицы
    T& getElement(unsigned i, unsigned j) const {
        return __data[j + i * _n]; // Возвращаем элемент по индексам i и j
    }

    unsigned rows() const { return _m; }
    unsigned cols() const { return _n; }
};

// Структура для хранения матриц
struct MatrixData {
    MatrixDense<double> matrix;
    unsigned rows;
    unsigned cols;

    // Конструктор по умолчанию
    MatrixData() : rows(0), cols(0), matrix(MatrixDense<double>(0, 0)) {}
};

struct CalcProblemParams
{
   string filePath1;
   string filePath2;
   enum class operations {vv_sum, vv_sub, mm_sum};
   operations op;
};

//Структура для хранения значений векторов и их размерностей:
struct VectorData {
    vector<double> values;
    int size = 0;
};

struct ExportConfig {
    string path; // Путь к файлу
};

struct CalcResults {
    VectorData result; // Результат вычислений для векторов
    MatrixData matrixResult; // Результат вычислений для матриц
};

int logAll(string data)
{
    ofstream logFile("log.txt", ios::app); // Открываем файл в режиме добавления
    if (!logFile) {
        return -1;
    }
    //ios::app — это флаг, который указывает, что файл должен быть открыт в режиме добавления
    time_t t = time(0); // Получаю текущее время
    struct tm *now = localtime(&t); // Преобразую в локальное время

    // Записываем дату и время в файл
    logFile <<put_time(now, "%Y-%m-%d %H:%M:%S") << " - "; // Форматирую и записываем дату и время

    logFile << data << endl; // Записываю данные в файл
    logFile.close(); // Закрываю файл
    return 0; 
}

//Функция для чтения из файлов и проверки значений
VectorData readDataFromFile(const string& filepath) {
    string line;
    VectorData vectorData;
    ifstream dataFile(filepath); // Открываем файл в режиме чтения
    if (!dataFile) {
        logAll("Ошибка открытия файла");
        return vectorData; // Возвращаем пустой вектор
    }

    while (getline(dataFile, line)) {
        logAll(line);
        if (line == "vector") {
            logAll("В файле обнаружено значение vector");
            // Читаем размер вектора
            if (getline(dataFile, line)) {
                vectorData.size = stoi(line);
                vectorData.values.resize(vectorData.size);
                // Читаем значения вектора
                if (getline(dataFile, line)) {
                    /*Istringstream - это строковой поток ввода. Конструктор istringstream () со строковым аргументом прикрепляет создаваемый объект потока к указанной строке. То есть теперь информация будет считываться из стандартной строки точно таким же образом, как и из cin, объекта класса потока ввода istream.*/
                    istringstream iss(line);
                    for (int i = 0; i < vectorData.size; ++i) {
                        iss >> vectorData.values[i];
                    }
                }
            }
        }
    }
    dataFile.close(); // Закрываем файл
    return vectorData; 
}

//Функция для чтения данных матрицы из файла
MatrixData readMatrixFromFile(const string& filepath) {
    string line;
    MatrixData matrixData;
    ifstream dataFile(filepath);
    
    if (!dataFile) {
        logAll("Ошибка открытия файла: " + filepath);
        return matrixData;
    }

    while (getline(dataFile, line)) {
        logAll("Чтение строки: " + line);
        if (line == "matrix") {
            logAll("В файле обнаружено значение 'matrix'");
            if (getline(dataFile, line)) {
                // Изменяем формат чтения размеров матрицы
                size_t xPos = line.find('x');
                if (xPos != string::npos) {
                    matrixData.rows = stoi(line.substr(0, xPos));
                    matrixData.cols = stoi(line.substr(xPos + 1));
                    logAll("Размеры матрицы: " + to_string(matrixData.rows) + "x" + to_string(matrixData.cols));
                } else {
                    logAll("Ошибка: неверный формат размеров матрицы. Ожидалось 'MxN'.");
                    return matrixData; // Возвращаем пустую матрицу в случае ошибки
                }
                
                matrixData.matrix = MatrixDense<double>(matrixData.rows, matrixData.cols);
                
                // Читаем значения матрицы
                for (unsigned i = 0; i < matrixData.rows; ++i) {
                    if (getline(dataFile, line)) {
                        logAll("Чтение строки матрицы: " + line);
                        istringstream iss(line);
                        for (unsigned j = 0; j < matrixData.cols; ++j) {
                            if (!(iss >> matrixData.matrix.getElement(i, j))) {
                                logAll("Ошибка: не удалось прочитать элемент матрицы на позиции (" + to_string(i) + ", " + to_string(j) + ")");
                                return matrixData; // Возвращаем пустую матрицу в случае ошибки
                            }
                        }
                    } else {
                        logAll("Ошибка: недостаточно строк для матрицы. Ожидалось " + to_string(matrixData.rows) + " строк.");
                        return matrixData; // Возвращаем пустую матрицу в случае ошибки
                    }
                }
            }
        }
    }
    
    dataFile.close();
    return matrixData;
}

//Функция для сложения матриц
MatrixData Calck_mm_sum(const MatrixData& mat1, const MatrixData& mat2) {
    // Проверка на равенство размерностей
    if (mat1.rows != mat2.rows || mat1.cols != mat2.cols) {
        logAll("Ошибка! Размерности матриц не совпадают");
        return MatrixData();
    }

    MatrixData result; // Создаем объект для хранения результата сложения матриц
    result.rows = mat1.rows; // Устанавливаем размер результирующей матрицы
    result.cols = mat1.cols;
    result.matrix = MatrixDense<double>(result.rows, result.cols); // Инициализируем матрицу

    // Сложение матриц
    for (unsigned i = 0; i < result.rows; ++i) {
        for (unsigned j = 0; j < result.cols; ++j) {
            result.matrix.getElement(i, j) = mat1.matrix.getElement(i, j) + mat2.matrix.getElement(i, j);
        }
    }

    return result;
}

// Функция для сложения двух векторов
VectorData Calck_vv_sum(const VectorData& vec1, const VectorData& vec2) {
    // Проверка на равенство размерностей
    if (vec1.size != vec2.size) {
        logAll("Ошибка! Размерность векторов не совпадает");
        return VectorData();
    }

    VectorData result; // Создаем объект для хранения результата сложения векторов
    result.size = vec1.size; // Устанавливаем размер результирующего вектора равным размеру первого вектора
    result.values.resize(result.size); // Изменяем размер массива значений результирующего вектора

    // Сложение векторов
    for (int i = 0; i < result.size; ++i) {
        result.values[i] = vec1.values[i] + vec2.values[i];
    }

    return result;
}

//Функция для вычитания векторов
VectorData Calck_vv_sub(const VectorData& vec1, const VectorData& vec2) {
    // Проверка на равенство размерностей
    if (vec1.size != vec2.size) {
        logAll("Ошибка! Размерность векторов не совпадает");
        return VectorData();
    }

    VectorData result; // Создаем объект для хранения результата вычитания векторов
    result.size = vec1.size; // Устанавливаем размер результирующего вектора равным размеру первого вектора
    result.values.resize(result.size); // Изменяем размер массива значений результирующего вектора

    // Вычитание векторов
    for (int i = 0; i < result.size; ++i) {
        result.values[i] = vec1.values[i] - vec2.values[i];
    }

    return result;
}

// Функция для экспорта результатов рассчётов в файл
int Export(const CalcResults& calcResults, const ExportConfig& config) {
    // Создаю файл и открываю его на дозапись
    logAll("Открываю " + config.path);
    ofstream dataFile(config.path, ios::app); // Открываем файл в режиме добавления
    if (!dataFile) {
        logAll("Ошибка открытия файла: " + config.path);
        return -1;
    }

    // Проверка на наличие данных для записи в вектор
    if (calcResults.result.size > 0) {
        dataFile << "Vector Result: "; // Заголовок для вектора

        // Записываем данные вектора в файл
        for (const auto& value : calcResults.result.values) {
            dataFile << value << " "; // Записываем значения вектора
        }
        dataFile << endl; // Переход на новую строку
    } else {
        logAll("Нет данных для записи вектора, ничего не записывается в файл.");
    }

    // Подсчет элементов матрицы
    unsigned countMatrixElements = calcResults.matrixResult.rows * calcResults.matrixResult.cols;

    // Проверка на наличие данных для записи в матрицу
    if (countMatrixElements > 0) {
        dataFile << "Matrix Result:" << endl; // Заголовок для матрицы
        for (unsigned i = 0; i < calcResults.matrixResult.rows; ++i) {
            for (unsigned j = 0; j < calcResults.matrixResult.cols; ++j) {
                dataFile << calcResults.matrixResult.matrix.getElement(i, j) << " "; // Записываем элементы матрицы
            }
            dataFile << endl; // Переход на новую строку после каждой строки матрицы
        }
    } else {
        logAll("Нет данных для записи матрицы.");
    }

    dataFile.close(); // Закрываю файл
    return 0; 
}

int main(int argc, char* argv[]) {
    VectorData vector1, vector2;
    MatrixData matrix1, matrix2;
    CalcProblemParams calcParams;
    CalcResults calcResult;

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--fp1") {
            if (i + 1 < argc) {
                calcParams.filePath1 = argv[i + 1];
                logAll("Путь к файлу 1: " + calcParams.filePath1);
                vector1 = readDataFromFile(calcParams.filePath1);
                i++;} 
        } else if (string(argv[i]) == "--fp2") {
            if (i + 1 < argc) {
                calcParams.filePath2 = argv[i + 1];
                logAll("Путь к файлу 2: " + calcParams.filePath2);
                vector2 = readDataFromFile(calcParams.filePath2);
                i++;
            }
        } if (string(argv[i]) == "--matrix_fp1") {
            if (i + 1 < argc) {
                calcParams.filePath1 = argv[i + 1];
                logAll("Путь к файлу матрицы 1: " + calcParams.filePath1);
                matrix1 = readMatrixFromFile(calcParams.filePath1);
                i++;
            } else {
                logAll("Ошибка: нет аргумента после --matrix_fp1");
            }
        } if (string(argv[i]) == "--matrix_fp2") {
            if (i + 1 < argc) {
                calcParams.filePath2 = argv[i + 1];
                logAll("Путь к файлу матрицы 2: " + calcParams.filePath2);
                matrix2 = readMatrixFromFile(calcParams.filePath2);
                i++;
            } else {
                logAll("Ошибка: нет аргумента после --matrix_fp2");
            }
        } else if (string(argv[i]) == "--op") {
            if (i + 1 < argc) {
                string operation = argv[i + 1];
                logAll("Операция: " + operation);
                if (operation == "vv_sum") {
                    calcParams.op = CalcProblemParams::operations::vv_sum;
                    logAll("Вызвана операция: суммирование векторов");
                    calcResult.result = Calck_vv_sum(vector1, vector2);
                } else if (operation == "vv_sub") {
                    calcParams.op = CalcProblemParams::operations::vv_sub;
                    logAll("Вызвана операция: вычитание векторов");
                    calcResult.result = Calck_vv_sub(vector1,vector2);
                } else if (operation == "mm_sum") {
                    calcParams.op = CalcProblemParams::operations::mm_sum;
                    logAll("Вызвана операция: суммирование матриц");
                    calcResult.matrixResult = Calck_mm_sum(matrix1, matrix2);
                    // Вывод результата сложения матриц
                    // for (unsigned i = 0; i < calcResult.matrixResult.rows; ++i) {
                    //     for (unsigned j = 0; j < calcResult.matrixResult.cols; ++j) {
                    //         cout << calcResult.matrixResult.matrix.getElement(i, j) << " ";
                    //     }
                    //     cout << endl; // Переход на новую строку после каждой строки матрицы
                    // }
                } else {
                    logAll("Ошибка: неизвестная операция");
                }
                i++; // Увеличиваем индекс, чтобы пропустить следующий аргумент
            } else {
                logAll("Ошибка: нет аргумента после --op");
            }
        }else if (string(argv[i]) == "--exp") {
        if (i + 1 < argc) { // Проверка границ
            ExportConfig conf;
            conf.path = argv[i + 1]; // Сохраняем путь к файлу
            logAll("Путь и имя выходного файла: " + conf.path);
            // Вызов функции Export с нужными аргументами
            int exportResult = Export(calcResult, conf);
            if (exportResult != 0) {
                logAll("Ошибка при экспорте данных");
            }
        } else {
            logAll("Ошибка: нет аргументов после --exp");
        }
    }
}
    return 0;
}