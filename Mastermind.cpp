#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <algorithm>
#include <numeric>
#include <random>
#include <functional>
#include <string>

#include <doctest.h>

// The colors supported by this version of Mastermind
enum class Colors {
    ColorsStart = 0,
    Red = 0,
    Orange,
    Yellow,
    Green,
    Blue,
    Purple,
    ColorsCount,
};
// The number of colors in the code and guess supported
// by this version of Mastermind
constexpr int colors_in_code = 4;

using Code = std::array<Colors, colors_in_code>;

// A container for holding both the code and a guess.
struct CodeAndGuess {
    Code code;
    Code guess;
};

// Prints the code to the stream.
template<typename OutStream>
void PrintCode(OutStream& out, const Code& code ) {
    std::unordered_map<Colors, std::string> color_to_string {
        {Colors::Red   , "Red"},
        {Colors::Orange, "Orange"},
        {Colors::Yellow, "Yellow"},
        {Colors::Green , "Green"},
        {Colors::Blue  , "Blue"},
        {Colors::Purple, "Purple"}
    };
    for( int i = 0; i < colors_in_code; ++i ) {
        out << " " << color_to_string[ code[i] ];
    }
}

// Returns the number of pegs that the guess has that are both
// the right color and in the right position as the code.
int RightColorRightPosition( const CodeAndGuess& code_and_guess) {
    const auto& [code, guess] = code_and_guess;
    return std::transform_reduce(std::begin(guess), std::end(guess)
                                , std::begin(code)
                                , 0
                                , std::plus<int>{} // reduce
                                , std::equal_to<Colors>{} // transform
                                );
}

namespace{ 
    // Returns a map from color to the number of times that color occurs in
    // the code.
    std::unordered_map<Colors, int> GetColorIncidences( const Code& code ) {
        std::unordered_map<Colors, int> color_incidences;
        for( const auto color : code ) {
            color_incidences[color]++;
        }
        return color_incidences;
    }
}  // namespace

// Returns the number of pegs that the guess has that are in
// the right color and but not in the right position as the code. 
// This count does not include element that are RightColorRightPosition.
int RightColorWrongPosition( const CodeAndGuess& code_and_guess) {
    const auto& [code, guess] = code_and_guess;
    std::unordered_map<Colors,int> code_color_incidences = GetColorIncidences(code);
    std::unordered_map<Colors,int> guess_color_incidences = GetColorIncidences(guess);
    int right_colors = 0;
    for( Colors color = Colors::ColorsStart; 
         color < Colors::ColorsCount; 
         color = static_cast<Colors>(static_cast<int>(color)+1) 
        ) {
        right_colors += std::min(code_color_incidences[color], guess_color_incidences[color]);
    }
    return right_colors - RightColorRightPosition(code_and_guess);
}

// Returns a random code of supported colors with length colors_in_code.
Code GetRandomCode() {
    std::random_device rd;
    std::mt19937_64 eng(rd()); 
    std::uniform_int_distribution<unsigned long long> distr;
    Code code{};
    for( int color_in_code = 0; color_in_code < colors_in_code; ++color_in_code ) {
        code[color_in_code] = 
            static_cast<Colors>(distr(eng) % static_cast<unsigned long long>(Colors::ColorsCount));
    }
    return code;
}

// Play the game Mastermind.
void PlayMastermind() {
    std::cout << "Welcome To Mastermind!";
    const Code code = GetRandomCode();
    Code guess{};
    do {
        std::cout << "Please Enter Your Guess: ";
        std::string guess_str;
        std::cin >> guess_str;
        std::transform(guess_str.begin()
                      , std::next(guess_str.begin(), colors_in_code)
                      , guess.begin(), [](const char c){ return static_cast<Colors>(c);}
                      );
        std::cout << "Right Color Wrong Position: " 
                  << RightColorWrongPosition({.code = code, .guess = guess}) 
                  << std::endl;
        std::cout << "Right Color Right Position: " 
                  << RightColorRightPosition({.code = code, .guess = guess}) 
                  << std::endl;
    } while ( code != guess );
    std::cout << "You win! The code was ";
    PrintCode(std::cout, code);
    std::cout << "." << std::endl;
}

TEST_CASE("Mastermind_RightColorRightPosition_0Correct") {    
    const Code code{Colors::Red, Colors::Red, Colors::Red, Colors::Red};
    const Code guess{Colors::Blue, Colors::Blue, Colors::Blue, Colors::Blue};
    REQUIRE( RightColorRightPosition({.code= code, .guess=guess}) == 0 );
}

TEST_CASE("Mastermind_RightColorRightPosition_1Correct") {
    const Code code{Colors::Red, Colors::Blue, Colors::Blue, Colors::Blue};
    const Code guess{Colors::Red, Colors::Yellow, Colors::Yellow, Colors::Yellow};
    REQUIRE( RightColorRightPosition({.code= code, .guess=guess}) == 1 );
}

TEST_CASE("Mastermind_RightColorRightPosition_AllCorrect") {    
    const Code code{Colors::Red, Colors::Blue, Colors::Yellow, Colors::Purple};
    const Code guess{Colors::Red, Colors::Blue, Colors::Yellow, Colors::Purple};
    REQUIRE( RightColorRightPosition({.code= code, .guess=guess}) == 4 );
}

TEST_CASE("Mastermind_RightColorWrongPosition_0Correct") {
    const Code code{Colors::Red, Colors::Red, Colors::Red, Colors::Red};
    const Code guess{Colors::Blue, Colors::Blue, Colors::Blue, Colors::Blue};
    REQUIRE( RightColorWrongPosition({.code= code, .guess=guess}) == 0 );
}

TEST_CASE("Mastermind_RightColorWrongPosition_1Correct") {
    const Code code{Colors::Blue, Colors::Red, Colors::Red, Colors::Red};
    const Code guess{Colors::Orange, Colors::Blue, Colors::Yellow, Colors::Green};
    REQUIRE( RightColorWrongPosition({.code= code, .guess=guess}) == 1 );
}

TEST_CASE("Mastermind_RightColorWrongPosition_1Correct_Also1RightColorRightPosition") {
    const Code code{Colors::Blue, Colors::Red, Colors::Red, Colors::Red};
    const Code guess{Colors::Orange, Colors::Blue, Colors::Yellow, Colors::Red};
    REQUIRE( RightColorWrongPosition({.code= code, .guess=guess}) == 1 );
}

TEST_CASE("Mastermind_GetRandomCode_ReturnsValidCode") {
    for( int i = 0; i < 25; ++i ) {
        const Code code = GetRandomCode();
        REQUIRE( std::size(code) == colors_in_code );
        REQUIRE( std::all_of( std::begin(code), std::end(code), [](const Colors color){
            return Colors::ColorsStart <= color && color < Colors::ColorsCount;
        }));
    }
}

// Need to declare main or CompilerExplorer generates a stub main,
// which causes an ODR violation with the main that DocTest defines.
int main(int argc, char** argv);
