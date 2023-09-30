#include <iostream>
#include <utility>
#include <random>
#include <bitset>
#include <tuple>
#include <vector>
#include <algorithm>
#include <unordered_map>

/**
 * Generates random integer between min(inclusive) and max(inclusive).
 * Would be better to reuse the random_device etc., but it's a small project
 * so I don't care.
 * @param min smallest number(inclusive) which can be generated.
 * @param max largest number(inclusive) which can be generated.
 * @return random integer number
 */
int get_random_number(uint32_t min, uint32_t max) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);

    return static_cast<int>(dist(rng));
}

/**
 * Calculates the fitness of a genom. The genom is a 10 bit number.
 * Each bit corresponds to a value and weight. You can find each pair in the cost map.
 * For example: Genom 0b0000000001 -> 0th bit is set -> cost_map[0] = {375, 3.5f}.
 * This genom would have the value 375 and weight 3.5. If other bits are set, the values and weights accumulate.
 * @param genom the genom for which the cost will be calculated.
 * @param cost_map pairs of value and weight for each bit position.
 * @return the fitness as integer.
 */
int get_fitness(uint16_t genom, std::unordered_map<uint8_t, std::pair<int, float>> &cost_map) {
    int sum = 0;
    float weight = 0;

    for (uint8_t i = 0; i < 10; ++i) {
        // Get the curren bit
        uint8_t bit = (genom >> i) & 0x01;

        // If bit is set add value and weight
        if (bit) {
            sum += cost_map[i].first;
            weight += cost_map[i].second;
        }
    }

    // If the weigt is > 15 the fitness is 0
    return (weight > 15) ? 0 : sum;
}

/**
 * The is a small chance of 0.01 % that the genom mutates.
 * @param genom the genom which could mutate.
 */
void mutate(uint16_t &genom) {
    int chance = get_random_number(1, 1000);

    // If the genom mutates, get a random bit and flip it.
    if (chance == 0) {
        int r = get_random_number(0, 9);
        genom ^= (1 << r);
    }
}

/**
 * Creates two new genoms. For example:
 * 11111|00000 00000|11111 -> 1111111111 0000000000
 * @param genom_1 The first genom for crossover.
 * @param genom_2 The second genom for crossover.
 * @return Two new genoms as a tuple.
 */
std::tuple<uint16_t, uint16_t> crossover(uint16_t genom_1, uint16_t genom_2) {
    // Get a random position for the crossover.
    int r = get_random_number(1, 9);

    // Example: if r = 4:
    // 0000 ... 1111
    int tmp = ((1 << (r - 1)) | ((1 << r) - 1));

    // Right half of genom 2
    auto var1 = genom_2 & tmp;

    // Right half of genom 1
    auto var2 = genom_1 & tmp;

    // This clears the right half of the genoms.
    // Negation of tmp is the left half.
    // We have 10 bit, but this is a 16 bit number, so we need to clear the first 6 bits -> and with 0x03FF.
    // 0x03FF = 0000 0011 1111 1111 -> we keep only 10 bits.
    auto z1 = genom_1 & (0x03FF & ~tmp);
    auto z2 = genom_2 & (0x03FF & ~tmp);

    // Now the right half of both genoms is cleared, so we can combine it and get the crossover.
    z1 |= var1;
    z2 |= var2;

    return {z1, z2};
}

int main() {
    // This cost map decodes specific bits of a genom to a value and a weight.
    std::unordered_map<uint8_t, std::pair<int, float>> cost_map{{0, {375, 3.5f}},
                                                                {1, {300, 2.5}},
                                                                {2, {100, 2.0}},
                                                                {3, {225, 3.0}},
                                                                {4, {50,  1.0}},
                                                                {5, {125, 1.75}},
                                                                {6, {75,  0.75}},
                                                                {7, {275, 3.0}},
                                                                {8, {150, 2.5}},
                                                                {9, {50,  2.25}}};

    // How often we will run the "simulation"
    int cycles = 500;

    std::size_t population_size = 1000;
    std::vector<std::pair<uint16_t, int>> population(population_size);

    // Fill the initial population with random genoms. Each genom has a fitness score.
    // 1023 because we have 10 bits in a genom. (2^10 - 1).
    for (auto &i: population) {
        auto genom = static_cast<uint16_t>(get_random_number(0, 1023));
        i = {genom, get_fitness(genom, cost_map)};
    }

    // Simulate one generation.
    // Two best genoms are selected and combined with crossover. The genoms have a chance for mutation.
    for (int i = 0; i < cycles + 1; ++i) {
        // We need to sort it, because then we can select pairs of best genoms.
        std::sort(population.begin(), population.end(), [](const auto &p1, const auto &p2) {
            return p1.second > p2.second;
        });

        if (i == cycles) break;

        // Generate new genoms and the corresponding fitness score.
        for (std::size_t i = 0; i < population.size(); i = i + 2) {
            auto &g1 = population[i];
            auto &g2 = population[i + 1];

            auto [n1, n2] = crossover(g1.first, g2.first);
            mutate(n1);
            mutate(n2);

            g1.first = n1;
            g1.second = get_fitness(n1, cost_map);
            g2.first = n2;
            g2.second = get_fitness(n2, cost_map);
        }
    }

    // Print the best genom and it's score.
    std::cout << std::bitset<10>(population[0].first) << " - " << population[0].second << std::endl;

    return 0;
}
