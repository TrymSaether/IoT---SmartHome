// Bibliotek
#include <Arduino.h>
#include "Matrix.h"

/* ----------------- 3 Dimensjonal Matriseklasse   ----------------------
den vil bli brukt for å beregne strømforbruk, samtidig som å holde oversikt over de ulike rommene, med ulike komponenter.
dette vil kunne effektivisere esp32, ved å lage et konkret minne, 
basert på X*Y størrelsen på matrisen, 
for hver justering i strømforbruk og komponent.
    * Kolonne: Rom.
    * Rad: komponent i Rom.
- Konstruktører -
ingen gitt verdi: nullpeker* se operatør */ 
Matrix::Matrix():Matrix(0){ //
    matrix = nullptr;
}

// NULL-matrise, n Rader og N kolonner.
Matrix::Matrix(int nRows, int nColumns) : rows{nRows}, columns{nColumns}
{
    if (nRows < 0 && nColumns < 0) // debug - "Catch"
    {
        Serial.println("Matrise - ugyldige verdier");
    }
    matrix = new float*[nRows];
    for (int i = 0; i < nRows; i++)
    {
        matrix[i] = new float[nColumns];
        for (int j = 0; j < nColumns; j++)
        {
            matrix[i][j] = 0;
        }
    }
}

// Identitetsmatrise: hvis man skal finne en ukjent  (A*A^-1 = I)
Matrix::Matrix(int identity) : Matrix(identity, identity)
{
    for (int k = 0; k < identity; k++)
    {
        matrix[k][k] = 1;
    }
}
// Setter element x,y
void Matrix::set(int row, int col, float value)
{
    matrix[row][col] = value;
}
// henter element x,y
float Matrix::get(int row, int col) const
{ // Henter elementer i rad og kolonne.
    return (matrix[row][col]);
}


// Henter rad
int Matrix::getRows() const
{
    return (rows);
}

// Kolonner 
int Matrix::getColumns() const
{
    return (columns);
}

// Henter summen av kolonner basert på valg av rom.
int Matrix::getSumColumn(int room){
    int colsum = 0;
    for (int i = 0; i < rows; i++)
    {  
        colsum = colsum + matrix[rows][room];
    }
    return(colsum);
}

// Henter totalsum av matrisen
int Matrix::getSum()
{
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
             sum += matrix[i][j];
        }
    }
    return(sum);
}





// -- Operatører -- 
// for matrise med nullpeker, lager en matrise med x/y rader, og allokerer ett minne til den
// rhs: righthandside. 
Matrix::Matrix(const Matrix &rhs) : matrix{nullptr}
{
	this->rows = rhs.rows; // Kopiering
	this->columns = rhs.columns; 
	this->matrix = new float*[rhs.rows] {}; // Liste med pekere til "nytt" array

	for (int i = 0; i < this->rows; ++i)
	{
		matrix[i] = new float[this->columns] {}; // Allokerer nytt minne
		for (int j = 0; j < this->columns; j++)
		{
			this->matrix[i][j] = rhs.matrix[i][j]; // Tar med alle verdier til ny variabel
		}
	}
}

void Matrix::print(){
    int hor = rows;
    int vert = columns;
    if (rows < columns)
    {
        hor = columns;
        vert = rows;
    }
    for (int i = 0; i < hor; i++)
    {
        for (int n = 0; n < vert; n++)
        {
            Serial.print("  ");
            Serial.print(get(i,n));
        }
        Serial.println();
    }
}

// Legge sammen matrise A += B
Matrix &Matrix::operator+=(const Matrix& rhs){
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            this->matrix[i][j] += rhs.get(i, j);
        }
    }
    return(*this);
}

// Legge sammen A+B
Matrix operator+(Matrix lhs, const Matrix& rhs){
    lhs +=  rhs;
    return(lhs);
}

// Kopi A = B; 
/*
Matrix &Matrix::operator=(Matrix rhs)
{
    swap(rows, rhs.rows);
    swap(columns, rhs.columns);
    swap(matrix, rhs.matrix);
    return (*this);
}
*/

// --- Destruktør ---
// Sletter alle pekere til minne, gjør at den ikke bottlenecker med å skape nytt minne.
Matrix::~Matrix()
{
    for (int i = 0; i < rows; i++){
        delete[] matrix[i];
        delete[] matrix;
    }
}
