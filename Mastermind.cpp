#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <algorithm>
#include <numeric>
#include <random>
#include <functional>
#include <string>

#include <doctest.h>

constexpr std::string_view colors = "roygbp";
constexpr int colors_in_code = 4;

struct CodeAndGuess {
    std::string code;
    std::string guess;
};

int RightColorRightPosition( CodeAndGuess code_and_guess) {
    const auto& [code, guess] = code_and_guess;
    return std::transform_reduce(std::begin(guess), std::end(guess)
                                , std::begin(code)
                                , 0
                                , std::plus<int>{} // reduce
                                , std::equal_to<int>{} // transform
                                );
}

namespace{ 
    std::unordered_map<char, int> GetColorIncidences( std::string code ) {
        std::unordered_map<char, int> color_incidences;
        for( const auto color : code ) {
            color_incidences[color]++;
        }
        return color_incidences;
    }
}  // namespace

int RightColorWrongPosition( CodeAndGuess code_and_guess) {
    const auto& [code, guess] = code_and_guess;
    std::unordered_map<char,int> code_color_incidences = GetColorIncidences(code);
    std::unordered_map<char,int> guess_color_incidences = GetColorIncidences(guess);
    int right_colors = 0;
    for( const auto color : colors ) {
        right_colors += std::min(code_color_incidences[color], guess_color_incidences[color]);
    }
    return right_colors - RightColorRightPosition(code_and_guess);
}

std::string GetRandomCode() {
    std::random_device rd;
    std::mt19937_64 eng(rd()); 
    std::uniform_int_distribution<unsigned long long> distr;
    std::string code;
    for( int color_in_code = 0; color_in_code < colors_in_code; ++color_in_code ) {
        code += colors[distr(eng) % std::size(colors)];
    }
    return code;
}

void PlayMatermind() {
    std::cout << "Welcome To Mastermind!";

    std::string guess = "";
    std::string code = GetRandomCode();
    do {
        std::cout << "Please Enter Your Guess: ";
        std::cin >> guess;
        std::cout << "Right Color Wrong Position: " 
                  << RightColorWrongPosition({.code = code, .guess = guess}) 
                  << std::endl;
        std::cout << "Right Color Right Position: " 
                  << RightColorRightPosition({.code = code, .guess = guess}) 
                  << std::endl;
    } while ( code != guess );
    std::cout << "You win! The code was " << code << "." << std::endl;
}

TEST_CASE("Mastermind_RightColorRightPosition_0Correct") {    
    std::string code = "rrrr";
    std::string guess = "bbbb";
    REQUIRE( RightColorRightPosition({.code= code, .guess=guess}) == 0 );
}

TEST_CASE("Mastermind_RightColorRightPosition_1Correct") {    
    std::string code = "rbbb";
    std::string guess = "ryyy";
    REQUIRE( RightColorRightPosition({.code= code, .guess=guess}) == 1 );
}

TEST_CASE("Mastermind_RightColorRightPosition_AllCorrect") {    
    std::string code = "rbyp";
    std::string guess = "rbyp";
    REQUIRE( RightColorRightPosition({.code= code, .guess=guess}) == 4 );
}

TEST_CASE("Mastermind_RightColorWrongPosition_0Correct") {
    std::string code = "rrrr";
    std::string guess = "bbbb";
    REQUIRE( RightColorWrongPosition({.code= code, .guess=guess}) == 0 );
}

TEST_CASE("Mastermind_RightColorWrongPosition_1Correct") {
    std::string code = "brrr";
    std::string guess = "obyg";
    REQUIRE( RightColorWrongPosition({.code= code, .guess=guess}) == 1 );
}

TEST_CASE("Mastermind_RightColorWrongPosition_1Correct_Also1RightColorRightPosition") {
    std::string code = "brrr";
    std::string guess = "obyr";
    REQUIRE( RightColorWrongPosition({.code= code, .guess=guess}) == 1 );
}

TEST_CASE("Mastermind_GetRandomCode_ReturnsValidCode") {
    for( int i = 0; i < 25; ++i ) {
        std::string code = GetRandomCode();
        REQUIRE( std::size(code) == colors_in_code );
        REQUIRE( std::all_of( std::begin(code), std::end(code), [](const char color){
            return std::find(std::begin(colors), std::end(colors), color) != std::end(colors);
        }));
    }
}

// Need to declare main or CompilerExplorer generates a stub main,
// which causes an ODR violation with the main that DocTest defines.
int main(int argc, char** argv);
