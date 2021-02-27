// Coursework.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "Maze.h"

const int input_set_size = 16;

bool in(char set[input_set_size], char val) {
    bool is_in = false;
    for (int i = 0; i < input_set_size; i++) {
        is_in = set[i] == val ? is_in | 1 : is_in | 0;
    }
    return is_in;
}

int main()
{
    
    char prev_input = 0;
    while (prev_input != '1' && prev_input != '2') {
        std::cout << "If you wish to load a maze from a file, enter 1.\nIf you wish to generate a new maze, enter 2.\n";
        std::cin >> prev_input;
    }
    int width = 0;
    int height = 0;
    std::string loc = "";
    if (prev_input == '2') {
        
        while (width <= 0 && height <= 0) {
            std::cout << "Enter maze width:\n";
            std::cin >> width;
            std::cout << "Enter maze height:\n";
            std::cin >> height;
        }
    }
    else {
        while (loc.size() <= 0) {
            std::cout << "Please enter the file location of a maze you wish to open:\n";
            std::getline(std::cin, loc);
        }
    }
    Maze* m = ((prev_input == '2') ? new Maze(width, height) : new Maze(loc));
    int seed = 0;
    m->SetSeed(seed);
    char available[input_set_size] = { 'G', 'Q', 'E', 'R', 'S', 'g', 'q', 'e', 'r', 's', 'V', 'v' , 'J', 'j', 'P', 'p'};
    while (prev_input != 82 && prev_input != 83) {
        /*Output options:
        * G: Generate
        * Q: Set Seed
        * E: Add Exit
        * R: Restart
        * V: Solve
        * J: Save
        * S: Stop
        */
        prev_input = 0;
        while (!in(available, prev_input)) {
            std::cout << "Please enter one of the following commands:\nG: Generate Maze\nQ: Set Seed\nE: Add Exit\nR: Reset Maze\nV: Solve\nP: Simulate Play (Requires 1 or more exits)\nJ: Save Maze\nS: Stop Program\n";
            std::cin >> prev_input;
        }
        system("CLS");
        prev_input = prev_input > 91 ? prev_input - 32 : prev_input;
        switch (prev_input) {
        case('G'):
            if (m->Generated()) {
                std::cout << "Maze already generated. Redrawing...\n";
            }
            else {
                m->Generate();
            }
            m->Draw();
            break;
        case('Q'):
            if (m->Generated()) {
                std::cout << "You can only set the seed before generation. Type 'R' to start again if you wish to set the seed.\n";
            }
            else {
                std::cout << "Please enter a seed of choice. Entering 0 will use a random seed:\n";
                std::cin >> seed;
                m->SetSeed(seed);
            }
            break;
        case('E'):
            if (m->Generated()) {
                m->AddExit();
                m->Draw();
                std::cout << "Exit created. Total: " << m->CountExits() << "\n";
            }
            else {
                std::cout << "Maze not generated. Please run 'G' to generate.\n";
            }
            break;
        case('V'):
            if (m->CountExits() == 0) {
                std::cout << "No exits set. Nothing to solve.\nTry adding a new exit with 'E'.\n";
            }
            else {
                m->Solve();
                m->Draw();
            }
            break;
        case('J'):
            while (loc.size() <= 0) {
                std::cout << "Please enter where you wish to save the file to:\n";
                std::getline(std::cin, loc);
            }
            m->Save(loc);
            break;
        case('P'):
            while (prev_input != 'y' && prev_input != 'n' && prev_input != 'Y' && prev_input != 'N') {
                std::cout << "Do you wish to animate the gameplay?\nY: Animate as frames\nN: Show every frame in a list\n";
                std::cin >> prev_input;
            }
            prev_input = prev_input > 91 ? prev_input - 32 : prev_input;
            int exit_code = m->Play(prev_input == 'Y' ? 1 : 0);
            
            std::cout << "Maze game exited with code: " << exit_code << ".\n";
            if (exit_code == 1) {
                std::cout << "All players reached the end.\n";
            }
            else if(exit_code == 2){
                std::cout << "Unsolvable as all players are stuck.\n";
            }
            else {
                std::cout << "No Exits / Maze not generated.\n";
            }
            break;
        }

    }
    return prev_input == 'R' ? main() : 0;
}