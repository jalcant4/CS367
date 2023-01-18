/* Only Edit This File
 * ------------------
 *  Name: Jed Alcantara
 *  GNumber: 00846927
 * 
 * The purpose of this 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_structs.h"
#include "common_functions.h"
#include "kifp.h"
//Macro Definitions
#define MOD2(x) (x%2)
#define ABS(x) ((x >= 0) ? x: (x * -1))
#define MAX_EXP 6 //pow2(KIFP_EXP_BITS) - 2	
#define BIAS 3
#define FLOOR_NEG (0x100)
#define NEG(x) (x | 0x100)
#define INF (0x0E0)
#define NAN (0x0FF)
#define MIN_NAN (0x0E1)
#define ZERO (0x0)
#define MSB_FRAC_FLAG(x) ((x >= 0x80000000) ? 1 : 0)
#define ZERO_FRAC_MSB(x) ((x) & 0x7FFFFFFF)
#define ONE_FRAC_MSB(x) ((x) | 0x80000000)
#define ADJUST_SIGN_EXP_BIT(x) (x & 0x100)
#define ADJUST_KIFP_EXP_BITS(x) ((x & 0x0E0) >> 5)
#define ADJUST_KIFP_FRAC_BITS(x) (x & 0x01F)
#define TOTAL_BITS 32
#define TRUE 1
#define FALSE 0

typedef struct adjusted_mantissa_fraction_and_e_struct{
	kifp_t mantissa;
	kifp_t fraction;
	kifp_t E;
} mfe_t;

//helpers
kifp_t normalized(Number_t *number);
kifp_t denormalized(Number_t *number);
kifp_t specialNumber(Number_t *number);
kifp_t writeKifp(kifp_t is_negative, kifp_t exp, kifp_t frac);
kifp_t readKifp(Number_t *number, kifp_t kifp);
kifp_t compNumber(Number_t *num1, Number_t *num2);
kifp_t addNumber(Number_t *larger, Number_t *smaller);
kifp_t multNumber(Number_t *num1, Number_t *num2);
void numberToMFE(Number_t *number, mfe_t *mfe);
void mfeToNumber(mfe_t *mfe, Number_t *number);
kifp_t isZero(Number_t *number);
// ----------Public API Functions (write these!)-------------------

// toKifp - Converts a Number Struct (whole and fraction parts) into a KIFP Value
// number is managed by zuon, DO NOT FREE number.
// Return the KIFP Value on Success or -1 on Failure.
kifp_t toKifp(Number_t *number) {
	if (number == NULL) return -1;
	if(number->is_nan) return NAN;
	if(number->is_infinity) {
		if(number->is_negative) return NEG(INF);
		else return INF;
	}
	return normalized(number);
}

// toNumber - Converts a KIFP Value into a Number Struct (whole and fraction parts)
// number is managed by zuon, DO NOT FREE or re-Allocate number. (It is already allocated)
// Return 0 on Success or -1 on Failure.
kifp_t toNumber(Number_t *number, kifp_t value) {
  	if (number == NULL) return -1;
	return (readKifp(number, value) == 0) ? 0 : -1;
}

// mulKIFP - Multiplies two KIFP Values together using the Techniques from Class.
// - To get credit, you must work with S, M, and E components.
// - You are allowed to shift/adjust M and E to multiply whole numbers.
// Return the resulting KIFP Value on Success or -1 on Failure.
kifp_t mulKifp(kifp_t val1, kifp_t val2) {
	//val1 == value[1]
	Number_t value[3];
	toNumber(&value[1], val1);
	toNumber(&value[2], val2);
	//• 𝑁𝑎𝑁 ∗ 𝐴𝑛𝑦𝑡ℎ𝑖𝑛𝑔 = 𝑁𝑎𝑁
	if(value[1].is_nan || value[2].is_nan){
		return NAN;
	}
	if(value[1].is_infinity || value[2].is_infinity) {
		//• ∞ ∗ 0 = 𝑁𝑎𝑁
		if (isZero(&value[1]) || isZero(&value[2])) return NAN;
		//• ∞ ∗ ∞ = ∞
		//• ∞ ∗ 𝑋 = ∞
		if(value[1].is_negative ^ value[2].is_negative) return NEG(INF);
		else return INF;
	}
	//• 0 ∗ 𝑋 = 0
	if (isZero(&value[1]) || isZero(&value[2])) return ZERO;
	return multNumber(&value[1], &value[2]);
}

// addKIFP - Adds two KIFP Values together using the Addition Techniques from Class.
// - To get credit, you must work with S, M, and E components.
// - You are allowed to shift/adjust M and E as needed.
// Return the resulting KIFP Value on Success or -1 on Failure.
kifp_t addKifp(kifp_t val1, kifp_t val2) {
	//val1 == value[1]
	Number_t value[3];
	toNumber(&value[1], val1);
	toNumber(&value[2], val2);
	//• 𝑁𝑎𝑁 ± 𝐴𝑛𝑦𝑡ℎ𝑖𝑛𝑔 = 𝑁𝑎𝑁
	if (value[1].is_nan || value[2].is_nan) return NAN;
	if (value[1].is_infinity && value[2].is_infinity) {
		//• ∞ − ∞ = 𝑁𝑎𝑁
		if (value[1].is_negative ^ value[2].is_negative) return NAN;
		//• −∞ − ∞ = −∞
		else if (value[1].is_negative && value[2].is_negative) return NEG(INF);
		//• ∞ + ∞ = ∞
		else return INF;
	}
	if (value[1].is_infinity ^ value[2].is_infinity) {
		//• −∞ ± 𝑋 = −∞
		if ((value[1].is_infinity && value[1].is_negative) ||
		(value[2].is_infinity && value[2].is_negative)) return NEG(INF);
		//• ∞ ± 𝑋 = ∞
		else return INF;
	}
	//𝑋 − 𝑋 = 0
	if (value[1].whole == value[2].whole &&
		value[1].fraction == value[2].fraction &&
		value[1].is_negative ^ value[2].is_negative) return ZERO;
	//Zero Tests
	kifp_t is_zero1 = isZero(&value[1]);
	kifp_t is_zero2 = isZero(&value[2]);
	if (is_zero1 && is_zero2) {
		//• −0 − 0 = −0
		if (value[1].is_negative && value[2].is_negative) return NEG(ZERO);
		//• 0 + 0 = 0
		//• 0 − 0 = 0
		//• −0 + 0 = 0
		else return ZERO;
	}
	Number_t *larger;
	Number_t *smaller;
	//val 1 is larger than val 2
	if (compNumber(&value[1], &value[2])) {
		larger = &value[1];
		smaller = &value[2];
	}
	else {
		larger = &value[2];
		smaller = &value[1];
	}
  	return addNumber(larger, smaller);
}

// subKIFP - Subtracts two KIFP Values together using the Addition Techniques from Class.
// - To get credit, you must work with S, M, and E components.
// - You are allowed to shift/adjust M and E as needed.
// Return the resulting KIFP Value on Success or -1 on Failure.
kifp_t subKifp(kifp_t val1, kifp_t val2) {
	//add the negation
  	return addKifp(val1, negateKifp(val2));
}

// negateKIFP - Negates a KIFP Value.
// Return the resulting KIFP Value on Success or -1 on Failure.
kifp_t negateKifp(kifp_t value) {
	//if value is negative, then set positive else set negative
  	return (value >= FLOOR_NEG) ? (value & 0x0FF) : NEG(value);
}

/*
	converts a number kifp_to a kifp_t
	@param	number is a the kifp_teger representation of a double
	@return	kifp_t if valid, else -1
*/
kifp_t normalized(Number_t *number) {
    kifp_t is_negative = number->is_negative;
    kifp_t frac = number->fraction;
    kifp_t E = 0;

    kifp_t adjusted_whole = number->whole;
    if (adjusted_whole >= 1) {
	//adjusted_whole >= 1	
        while (adjusted_whole > 0x1) {
			frac = ZERO_FRAC_MSB(frac >> 1);
			//ODD(adjusted_whole) <-> adjusted_whole = 2x + 1
            if (MOD2(adjusted_whole) == 1) {
				//add one
				frac = ONE_FRAC_MSB(frac);
			}
	        adjusted_whole >>= 1;
			//b is {0, 1}: K is {bbbb}: frac = 0xKKKKKKKKK|0x80000000
			//E += 1
			E++;
        }//adjusted_whole  1
	}//adjusted_whole =< 1
	//adjusted_whole == 1
	else {
		if (frac == ZERO && is_negative) return NEG(ZERO);
		else if(frac == ZERO) return ZERO;
		//adjusted_whole < 1
		while (adjusted_whole != 0x1) {
			//frac *= 2 && E -= 1
			if (!MSB_FRAC_FLAG(frac)) {frac <<= 1;}
			else {
				frac = ZERO_FRAC_MSB(frac) << 1;
				adjusted_whole += 1;
			}
			E--;			
		}//adjusted_whole >= 1
	}//adjusted_whole == 1
	kifp_t exp = E + BIAS;
	//Underflow
	if (exp <= 0) {
		return denormalized(number);
	}
	//Overflow
	else if(exp > MAX_EXP) {
		number->is_infinity = TRUE;
		return specialNumber(number);
	}	
	//exp > 0 && exp < KIFP_EXP_BITS		
	else if (exp > 0 && exp <= MAX_EXP) {
		return writeKifp(is_negative, exp, frac);
	}
	else return -1;
}
/*returns a kifp_t with a denormalized value
		by definition a denormalized value exp bits are 0
	@param	is_negative is 1 if the value is negative
			frac represents the bits after a leading 0
			E must equal 1 - BIAS
*/
kifp_t denormalized(Number_t *number) {
	kifp_t denormalized_E = 1 - BIAS;
	kifp_t E = 0;
	kifp_t frac = number->fraction;
	if (denormalized_E >= E) {
		while(E != denormalized_E) {
			frac = ZERO_FRAC_MSB(frac >> 1);
			E++;
		}
	}
	else {
		while(E != denormalized_E) {
			frac <<= 1;
			E--;
		}
	}
	//if the value can be represented
	return writeKifp(number->is_negative, 0, frac);
}
/*
	special numbers include cases of infinity and NaN
	@param	number
	@return	INF, NEG(INF), NAN if valid special number else -1
*/
kifp_t specialNumber(Number_t *number) {
	//infinity 		0b0 111 00000
	if(!number->is_negative && number->is_infinity) {return INF;} 
	//-infinity		0b1 111 00000
	else if(number->is_negative && number->is_infinity) {return NEG(INF);} 
	//NaN 			0b0 111 11111 to 0b0 111 000001 
	else if(number->is_nan) {return NAN;}
	else -1;
}
/*
	writes a kifp value according to the parameters
	@param 	is_negative is 1 if sign is negative
			exp are the series of bits of length KIFP_BIT_EXP
			frac are the series of bit of length KIFP_BIT_FRAC
	@return value of the kifp
*/
kifp_t writeKifp(kifp_t is_negative, kifp_t exp, kifp_t frac) {
	kifp_t value = is_negative << KIFP_EXP_BITS;
	value |= exp;
	value <<= KIFP_FRAC_BITS; 
	frac >>= (32 - KIFP_FRAC_BITS);
	frac &= 0x01F;
	//append frac
	value |= frac;
	return value;
}
/*
	This method reads a kifp and sets the members of number
	@param	number is the kifp_teger representation of float
			kifp is the hex representation of float
	@return -1 if failure else 0
*/
kifp_t readKifp(Number_t *number, kifp_t kifp) {
	if(number == NULL || kifp < 0) return -1;

	//NaN
	if (kifp <= NAN && kifp >= MIN_NAN)
		number->is_nan = TRUE;
	else {
		number->is_nan = FALSE;
		//negative
		if (ADJUST_SIGN_EXP_BIT(kifp))   
  			number->is_negative = TRUE;
		else number->is_negative = FALSE;
		//infinity
		if (kifp == NEG(INF) || kifp == INF)
			number->is_infinity = TRUE;
		else number->is_infinity = FALSE;
		//set whole and fraction
		kifp_t exp = ADJUST_KIFP_EXP_BITS(kifp);
		//frac = 0b 000b bbbb
		kifp_t frac = ADJUST_KIFP_FRAC_BITS(kifp);
		//exp is denormalized
		if (exp == 0) {
			number->whole = 0;
			number->fraction = frac << (TOTAL_BITS - KIFP_FRAC_BITS);
		}
		//normalized
		else {
			//E = exp - BIAS;
			kifp_t E = exp - BIAS;
			//Rep 0b 001b bbbb
			kifp_t mantissa = frac + 0x20;
			if (E >= 0) {
				//whole == (0b 001b bbbb << E) then remove the frac bits
				mantissa <<= E;
				number->whole = mantissa >> KIFP_FRAC_BITS;
			}
			else {
				mantissa >>= ABS(E);
				number->whole = mantissa >> KIFP_FRAC_BITS;
			}
			mantissa &= 0x01F;
			number->fraction = mantissa << 27;
		}
	}
	return 0;
}
/*
*	@param	num1 must be normalized or denormalized
			num2 must be normalized or denormalized
*	@return	1 if num1 > num2
			0 if num == num2
			-1 if num1 < num2
*/
kifp_t compNumber(Number_t *num1, Number_t *num2) {
	if (num1->whole > num2->whole) return 1;
	else if (num1->whole < num2->whole) return -1;
	else {
		if (num1->fraction > num2->fraction) return 1;
		else if (num1->fraction < num2->fraction) return -1;
	}
	return 0;
}
/*
	this method manipulates S M E to produce a kifp
	@return -1 on failure else returns kifp
*/
kifp_t addNumber(Number_t *larger, Number_t *smaller) {
	//this method only accepts values where larger >= smaller
	if (compNumber(larger, smaller) < 0) return -1;
	//S = number.is_negative
	//M = SUM(number.whole) && SUM(number.fraction)
	//E = (implicitly) 0 
	Number_t number =
	{"0",
	 	0,//is_negative
		0,//is_infinity
		0,//is_nan
		0,//whole
		0x0,//fraction
		0};//precision
	number.is_negative = larger->is_negative;
	//-larger + smaller or larger - smaller
	if(larger->is_negative ^ smaller->is_negative) {
		number.whole =  larger->whole - smaller->whole;
		if(ZERO_FRAC_MSB(smaller->fraction >> 1) > ZERO_FRAC_MSB(larger->fraction >> 1)) {
			number.whole -= 1;
			number.fraction = larger->fraction + (0xFFFFFFFF - smaller->fraction) + 0x1;
		}
		else number.fraction = larger->fraction - smaller->fraction;
	}
	//larger + smaller or -larger - smaller
	else {
		number.whole =  larger->whole + smaller->whole;
		if(MSB_FRAC_FLAG(larger->fraction) && MSB_FRAC_FLAG(smaller->fraction)) {
			number.whole += 1;
			kifp_t large_frac = ZERO_FRAC_MSB(larger->fraction);
			kifp_t small_frac = ZERO_FRAC_MSB(smaller->fraction);
			number.fraction = large_frac + small_frac;
		}
		else number.fraction = larger->fraction + smaller->fraction;
	}
	return toKifp(&number);
}
/*
	this method manipulates S M E to produce a kifp
	@return -1 on failure else returns kifp
*/
kifp_t multNumber(Number_t *num1, Number_t *num2) {
	//val1 == mfe[1]
	mfe_t mfe[3];
	mfe_t *p_mfe = &mfe[0];
	numberToMFE(num1, &mfe[1]);
	numberToMFE(num2, &mfe[2]);
	//S = S1 ^ S2;
	kifp_t sign = num1->is_negative ^ num2->is_negative;
	//M = M1 * M2;
	p_mfe->mantissa = mfe[1].mantissa * mfe[2].mantissa;
	//E = E1 + E2;
	p_mfe->E= mfe[1].E + mfe[2].E;
	Number_t number = 
	{"0",
	 	sign,//negative
		0,//infinity
		0,//nan
		0,//whole
		0x00000000,//fraction
		0};
	mfeToNumber(p_mfe, &number);
	return toKifp(&number);
}
/*adjusts MFE where number->frac is 0
*	@param		me is the adjusted mantissa and E
				number is the kifp_teger representation of float
*/
void numberToMFE(Number_t *number, mfe_t *mfe) {
	mfe->E = 0;
	mfe->mantissa = number->whole;
	kifp_t frac = number->fraction;
	while (frac != 0) {
		//if the MSB is 1
		if (MSB_FRAC_FLAG(frac)) {
			mfe->mantissa = (mfe->mantissa << 1) + 1;
		}
		//update values
		frac = ZERO_FRAC_MSB(frac) << 1;
		mfe->E--;
	}
}
/*adjusts MFE, sets it as a number
*	@param		mfe is the adjusted mantissa fraction and E
				number is the kifp_teger representation of float
				adjusts mfe while mfe.e != goal_E 
*/
void mfeToNumber(mfe_t *mfe, Number_t *number) {
	//for all number_t number, E is ZERO
	if (mfe->E >= ZERO) {
		while (mfe->E != ZERO) {
			if (!MSB_FRAC_FLAG(mfe->fraction)) {mfe->fraction <<= 1;}
			else {
				mfe->fraction = ZERO_FRAC_MSB(mfe->fraction) << 1;
				mfe->mantissa += 1;
			}
			mfe->E--;
		}
	}
	else {
		while (mfe->E != ZERO) {
			mfe->fraction = ZERO_FRAC_MSB(mfe->fraction >> 1);
			//ODD(mfe->whole) <-> mfe->whole = 2x + 1
            if (MOD2(mfe->mantissa) == 1) {
				//add one
				mfe->fraction = ONE_FRAC_MSB(mfe->fraction);
			}
	        mfe->mantissa >>= 1;
			//b is {0, 1}: K is {bbbb}: frac = 0xKKKKKKKKK|0x80000000
			//E += 1
			mfe->E++;
		}
	}
	number->whole = mfe->mantissa;
	number->fraction = mfe->fraction;
}
/*evaluates if the number is ZERO
	@param		a representation of number with kifp_tegers	
	@return		1 if the number is ZERO else 0
*/
kifp_t isZero(Number_t *number) {
	return (number->fraction == ZERO && number->whole == ZERO) ? 1 : 0;
}

