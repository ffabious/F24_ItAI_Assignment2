#include <bits/stdc++.h>

using namespace std;

#define POP_MAX 1000

class Sudoku {
private:
    vector <vector <char>> stencil;
    vector <vector <char>> solution;
    bool solutionFound = false;
    vector <vector <vector <char>>> population;


    void inputStencil() {
        /*
        This function performs input of the initial data
        about the puzzle
        */

        stencil.resize(9);
        for (int i = 0; i < 9; i++) {
            stencil[i].resize(9);
            for (int j = 0; j < 9; j++) {
                cin >> stencil[i][j];
            }
        }
    }

    void createPopulation() {
        /*
        This function generates a population based on the input data.
        Initially, every row has the correct 1-9 elements, which are randomly shuffled,
        except those given, those are fixed in place.
        */
        population.resize(POP_MAX);
        for (int k = 0; k < POP_MAX; k++) {
            vector <vector <char>> soul;
            soul.resize(9);
            for (int i = 0; i < 9; i++) {
                vector <char> perm{'1', '2', '3', '4', '5', '6', '7', '8', '9'};
                for (int j = 0; j < 9; j++) {
                    soul[i].resize(9);
                    if (find(perm.begin(), perm.end(), stencil[i][j]) != perm.end()) {
                        perm.erase(find(perm.begin(), perm.end(), stencil[i][j]));
                    }
                }
                random_shuffle(perm.begin(), perm.end());
                for (int j = 0; j < 9; j++) {
                    if (stencil[i][j] == '-') {
                        soul[i][j] = perm[0];
                        perm.erase(perm.begin());
                    }
                    else {
                        soul[i][j] = stencil[i][j];
                    }
                }
            }
            population[k] = soul;
        }
    }

    double fitnessBasedOnRows(vector <vector<char>> soul) {
        /*
        This function calculates fitness of a soul in range [0;1]
        based on the number of unique digits in the rows

        Params:
            soul - matrix of values in the soul

        Returns:
            fitness score based on rows in range [0;1]
        */
        double instances = 0.0;
        for (int i = 0; i < 9; i++) {
            set <char> numbers_in_row;
            for (int j = 0; j < 0; j++) {
                numbers_in_row.insert(soul[i][j]);
            }
            instances += numbers_in_row.size();
        }
        return instances / 81.0;
    }

    double fitnessBasedOnCols(vector <vector <char>> soul) {
        /*
        This function calculates fitness of a soul in range [0;1]
        based on the number of unique digits in the columns

        Params:
            soul - matrix of values in the soul

        Returns:
            fitness score based on columns in range [0;1]
        */
        double instances = 0.0;
        for (int j = 0; j < 9; j++) {
            set <char> numbers_in_col;
            for (int i = 0; i < 9; i++) {
                numbers_in_col.insert(soul[i][j]);
            }
            instances += numbers_in_col.size();
        }
        return instances / 81.0;
    }

    double fitnessBasedOnBlocks(vector <vector <char>> soul) {
        /*
        This function calculates fitness of a soul in range [0;1]
        based on the number of unique digits in the 3x3 blocks

        Params:
            soul - matrix of values in the soul

        Returns:
            fitness score based on blocks in range [0;1]
        */
        double instances = 0.0;
        for (int k = 0; k < 9; k += 3) {
            for (int g = 0; g < 9; g += 3) {
                set <char> numbers_in_block;
                for (int i = k; i < k + 3; i++) {
                    for (int j = g; j < g + 3; j++) {
                        numbers_in_block.insert(soul[i][j]);
                    }
                }
                instances += numbers_in_block.size();
            }
        }
        return instances / 81.0;
    }
    
    double fitness(vector <vector <char>> soul) {
        /*
        This function calculates the overall fitness of a soul in
        range [0;1] as the average of fitness scores based on rows,
        columns and 3x3 blocks

        Params:
            soul - matrix of values in the soul

        Returns:
            overall fitness score - average of fitness scores based
            on rows, columns and blocks
        */
        double fitnessValue = (fitnessBasedOnRows(soul) + fitnessBasedOnCols(soul) + fitnessBasedOnBlocks(soul)) / 3.0;
        if (fitnessValue == 1.0) {
            solutionFound = true;
            solution = soul;
        }
       return fitnessValue;
    }

