/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef ADC_CPP_CALLF_HXX
#define ADC_CPP_CALLF_HXX

// USED SERVICES




/**	This represents a function to be called, if a specific kind of token
	arrives in the semantic parser.

	@descr This class is only to be used as member of PeStatus<PE>.
	@template PE
		The owning ParseEnvironment.
	@see PeStatus, ParseEnvironment
*/
template <class PE>
class CallFunction
{
  public:
	typedef void (PE::*F_Tok)(const char *);

						CallFunction(
							F_Tok				i_f2Call,
							INT16				i_nTokType );

	F_Tok				GetF() const;
	INT16				TokType() const;

  private:
	// DATA
	F_Tok				f2Call;
	INT16				nTokType;
};


/**	One state within a ParseEnvironment.

	@template PE
		The owning ParseEnvironment.
*/
template <class PE>
class PeStatus
{
  public:
	typedef typename CallFunction<PE>::F_Tok  F_Tok;

						PeStatus(
							PE &	  			i_rMyPE,
							uintt				i_nSize,
							F_Tok *				i_pFuncArray,
							INT16 *             i_pTokTypeArray,
							F_Tok				i_pDefault );
	virtual             ~PeStatus();

	virtual void	   	Call_Handler(
							INT16				i_nTokTypeId,
							const char *		i_sTokenText ) const;

  private:
	bool				CheckForCall(
							uintt				i_nPos,
							INT16				i_nTokTypeId,
							const char *		i_sTokenText ) const;

	PE *	  			pMyPE;
	std::vector< CallFunction<PE> >
						aBranches;
	F_Tok				fDefault;
};


template <class PE>
class PeStatusArray
{
  public:
	typedef typename PE::E_State	State;

						PeStatusArray();
	void				InsertState(
							State				i_ePosition,
							DYN PeStatus<PE> &	let_drState );
						~PeStatusArray();

	const PeStatus<PE> &
						operator[](
							State				i_ePosition ) const;

	void				SetCur(
							State				i_eCurState );
	const PeStatus<PE> &
						Cur() const;

  private:
	DYN PeStatus<PE> *	aStati[PE::size_of_states];
	State				eState;
};



// IMPLEMENTATION


// CallFunction

template <class PE>
CallFunction<PE>::CallFunction(	F_Tok 	i_f2Call,
								INT16	i_nTokType )
	:	f2Call(i_f2Call),
		nTokType(i_nTokType)
{
}

template <class PE>
inline typename CallFunction<PE>::F_Tok
CallFunction<PE>::GetF() const
{
	return f2Call;
}

template <class PE>
inline INT16
CallFunction<PE>::TokType() const
{
	return nTokType;
}



// PeStatus

template <class PE>
PeStatus<PE>::PeStatus( PE &	  	i_rMyPE,
						uintt		i_nSize,
						F_Tok *		i_pFuncArray,
						INT16 *     i_pTokTypeArray,
						F_Tok		i_fDefault )
	:	pMyPE(&i_rMyPE),
		fDefault(i_fDefault)
{
	aBranches.reserve(i_nSize);
	for ( uintt i = 0; i < i_nSize; ++i )
	{
//		csv_assert(i > 0 ? i_pTokTypeArray[i] > i_pTokTypeArray[i-1] : true);
    	aBranches.push_back( CallFunction<PE>( i_pFuncArray[i], i_pTokTypeArray[i]) );
	}  // end for
}

template <class PE>
PeStatus<PE>::~PeStatus()
{

}

template <class PE>
void
PeStatus<PE>::Call_Handler( INT16				i_nTokTypeId,
							const char *		i_sTokenText ) const
{
	uintt nSize = aBranches.size();
	uintt nPos = nSize / 2;

	if ( i_nTokTypeId < aBranches[nPos].TokType() )
	{
		for ( --nPos; intt(nPos) >= 0; --nPos )
		{
			if (CheckForCall(nPos, i_nTokTypeId, i_sTokenText))
				return;
		}
	}
	else
	{
		for ( ; nPos < nSize; ++nPos )
		{
			if (CheckForCall(nPos, i_nTokTypeId, i_sTokenText))
				return;
		}
	}

	(pMyPE->*fDefault)(i_sTokenText);
}

template <class PE>
bool
PeStatus<PE>::CheckForCall(	uintt				i_nPos,
							INT16				i_nTokTypeId,
							const char *		i_sTokenText ) const
{
	if ( aBranches[i_nPos].TokType() == i_nTokTypeId )
	{
		(pMyPE->*aBranches[i_nPos].GetF())(i_sTokenText);
		return true;
	}
	return false;
}

// PeStatusArray

template <class PE>
PeStatusArray<PE>::PeStatusArray()
	:	eState(PE::size_of_states)
{
	memset(aStati, 0, sizeof aStati);
}

template <class PE>
void
PeStatusArray<PE>::InsertState(	State 				i_ePosition,
								DYN PeStatus<PE> &	let_drState )
{
	csv_assert(aStati[i_ePosition] == 0);
	aStati[i_ePosition] = &let_drState;
}

template <class PE>
PeStatusArray<PE>::~PeStatusArray()
{
	int i_max = PE::size_of_states;
	for (int i = 0; i < i_max; i++)
	{
		delete aStati[i];
	}  // end for
}

template <class PE>
inline const PeStatus<PE> &
PeStatusArray<PE>::operator[]( State i_ePosition ) const
{
	csv_assert( uintt(i_ePosition) < PE::size_of_states );
	csv_assert( aStati[i_ePosition] != 0 );
	return *aStati[i_ePosition];
}

template <class PE>
inline void
PeStatusArray<PE>::SetCur( State i_eCurState )
{
	eState = i_eCurState;
}


template <class PE>
const PeStatus<PE> &
PeStatusArray<PE>::Cur() const
{
	return (*this)[eState];
}

#define SEMPARSE_CREATE_STATUS(penv, state, default_function) \
	pStati->InsertState( state, \
						*new PeStatus<penv>( \
		*this, \
		sizeof( stateT_##state ) / sizeof (INT16), \
		stateF_##state, \
		stateT_##state, \
		&penv::default_function ) )


#endif

