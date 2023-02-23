#include <iostream>
#include <utility>
#include <random>
#include <bitset>
#include <tuple>
#include <vector>
#include <algorithm>
#include <unordered_map>

int get_random_number(uint32_t min, uint32_t max) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);

  return static_cast<int>(dist(rng));
}

int get_fitness(uint16_t genom, std::unordered_map<uint8_t, std::pair<int, float>>& cost_map) {
  int sum = 0;
  float weight = 0;
  
  for (uint8_t i = 0; i < 10; ++i) {
    bool bit = (genom >> i) & 0x01;
    if (bit) { sum += cost_map[i].first; weight += cost_map[i].second; }
  }

  return (weight > 15) ? 0 : sum;
}

void mutate(uint16_t& genom) {
  int chance = get_random_number(1, 1000);

  if (chance == 1) {
    int r = get_random_number(0, 9);
    genom ^= (1 << r);
  }
}

std::tuple<uint16_t, uint16_t> crossover(uint16_t genom_1, uint16_t genom_2) {
  int r = get_random_number(1, 9);

  int tmp = ((1 << (r - 1)) | ((1 << r) - 1));

  auto var1 = genom_2 & tmp;
  auto var2 = genom_1 & tmp;

  auto z1 = genom_1 & (0x03FF & ~tmp);
  auto z2 = genom_2 & (0x03FF & ~tmp);

  z1 |= var1;
  z2 |= var2;

  return {z1, z2};
}

int main() {
  std::unordered_map<uint8_t, std::pair<int, float>> cost_map { {0, {375, 3.5f}}, {1, {300, 2.5}}, {2, {100, 2.0}}, 
    {3, {225, 3.0}}, {4, {50, 1.0}}, {5, {125, 1.75}}, {6, {75, 0.75}}, {7, {275, 3.0}}, {8, {150, 2.5}}, {9, {50, 2.25}} };

  int cycles = 500;
  std::size_t population_size = 1000;
  std::vector<std::pair<uint16_t, int>> population(population_size);

  for (auto & i : population) {
    auto genom = static_cast<uint16_t>(get_random_number(0, 1023));
    i = {genom, get_fitness(genom, cost_map)};
  }

  for (int i = 0; i < cycles + 1; ++i) {
    std::sort(population.begin(), population.end(), [] (const auto& p1, const auto& p2) {
      return p1.second > p2.second;
    });

    if (i == cycles) break;

    for (std::size_t i = 0; i < population.size(); i = i + 2) {
      auto& g1 = population[i];
      auto& g2 = population[i + 1];

      auto [n1, n2] = crossover(g1.first, g2.first);
      mutate(n1);
      mutate(n2);

      g1.first = n1;
      g1.second = get_fitness(n1, cost_map);
      g2.first = n2;
      g2.second = get_fitness(n2, cost_map);
    }
  }
 
  std::cout << std::bitset<10>(population[0].first) << " - " << population[0].second << std::endl;

  return 0;
}
