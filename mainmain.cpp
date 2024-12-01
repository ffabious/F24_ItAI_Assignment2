#include <bits/stdc++.h>

using namespace std;

#define POP_MAX 200
#define MUTATION_RATE 0.5
#define LOW 1
#define HIGH 99

unordered_map<string, int> fitnessMap;

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

        fitnessMap.insert({genetics, fitness_value});
    }

    Soul() {}

    Soul(string genetics, string stencil) : genetics(genetics) {
        if (fitnessMap.find(genetics) == fitnessMap.end()) {
            calculateFitness(stencil);
        }
        else {
            fitness_value = fitnessMap[genetics];
        }
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

    void createPopulation(int start) {
        /*
        This function generates a population based on the input data.
        Initially, every row has the correct 1-9 elements, which are
        randomly shuffled, except the given ones, those are fixed in place.
        At the end population is sorted based on fitness.
        */
        for (int k = start; k < POP_MAX; k++) {
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

        sort(
            population.begin(),
            population.end(),
            [this](Soul soul1, Soul soul2) {
                return this->sortBasedOnFitness(soul1, soul2);
            }
        );
    }

    bool sortBasedOnFitness(const Soul& soul1, const Soul& soul2) {
        return soul1.fitness_value > soul2.fitness_value;
    }

    void exchangeIndices(int &ft, int &sd) {
        if (ft <= sd) return;
        else {
            int temp = ft;
            ft = sd;
            sd = temp;
        }
    }

    void crossover(vector <Soul> &pop) {
        vector <Soul> new_pop(pop.begin(), pop.begin() + POP_MAX / 2);

        // random_shuffle(new_pop.begin(), new_pop.end());

        for (int i = 0; i < POP_MAX / 2; i += 2) {
            int ft = rand() % 80 + 1, sd = rand() % 80 + 1;
            exchangeIndices(ft, sd);
            string gen1 = new_pop[i].genetics.substr(0, ft)
                + new_pop[i + 1].genetics.substr(ft, sd - ft)
                + new_pop[i].genetics.substr(sd, 81);
            string gen2 = new_pop[i + 1].genetics.substr(0, ft)
                + new_pop[i].genetics.substr(ft, sd - ft)
                + new_pop[i + 1].genetics.substr(sd, 81);
            new_pop.push_back(Soul(gen1, stencil));
            new_pop.push_back(Soul(gen2, stencil));
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

    void mutation(vector <Soul> &pop) {
        vector <Soul> mutated_population = pop;

        for (int itr = 0; itr < pop.size(); itr++) {
            string gen = pop[itr].genetics;
            for (int i = 0; i < 81; i += 9) {
                double c = ((double) rand() / (RAND_MAX));
                int id1 = rand() % 9, id2 = rand() % 9;
                if (c <= MUTATION_RATE) {
                    while (stencil[i + id1] == '-') { id1 = rand() % 9; }
                    while (stencil[i + id2] == '-') { id2 = rand() % 9; }
                    string gen = pop[i].genetics;
                    char temp = gen[i + id1];
                    gen[i + id1] = gen[i + id2];
                    gen[i + id2] = temp;
                }
            }
            mutated_population[itr] = Soul(gen, stencil);
        }

        // for (int itr = 0; itr < mutated_population.size(); itr++) {
        //     for (int i = 0; i < 9; i++) {
        //         for (int j = 0; j < 9; j++) {
        //             double c = ((double) rand() / (RAND_MAX));
        //             int other = rand() % 9;
        //             if (c <= MUTATION_RATE && stencil[i * 9 + j] == '-' && other != j && stencil[i * 9 + other] == '-') {
        //                 string gen = mutated_population[itr].genetics;
        //                 char temp = gen[i * 9 + j];
        //                 gen[i * 9 + j] = gen[i * 9 + other];
        //                 gen[i * 9 + other] = temp;
        //                 mutated_population[itr] = Soul(gen, stencil);
        //                 continue;
        //             }
        //         }
        //     }
        // }

        pop = mutated_population;

    }

    void selectionEtAl() {

        vector <Soul> new_population = population;

        crossover(new_population);

        vector <Soul> best_population(new_population.begin(), new_population.begin() + POP_MAX * LOW / 100);
        vector <Soul> avg_population(new_population.begin() + POP_MAX * LOW / 100, new_population.begin() + POP_MAX * HIGH / 100);
        vector <Soul> worst_population(new_population.begin() + POP_MAX * HIGH / 100, new_population.end());

        mutation(avg_population);

        new_population.clear();
        new_population.insert(new_population.end(), best_population.begin(), best_population.end());
        new_population.insert(new_population.end(), avg_population.begin(), avg_population.end());
        new_population.insert(new_population.end(), worst_population.begin(), worst_population.end());
 
        sort(
            new_population.begin(),
            new_population.end(),
            [this](Soul soul1, Soul soul2) {
                return this->sortBasedOnFitness(soul1, soul2);
            }
        );

        population = new_population;

        if (population[0].fitness_value == 243) {
            solution = new_population[0];
            solutionFound = true;
        }

    }

public:
    void outputPopulation(vector <Soul> pop) {
        for (auto &soul : pop) {
            cout << soul.genetics << ' ' << soul.fitness_value << '\n';
        }
    }
    
    Sudoku() {
        population.resize(POP_MAX);
        inputStencil();
    }

    void solve() {
        /*
            (1) Create population
            (2) Selection
                (2.1) Sort population based on fitness
                (2.2) Preserve some souls
            (3) Crossover on other
            (4) Mutation on new
            (5) Run it back!
        */

        createPopulation(0);

        int fit_max = 0, generation = 0;

        while (!solutionFound) {
            selectionEtAl();

            if (fit_max < population[0].fitness_value) {
                cout << population[0].genetics << ' ' << population[0].fitness_value << '\n';
                cout << population[1].genetics << ' ' << population[1].fitness_value << '\n';
                cout << population[2].genetics << ' ' << population[2].fitness_value << "\n\n";
                fit_max = population[0].fitness_value;
                generation = 0;
            }
            else { generation++; }

            if (generation >= 600) {
                createPopulation(0);
                fit_max = 0;
                generation = 0;
                cout << "\nBOOOOOOOOOOOOOOOOOOOOOOOOOOOOOM!\n\n";
            }
        }
    }
};

int main() {

    freopen("input.txt", "r", stdin);
    // freopen("output.txt", "w", stdout);

    // srand(time(0));

    auto sudoku = Sudoku();
    
    sudoku.solve();

    exit(EXIT_SUCCESS);
}