    void crossoverOfRows(vector <char> &row1, vector <char> &row2, vector <char> stencilRow, int pos) {
        /*
        This function is responsible for performing crossover on
        corresponding rows from 2 souls

        Params:
            row1 - row of digits from 1st soul
            row2 - corresponding row of digits from 2nd soul
            stencilRow - corresponding row of digits and 'dashes'
                from the stencil
            pos - index of a cut in chromosomes:
                row1 saves [0;pos) from soul1
                row2 saves [pos;8] from soul2
                then they are filled accordingly
        */
        vector <char> new_row1 = stencilRow, new_row2 = stencilRow;
        
        // Save part from the first row
        set <char> used_in_row1;
        for (int j = 0; j < pos; j++) {
            new_row1[j] = row1[j];
            used_in_row1.insert(row1[j]);
        }

        // Add the rest of digits in order of their occurence
        // in the second row, if they weren't already added
        int k = pos;
        for (int j = pos; j < pos + 9; j++) {
            if (used_in_row1.find(row2[j % 9]) == used_in_row1.end()) {
                new_row1[k] = row2[j % 9];
                used_in_row1.insert(row2[j % 9]);
                k++;
            }
            if (k == 9) {
                break;
            }
        }

        // Save part from the second row
        set <char> used_in_row2;
        for (int j = pos; j < 9; j++) {
            new_row2[j] = row2[j];
            used_in_row2.insert(row2[j]);
        }

        // Add the rest of digits in order of their occurence
        // in the first row, if they weren't already added
        k = 0;
        for (int j = 0; j < 9; j++) {
            if (used_in_row2.find(row1[j]) == used_in_row2.end()) {
                new_row2[k] = row1[j];
                used_in_row2.insert(row1[j]);
                k++;
            }
            if (k == pos) {
                break;
            }
        }

        row1 = new_row1;
        row2 = new_row2;
    }
    
    pair <vector <vector <char>>, vector <vector <char>>> crossoverOfSouls(vector <vector <char>> soul1, vector <vector <char>> soul2) {
        /*
        This function is responsible for the crossover of
        2 souls.

        Params:
            soul1 - matrix of values in the 1st soul
            soul2 - matrix of values in the 2nd soul

        Returns:
            result - pair of matrices, which represent 2 
            'children' of the given souls
        */
        int pos = rand() % 8 + 1;
        
        for (int i = 0; i < 9; i++) {
            vector <char> &row1 = soul1[i], &row2 = soul2[i], stencilRow = stencil[i];
            crossoverOfRows(row1, row2, stencilRow, pos);
        }
        pair <vector <vector <char>>, vector <vector <char>>> result = {soul1, soul2};
        return result;
    }

    vector <vector <vector <char>>> ultimateCrossover() {
        /*
        This function is responsible for performing crossover
        on the entire population. It takes souls with indexes
        k & k + 1 and runs crossoverOfSouls() on them, then adds
        the resulting 'children' into the new population.
        
        Returns:
            new_population - array of 2*POP_MAX souls (POP_MAX previous
                souls & POP_MAX new souls)
        */
        auto new_population = population;
        for (int k = 0; k < POP_MAX - 1; k += 2) {
            auto couple = crossoverOfSouls(population[k], population[k + 1]);
            new_population.push_back(couple.first);
            new_population.push_back(couple.second);
        }
        return new_population;
    }

    bool sortBasedOnFitness(const vector <vector <char>> &soul1, const vector <vector <char>> &soul2) {
        return fitness(soul1) > fitness(soul2);
    }

    void selection(vector <vector <vector <char>>> newPopulation) {

        sort(newPopulation.begin(), newPopulation.end(), 
        [this](vector <vector <char>> soul1, vector <vector <char>> soul2) {
            return this->sortBasedOnFitness(soul1, soul2);
        });
        vector <vector <vector <char>>> slice(newPopulation.begin(), newPopulation.begin() + POP_MAX);
        printf("%f\n", fitness(slice[0]));
        random_shuffle(slice.begin(), slice.end());
        population = slice;
    }    

public:
    Sudoku() {
        inputStencil();
    }

    void outputStencil() {
        /*
        This function outputs the stencil
        */
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                cout << stencil[i][j] << ' ';
            }
            cout << '\n';
        }
    }

    void outputPopulation() {
        /*
        This function outputs the souls from the population
        */
        for (auto &soul : population) {
            for (auto &row : soul) {
                for (auto & el : row) {
                    printf("%c ", el);
                }
                cout << '\n';
            }
            cout << "------------------\n";
        }

    }

    void outputFitnessScores() {
        /*
        This function outputs the fitness scores of the
        entire population sorted in the ascending order
        */
        vector <double> fitnessScores;
        for (auto &soul : population) {
            fitnessScores.push_back(fitness(soul));
        }
        sort(fitnessScores.begin(), fitnessScores.end());
        for (auto &score : fitnessScores) printf("%f\n", score);
    }

    void testOfCrossoverRows() {
        /*
        This function was used to test the correct work of
        crossoverOfRows() function
        */
        auto &soul1 = population[0], &soul2 = population[1];
        for (int j = 0; j < 9; j++) printf("%c ", soul1[0][j]);
        printf("\n");
        for (int j = 0; j < 9; j++) printf("%c ", soul2[0][j]);
        printf("\n");
        crossoverOfRows(soul1[0], soul2[0], stencil[0], 4);
        for (int j = 0; j < 9; j++) printf("%c ", soul1[0][j]);
        printf("\n");
        for (int j = 0; j < 9; j++) printf("%c ", soul2[0][j]);
        printf("\n");
    }

    void solve() {
        /*
        This function combines all stages of an evolutionary algorithm
        and solves the puzzle.
        */
        createPopulation();
        while (!solutionFound) {
            auto new_population = ultimateCrossover();
            selection(new_population);
        }
        for (auto &row : solution) {
            for (auto &el : row) {
                printf("%s ", el);
            }
            printf("\n");
        }
    }
};

int main() {

    srand(time(0));

    auto sudoku = Sudoku();

    sudoku.solve();

    exit(EXIT_SUCCESS);
}