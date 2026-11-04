#pragma once

#include <array>

class Item {
  public:
    long registrationNumber;
    float grade;
    std::array<char, 2> state;
    std::array<char, 50> city;
    std::array<char, 30> degree;

    Item(long registrationNumber, float grade, std::array<char, 2> state,
         std::array<char, 50> city, std::array<char, 30> degree)
        : registrationNumber(registrationNumber), grade(grade), state(state),
          city(city), degree(degree) {}
};
