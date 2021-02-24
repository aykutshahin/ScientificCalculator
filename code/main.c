/*
 * main.c
 *
 * Created by Aykut Sahin
 */
#include "ssdconfig.h" // For SSD
#include <stdbool.h> // For boolean type
#include "math.h" // For pow function
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Created to change value into degree
#define DEG_TO_RAD 0.0174532925f 

// Created to store int variables that will be shown on SSD
int digits[2][4];
// Created to store char variables that will be shown on SSD
char* letters;
// Created to store last calculated int result 
int* lastIntNum;
// Created to store last calculated float result
float* lastFloatNum;
//Created to store current second index of digits 
unsigned int currIndex = 0;
//Created to store current index of letters
unsigned int currLetterIndex = 0;
//Created to store current first index of digits that 
//represents number that will be taken from user
unsigned int currArrIndex = 0; 
//Created to store showing index
unsigned int showIndex = 0; 
// Created to store result after calculations
int result[4];
//Created to store result size 
unsigned int resultSize = 0;
// Created to be able to calculate between float variables 
float floatingPointValue = 0;
// Created to store decimal point index to show float 
//variables properly on SSD 
unsigned int pointIndex = 0; 

// Created to check calculation request
bool calculationFlag = false;
// Created to check operator input
bool operatorFlag = false; 
// Created to check result presentation
bool showResult = false;
// Created to show properly variables that 
// have less than four digits 
bool changeDisplay = false;
//Created to show floating variables properly 
bool floatingDisplay = false; 
//Created to check whether result is overflowed or not
bool overflowDisplay = false;
//Created to check whether user requested an 
//operation on current last value or not 
bool ansQuality = false;
//Created to check whether operation is invalid or not
bool invalidDisplay = false;
//Created to check whether result is negative or not
bool negativeDisplay = false;
//Created to avoid button debouncing
bool canButtonPress = true;
//Created to check calculation has been isCalcFinisheded
bool isCalcFinished = false;
//Created to check button signals which must be exist in 10 sec 
bool buttonpressed = false;
//Created to control pi request
bool requestedPi = false;
//Created to enable button after button press 
//to avoid button debouncing 100 represents 100 ms because 
//timer 2 interrupt occurs in every 1ms 
int buttonPressCounter = 100;
							 
//Operation Flags
typedef enum OPERATION
{
	ADD,
	SUB,
	MUL,
	DIV,
	LOG,
	LN,
	SQRT,
	POW2,
	SIN,
	COS,
	TAN,
	COT,
	PI
}OPERATION;

//Created to store current operation flag
OPERATION currOperation;

//TIMER 1 INTERRUPT
void TIM1_BRK_UP_TRG_COM_IRQHandler() {
	buttonpressed = false;
	if (!buttonpressed) {
		//Resetting all flags and variables to their default values
		//so we can return idle state properly
		//Reset digits to zero
		Init_Counter(digits);
		//Reset letters to "-"
		Init_Letters(letters);
		//Resetting variable to true because we will return idle state
		buttonpressed = true;
		//Resetting first index of digit to zero
		currArrIndex = 0;
		//Resetting index of letters to zero
		currLetterIndex = 0;
		//Resetting second index of digit to zero
		currIndex = 0;
		//Resetting showing index to zero
		showIndex = 0;
		//Resetting calculationFlag to false
		calculationFlag = false;
		//Resetting operatorFlag to false
		operatorFlag = false;
		//Resetting showResult to false
		showResult = false;
		//Resetting isCalcFinished to false
		isCalcFinished = false;
		//Resetting changeDisplay to false
		changeDisplay = false;
		//Resetting floatingDisplay to false
		floatingDisplay = false;
		//Resetting overflowDisplay to false
		overflowDisplay = false;
		//Resetting requestedPi to false
		requestedPi = false;
		//Resetting ansQuality to false
		ansQuality = false;
		//Resetting invalidDisplay to false
		invalidDisplay = false;
		//Resetting negativeDisplay to false
		negativeDisplay = false;
	}
	//Resetting pending register to continue process
	TIM1->SR &= ~(1U << 0);
}
//Initializing timer 1
void Init_TIMER1() {
	RCC->APBENR2 |= (1U << 11);

	TIM1->CR1 = 0;
	TIM1->CR1 |= (1 << 7);
	TIM1->CNT = 0;

	TIM1->PSC = 9999;
	TIM1->ARR = 16000;

	TIM1->DIER = (1 << 0);
	TIM1->CR1 = (1 << 0);

	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1);
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
}
//TIMER 2 INTERRUPT
void TIM2_IRQHandler()
{
	//To check any button pressed
	if (!canButtonPress)
	{
		//Decreasing button counter time
		buttonPressCounter--;
		//Checking whether buttonPressCounter has reached zero or lower than zero
		if (buttonPressCounter <= 0)
		{
			//Resetting debouncing preventer elements so that any button press can be read
			buttonPressCounter = 100;
			canButtonPress = true;
		}
	}
	TIM2->SR &= ~(1U << 0);
}
//Initializing Timer 2 Interrupt
void Init_TIMER2()
{
	RCC->APBENR1 |= (1U);
	TIM2->CR1 = 0;
	TIM2->CR1 |= (1 << 7);
	TIM2->CNT = 0;
	TIM2->PSC = 1;
	TIM2->ARR = 16000;
	TIM2->DIER = (1 << 0);
	TIM2->CR1 = (1 << 0);
	NVIC_SetPriority(TIM2_IRQn, 0);
	NVIC_EnableIRQ(TIM2_IRQn);
}

