/* $Id$ */
# ifndef CPPAD_COMPARE_INCLUDED
# define CPPAD_COMPARE_INCLUDED

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

/*
-------------------------------------------------------------------------------
$begin Compare$$
$spell
	cos
	Op
	bool
	const
$$

$index binary, AD compare operator$$
$index AD, binary compare operator$$
$index compare, AD binary operator$$
$index operator, AD binary compare$$

$index <, AD operator$$
$index <=, AD operator$$
$index >, AD operator$$
$index >=, AD operator$$
$index ==, AD operator$$
$index !=, AD operator$$

$section AD Binary Comparison Operators$$


$head Syntax$$

$icode%b% = %x% %Op% %y%$$


$head Purpose$$
Compares two operands where one of the operands is an
$codei%AD<%Base%>%$$ object.
The comparison has the same interpretation as for 
the $icode Base$$ type.


$head Op$$
The operator $icode Op$$ is one of the following:
$table
$bold Op$$ $pre $$  $cnext $bold Meaning$$                           $rnext
$code <$$   $cnext is $icode x$$ less than $icode y$$              $rnext
$code <=$$  $cnext is $icode x$$ less than or equal $icode y$$     $rnext
$code >$$   $cnext is $icode x$$ greater than $icode y$$           $rnext
$code >=$$  $cnext is $icode x$$ greater than or equal $icode y$$  $rnext
$code ==$$  $cnext is $icode x$$ equal to $icode y$$               $rnext
$code !=$$  $cnext is $icode x$$ not equal to $icode y$$
$tend
 
$head x$$
The operand $icode x$$ has prototype
$codei%
	const %Type% &%x%
%$$
where $icode Type$$ is $codei%AD<%Base%>%$$, $icode Base$$, or $code int$$.

$head y$$
The operand $icode y$$ has prototype
$codei%
	const %Type% &%y%
%$$
where $icode Type$$ is $codei%AD<%Base%>%$$, $icode Base$$, or $code int$$.

$head b$$
The result $icode b$$ has type
$codei%
	bool %b%
%$$

$head Operation Sequence$$
The result of this operation is a $code bool$$ value
(not an $cref/AD of Base/glossary/AD of Base/$$ object).
Thus it will not be recorded as part of an
AD of $icode Base$$
$cref/operation sequence/glossary/Operation/Sequence/$$.
$pre

$$
For example, suppose 
$icode x$$ and $icode y$$ are $codei%AD<%Base%>%$$ objects,
the tape corresponding to $codei%AD<%Base%>%$$ is recording,
$icode b$$ is true,
and the subsequent code is
$codei%
	if( %b% )
		%y% = cos(%x%);
	else	%y% = sin(%x%); 
%$$
only the assignment $icode%y% = cos(%x%)%$$ is recorded on the tape
(if $icode x$$ is a $cref/parameter/glossary/Parameter/$$, 
nothing is recorded).
The $cref CompareChange$$ function can yield
some information about changes in comparison operation results.
You can use $cref CondExp$$ to obtain comparison operations
that depends on the 
$cref/independent variable/glossary/Tape/Independent Variable/$$ 
values with out re-taping the AD sequence of operations.

$head Assumptions$$
If one of the $icode Op$$ operators listed above
is used with an $codei%AD<%Base%>%$$ object,
it is assumed that the same operator is supported by the base type 
$icode Base$$.

$head Example$$
$children%
	example/compare.cpp
%$$
The file
$cref compare.cpp$$
contains an example and test of these operations.
It returns true if it succeeds and false otherwise.

$end
-------------------------------------------------------------------------------
*/
//  BEGIN CppAD namespace
namespace CppAD {

template <class Base>

// -------------- RecordCompare(cop, result, left, right) --------------------
/// All these operations are done in \c Rec_, so we should move this
/// routine to <tt>recorder<Base></tt>.
void ADTape<Base>::RecordCompare(
	enum CompareOp  cop   ,
	bool           result ,
	const AD<Base> &left  ,
	const AD<Base> &right )
{	addr_t ind0, ind1, ind2, ind3;

	// ind[1] = base 2 representation of [result, Var(left), Var(right])
	ind1 = 0;

	// ind[2] = left address
	if( Parameter(left) )
		ind2 = Rec_.PutPar(left.value_);
	else
	{	ind1 += 2;
		ind2 =  left.taddr_;
	}

	// ind[3] = right address
	if( Parameter(right) )
		ind3 = Rec_.PutPar(right.value_);
	else
	{	ind1 += 4;
		ind3 =  right.taddr_;
	}

	// If both left and right are parameters, do not need to record
	if( ind1 == 0 )
		return;

	// ind[1] & 1 = result
	if( result )
		ind1+= 1;

	// ind[0] = cop 
	ind0 = size_t (cop);

	CPPAD_ASSERT_UNKNOWN( ind1 > 1 );
	CPPAD_ASSERT_UNKNOWN( NumArg(ComOp) == 4 );
	CPPAD_ASSERT_UNKNOWN( NumRes(ComOp) == 0 );

	// put the operator in the tape
	Rec_.PutOp(ComOp);
	Rec_.PutArg(ind0, ind1, ind2, ind3);
}

// -------------------------------- < -------------------------
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool operator < (const AD<Base> &left , const AD<Base> &right)
{	bool result =  (left.value_ < right.value_); 

	ADTape<Base> *tape = CPPAD_NULL;
	if( Variable(left) )
		tape = left.tape_this();
	else if ( Variable(right) )
		tape = right.tape_this();

	if( tape != CPPAD_NULL )
		tape->RecordCompare(CompareLt, result, left, right);

	return result;
}

// convert other cases into the case above
CPPAD_FOLD_BOOL_VALUED_BINARY_OPERATOR(<)

// -------------------------------- <= -------------------------
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool operator <= (const AD<Base> &left , const AD<Base> &right)
{ 	bool result =  (left.value_ <= right.value_); 

	ADTape<Base> *tape = CPPAD_NULL;
	if( Variable(left) )
		tape = left.tape_this();
	else if ( Variable(right) )
		tape = right.tape_this();

	if( tape != CPPAD_NULL )
		tape->RecordCompare(CompareLe, result, left, right);

	return result;
}

// convert other cases into the case above
CPPAD_FOLD_BOOL_VALUED_BINARY_OPERATOR(<=)


// -------------------------------- > -------------------------
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool operator > (const AD<Base> &left , const AD<Base> &right)
{	bool result =  (left.value_ > right.value_); 

	ADTape<Base> *tape = CPPAD_NULL;
	if( Variable(left) )
		tape = left.tape_this();
	else if ( Variable(right) )
		tape = right.tape_this();

	if( tape != CPPAD_NULL )
		tape->RecordCompare(CompareGt, result, left, right);


	return result;
}

// convert other cases into the case above
CPPAD_FOLD_BOOL_VALUED_BINARY_OPERATOR(>)

// -------------------------------- >= -------------------------
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool operator >= (const AD<Base> &left , const AD<Base> &right)
{ 	bool result =  (left.value_ >= right.value_); 

	ADTape<Base> *tape = CPPAD_NULL;
	if( Variable(left) )
		tape = left.tape_this();
	else if ( Variable(right) )
		tape = right.tape_this();

	if( tape != CPPAD_NULL )
		tape->RecordCompare(CompareGe, result, left, right);

	return result;
}

// convert other cases into the case above
CPPAD_FOLD_BOOL_VALUED_BINARY_OPERATOR(>=)


// -------------------------------- == -------------------------
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool operator == (const AD<Base> &left , const AD<Base> &right)
{	bool result =  (left.value_ == right.value_); 

	ADTape<Base> *tape = CPPAD_NULL;
	if( Variable(left) )
		tape = left.tape_this();
	else if ( Variable(right) )
		tape = right.tape_this();

	if( tape != CPPAD_NULL )
		tape->RecordCompare(CompareEq, result, left, right);

	return result;
}

// convert other cases into the case above
CPPAD_FOLD_BOOL_VALUED_BINARY_OPERATOR(==)

// -------------------------------- != -------------------------
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool operator != (const AD<Base> &left , const AD<Base> &right)
{	bool result =  (left.value_ != right.value_);

	ADTape<Base> *tape = CPPAD_NULL;
	if( Variable(left) )
		tape = left.tape_this();
	else if ( Variable(right) )
		tape = right.tape_this();

	if( tape != CPPAD_NULL )
		tape->RecordCompare(CompareNe, result, left, right);

	return result;
}

// convert other cases into the case above
CPPAD_FOLD_BOOL_VALUED_BINARY_OPERATOR(!=)

} // END CppAD namespace

# endif
