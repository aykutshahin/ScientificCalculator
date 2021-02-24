# ScientificCalculator

# INTRO
In this project, we have aimed to create a fully operational scientific calculator in C by using STM32 G031K8 MCU.
A 4x4 keypad has been connected to microcontroller GPIO ports to enter the numbers and execute basic scientific and trigonometric functions.
In addition, a 4-digit seven segment displayer used to display these numbers.
To achieve these, we have drawn a rough flowchart to make our developing path clearer so that we could choose our methods to approach some estimated problems.
For example, we decided to use external interrupts rather than using polling methods in main loop.
Also we have decided to do displaying utilities in main loop because displaying is continuous process in this project so creating another interrupt for this utility did not any make sense.
Lastly, we have been aware of button debouncing. To solve this issue, there are three common ways but software prevention method has been used instead of hardware prevention method that is RC debouncing filter.

# FEATURES
- Addition,Subtraction,Multiplication,Division
- Log,Ln,Square Root,Square Power
- Sin,Cos,Tan,Cot
- Pi Quality
- Floating Display
- Negative Display
- Invalid,Overflow,Idle Flags
- Ans Quality

# DOCS

## Project Setup
![project setup](https://github.com/aykutshahin/ScientificCalculator/blob/main/docs/project_setup.jpg)

## Block Diagram
![block diagram](https://github.com/aykutshahin/ScientificCalculator/blob/main/docs/blockdiagram.jpg)

## Flowchart
![flowchart](https://github.com/aykutshahin/ScientificCalculator/blob/main/docs/flowchart.jpg)

## Numerical Works
![numerical works](https://github.com/aykutshahin/ScientificCalculator/blob/main/docs/numerical_works.png)

