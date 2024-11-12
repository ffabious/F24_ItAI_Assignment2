#include <bits/stdc++.h>

using namespace std;

#define POP_MAX 1000
#define MUTATION_RATE 0.05

class Sudoku {
private:
    string stencil;
    string solution;
    bool solutionFound = false;
    vector <string> population;
    unordered_map<string, double> fitnessMap;

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
        Initially, every row has the correct 1-9 elements, which are randomly shuffled,
        except the given ones, those are fixed in place.
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
            population[k] = soul;
        }
    }

    double fitnessBasedOnRows(string soul) {
        /*
        This function calculates fitness of a soul in range [0;1]
        based on the number of unique digits in the rows

        Params:
            soul - chromosome values in the soul

        Returns:
            fitness score based on rows in range [0;1]
        */
        double instances = 0.0;
        for (int i = 0; i < 9; i++) {
            set <char> numbers_in_row;
            for (int j = 0; j < 9; j++) {
                numbers_in_row.insert(soul[i * 9 + j]);
            }
            instances += numbers_in_row.size();
        }
        return instances / 81.0;
    }

    double fitnessBasedOnCols(string soul) {
        /*
        This function calculates fitness of a soul in range [0;1]
        based on the number of unique digits in the columns

        Params:
            soul - chromosome values in the soul

        Returns:
            fitness score based on columns in range [0;1]
        */
        double instances = 0.0;
        for (int j = 0; j < 9; j++) {
            set <char> numbers_in_col;
            for (int i = 0; i < 9; i++) {
                numbers_in_col.insert(soul[i * 9 + j]);
            }
            instances += numbers_in_col.size();
        }
        return instances / 81.0;
    }

    double fitnessBasedOnBlocks(string soul) {
        /*
        This function calculates fitness of a soul in range [0;1]
        based on the number of unique digits in the 3x3 blocks

        Params:
            soul - chromosome values in the soul

        Returns:
            fitness score based on primary 3x3 blocks in range [0;1]
        */
        double instances = 0.0;
        for (int k = 0; k < 9; k += 3) {
            for (int g = 0; g < 9; g += 3) {
                set <char> numbers_in_block;
                for (int i = k; i < k + 3; i++) {
                    for (int j = g; j < g + 3; j++) {
                        numbers_in_block.insert(soul[i * 9 + j]);
                    }
                }
                instances += numbers_in_block.size();
            }
        }
        return instances / 81.0;
    }

    void fitness(string soul) {
        /*
        This function calculates the overall fitness of a soul in
        range [0;1] as the average of fitness scores based on rows,
        columns and 3x3 blocks

        Params:
            @param soul chromosome values in the soul

        Returns:
            @return overall fitness score - average of fitness scores based
            on rows, columns and blocks
        */
        double fitnessValue = (
            fitnessBasedOnRows(soul) +
            fitnessBasedOnCols(soul) +
            fitnessBasedOnBlocks(soul)
        ) / 3.0;
        if (fitnessValue == 1.0) {
            solutionFound = true;
            solution = soul;
        }

        fitnessMap.insert({soul, fitnessValue});
    }

    pair<string, string> crossoverOfRowsFast(string row1, string row2, string stencilRow, int pos) {
        
        string new_row1 = row1.substr(0, pos) + row2.substr(pos, row2.size());
        string new_row2 = row2.substr(0, pos) + row1.substr(pos, row2.size());

        return {new_row1, new_row2};
    }

    pair<string, string> crossoverOfRowsSmart(string row1, string row2, string stencilRow, int pos) {
        string new_row1 = stencilRow, new_row2 = stencilRow;

        set <char> used_in_row1;
        for (int j = 0; j < pos; j++) {
            new_row1[j] = row1[j];
            used_in_row1.insert(row1[j]);
        }
        int k = pos;
        for (int j = pos; j < pos + 9; j++) {
            if (new_row1[k] != '-') {
                k++;
                continue;
            }
            if (used_in_row1.find(row2[j % 9]) == used_in_row1.end()) {
                new_row1[k] = row2[j % 9];
                used_in_row1.insert(row2[j % 9]);
                k++;
            }
            if (k == 9) {
                break;
            }
        }

        set <char> used_in_row2;
        for (int j = pos; j < 9; j++) {
            new_row2[j] = row2[j];
            used_in_row2.insert(row2[j]);
        }
        k = 0;
        for (int j = 0; j < 9; j++) {
            if (new_row2[k] != '-') {
                k++;
                continue;
            }
            if (used_in_row2.find(row1[j]) == used_in_row2.end()) {
                new_row2[k] = row1[j];
                used_in_row2.insert(row1[j]);
                k++;
            }
            if (k == pos) {
                break;
            }
        }

        return {new_row1, new_row2};
    }

    pair <string, string> crossoverOfSouls(string soul1, string soul2) {
        /*
        This function is responsible for the crossover of
        2 souls.

        Params:
            soul1 - chromosome values in the 1st soul
            soul2 - chromosome values in the 2nd soul

        Result:
            result - pair of chromosome strings, which
            represent 2 'children' of the given souls
        */
        int pos = rand() % 8 + 1;

        string new_soul1 = "", new_soul2 = "";
        for (int i = 0; i < 9; i++) {
            auto new_rows = crossoverOfRowsSmart(
                soul1.substr(i * 9, 9),
                soul2.substr(i * 9, 9),
                stencil.substr(i * 9, 9),
                pos
            );
            new_soul1 += new_rows.first;
            new_soul2 += new_rows.second;
        }
        
        return {new_soul1, new_soul2};
    }

    vector <string> ultimateCrossover() {
        /*
        This function is responsible for performing crossover
        on the entire population. It takes souls with indexes
        k & k + 1 and runs crossoverOfSouls() on them, then adds
        the resulting 'children' into the new population.
        
        Returns:
            new_population - array of 2*POP_MAX souls (POP_MAX previous
                souls & POP_MAX new souls)
        */
        auto timer = chrono::system_clock::now().time_since_epoch();
        auto new_population = population;
        for (int k = 0; k < POP_MAX - 1; k += 2) {
            auto couple = crossoverOfSouls(
                population[k],
                population[k + 1]
            );
            new_population.push_back(couple.first);
            new_population.push_back(couple.second);
        }

        printf("Crossover(ms) = %d\n", chrono::duration_cast<chrono::milliseconds>
            (chrono::system_clock::now().time_since_epoch() - timer).count());

        return new_population;
    }

    vector <string> performMutation(vector <string> population) {
        vector <string> mutated_population = population;

        auto timer = chrono::system_clock::now().time_since_epoch();

        for (int itr = 3; itr < POP_MAX; itr++) {
            for (int k = 0; k < 81; k++) {
                if (stencil[k] == '-') {
                    double c = rand() % 100 / 100;
                    if (c < MUTATION_RATE) {
                        mutated_population[itr][k] = rand() % 9 + 49;
                    }
                }
            }
        }

        printf("Mutation(ms) = %d\n", chrono::duration_cast<chrono::milliseconds>
            (chrono::system_clock::now().time_since_epoch() - timer).count());

        outputSolutionLikeString(mutated_population[0]);

        return mutated_population;
    }

    bool sortBasedOnFitness(const string &soul1, const string &soul2) {
        if (fitnessMap.find(soul1) == fitnessMap.end()) {
            fitness(soul1);
        }
        if (fitnessMap.find(soul2) == fitnessMap.end()) {
            fitness(soul2);
        }
        return fitnessMap[soul1] > fitnessMap[soul2];
    }

    void selection(vector <string> newPopulation) {

        auto timer = chrono::system_clock::now().time_since_epoch();
        sort(
            newPopulation.begin(),
            newPopulation.end(),
            [this](string soul1, string soul2) {
                return this->sortBasedOnFitness(soul1, soul2);
            }
        );

        printf("Sort(ms) = %d\n", chrono::duration_cast<chrono::milliseconds>
            (chrono::system_clock::now().time_since_epoch() - timer).count());

        vector <string> mutated_population = performMutation(newPopulation);

        vector <string> slice(mutated_population.begin(), mutated_population.begin() + POP_MAX);

        printf("%f\n", fitnessMap[slice[0]]);

        random_shuffle(slice.begin(), slice.end());
        population = slice;
    }

public:
    Sudoku() {
        inputStencil();
    }

    void outputPopulation() {
        for (auto &soul : population) {
            cout << soul << '\n';
        }
    }

    

    void outputStencilLikeTable() {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                cout << stencil[i * 9 + j] << ' ';
            }
            printf("\n");
        }
    }

    void outputSolutionLikeString(string solution) {
        cout << solution << '\n';
    }

    void outputSolutionLikeTable(string solution) {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                cout << solution[i * 9 + j] << ' ';
            }
            printf("\n");
        }
    }

    void solve() {
        createPopulation();
        while (!solutionFound) {
            auto timer = chrono::system_clock::now().time_since_epoch();
            auto new_population = ultimateCrossover();
            selection(new_population);
            printf("Gen(ms) = %d\n", chrono::duration_cast<chrono::milliseconds>
                (chrono::system_clock::now().time_since_epoch() - timer).count());
            timer = chrono::system_clock::now().time_since_epoch();
            printf("------------------------\n");
        }
        outputSolutionLikeTable(solution);
    }
};

int main() {

    srand(time(0));

    auto sudoku = Sudoku();

    sudoku.solve();

    exit(EXIT_SUCCESS);
}