//Created to split floating number to separated digits
void SplitFloatingPointNumber(float _num, char* num)
{
	//Separating _num into digits to store num array
	sprintf(num, "%.3f", _num);
}

//Created to split int number into digits
void sepNumber(int* _digits, int n1) {
	int k;
	for (k = 3; k >= 0; k--) {
		_digits[k] = abs(n1) % 10;

		n1 = abs(n1) / 10;
	}
}
//Created to get size number arrays
unsigned int getSize(int* _digits) {
	unsigned int _size = 4;
	if (_digits[0] == 0) { _size--; }//size 3
	if (_digits[0] == 0 && _digits[1] == 0) { _size--; }//size 2
	if (_digits[0] == 0 && _digits[1] == 0 && _digits[2] == 0) { _size--; }//size 1
	return _size;
}
//Created to change digits array to a single number
int* getNumber(int(*_digits)[4]) {
	int k;
	static int number[2];
	for (k = 3; k >= 0; k--) {
		number[0] += _digits[0][k] * (int)pow(10, 3 - k);
		number[1] += _digits[1][k] * (int)pow(10, 3 - k);
	}
	return number;
}

//Created to calculate result
void CalculateResult(void)
{
	int *_n1 = NULL;
	_n1 = getNumber(digits);//Getting numbers from separated digits
	//Operation Control Pool
	switch (currOperation)
	{
	case ADD:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			_n1[1] = *lastIntNum;
		}
		//Addition operation
		_n1[0] = _n1[0] + _n1[1];
		//Checking whether our value is negative or not
		if (_n1[0] < 0)
		{
			negativeDisplay = true;
		}
		//Storing last calculated results
		*lastIntNum = _n1[0];
		*lastFloatNum = (float)_n1[0];
		break;
	case SUB:
		//Checking whether user requested ans or not
		if (ansQuality)
		{

			_n1[1] = _n1[0];
			_n1[0] = *lastIntNum;
		}
		//Subtraction operation
		_n1[0] = _n1[0] - _n1[1];
		//Checking whether our value is negative or not
		if (_n1[0] < 0)
		{
			negativeDisplay = true;
		}
		//Storing last calculated results
		*lastIntNum = _n1[0];
		*lastFloatNum = (float)_n1[0];
		break;
	case MUL:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			_n1[1] = *lastIntNum;
		}
		//Multiplication operation
		_n1[0] = _n1[0] * _n1[1];
		//Checking whether our value is negative or not
		if (_n1[0] < 0)
		{
			negativeDisplay = true;
		}
		//Storing last calculated results
		*lastIntNum = _n1[0];
		*lastFloatNum = (float)_n1[0];
		break;
	case DIV:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Checking whether operation is invalid or not
			if (_n1[0] == 0)
			{
				invalidDisplay = true;
				floatingDisplay = false;
			}
			else
			{
				//Dividing operation
				floatingPointValue = (float)(*lastFloatNum / (float)_n1[0]);
				//Checking whether our value is negative or not
				if (floatingPointValue < 0)
				{
					negativeDisplay = true;
				}
				//Storing last calculated results
				*lastFloatNum = floatingPointValue;
				*lastIntNum = (int)floatingPointValue;
			}
		}
		else
		{
			//Checking whether operation is invalid or not
			if (_n1[1] == 0)
			{
				invalidDisplay = true;
				floatingDisplay = false;
			}
			else
			{
				//Dividing operation
				floatingPointValue = (float)((float)_n1[0] / (float)_n1[1]);
				//Checking whether our value is negative or not
				if (floatingPointValue < 0)
				{
					negativeDisplay = true;
				}
				//Checking whether pi operation is requested or not
				if (requestedPi)
				{
					floatingPointValue *= 0.01f;
					floatingDisplay = true;
				}
				//Storing last calculated results
				*lastFloatNum = floatingPointValue;
				*lastIntNum = (int)floatingPointValue;
			}
		}
		break;
	case LOG:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Checking whether operation is invalid or not
			if (*lastFloatNum < 0)
			{
				invalidDisplay = true;
				floatingDisplay = false;
			}
			else
			{
				//Log10 operation
				floatingPointValue = (float)log10((double)*lastFloatNum);
				//Checking whether our value is negative or not
				if (floatingPointValue < 0)
				{
					negativeDisplay = true;
				}
				//Storing last calculated results
				*lastFloatNum = floatingPointValue;
				*lastIntNum = (int)floatingPointValue;
			}
		}
		else
		{
			//Checking whether operation is invalid or not
			if (_n1[0] < 0)
			{
				invalidDisplay = true;
				floatingDisplay = false;
			}
			else
			{
				//Log10 operation
				floatingPointValue = (float)log10((double)_n1[0]);
				//Checking whether our value is negative or not
				if (floatingPointValue < 0)
				{
					negativeDisplay = true;
				}
				//Storing last calculated results
				*lastFloatNum = floatingPointValue;
				*lastIntNum = (int)floatingPointValue;
			}
		}
		break;
	case LN:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Checking whether operation is invalid or not
			if (*lastFloatNum < 0)
			{
				invalidDisplay = true;
				floatingDisplay = false;
			}
			else
			{
				//LN operation
				floatingPointValue = (float)log((double)*lastFloatNum);
				//Checking whether our value is negative or not
				if (floatingPointValue < 0)
				{
					negativeDisplay = true;
				}
				//Storing last calculated results
				*lastFloatNum = floatingPointValue;
				*lastIntNum = (int)floatingPointValue;
			}
		}
		else
		{
			//Checking whether operation is invalid or not
			if (_n1[0] < 0)
			{
				invalidDisplay = true;
				floatingDisplay = false;
			}
			else
			{
				//LN operation
				floatingPointValue = (float)log((double)_n1[0]);
				//Checking whether our value is negative or not
				if (floatingPointValue < 0)
				{
					negativeDisplay = true;
				}
				//Storing last calculated results
				*lastFloatNum = floatingPointValue;
				*lastIntNum = (int)floatingPointValue;
			}
		}
		break;
	case SQRT:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Checking whether operation is invalid or not
			if (*lastFloatNum < 0)
			{
				invalidDisplay = true;
				floatingDisplay = false;
			}
			else
			{
				//Square root operation
				floatingPointValue = (float)sqrt((double)*lastFloatNum);
				//Storing last calculated results
				*lastFloatNum = floatingPointValue;
				*lastIntNum = (int)floatingPointValue;
			}
		}
		else
		{
			//Checking whether operation is invalid or not
			if (_n1[0] < 0)
			{
				invalidDisplay = true;
				floatingDisplay = false;
			}
			else
			{
				//Square root operation
				floatingPointValue = (float)sqrt((double)_n1[0]);
				//Storing last calculated results
				*lastFloatNum = floatingPointValue;
				*lastIntNum = (int)floatingPointValue;
			}
		}
		break;
	case POW2:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Power of 2 operation
			floatingPointValue = (float)pow((double)*lastFloatNum, 2);
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		else
		{
			//Power of 2 operation
			floatingPointValue = (float)pow((double)_n1[0], 2);
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		break;
	case SIN:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Sin operation
			floatingPointValue = (float)sin((double)*lastFloatNum * (double)DEG_TO_RAD);
			//Checking whether our value is negative or not
			if (floatingPointValue < 0)
			{
				negativeDisplay = true;
			}
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		else
		{
			//Sin operation
			floatingPointValue = (float)sin((double)_n1[0] * (double)DEG_TO_RAD);
			//Checking whether our value is negative or not
			if (floatingPointValue < 0)
			{
				negativeDisplay = true;
			}
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		break;
	case COS:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Cos operation
			floatingPointValue = (float)cos((double)*lastFloatNum * (double)DEG_TO_RAD);
			//Checking whether our value is negative or not
			if (floatingPointValue < 0)
			{
				negativeDisplay = true;
			}
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		else
		{
			//Cos operation
			floatingPointValue = (float)cos((double)_n1[0] * (double)DEG_TO_RAD);
			//Checking whether our value is negative or not
			if (floatingPointValue < 0)
			{
				negativeDisplay = true;
			}
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		break;
	case TAN:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Tan operation
			floatingPointValue = (float)tan((double)*lastFloatNum * (double)DEG_TO_RAD);
			//Checking whether our value is negative or not
			if (floatingPointValue < 0)
			{
				negativeDisplay = true;
			}
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		else
		{
			//Tan operation
			floatingPointValue = (float)tan((double)_n1[0] * (double)DEG_TO_RAD);
			//Checking whether our value is negative or not
			if (floatingPointValue < 0)
			{
				negativeDisplay = true;
			}
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		break;
	case COT:
		//Checking whether user requested ans or not
		if (ansQuality)
		{
			//Cot operation
			floatingPointValue = 1.0f / (float)tan((double)*lastFloatNum * (double)DEG_TO_RAD);
			//Checking whether our value is negative or not
			if (floatingPointValue < 0)
			{
				negativeDisplay = true;
			}
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}
		else
		{
			//Cot operation
			floatingPointValue = 1.0f / (float)tan((double)_n1[0] * (double)DEG_TO_RAD);
			//Checking whether our value is negative or not
			if (floatingPointValue < 0)
			{
				negativeDisplay = true;
			}
			//Storing last calculated results
			*lastFloatNum = floatingPointValue;
			*lastIntNum = (int)floatingPointValue;
		}

		break;
	case PI:
		//Assigning pi to value that will be shown on SSD
		floatingPointValue = 3.141f;
		break;
	default:
		break;
	}
	//Checking whether our result is float or not
	if (!floatingDisplay && !invalidDisplay) {
		//Checking our result is greater than -100 as well as smaller than 9999
		if (_n1[0]<10000 && (int)_n1[0] > -1000) {
			sepNumber(result, _n1[0]);//Changing single number into separated digits
			resultSize = getSize(result);//Getting result size
		}
		//If our value does not lay in our boundaries that we mentioned above, overflow flag would be true
		else {
			overflowDisplay = true;
		}
	}
	//Checking whether result is invalid or not
	else if (!invalidDisplay) {
		char *number = (char*)malloc(sizeof(char) * 6);
		SplitFloatingPointNumber(floatingPointValue, number);//Changing single float number into separated digits
		int i;
		//Finding decimal point index of floating number
		for (i = 0; i < 5; i++) {
			if (*(number + i) == '.')
			{
				pointIndex = (unsigned int)(i - 1);
				//If pointIndex is equal to 4 then it means that our float value is an integer
				//so showing decimal point does not any make sense
				if (i == 4) {
					//Doing process of separating an integer
					_n1[0] = (int)floatingPointValue;
					sepNumber(result, _n1[0]);
					resultSize = getSize(result);
					floatingDisplay = false;
					break;
				}
			}
		}
		//Checking whether pi operation request exists, if so set pointIndex to zero
		if (requestedPi)
		{
			pointIndex = 0;
		}
		//Checking whether pointIndex is not equal to 3
		if (pointIndex != 3) {
			int index = 3;
			//Converting char variables to int digits by substracting '0' ASCII character from our characters
			for (i = 4; i >= 0; i--) {
				if (*(number + i) != '.') {
					result[index] = (int)(*(number + i) - '0');
					index--;
				}
			}
			//If our variables is negative then we should show only 3 digits to show negative sign
			resultSize = negativeDisplay == false ? 4 : 3;
		}

	}
	//If result is invalid then properties of invalid flag must be set
	else
	{
		letters[0] = 'i';
		letters[1] = 'n';
		letters[2] = 'v';
		letters[3] = 'd';
		currLetterIndex = 4;
	}
	//Resetting variables because result has been obtained
	showIndex = 0;
	_n1[0] = 0;
	_n1[1] = 0;
	//Checking whether overflow exists or not, if it exists set properties of overflow
	if (overflowDisplay) {
		letters[0] = 'o';
		letters[1] = 'u';
		letters[2] = 'f';
		letters[3] = 'l';
		currLetterIndex = 4;
	}
	//Checking whether result is invalid or not, if it is not invalid assign showResult to display result value
	else if (!invalidDisplay) {
		showResult = true;
	}
	//Resetting flags and variables because result has been obtained
	calculationFlag = false;
	changeDisplay = false;
	ansQuality = false;
	currIndex = 0;
	currArrIndex = 0;
	Reset_Counter(digits);
}

//Simple delay function created to display numbers more smoothly on SSD
void delay(unsigned int s)
{
	for (; s > 0; s--);
}

//CLEAR KEYPAD ROWS
void rowClear(void) {
	GPIOB->BRR = (1U << 4);
	GPIOB->BRR = (1U << 5);
	GPIOB->BRR = (1U << 8);
	GPIOB->BRR = (1U << 9);
}
//SET KEYPAD ROWS
void rowSet(void) {
	GPIOB->ODR |= (1U << 4);
	GPIOB->ODR |= (1U << 5);
	GPIOB->ODR |= (1U << 8);
	GPIOB->ODR |= (1U << 9);
}

//External interrupt for column 1 and 2
void EXTI0_1_IRQHandler(void) {
	//Disabling timer 1 to avoid interrupts during calculation
	buttonpressed = true;
	TIM1->CNT = 0;

	//If overflow exists before new input then assign it to default value
	if (overflowDisplay && !isCalcFinished && canButtonPress) {
		overflowDisplay = false;
		currLetterIndex = 0;
		Init_Letters(letters);
	}
	//If result is invalid before new input then assign it to default value
	if (invalidDisplay && !isCalcFinished && canButtonPress)
	{
		invalidDisplay = false;
		currLetterIndex = 0;
		Init_Letters(letters);
	}
	//If result is negative before new input then assign it to default value
	if (negativeDisplay && !isCalcFinished && canButtonPress)
	{
		negativeDisplay = false;
	}
	//If calculation made before new input then assign it to default value
	if (!calculationFlag && !isCalcFinished && canButtonPress)
	{
		calculationFlag = true;
		Reset_Counter(digits);
	}
	//If result has been found before new input then assign it to default value
	if (showResult && !isCalcFinished && canButtonPress) {
		showResult = false;
		requestedPi = false;
		floatingDisplay = floatingDisplay == true ? false : floatingDisplay;
	}
	ResetDisplay();
	TurnBeginning();
	//COLUMN 1 INPUT
	if (((GPIOA->IDR >> 0) & 1))
	{
		//If debouncing does not exist go inside
		if (canButtonPress)
		{
			rowClear();
			//Checking column 1 inputs respectively (1,4,7,F(*))
			GPIOB->ODR ^= (1U << 4);
			if (((GPIOA->IDR >> 0) & 1) && currIndex < 4 && currArrIndex < 2) {
				//1 Button
				digits[currArrIndex][currIndex] = 1;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 4);

			GPIOB->ODR ^= (1U << 5);
			if ((((GPIOA->IDR >> 0) & 1)) && currIndex < 4 && currArrIndex < 2) {
				//4 Button
				digits[currArrIndex][currIndex] = 4;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 5);

			GPIOB->ODR ^= (1U << 8);
			if ((((GPIOA->IDR >> 0) & 1)) && currIndex < 4 && currArrIndex < 2) {
				//7 Button
				digits[currArrIndex][currIndex] = 7;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 8);
			GPIOB->ODR ^= (1U << 9);
			if ((((GPIOA->IDR >> 0) & 1) && !isCalcFinished)) {
				//F Button(*)
				canButtonPress = false;
				//If button press that has been made before (*) key is not equal to any operation key
				if (!operatorFlag)
				{
					//If our current number has less than four digits go inside and shift digits right in array
					//non-in place algorithm used it can be improved
					if (currIndex < 4)
					{
						int i, j;
						changeDisplay = true;//Representing our value has less than four digits
											//This flag was created to avoid displaying zeros of array on SSD
						int temp;
						for (i = (int)currIndex - 1; i >= 0; --i)
						{
							for (j = 1; j < (int)(4 - ((int)currIndex - 1)); j++)
							{
								temp = digits[currArrIndex][i + j - 1];
								digits[currArrIndex][i + j - 1] = digits[currArrIndex][i + j];
								digits[currArrIndex][i + j] = temp;
							}
						}
					}
					//Increasing number index to be ready to get new input
					if ((floatingDisplay && currArrIndex == 0) || !floatingDisplay || (floatingDisplay && currOperation == DIV)) {
						++currArrIndex;
					}
				}
				//Checking whether calculation requested or not
				if ((currArrIndex >= 2 && calculationFlag) || (floatingDisplay && currArrIndex == 1 && currOperation != PI))
				{
					isCalcFinished = true;
					CalculateResult();
				}
				//Checking whether an operation requested or not
				if (operatorFlag && !isCalcFinished)
				{
					//If we displayed pi before calculation we should turn off floatingDisplay
					if (requestedPi)
					{
						floatingDisplay = false;
					}
					if (letters[0] == 'a' && letters[1] == '-' && letters[2] == '-' && letters[3] == '-')
					{
						//ADD
						currOperation = ADD;
						ansQuality = currArrIndex == 0 ? true : ansQuality;
					}
					else if (letters[0] == 'b' && letters[1] == '-' && letters[2] == '-' && letters[3] == '-')
					{
						//SUB
						currOperation = SUB;
						ansQuality = currArrIndex == 0 ? true : ansQuality;
					}
					else if (letters[0] == 'c' && letters[1] == '-' && letters[2] == '-' && letters[3] == '-')
					{
						//MUL
						currOperation = MUL;
						ansQuality = currArrIndex == 0 ? true : ansQuality;
					}
					else if (letters[0] == 'd' && letters[1] == '-' && letters[2] == '-' && letters[3] == '-')
					{
						//DIV
						currOperation = DIV;
						if (!requestedPi)
						{
							floatingDisplay = true;
						}
						ansQuality = currArrIndex == 0 ? true : ansQuality;
					}
					else if (letters[0] == 'e' && letters[1] == 'a' && letters[2] == '-' && letters[3] == '-')
					{
						//LOG10
						currOperation = LOG;
						floatingDisplay = true;
					}
					else if (letters[0] == 'e' && letters[1] == 'b' && letters[2] == '-' && letters[3] == '-')
					{
						//LN
						currOperation = LN;
						floatingDisplay = true;
					}
					else if (letters[0] == 'e' && letters[1] == 'c' && letters[2] == '-' && letters[3] == '-')
					{
						//SQUARE ROOT
						currOperation = SQRT;
						floatingDisplay = true;
					}
					else if (letters[0] == 'e' && letters[1] == 'd' && letters[2] == '-' && letters[3] == '-')
					{
						//POWER OF 2
						currOperation = POW2;
						floatingDisplay = true;
					}
					else if (letters[0] == 'e' && letters[1] == 'e' && letters[2] == 'a' && letters[3] == '-')
					{
						//SIN
						currOperation = SIN;
						floatingDisplay = true;
					}
					else if (letters[0] == 'e' && letters[1] == 'e' && letters[2] == 'b' && letters[3] == '-')
					{
						//COS
						currOperation = COS;
						floatingDisplay = true;
					}
					else if (letters[0] == 'e' && letters[1] == 'e' && letters[2] == 'c' && letters[3] == '-')
					{
						//TAN
						currOperation = TAN;
						floatingDisplay = true;
					}
					else if (letters[0] == 'e' && letters[1] == 'e' && letters[2] == 'd' && letters[3] == '-')
					{
						//COT
						currOperation = COT;
						floatingDisplay = true;
					}
					else if (letters[0] == 'e' && letters[1] == 'e' && letters[2] == 'e' && letters[3] == '-')
					{
						//PI
						currOperation = PI;
						digits[currArrIndex][0] = 3;
						digits[currArrIndex][1] = 1;
						digits[currArrIndex][2] = 4;
						digits[currArrIndex][3] = 1;
						currArrIndex += 1;
						pointIndex = 0;
						requestedPi = true;
						floatingDisplay = true;
					}
					//If current operation is pi then we should set current index to zero
					//to be ready to get new input from user
					currIndex = currOperation == PI ? 4 : 0;
					//If user has already got a number we should increase showing index to display second input number
					if ((!floatingDisplay && !ansQuality) || (currOperation == DIV && floatingDisplay && !ansQuality))
					{
						showIndex++;
					}
					//Setting input properties to default values because operation request has been acquired
					currLetterIndex = 0;
					changeDisplay = false;
					operatorFlag = false;
					Init_Letters(letters);
				}
				TurnBeginning();
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 9);
			rowSet();
			//If ans request exists then we should calculate immediately
			if ((ansQuality && currArrIndex == 1) || (ansQuality && floatingDisplay && currOperation != DIV))
			{
				CalculateResult();
			}
		}
		//If result found, we should assign isCalcFinished flag to false
		if (isCalcFinished)
		{
			isCalcFinished = false;
		}
		EXTI->RPR1 |= (1U); //It provides to continue the program.
	}
	//COLUMN 2 INPUT
	else
	{
		//If debouncing does not exist go inside
		if (canButtonPress)
		{
			//Checking column 2 inputs respectively (2,5,8,0)
			rowClear();
			GPIOB->ODR ^= (1U << 4);
			if (((GPIOA->IDR >> 1) & 1) && currIndex < 4 && currArrIndex < 2) {
				//2 Button
				digits[currArrIndex][currIndex] = 2;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 4);

			GPIOB->ODR ^= (1U << 5);
			if ((((GPIOA->IDR >> 1) & 1)) && currIndex < 4 && currArrIndex < 2) {
				//5 Button
				digits[currArrIndex][currIndex] = 5;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 5);

			GPIOB->ODR ^= (1U << 8);
			if ((((GPIOA->IDR >> 1) & 1)) && currIndex < 4 && currArrIndex < 2) {
				//8 Button
				digits[currArrIndex][currIndex] = 8;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 8);

			GPIOB->ODR ^= (1U << 9);
			if ((((GPIOA->IDR >> 1) & 1)) && currIndex < 4 && currArrIndex < 2) {
				//0 Button
				digits[currArrIndex][currIndex] = 0;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 9);
			rowSet();
		}
		EXTI->RPR1 |= (1U << 1); //It provides to continue the program.
	}

}
//External interrupt for column 3 and 4
void EXTI4_15_IRQHandler(void) {
	//Disabling timer 1 to avoid interrupts during calculation
	buttonpressed = true;
	TIM1->CNT = 0;
	//If overflow exists before new input then assign it to default value
	if (overflowDisplay && !isCalcFinished && canButtonPress) {
		overflowDisplay = false;
		currLetterIndex = 0;
		Init_Letters(letters);
	}
	//If result is invalid before new input then assign it to default value
	if (invalidDisplay && !isCalcFinished && canButtonPress)
	{
		invalidDisplay = false;
		currLetterIndex = 0;
		Init_Letters(letters);
	}
	//If result is negative before new input then assign it to default value
	if (negativeDisplay && !isCalcFinished && canButtonPress)
	{
		negativeDisplay = false;
	}
	//If calculation made before new input then assign it to default value
	if (!calculationFlag && !isCalcFinished && canButtonPress)
	{
		calculationFlag = true;
		Reset_Counter(digits);
	}
	//If result has been found before new input then assign it to default value
	if (showResult && !isCalcFinished && canButtonPress) {
		showResult = false;
		requestedPi = false;
		floatingDisplay = floatingDisplay == true ? false : floatingDisplay;
	}
	ResetDisplay();
	TurnBeginning();
	//COLUMN 3 INPUT
	if ((GPIOA->IDR >> 11) & 1)
	{
		//If debouncing does not exist go inside
		if (canButtonPress)
		{
			//Checking column 3 inputs respectively (3,6,9,E(#))
			rowClear();
			GPIOB->ODR ^= (1U << 4);
			if (((GPIOA->IDR >> 11) & 1) && currIndex < 4 && currArrIndex < 2) {
				//3 Button
				digits[currArrIndex][currIndex] = 3;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 4);

			GPIOB->ODR ^= (1U << 5);
			if ((((GPIOA->IDR >> 11) & 1)) && currIndex < 4 && currArrIndex < 2) {
				//6 Button
				digits[currArrIndex][currIndex] = 6;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 5);

			GPIOB->ODR ^= (1U << 8);
			if ((((GPIOA->IDR >> 11) & 1)) && currIndex < 4 && currArrIndex < 2) {
				//9 Button
				digits[currArrIndex][currIndex] = 9;
				currIndex = currIndex < 4 ? currIndex + 1 : currIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 8);

			GPIOB->ODR ^= (1U << 9);
			if ((((GPIOA->IDR >> 11) & 1)) && currLetterIndex < 4) {
				//E Button(#)
				DisplayLetter('e');
				letters[currLetterIndex] = 'e';
				operatorFlag = true;
				currLetterIndex = currLetterIndex < 4 ? currLetterIndex + 1 : currLetterIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 9);
			rowSet();
		}
		EXTI->RPR1 |= (1U << 11); //It provides to continue the program.
	}
	//COLUMN 4 INPUT
	else
	{
		//If debouncing does not exist go inside
		if (canButtonPress)
		{
			//Checking column 3 inputs respectively (A,B,C,D)
			rowClear();
			GPIOB->ODR ^= (1U << 4);
			if (((GPIOA->IDR >> 12) & 1) && currLetterIndex < 4) {
				//A Button
				DisplayLetter('a');
				letters[currLetterIndex] = 'a';
				operatorFlag = true;
				currLetterIndex = currLetterIndex < 4 ? currLetterIndex + 1 : currLetterIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 4);

			GPIOB->ODR ^= (1U << 5);
			if ((((GPIOA->IDR >> 12) & 1)) && currLetterIndex < 4) {
				//B Button
				DisplayLetter('b');
				letters[currLetterIndex] = 'b';
				operatorFlag = true;
				currLetterIndex = currLetterIndex < 4 ? currLetterIndex + 1 : currLetterIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 5);

			GPIOB->ODR ^= (1U << 8);
			if ((((GPIOA->IDR >> 12) & 1)) && currLetterIndex < 4) {
				//C Button
				DisplayLetter('c');
				letters[currLetterIndex] = 'c';
				operatorFlag = true;
				currLetterIndex = currLetterIndex < 4 ? currLetterIndex + 1 : currLetterIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 8);

			GPIOB->ODR ^= (1U << 9);
			if ((((GPIOA->IDR >> 12) & 1)) && currLetterIndex < 4) {
				//D Button
				DisplayLetter('d');
				letters[currLetterIndex] = 'd';
				operatorFlag = true;
				currLetterIndex = currLetterIndex < 4 ? currLetterIndex + 1 : currLetterIndex;
				canButtonPress = false;
			}
			GPIOB->ODR ^= (1U << 9);
			rowSet();
		}
		EXTI->RPR1 |= (1U << 12); //It provides to continue the program.
	}
}

//Enable proper GPIOA-B PORTS
void GPIOInit(void)
{
	//Enable GPIO_A-B block
	RCC->IOPENR |= (3U);
	//Set B4 B5 B8 B9 as output rows
	GPIOB->MODER &= ~(3U << 2 * 4);
	GPIOB->MODER |= (1U << 2 * 4);

	GPIOB->MODER &= ~(3U << 2 * 5);
	GPIOB->MODER |= (1U << 2 * 5);

	GPIOB->MODER &= ~(3U << 2 * 8);
	GPIOB->MODER |= (1U << 2 * 8);

	GPIOB->MODER &= ~(3U << 2 * 9);
	GPIOB->MODER |= (1U << 2 * 9);

	//Set A0 A1 A11 A12 as input column
	GPIOA->MODER &= ~(3U << 2 * 0);
	GPIOA->PUPDR |= (2U << 2 * 0);

	GPIOA->MODER &= ~(3U << 2 * 1);
	GPIOA->PUPDR |= (2U << 2 * 1);

	GPIOA->MODER &= ~(3U << 2 * 11);
	GPIOA->PUPDR |= (2U << 2 * 11);

	GPIOA->MODER &= ~(3U << 2 * 12);
	GPIOA->PUPDR |= (2U << 2 * 12);

	//SSD B0 PORT
	GPIOB->MODER &= ~(3U << 2 * 0);
	GPIOB->MODER |= (1U << 2 * 0);
	GPIOB->ODR |= (1U << 1 * 0);

	//KEYPAD POSITIVE VOLTAGE PORTS
	GPIOB->ODR |= (1U << 4);
	GPIOB->ODR |= (1U << 5);
	GPIOB->ODR |= (1U << 8);
	GPIOB->ODR |= (1U << 9);

}
int main(void) {

	//Initialize ports
	GPIOInit();

	//Set external interrupt properties of column 1 and 2
	EXTI->EXTICR[0] |= (0U << 8 * 0);//A0
	EXTI->RTSR1 |= (1U << 0);
	EXTI->IMR1 |= (1U << 0);

	EXTI->EXTICR[0] |= (0U << 8 * 1);//A1
	EXTI->RTSR1 |= (1U << 1);
	EXTI->IMR1 |= (1U << 1);

	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	NVIC_EnableIRQ(EXTI0_1_IRQn);

	//Set external interrupt properties of column 3 and 4
	EXTI->EXTICR[2] |= (0U << 8 * 0);//A11
	EXTI->RTSR1 |= (1U << 11);
	EXTI->IMR1 |= (1U << 11);

	EXTI->EXTICR[3] |= (0U << 8 * 3);//A12
	EXTI->RTSR1 |= (1U << 12);
	EXTI->IMR1 |= (1U << 12);

	NVIC_SetPriority(EXTI4_15_IRQn, 0);
	NVIC_EnableIRQ(EXTI4_15_IRQn);

	//Initialize timer 2
	Init_TIMER2();

	//Memory allocation for global pointers
	letters = (char*)malloc(sizeof(char) * 4);
	lastIntNum = (int*)malloc(sizeof(int));
	lastFloatNum = (float*)malloc(sizeof(float));

	//Initializing digits, letters and timer 1
	Init_Counter(digits);
	Init_Letters(letters);
	Init_TIMER1();

	int iterator = 0;
	int offset = 0;
	while (1) {
		//Idle State Displayer
		if (!calculationFlag && !showResult && !overflowDisplay && !invalidDisplay)
		{
			offset = 0;
			for (iterator = 3; iterator >= 0; --iterator)
			{
				DisplayNumber(digits[0][iterator]);
				delay(100);//Delayed for smoothness
				ResetDisplay();
				ShiftDigit((unsigned int)offset);
				offset += 1;
			}
		}
		//Displaying input that has been taken while waiting operation input
		else if (!operatorFlag && calculationFlag && !showResult)
		{
			offset = 0;
			//If number has digits less than four then go inside
			if (!changeDisplay) {
				for (iterator = (int)(currIndex - 1); iterator >= 0; --iterator)
				{
					DisplayNumber(digits[showIndex][iterator]);
					if (floatingDisplay && pointIndex == (unsigned int)iterator && requestedPi)
					{
						GPIOA->MODER &= ~(3U << 2 * 15);
						GPIOA->MODER |= (1U << 2 * 15);
					}
					delay(100);
					ResetDisplay();
					if (floatingDisplay && pointIndex == (unsigned int)iterator && requestedPi)
					{
						GPIOA->MODER &= ~(3U << 2 * 15);
						GPIOA->MODER |= (3U << 2 * 15);
					}
					ShiftDigit((unsigned int)offset);
					offset += 1;
				}
				if (currIndex < 4)
				{
					TurnBeginning();
				}
			}
			//If number has four digits then go inside
			else {

				for (iterator = (int)(3); iterator >= (int)(4 - (int)getSize((int*)&digits[showIndex])); --iterator)
				{
					DisplayNumber(digits[showIndex][iterator]);
					delay(100);
					ResetDisplay();
					ShiftDigit((unsigned int)offset);
					offset += 1;
				}
				if (currIndex < 4)
				{
					TurnBeginning();
				}

			}
		}
		//If result is overflowed or invalid, or if an operation input has been taken go inside
		else if ((operatorFlag && !showResult) || overflowDisplay || invalidDisplay)
		{
			offset = 0;
			for (iterator = (int)(currLetterIndex - 1); iterator >= 0; --iterator)
			{
				DisplayLetter(letters[iterator]);
				delay(100);
				ResetDisplay();
				ShiftDigit((unsigned int)offset);
				offset += 1;
			}
			if (currLetterIndex < 4)
			{
				TurnBeginning();
			}
		}
		//If calculation made and we have an result value then go inside
		else if (showResult)
		{
			offset = 0;
			for (iterator = 3; iterator >= (int)4 - ((int)resultSize); --iterator)
			{
				DisplayNumber(result[iterator]);
				if (floatingDisplay && pointIndex == (unsigned int)iterator)
				{
					GPIOA->MODER &= ~(3U << 2 * 15);
					GPIOA->MODER |= (1U << 2 * 15);
				}
				delay(100);
				ResetDisplay();
				if (floatingDisplay && pointIndex == (unsigned int)iterator)
				{
					GPIOA->MODER &= ~(3U << 2 * 15);
					GPIOA->MODER |= (3U << 2 * 15);
				}
				ShiftDigit((unsigned int)offset);
				offset += 1;
				if (negativeDisplay && iterator == (int)4 - ((int)resultSize))
				{
					SetNegativeSign();
					delay(100);
					ResetDisplay();
					ShiftDigit((unsigned int)offset);
				}
			}
			if (resultSize < 4) { TurnBeginning(); }
		}

	}
	return 0;
}
