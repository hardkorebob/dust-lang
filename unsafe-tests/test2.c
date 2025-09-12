#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct Game Game;
struct Game {
int secret_number;
int last_guess;
int attempts;
};
// Forward declarations
int main();
void checkGuess(const Game* game, int guess);
int getPlayerGuess();
void printWelcome();

int MINNUMBER = 1;
int MAXNUMBER = 10;
void printWelcome() {
printf("=============================\n");
printf("= Welcome to Guess the Number =\n");
printf("=============================\n");
printf("I'm thinking of a number between %d and %d.\n", MINNUMBER, MAXNUMBER);
}
int getPlayerGuess() {
int guess = 0;
printf("Enter your guess: ");
scanf("%d", &guess);
return guess;
}
void checkGuess(const Game* game, int guess) {
if ((guess < game->secret_number)) {
printf("--> Too low! Try again.\n");
} else if ((guess > game->secret_number)) {
printf("--> Too high! Try again.\n");
} else {
printf("--> You got it! The number was %d.\n", game->secret_number);
}
}
int main() {
srand(time(NULL));
printWelcome();
Game game;
int secret = ((rand() % MAXNUMBER) + MINNUMBER);
game.secret_number = secret;
int player_guess = getPlayerGuess();
checkGuess(&game, player_guess);
if ((player_guess != game.secret_number)) {
int second_guess = getPlayerGuess();
checkGuess(&game, second_guess);
}
return 0;
}
