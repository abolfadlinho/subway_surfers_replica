# Urban Runner - 3D OpenGL Game

## Overview
Urban Runner is a 3D game developed using OpenGL 3D and C++. The game is a replica of the popular Subway Surfers, where the player must collect points and avoid obstacles, such as trains and trash cans, to win the game. The game is designed to simulate a runner game with multiple scenes and various levels of difficulty.

## Gameplay

In **Urban Runner**, the player collects points and avoids obstacles in order to progress through levels. The game features:
- **Collecting Coins/Diamonds**: Collecting coins in Level 1 or diamonds in Level 2 increments the player's score.
- **Avoiding Hurdles/Trash Cans**: Hitting hurdles in Level 1 or trash cans in Level 2 reduces the player's score.
- **Game Over**: The game ends when the player collides with a train.
- **Game Won**: The game is won when the player reaches the finish line.

### Levels
- **Level 1**: Urban subway environment with buildings, bridges, and grass.
- **Level 2**: Desert scene with dusty winds and sand ground.

## Controls

- **SPACE**: Start the game & Play again.
- **P**: Pause & Resume the game.
- **L**: Toggle lighting effects in the game.
- **U**: Switch between different players (rolling ball/runner).
- **Left Mouse Click**: Jump (Alternate: **SPACE**).
- **Right Mouse Click**: Toggle camera view (First-person/Third-person).
- **Left Arrow**: Move Left (Alternate: **-**).
- **Right Arrow**: Move Right (Alternate: **+**).
- **Z**: Switch to first-person view.
- **X**: Switch to third-person view.
- **ESC**: Exit the game.

## Scenes

The game features two scenes that the player will navigate through:
1. **Urban Subway**: Includes buildings, bridges, and grass.
2. **Desert**: Features dusty winds, sand ground, and desert-like obstacles.

## Collision Mechanics

- **Level 1 (Coins & Hurdles)**:
  - **Coins**: Collecting coins increments points.
  - **Hurdles**: Colliding with hurdles decreases points.
- **Level 2 (Diamonds & Trash Cans)**:
  - **Diamonds**: Collecting diamonds increases points.
  - **Trash Cans**: Colliding with trash cans decreases points.
- **Trains**: Any collision with a train results in a game over.
- **Finish Lines**: Reaching the finish line results in winning the game.

## Technical Details

- **Rendering**: The game uses OpenGL 3D rendering for the graphics, using 3D models for the scenes and objects (such as the player and obstacles).
- **3D Models**: The game makes use of **.3ds** models to create a realistic 3D world.
- **Graphics Course**: This project was developed as part of the **DMET 502 Graphics course** at the **German University in Cairo**.

## Setup and Installation

1. **Install the zip file projectClean.zip** this will be your project directory.
2. **Install OpenGLMeshLoader.cpp** this is the main source file.
3. **Install the zip file over here: https://drive.google.com/drive/folders/1c5-VdcPIngfy_hrVFqtSu975b_dEedY7?usp=drive_link** this folder contains the models, textures and sounds folders of the project.
4. **Paste .cpp and contents of drive in your project directory** and you are ready to go.
5. **Find project presentation at https://drive.google.com/file/d/1fz8zv7EaJNzdA0MjhkYAYiyslQS9F3I6/view?usp=drive_link**

## Usage

- **Start Game**: Press **SPACE** to start the game.
- **Pause/Resume**: Press **P** to pause and resume the game.
- **Switch Player**: Press **U** to switch between different player modes (rolling ball/runner).
- **Camera View**: Toggle between first-person and third-person views using **Z** and **X**.

## Future Improvements

- Add more levels with new environments and obstacles.
- Implement power-ups and bonuses that enhance gameplay.
- Add multiplayer mode for competitive play.
- Improve player animations for a more dynamic experience.

## License

This project is open-source and is released under the MIT License.
