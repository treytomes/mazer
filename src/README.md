# Idea v2

## Main Menu

Each maze has a numeric seed.  You can select a maze seed from the start menu, and each maze seed will have a separate high score list.

I might implement a way for the player to spend points they earn on extra hearts / starting time / blocks / etc.

Need a way to view game instructions.

It'd be nice to have a demo screen.

## HUD

* Hearts
* Score
* Blocks
* Time

## The Goal

The goal of the game is to solve the maze.  There are several different types of mazes.  You need to get from start to finish within the time limit, without running out of health.

You start at the green flashing "S" marker ("S" for start).  You're trying to reach the red flashing "F" marker ("F" for finish).

You get an OCD award (and a pile of extra points) if you manage to collect all of the prizes on the board.

## Game Over

The game is over when you run out of hearts, run out of time, or reach the Finish marker.

## Pellets

Each pellet gives you a point.  Collect all of the pellets for an OCD award.

## Clocks

Grab a clock item to add some time to the timer in the HUD.

## Hearts

You can earn more time and more hearts by collecting items around the maze.

## Whirligigs

Whirligigs and the spinn-y things that you find wandering randomly around the maze.  If you collide with one you'll lose the heart and the screen will shake for a bit, and a new whirligig will be born somewhere on the board.  When there is less than 10 seconds left on the clock they will turn bright blue and start tracking you down mercilessly.

## Blocks

You can block whirligigs (and yourself) by placing blocks, but you only have a limited number.  A block lasts 10 seconds; once it fades away you can place another.

# Idea v1

I want to make a survival game.  Again.  I'm fascinated by these games, getting the player to manage as many survival variables as possible.

The player needs to collect food and water, which will continuously deplete.
When endurance runs out the player will need to rest.  About 8 hours of rest a day.
Don't get too hot or too cold.

# Coding Standards

* Class names are initial caps, e.g. ClassName
* Function names are camel case, e.g. myAwesomeFunction
* Variables are lower case with "_" dividing words, e.g. a_variable
* Private variables are lower case with a leading "_", e.g. _my_private_variable
* Constants are upper case, e.g. GLOBAL_VALUE
