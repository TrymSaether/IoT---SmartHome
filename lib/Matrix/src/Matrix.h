#pragma once
#include <Arduino.h>


enum class Rooms{
    kitchen = 1,
    livingroom = 2,
    bathroom = 3,
    bedroom1 = 4,
    bedroom2 = 5,
    bedroom3 = 6,
    bedroom4 = 7,
    bedroom5 = 8,
    bedroom6 = 9
};
// Matriseklasse
class Matrix
{
private:
// Dimensjoner
    int rows; // Rader i matrise.
    int columns; // Kolonner i matrise.
    float** matrix; // 2. Dimensjonal matrise. == Matrise[][] bare med allokering og dynamisk håndtering av minne.
    int sum;
public:
// Konstruktører
    Matrix();
    Matrix(const int nRows,const int nColumns); // utgangspunkt som nullmatrise, men kan ekspanderes til ønsket N*M Matrise.
    Matrix(int identity); // Identitetsmatrise.
// funksjoner
    float get(int row, int col) const; // henter element i rad og kolonne.
    void set(int row, int col, float value); // Setter verdi på et element.
    int getRows() const; // lengde rad.
    int getColumns() const; // lengde kolonne.
    int getSumColumn(int room);
    int getSum();
    void print();
// operatører
    Matrix &operator=(Matrix rhs);
    Matrix(const Matrix &rhs);
    Matrix &operator+=(const Matrix& rhs);
    friend Matrix operator+(Matrix lhs, const Matrix& rhs);  
// Destruktør
    ~Matrix();
};


