#include <bits/stdc++.h>

using namespace std;

#define POP_MAX 1000
#define MUTATION_RATE 0.1

class Soul {
public:
    string genetics;
    int fitness_value = 0;

    void calculateFitness(string stencil) {
        /*
        This function calculates the overall fitness of
        a soul based on rows, columns and 3x3 blocks

        Params:
            @param soul chromosome values in the soul

        Returns:
            @return overall fitness score
        */
        for (int i = 0; i < 9; i++) {
            set <char> numbers_in_row;
            for (int j = 0; j < 9; j++) {
                numbers_in_row.insert(genetics[i * 9 + j]);
            }
            fitness_value += numbers_in_row.size();
        }
        
        for (int j = 0; j < 9; j++) {
            set <char> numbers_in_col;
            for (int i = 0; i < 9; i++) {
                numbers_in_col.insert(genetics[i * 9 + j]);
            }
            fitness_value += numbers_in_col.size();
        }

        for (int k = 0; k < 9; k += 3) {
            for (int g = 0; g < 9; g += 3) {
                set <char> numbers_in_block;
                for (int i = k; i < k + 3; i++) {
                    for (int j = g; j < g + 3; j++) {
                        numbers_in_block.insert(genetics[i * 9 + j]);
                    }
                }
                fitness_value += numbers_in_block.size();
            }
        }
    }

    Soul(string genetics, string stencil) : genetics(genetics) {
        calculateFitness(stencil);
    }
};

class Sudoku {
private:
    string stencil;
    Soul solution;
    bool solutionFound;
    vector <Soul> population;

    void inputStencil() {
        /*
        This function performs input of the initial data
        about the puzzle
        */
        stencil = "";
        for (int i = 0; i < 9; i++) {
            string temp_row = "";
            for (int j = 0; j < 9; j++) {
                string chr;
                cin >> chr;
                temp_row += chr;
            }
            stencil += temp_row;
        }
    }

    void createPopulation() {
        /*
        This function generates a population based on the input data.
        Initially, every row has the correct 1-9 elements, which are
        randomly shuffled, except the given ones, those are fixed in place.
        */
        population.resize(POP_MAX);
        for (int k = 0; k < POP_MAX; k++) {
            string soul = "";
            for (int i = 0; i < 9; i++) {
                vector <char> perm{'1', '2', '3', '4', '5', '6', '7', '8', '9'};
                for (int j = 0; j < 9; j++) {
                    if (find(perm.begin(), perm.end(), stencil[i * 9 + j]) != perm.end()) {
                        perm.erase(find(perm.begin(), perm.end(), stencil[i * 9 + j]));
                    }
                }
                random_shuffle(perm.begin(), perm.end());
                for (int j = 0; j < 9; j++) {
                    if (stencil[i * 9 + j] == '-') {
                        soul += perm[0];
                        perm.erase(perm.begin());
                    }
                    else {
                        soul += stencil[i * 9 + j];
                    }
                }
            }
            population[k] = Soul(soul, stencil);
        }
    }

public:
    void outputPopulation() {
        for (auto &soul : population) {
            cout << soul.genetics << ' ' << soul.fitness_value << '\n';
        }
    }
    
    Sudoku() {
        inputStencil();
    }
};

int main() {

    auto sudoku = Sudoku();

    sudoku.outputPopulation();

    exit(EXIT_SUCCESS);
}