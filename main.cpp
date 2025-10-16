#include <iostream>
#include <iomanip>
#include <stdexcept>

template <typename T>
class Simulador2D {
private:
    T **_grid;
    int _filas;
    int _columnas;
    T *_fuentes;
    int _numFuentes;
    int _capacidadFuentes;
    float _constantes[3];

    void asignarGrid(int f, int c) {
        if (f <= 0 || c <= 0) throw std::invalid_argument("Filas/columnas deben ser > 0");
        _filas = f;
        _columnas = c;
        _grid = new T*[_filas];
        for (int i = 0; i < _filas; ++i) {
            _grid[i] = new T[_columnas];
            for (int j = 0; j < _columnas; ++j) _grid[i][j] = T{};
        }
    }

    void liberarGrid() {
        if (_grid != nullptr) {
            for (int i = 0; i < _filas; ++i) delete[] _grid[i];
            delete[] _grid;
            _grid = nullptr;
        }
        _filas = 0;
        _columnas = 0;
    }

    void copiarDesde(const Simulador2D<T>& otro) {
        asignarGrid(otro._filas, otro._columnas);
        for (int i = 0; i < _filas; ++i)
            for (int j = 0; j < _columnas; ++j)
                _grid[i][j] = otro._grid[i][j];
        _numFuentes = otro._numFuentes;
        _capacidadFuentes = otro._capacidadFuentes;
        if (_capacidadFuentes > 0) {
            _fuentes = new T[_capacidadFuentes];
            for (int k = 0; k < _numFuentes; ++k) _fuentes[k] = otro._fuentes[k];
        } else {
            _fuentes = nullptr;
        }
        _constantes[0] = otro._constantes[0];
        _constantes[1] = otro._constantes[1];
        _constantes[2] = otro._constantes[2];
    }

public:
    Simulador2D(int f = 1, int c = 1) : _grid(nullptr), _filas(0), _columnas(0),
                                       _fuentes(nullptr), _numFuentes(0), _capacidadFuentes(0) {
        _constantes[0] = 1.0f;
        _constantes[1] = 1.0f;
        _constantes[2] = 1.0f;
        asignarGrid(f, c);
        _capacidadFuentes = 2;
        _fuentes = new T[_capacidadFuentes];
        _numFuentes = 0;
    }

    Simulador2D(const Simulador2D<T>& otro) : _grid(nullptr), _filas(0), _columnas(0),
                                              _fuentes(nullptr), _numFuentes(0), _capacidadFuentes(0) {
        copiarDesde(otro);
    }

    Simulador2D<T>& operator=(const Simulador2D<T>& otro) {
        if (this == &otro) return *this;
        liberarGrid();
        if (_fuentes != nullptr) { delete[] _fuentes; _fuentes = nullptr; }
        copiarDesde(otro);
        return *this;
    }

    ~Simulador2D() {
        liberarGrid();
        if (_fuentes != nullptr) {
            delete[] _fuentes;
            _fuentes = nullptr;
        }
    }

    int filas() const { return _filas; }
    int columnas() const { return _columnas; }

    void redimensionarGrid(int nuevaF, int nuevaC) {
        if (nuevaF <= 0 || nuevaC <= 0) throw std::invalid_argument("Nuevas dimensiones invalidas");
        if (nuevaF == _filas && nuevaC == _columnas) return;
        T **temp = new T*[nuevaF];
        for (int i = 0; i < nuevaF; ++i) {
            temp[i] = new T[nuevaC];
            for (int j = 0; j < nuevaC; ++j) temp[i][j] = T{};
        }
        int minF = (nuevaF < _filas) ? nuevaF : _filas;
        int minC = (nuevaC < _columnas) ? nuevaC : _columnas;
        for (int i = 0; i < minF; ++i)
            for (int j = 0; j < minC; ++j)
                temp[i][j] = _grid[i][j];
        liberarGrid();
        _grid = temp;
        _filas = nuevaF;
        _columnas = nuevaC;
    }

    void agregarFuente(T valor) {
        if (_numFuentes >= _capacidadFuentes) {
            int nuevaCap = (_capacidadFuentes == 0) ? 1 : (_capacidadFuentes * 2);
            T *nuevo = new T[nuevaCap];
            for (int i = 0; i < _numFuentes; ++i) nuevo[i] = _fuentes[i];
            delete[] _fuentes;
            _fuentes = nuevo;
            _capacidadFuentes = nuevaCap;
        }
        _fuentes[_numFuentes] = valor;
        ++_numFuentes;
    }