/*
kifp_t main() {
	printf("%x * %x = %x\n", 160, 8, mulKifp(160, 8));

	Number_t number1 = 
	{"4",
	 	1,//negative
		1,//infinity
		0,//nan
		4,//whole
		0x00000000,//fraction
		0};

	Number_t number2 = 
	{"",
	 	0,//negative
		0,//infinity
		0,//nan
		0,//whole
		0x10000000,//fraction
		0};
	
	Number_t *p_number1 = &number1;
	Number_t *p_number2 = &number2;

	kifp_t kifp1 = toKifp(p_number1);
	kifp_t kifp2 = toKifp(p_number2);

	printf("Whole: %d\tFraction: %x\n", p_number1->whole, p_number1->fraction);
	printf("Whole: %d\tFraction: %x\n", p_number2->whole, p_number2->fraction);
	printf("Kifp:%x\n", kifp1);
	printf("Kifp:%x\n", kifp2);

	kifp_t kifp3 = mulKifp(kifp1, kifp2);
	Number_t *p_number3 = malloc(sizeof(struct number_struct));
	toNumber(p_number3, kifp3);
	printf("Whole: %d\tFraction: %x\tHex: %x\n", p_number3->whole, p_number3->fraction, kifp3);
	free(p_number3);
	return 0;
}*/
