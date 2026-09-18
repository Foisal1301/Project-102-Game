# Bouncing Ball

**Bouncing Ball** is a 2D arcade-style bubble shooter game built with the C programming language and the Raylib graphics library. The objective is to shoot colored balls from a rotating cannon to clear all matching balls on the grid before time runs out or the balls cross the danger line.

---

## Gameplay Demo

> **[Watch on YouTube](https://youtu.be/edtzW-mmVh4)** — See the gameplay mechanics and sound effects in action!

---

## Technical Specifications

- **Language:** C
- **Library:** [Raylib](https://www.raylib.com/) (Graphics & Audio)
- **Window Resolution:** 800 x 600 pixels
- **Target Frame Rate:** 60 FPS
- **Data Persistence:** Local file I/O (`highScore.txt`) for saving top 5 high scores

---

## Game Rules & How to Play

### Objective
Clear the entire board of balls by aiming and shooting colored ammunition from your cannon to make matching groups.

### Rules of Play
1. **Match-3 Mechanics:** Connect **3 or more balls** of the same color to pop them and score points.
2. **Timer:** You start each round with **60 seconds** on the clock.
3. **Time Bonus Ball:** Hitting special **Timer Balls** adds an extra **+10 seconds** to your remaining time.
4. **Game Over Conditions:**
   - **Time Up:** Running out of time results in a game over (`GAMEOVER! TIME UP!`).
   - **Line Cross:** Allowing the balls to stack down and cross the lower danger line results in an instant loss (`GAMEOVER! YOU CROSSED THE LINE!`).
5. **Victory Condition:**
   - Clear all balls from the grid to win! Remaining time is converted into bonus points (`score += timeRemaining * 100`).

---

## Controls

| Key / Input | Action |
| :--- | :--- |
| **Mouse Pointer** | Aim the cannon angle |
| **SPACE / Left Mouse Click** | Fire/Shoot ball |
| **P** | Pause game / Open Resume Menu |
| **E** | Go back from About or Leaderboard pages |
| **UP / DOWN / LEFT / RIGHT** | Navigate menu items |
| **ENTER** | Select menu item |

---

## Audio & Sound Guide

Every gameplay action and event is paired with audio cues and music tracks:

### Background Music (BGM)
- **`menu.wav`**: Plays in the Main Menu, About, and Leaderboard screens.
- **`Gameplay_Resume.mp3`**: Plays during active gameplay and on the Pause/Resume screen.
- **`gameover.mp3`**: Plays on the Game Over screen after a win or loss.
- **`countdown.wav`**: Loops as an audio alert when remaining time falls to **10 seconds or lower**.

### Sound Effects (SFX)
- **`navigate.mp3`**: Triggers when pressing Arrow Keys to navigate menu options or pressing `E`/`P` keys.
- **`Click.mp3`**: Triggers when pressing `ENTER` to confirm a menu selection.
- **`blast.mp3`**: Plays upon firing a ball from the cannon.
- **`Bonus.wav`**: Plays when successfully hitting a special Time Ball to reward +10 seconds.
- **`win.wav`**: Victory chime that plays upon clearing all balls from the board.
- **`gameover_instant.mp3`**: Plays instantly when the timer hits zero or balls cross the danger line.

---

##  Supervisor

- **Md Zim Mim Siddiqee Sowdha**  
  Lecturer, Department of Computer Science and Engineering (CSE), BUET

---

##  Team Members

- **Md. Foisal** - [@Foisal1301](https://github.com/Foisal1301)
- **Shahariar Sajid Swapno** - [@SajidSwapno](https://github.com/SajidSwapno)

---

##  Build & Run

### Prerequisites
Make sure **Raylib** and a **GCC Compiler** are installed on your environment.

### Compilation
Run the following command in your terminal:

```bash
gcc main.c -o bouncing_ball -lraylib -lGL -lm -lpthread -ldl -lrt -lX11