    bool aplicarFuenteEn(int indiceFuente, int fila, int col) {
        if (indiceFuente < 0 || indiceFuente >= _numFuentes) return false;
        if (fila < 0 || fila >= _filas || col < 0 || col >= _columnas) return false;
        _grid[fila][col] = _fuentes[indiceFuente];
        return true;
    }

    void imprimirGrid(const std::string& titulo = "") const {
        if (!titulo.empty()) std::cout << titulo << "\n";
        for (int i = 0; i < _filas; ++i) {
            std::cout << "| ";
            for (int j = 0; j < _columnas; ++j)
                std::cout << std::setw(7) << _grid[i][j] << " ";
            std::cout << "|\n";
        }
    }

    void simularPaso() {
        if (_filas <= 1 || _columnas <= 1) return;
        T **temp = new T*[_filas];
        for (int i = 0; i < _filas; ++i) {
            temp[i] = new T[_columnas];
            for (int j = 0; j < _columnas; ++j) temp[i][j] = T{};
        }
        for (int j = 0; j < _columnas; ++j) {
            temp[0][j] = _grid[0][j];
            temp[_filas-1][j] = _grid[_filas-1][j];
        }
        for (int i = 0; i < _filas; ++i) {
            temp[i][0] = _grid[i][0];
            temp[i][_columnas-1] = _grid[i][_columnas-1];
        }
        for (int i = 1; i < _filas - 1; ++i) {
            for (int j = 1; j < _columnas - 1; ++j) {
                T arriba = _grid[i-1][j];
                T abajo  = _grid[i+1][j];
                T izq    = _grid[i][j-1];
                T der    = _grid[i][j+1];
                temp[i][j] = (arriba + abajo + izq + der) / static_cast<T>(4);
            }
        }
        for (int i = 0; i < _filas; ++i)
            for (int j = 0; j < _columnas; ++j)
                _grid[i][j] = temp[i][j];
        for (int i = 0; i < _filas; ++i) delete[] temp[i];
        delete[] temp;
    }

    int cantidadFuentes() const { return _numFuentes; }

    void setConstante(int idx, float val) {
        if (idx < 0 || idx >= 3) throw std::out_of_range("Indice de constante fuera de rango");
        _constantes[idx] = val;
    }
    float getConstante(int idx) const {
        if (idx < 0 || idx >= 3) throw std::out_of_range("Indice de constante fuera de rango");
        return _constantes[idx];
    }
};

int main() {
    try {
        std::cout << "--- Simulador Generico de Fluidos 2D (Difusion simplificada) ---\n\n";
        std::cout << ">> Inicializando sistema (Tipo FLOAT) <<\n";
        Simulador2D<float> simF(5,5);
        std::cout << "Agregando fuentes de concentracion...\n";
        simF.agregarFuente(100.0f);
        simF.agregarFuente(50.0f);
        std::cout << "Fuentes agregadas: " << simF.cantidadFuentes() << "\n";
        simF.aplicarFuenteEn(0, 2, 2);
        simF.aplicarFuenteEn(1, 4, 0);
        std::cout << "\n--- Grid Inicial (Paso 0) ---\n";
        simF.imprimirGrid();
        std::cout << "\nSimulando 1 paso...\n";
        simF.simularPaso();
        std::cout << "\n--- Grid despues de Paso 1 ---\n";
        simF.imprimirGrid();
        std::cout << "\nRedimensionando grid a 6x6...\n";
        simF.redimensionarGrid(6,6);
        std::cout << "Grid despues de redimensionar:\n";
        simF.imprimirGrid();
        std::cout << "\n>> Ahora prueba con Tipo INT <<\n";
        Simulador2D<int> simI(4,4);
        simI.agregarFuente(9);
        simI.aplicarFuenteEn(0, 1, 1);
        std::cout << "\nGrid INT inicial:\n";
        simI.imprimirGrid();
        std::cout << "\nSimulando paso en INT\n";
        simI.simularPaso();
        std::cout << "\nGrid INT despues de 1 paso:\n";
        simI.imprimirGrid();
        std::cout << "\nLiberando memoria y saliendo...\n";
    } catch (const std::bad_alloc& e) {
        std::cerr << "Error: no se pudo asignar memoria (" << e.what() << ")\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error argumento: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error inesperado: " << e.what() << "\n";
    }
    return 0;
}
