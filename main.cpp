#include <bits/stdc++.h>

using namespace std;

#define POP_MAX 200
#define TH 1 // Threshold
#define MUTATION_AMOUNT 4
#define MUTATION_RATE 0.9
#define EXCHANGE_RATE 0.5
#define CROSSOVER_RATE 0.9

unordered_map <string, int> fitnessMap;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> int_dist(1, 9);
uniform_real_distribution<> real_dist(0, 1);
int digit()
{
    return int_dist(gen);
}
double real()
{
    return real_dist(gen);
}

struct Soul {
    string dna;
    int fitness;

    Soul () {}
    Soul(string dna, int fitness) : dna(dna), fitness(fitness) {}
};

class Sudoku {
private:
    string stencil;
    Soul solution;
    bool solutionFound = false;
    vector <Soul> pop;

    int getFitness(string genetics) {
        /*
        This function calculates the overall fitness of
        a soul based on columns and 3x3 blocks

        Params:
            @param soul chromosome values in the soul

        Returns:
            @return overall fitness score
        */

        if (fitnessMap.find(genetics) != fitnessMap.end()) {
            return fitnessMap[genetics];
        }

        int fitness_value = 0;

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

        // for (int k = 0; k < 9; k += 3) {
        //     for (int g = 0; g < 9; g += 3) {
        //         set <char> numbers_in_block;
        //         for (int i = k; i < k + 3; i++) {
        //             for (int j = g; j < g + 3; j++) {
        //                 numbers_in_block.insert(genetics[i * 9 + j]);
        //             }
        //         }
        //         fitness_value += numbers_in_block.size();
        //     }
        // }

        fitnessMap.insert({genetics, fitness_value});

        if (fitness_value == 162) {
            solutionFound = true;
            solution = Soul(genetics, fitness_value);
        }

        return fitness_value;
    }

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

    bool sortBasedOnFitness(const Soul& soul1, const Soul& soul2) {
        return soul1.fitness > soul2.fitness;
    }

    void createPopulation(int start) {
        /*
        This function generates a population based on the input data.
        Initially, every row has the correct 1-9 elements, which are
        randomly shuffled, except the given ones.
        At the end population is sorted based on fitness.
        */
        for (int k = start; k < POP_MAX; k++) {
            vector <char> perm{'1', '2', '3', '4', '5', '6', '7', '8', '9'};

            char dna[9][9];
            
            for (int k = 0; k < 9; k += 3) {
                for (int g = 0; g < 9; g += 3) {
                    vector <char> perm{'1', '2', '3', '4', '5', '6', '7', '8', '9'};
                    
                    for (int i = k; i < k + 3; i++) {
                        for (int j = g; j < g + 3; j++) {
                            if (stencil[i * 9 + j] != '-') {
                                perm.erase(find(perm.begin(), perm.end(), stencil[i * 9 + j]));
                            }
                        }
                    }

                    random_shuffle(perm.begin(), perm.end());

                    for (int i = k; i < k + 3; i++) {
                        for (int j = g; j < g + 3; j++) {
                            if (stencil[i * 9 + j] == '-') {
                                dna[i][j] = perm[0];
                                perm.erase(perm.begin());
                            }
                            else {
                                dna[i][j] = stencil[i * 9 + j];
                            }
                        }
                    }
                }
            }
            string soul = "";

            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    soul += dna[i][j];
                }
            }

