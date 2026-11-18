# THE LOCKER HEIST

### A Console Based Detective Mystery Game

**Object-Oriented Programming Semester Project**\

## Team Members

  ----------------------------------------------------------------------
  No.               Name                               Roll Number      
  ----------------- ---------------------------------- -----------------

  1                 `Kashaf Fatima`                  `25F-3000`    

  2                 `Noor Ul Eiman`                  `25F-3039`

  ----------------------------------------------------------------------

**Section:** `BSE-3B`\
**Lab Instructor:** `Sir Mozeb Ahmad`

------------------------------------------------------------------------

## Project Overview

**The Locker Heist** is a single-player, menu-driven detective mystery
game developed in C++ as a semester project for the Object-Oriented
Programming course. The project applies object-oriented programming
principles to model the characters, locations, clues, tile racks,
Scrabble board, and game engine as interacting components.

The detective must investigate a theft, collect clues, solve a two-word
puzzle, and identify the culprit before running out of turns or
credibility.

## Game Scenario

A valuable locker hidden behind a bookshelf in a wealthy business
owner's cottage has been robbed during a party. The detective is hired
to investigate the incident.

The cottage contains **seven locations** and **eight people**: the
business owner (victim) and seven suspects. Each location provides a
letter for the first word, while each person provides a letter for the
second word. The detective collects these letters and places them at
their assigned positions on a virtual Scrabble board.

Once both words are complete, they provide a clue to the culprit's role
and nationality. The detective must then make the correct accusation to
win the game.


## Gameplay

The game follows a turn-based investigation process:

1.  Start a new game and enter the detective's name.
2.  Visit the cottage's locations to collect the letters for **Word 1**.
3.  Interview the people present at the cottage to collect the letters
    for **Word 2**.
4.  View the Scrabble board to check collected and placed letters.
5.  Form the two words using the collected clues.
6.  Use the revealed words to determine the culprit's role.
7.  Accuse the suspected character before the game-ending conditions are
    reached.

## Features

The project is designed around the following functionality.

-   Menu-driven interface with options to start a game, view scores,
    read information and rules, or exit.
-   Investigation of seven cottage locations.
-   Interviews with the victim and seven suspects.
-   Collection and placement of clue letters on a two-row Scrabble
    board.
-   Word formation and culprit identification.
-   Credibility and turn tracking.
-   Win and lose conditions.
-   Persistent leader/score board.
-   Input validation and exception handling.

## Project Structure

The project uses a three-file C++ structure to separate declarations,
implementations, and program execution.

``` text
The-Locker-Heist/
│
├── heist.h               
├── heist.cpp            
├── main.cpp             
│
├── characters.txt       
├── locations.txt           
├── clue.txt                
├── rules.txt
├── about.txt               
│
├── leader/
│   └── scoreBoard.txt    
│
└── README.md               
```

## Data Files

  -----------------------------------------------------------------------
  File                                Purpose
  ----------------------------------- -----------------------------------
  `characters.txt`                    Stores the names, roles,
                                      nationalities, and character clue
                                      data.

  `locations.txt`                     Stores cottage locations and their
                                      clue-letter information.

  `clue.txt`                          Stores the word profiles associated
                                      with the possible culprits.

  `rules.txt`                         Stores game limits, credibility
                                      settings, and win/lose conditions.

  `about.txt`                         Contains information about the game
                                      and its developers.

  `leader/scoreBoard.txt`             Stores previous game results and
                                      scores.
  -----------------------------------------------------------------------

## Known Limitations
- Console only. There is no graphical interface.
- The data files must be in the same folder as the program.
- The game has fixed sizes: 7 locations, 8 people, 7 possible culprits.
- A game cannot be saved and resumed. Quitting loses the current game.
- Characters and words are changed only by editing the `.txt` files
