#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>

using namespace std;

// Global variables
int initial_population_size = 200; // Size of the population
vector<vector<int>> board(9, vector<int>(9, 0));
vector<vector<int>> available(9, vector<int>(9, 0)); // Tracks available numbers for each cell (used for population generation)
vector<vector<int>> changable_indexes; // Tracks indices of cells that can be changed during mutation

// Random number generation setup
std::random_device rd;
std::mt19937 gen(rd());
vector<uniform_int_distribution<>> dis_for_mutate; // Random distributions for mutation
std::uniform_real_distribution<> rate(0, 1); // Random distribution for crossover and mutation rates
double crossover_rate1 = 0.5; // Probability for crossover between individuals
double crossover_rate2 = 0.4; // Secondary crossover rate for gene selection
double mutationRate = 0.1;    // Probability of mutation for a gene

// Class representing an individual in the population
class Individual {
    vector<vector<int>> field; // Sudoku field

public:
    int fitnessScore = 0; // Fitness score (0 means a valid solution)

    // Constructor for creating a new individual by randomly filling available cells
    Individual() : field(9, vector<int>(9, 0)) {
        vector<vector<bool>> is_available(9, vector<bool>(9, true));
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (board[i][j] != 0) { // If the cell is pre-filled in the original puzzle
                    field[i][j] = board[i][j];
                    is_available[i][board[i][j] - 1] = false;
                }
            }
        }

        // Randomly fill changeable cells with available numbers
        for (int i = 0; i < 9; i++) {
            for (int j : changable_indexes[i]) {
                vector<int> available_nums;
                for (int k = 0; k < 9; k++) {
                    if (is_available[i][k]) available_nums.push_back(k + 1);
                }
                std::uniform_int_distribution<> dis(0, available_nums.size() - 1);
                field[i][j] = available_nums[dis(gen)];
                is_available[i][field[i][j] - 1] = false;
            }
        }
        fitnessScore = fitness();
    }

    // Constructor for creating a new individual using crossover
    Individual(Individual &p1, Individual &p2) {
        for (int i = 0; i < 9; i++) {
            field.push_back((rate(gen) <= crossover_rate2) ? p2.field[i] : p1.field[i]); // Select row from parent
            mutate(i); // Apply mutation to the row
        }
        fitnessScore = fitness();
    }

    // Mutate a specific row by swapping elements in changeable cells
    void mutate(int row) {
        for (int j : changable_indexes[row]) {
            if (rate(gen) <= mutationRate) { // Mutation occurs with a probability
                swap(field[row][j], field[row][changable_indexes[row][dis_for_mutate[row](gen)]]);
            }
        }
    }

    // Calculate fitness score of the individual
    int fitness() const {
        int score = 0;

        // Arrays to check for duplicates in columns and blocks
        bool cols[9][9] = {false};
        bool blocks[9][9] = {false};

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                int value = field[i][j] - 1; // Adjust value for 0-based indexing
                // Check for duplicates in columns
                if (cols[j][value]) {
                    score++;
                } else {
                    cols[j][value] = true;
                }
                // Check for duplicates in blocks
                int blockIndex = (i / 3) * 3 + (j / 3);
                if (blocks[blockIndex][value]) {
                    score++;
                } else {
                    blocks[blockIndex][value] = true;
                }
            }
        }
        return score;
    }

    // Print the Sudoku board of the individual
    void print() const {
        for (const auto &row : field) {
            for (int cell : row) cout << cell << " ";
            cout << endl;
        }
    }

    // Comparator for sorting individuals based on fitness score
    bool operator<(const Individual &other) const {
        return fitnessScore < other.fitnessScore;
    }
};

// Input handler
void get_board() {
    ifstream file("input.txt");
    if (!file.is_open()) {
        cerr << "Failed to open file!" << endl;
        exit(1);
    }

    for (int i = 0; i < 9; i++) {
        vector<int> sw; // Stores indices of changeable cells in the row
        for (int j = 0; j < 9; j++) {
            string inp;
            file >> inp;
            if (inp == "-") {
                board[i][j] = 0; // Unfilled cells are represented as 0
                sw.push_back(j);
            } else {
                board[i][j] = stoi(inp);
            }
        }
        changable_indexes.push_back(sw);
        dis_for_mutate.push_back(uniform_int_distribution<>(0, sw.size() - 1)); // Mutation distribution for this row
    }
    file.close();
}

uniform_int_distribution<> random_parent(0, initial_population_size - 1); // Random parent selection

// Start the evolutionary algorithm
void start_ea() {
    // freopen("output.txt", "w", stdout);

    vector<Individual> population;

    // Create the initial population
    for (int i = 0; i < initial_population_size; i++) {
        population.emplace_back();
    }

    sort(population.begin(), population.end());

    int old_score = -1, stagnation = 0; // Variables to track progress and stagnation
    while (population[0].fitnessScore != 0) { // Repeat until a solution is found
        size_t pop_size = population.size();
        for (size_t i = 0; i < pop_size; i++) {
            if (rate(gen) <= crossover_rate1) { // Apply crossover with a certain probability
                population.emplace_back(population[i], population[random_parent(gen)]);
            }
        }

        sort(population.begin(), population.end()); // Sort population by fitness
        population.resize(initial_population_size); // Trim population to initial size

        if (old_score == population[0].fitnessScore) { // Check if fitness stagnates
            if (++stagnation == 500) { // Reset population if stagnation limit is reached
                population.clear();
                stagnation = 0;
                for (int i = 0; i < initial_population_size; i++) {
                    population.emplace_back();
                }
            }
        } else {
            stagnation = 0;
            old_score = population[0].fitnessScore;
        }
    }
    population[0].print(); // Print the solution
}

int main() {
    auto start_time = std::chrono::high_resolution_clock::now(); // Start time
    get_board(); // Load Sudoku board
    start_ea(); // Run the evolutionary algorithm
    auto end_time = std::chrono::high_resolution_clock::now(); // End time
    chrono::duration<double> duration = end_time - start_time;

    cout << duration.count() << endl; // Output runtime
    cout << 0 << endl;
    return 0;
}