            pop[k] = Soul(soul, getFitness(soul));
        }
    }

    void selection(vector <Soul> &pop) {
        // vector <Soul> new_pop(pop.begin(), pop.begin() + POP_MAX / 2);
        // pop = new_pop;
    }

    void couplesTherapy(Soul &soul1, Soul &soul2) {
        string child1 = soul1.dna, child2 = soul2.dna;
        
        for (int k = 0; k < 9; k += 3) {
            for (int g = 0; g < 9; g += 3) {
                double c = real();
                if (c < EXCHANGE_RATE) {
                    for (int i = k; i < k + 3; i++) {
                        for (int j = g; j < g + 3; j++) {
                            char temp = child1[i * 9 + j];
                            child1[i * 9 + j] = child2[i * 9 + j];
                            child2[i * 9 + j] = temp;
                        }
                    }
                }
                else {
                    continue;
                }
            }
        }

        soul1 = Soul(child1, getFitness(child1));
        soul2 = Soul(child2, getFitness(child2));
    }

    void crossover(vector <Soul> &pop) {
        for (int i = 0; i < POP_MAX; i++) {
            double c = real();
            if (c < CROSSOVER_RATE) {
                Soul soul1 = pop[i], soul2 = pop[rand() % (POP_MAX)];
                couplesTherapy(soul1, soul2);
                mutateSoul(soul1);
                mutateSoul(soul2);
                pop.push_back(soul1);
                pop.push_back(soul2);
            }
        }

        sort(
            pop.begin(),
            pop.end(),
            [this](Soul soul1, Soul soul2) {
                return this->sortBasedOnFitness(soul1, soul2);
            }
        );

        vector <Soul> new_pop(pop.begin(), pop.begin() + POP_MAX);
        pop = new_pop;
    }

    void mutateSoul(Soul &soul) {
        string dna = soul.dna;
        int amount = rand() % MUTATION_AMOUNT + 1;

        int k = digit() / 3;
        int g = digit() / 3;

        for (int itr = 0; itr < amount; itr++) {
            int id1 = digit() - 1;
            while (stencil[(k + id1 / 3) * 9 + (g + id1 % 3)] == '-') id1 = digit() - 1;
            int id2 = digit() - 1;
            while (stencil[(k + id2 / 3) * 9 + (g + id2 % 3)] == '-' && id1 == id2) id2 = digit() - 1;

            char temp = dna[(k + id1 / 3) * 9 + (g + id1 % 3)];
            dna[(k + id1 / 3) * 9 + (g + id1 % 3)] = dna[(k + id2 / 3) * 9 + (g + id2 % 3)];
            dna[(k + id2 / 3) * 9 + (g + id2 % 3)] = temp;
        }
        soul = Soul(dna, getFitness(dna));
    }

    void mutation(vector <Soul> &pop) {
        vector <Soul> new_pop = pop;
        for (int i = POP_MAX * TH / 100; i < POP_MAX; i++) {
            double c = real();
            if (c <= MUTATION_RATE) {
                Soul &soul = new_pop[i];
                mutateSoul(soul);
            }
        }

        sort(
            new_pop.begin(),
            new_pop.end(),
            [this](Soul soul1, Soul soul2) {
                return this->sortBasedOnFitness(soul1, soul2);
            }
        );

        pop = new_pop;
    }

    void outputStats(vector <Soul> pop) {
        for (int i = 0; i < 1; i++) {
            cout << pop[i].fitness << '\n';
        }
        cout << "-----------------------\n";
    }

public:
    Sudoku() {
        pop.resize(POP_MAX);
        inputStencil();
    }

    void solve() {

        createPopulation(0);

        
        int fit_max = 0;
        int gen = 0;
        int total = 0;

        while (!solutionFound) {
            total++;
            vector <Soul> new_pop = pop;

            selection(new_pop);

            crossover(new_pop);

            // mutation(new_pop);

            pop = new_pop;

            if (pop[0].fitness > fit_max) {
                // outputStats(pop);
                gen = 0;
                fit_max = pop[0].fitness;
            }
            else {
                gen++;
            }

            if (fit_max == 160 && gen > 450) {
                createPopulation(5);
                // random_shuffle(pop.begin() + 1, pop.end());
                gen = 0;
            }

            if (gen >= 500) {
                // cout << "\n\nBOOOOOOOOOOOOOOOOOOOOOOM!!!!!\n\n";
                fit_max = 0;
                createPopulation(0);
                // random_shuffle(pop.begin() + 1, pop.end());
                gen = 0;
            }
        }
        
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 8; j++) {
                cout << solution.dna[i * 9 + j] << ' ';
            }
            cout << solution.dna[i * 9 + 8] << '\n';
        }

        exit(0);
    }
};

int main() {

    // freopen("input.txt", "r", stdin);

    auto sudoku = Sudoku();

    sudoku.solve();

    exit(0);
}