#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

class symplix
{
private:
    double* c;
    int length;

    double* b;
    int height;

    double** A;
    int rows, columns;

    double** symplix_table;
    int rows_, columns_;

    double* x;
    int num;

    int* basic;
    int* bogus;

private:
    void create_symplix_table()
    {
        rows_ = rows + 1;
        columns_ = columns + 1;

        symplix_table = new double*[rows_];
        for (int i = 0; i < rows_; ++i)
        {
            symplix_table[i] = new double[columns_];
            for (int j = 0; j < columns_; ++j)
            {
                symplix_table[i][j] = 0;
            }
        }

        for (int i = 1; i < rows_; ++i)
        {
            symplix_table[rows_ - 1][i] = c[i - 1];
        }

        for (int i = 0; i < (columns_ - 1); ++i)
        {
            symplix_table[i][0] = b[i];
        }

        symplix_table[rows_ - 1][0] = 0;

        for (int i = 0; i < (rows_ - 1); ++i)
        {
            for (int j = 1; j < columns_; ++j)
            {
                symplix_table[i][j] = A[i][j - 1];
            }
        }
    }

    void jordan_exception(int r, int k)
    {
        double** demo = new double*[rows_];
        for (int i = 0; i < rows_; ++i)
        {
            demo[i] = new double[columns_];
            for (int j = 0; j < columns_; ++j)
            {
                demo[i][j] = symplix_table[i][j];
            }
        }

        symplix_table[r][k] = 1 / demo[r][k];

        for (int i = 0; i < rows_; ++i)
        {
            if (i == r)
                continue;
            symplix_table[i][k] = -demo[i][k] / demo[r][k];
        }

        for (int j = 0; j < columns_; ++j)
        {
            if (j == k)
                continue;
            symplix_table[r][j] = demo[r][j] / demo[r][k];
        }

        for (int i = 0; i < rows_; ++i)
        {
            if (i == r)
                continue;
            for (int j = 0; j < columns_; ++j)
            {
                if (j == k)
                    continue;
                symplix_table[i][j] = demo[i][j] - (demo[i][k] * demo[r][j] / demo[r][k]);
            }
        }

        std::swap(basic[r], bogus[k - 1]);

        for (int i = 0; i < height; ++i)
        {
            x[basic[i] - 1] = symplix_table[i][0];
        }

        for (int i = 0; i < length; ++i)
        {
            x[bogus[i] - 1] = 0;
        }

        for (int i = 0; i < rows_; ++i)
        {
            delete[] demo[i];
        }
        delete[] demo;
    }

    void support_solution(bool& success)
    {
        int r = -1;
        int k = -1;

        for (int i = 0; i < (rows_ - 1); ++i)
        {
            if (symplix_table[i][0] < 0)
            {
                r = i;
                for (int j = 1; j < columns_; ++j)
                {
                    if (symplix_table[i][j] < 0)
                    {
                        k = j;
                        break;
                    }
                }
                if (k != -1)
                    break;
            }
        }

        if (r == -1)
        {
            success = false;
            return;
        }
        if (r != -1 && k == -1)
            throw std::logic_error("decision_error");

        double min = symplix_table[r][0] / symplix_table[r][k];
        for (int i = 0; i < (rows_ - 1); ++i)
        {
            double min_ = symplix_table[i][0] / symplix_table[i][k];
            if (min_ < min && min_ > 0)
            {
                min = min_;
                r = i;
            }
        }
        jordan_exception(r, k);
    }

    void optimal_solution(bool& success)
    {
        int r = -1;
        int k = -1;

        for (int j = 1; j < columns_; ++j)
        {
            if (symplix_table[rows_ - 1][j] > 0)
            {
                k = j;
                break;
            }
        }

        if (k == -1)
        {
            success = false;
            return;
        }
        else
        {
            double min = 0;
            for (int i = 0; i < (rows_ - 1); ++i)
            {
                double min_ = symplix_table[i][0] / symplix_table[i][k];
                if ((min_ < min || min == 0) && min_ > 0)
                {
                    min = min_;
                    r = i;
                }
            }
            if (min == 0)
                std::logic_error("decision_error");
            else
                jordan_exception(r, k);
        }
    }

    void print()
    {
        std::cout << "[]		S		";
        for (int j = 2; j < (columns_ + 1); ++j)
        {
            std::cout << "X" << bogus[j - 2] << "		";
        }
        std::cout << '\n';

        for (int i = 1; i < (rows_); ++i)
        {
            std::cout << "X" << basic[i - 1] << "		";
            for (int j = 1; j < (columns_ + 1); ++j)
            {
                std::cout << symplix_table[i - 1][j - 1] << "		";
            }
            std::cout << '\n';
        }
        std::cout << "F		";
        for (int j = 1; j < (columns_ + 1); ++j)
        {
            std::cout << symplix_table[rows_ - 1][j - 1] << "		";
        }
        std::cout << '\n';
    }

public:
    symplix()
    {
        std::ifstream fin(R"(C:\Users\Public\Documents\lab01.txt)");
        if (!fin.is_open())
            throw std::logic_error("input_error");

        fin >> length;
        c = new double[length];
        for (int i = 0; i < length; ++i)
        {
            fin >> c[i];
        }

        char op;
        fin >> rows;
        fin >> op;
        fin >> columns;
        A = new double*[rows];
        for (int i = 0; i < rows; ++i)
        {
            A[i] = new double[columns];
            for (int j = 0; j < columns; ++j)
            {
                fin >> A[i][j];
            }
        }

        fin >> height;
        b = new double[height];
        for (int i = 0; i < height; ++i)
        {
            fin >> b[i];
        }

        num = length + height;
        x = new double[num];
        for (int i = 0; i < rows; ++i)
        {
            x[i] = 0;
        }

        bogus = new int[length];
        for (int i = 0; i < length; ++i)
        {
            bogus[i] = i + 1;
        }

        basic = new int[height];
        for (int i = 0; i < height; ++i)
        {
            basic[i] = length + i + 1;
        }

        fin.close();

        create_symplix_table();
    }

    ~symplix()
    {
        delete[] c;

        delete[] b;

        for (int i = 0; i < rows; ++i)
        {
            delete[] A[i];
        }
        delete[] A;

        for (int i = 0; i < rows_; ++i)
        {
            delete[] symplix_table[i];
        }
        delete[] symplix_table;

        delete[] x;

        delete[] basic;

        delete[] bogus;
    }

    void algorithm()
    {
        bool success = true;
        while (success)
        {
            support_solution(success);
            print();
        }

        for (int i = 0; i < height; ++i)
        {
            if (x[i] != 0)
            {
                std::cout << "X" << (i + 1) << " = " << x[i] << '\n';
            }
        }
        std::cout << "F = " << -symplix_table[rows_ - 1][0] << '\n';

        success = true;
        while (success)
        {
            optimal_solution(success);
            print();
        }

        for (int i = 0; i < height; ++i)
        {
            if (x[i] != 0)
            {
                std::cout << "X" << (i + 1) << " = " << x[i] << ";	";
            }
        }
        std::cout << "F = " << -symplix_table[rows_ - 1][0] << ";	";
    }
};

int main()
{
    symplix SYM;
    SYM.algorithm();
    return 0;
}
