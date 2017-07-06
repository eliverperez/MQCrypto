/*
 * $Id: PFLASH.c 1018 2008-05-08 08:34:39Z daven $
 */

#include "SignScheme.h"
#include <string.h>

inline void polyMul(byte * outZ , byte * x , byte * y);
inline void polySqu(byte * outZ , byte * x );
inline void polyPow8(byte * outZ , byte * x );
inline void polyPow24(byte * outZ , byte * x );
inline void polyPow56(byte * outZ , byte * x );
inline void polyPow88(byte * outZ , byte * x );

//extern inline void polyPow11( byte * outPoly , byte * x );
//extern inline void fillMat2(byte *AA, byte *B);

static byte _B[L_SIGN];
int PFLASH_QMap( byte * outB , byte * A )
{
	polyPow88( _B , A );       // _B = A^256^11 
	polyMul( outB , _B , A );  // outB = A^256^11 * A
	return 0;    
}


/*
AIM:
100000001000000010000000011111110111111101111111
01111111 10000000
100000001000000010000000011111110111111101111111
10000000
100000001000000010000000011111110111111101111111
01111111 
100000001000000010000000011111110111111101111111
01111111 10000000
100000001000000010000000011111110111111101111111
10000000

12345678911234567892123456789312345678941234567895123456
10000000000000000000000001111111                            P2
        10000000000000000000000001111111                    P2
                10000000000000000000000001111111            P2
100000001000000010000000011111110111111101111111            P3
10000000100000001000000001111111011111110111111100000000    P4
10000000100000001000000001111111011111110111111110000000    P2
10000000100000001000000001111111011111110111111101111111    P4
01111111    P1
10000000    P5
*/

int PFLASH_ivsQMap( byte * outA , byte * B )
{
	byte P1[37],P2[37],P3[37],P4[37],P5[37];
	
	polySqu( P5 , B );       //  P5 =       10
	polySqu( P5 , P5 );      //  P5 =      100
	polyMul( P2 , P5 , B );  //  P2 =      101
	polySqu( P5 , P5 );      //  P5 =     1000
	polySqu( P5 , P5 );      //  P5 =    10000
	polyMul( P2 , P5 , P2 ); //  P2 =    10101
	polySqu( P1 , P2 );      //  P1 =   101010
	polyMul( P1 , P1 , P2 ); //  P1 =   111111
	polySqu( P5 , P5 );      //  P5 =   100000
	polySqu( P5 , P5 );      //  P5 =  1000000
	polyMul( P1 , P5 , P1 ); //  P1 =  1111111   , done
	polySqu( P5 , P5 );      //  P5 = 10000000   , done
	
	polyPow24( P2 , P5 );    //  P2 = 10000000000000000000000000000000
	polyMul( P2 , P2 , P1 ); //  P2 = 10000000000000000000000001111111  , done
	
	polyPow8( P3 , P2 );
	polyMul( P3 , P3 , P2 );
	polyPow8( outA , P3 );
	polyMul( P3 , outA , P2 );  // P3 , done  100000001000000010000000011111110111111101111111 
	polyPow8( P4 , P3 );        // P4 , done 
	polyMul( P2 , P4 , P5 );    // P2 , done
	polyMul( P4 , P4 , P1 );    // P4 , done
	
	polyPow8( outA , P4 );
	polyMul( P3 , outA , P5 );
	
	polyPow56( outA , P3 );
	polyMul( P3 , outA , P2 );
	
	polyPow56( outA , P3 );
	polyMul( P3 , outA , P4 );
	
	polyPow56( outA , P3 );
	polyMul( P3 , outA , P4 );
	polyPow8( outA , P3 );
	polyMul( P3 , outA , P5 );
	
	polyPow56( outA , P3 );
	polyMul( outA , outA , P2 );

	return 0;	
}

byte _Z[73];
inline void polySqu(byte * outZ , byte * x)
{
	int i;
	memset( _Z , 0 , 73 );
	for( i = 0 ; i < 37 ; i++ )
	{
		int logX = logtab[x[i]];
		if( logX ) _Z[i<<1] = alogtab[logX<<1];
	}
	byte temp;
	for( i = 72 ; i > 36 ; i-- )
	{
		if( _Z[i] )
		{
			temp = _Z[i];
			_Z[i-(37-12)] ^= temp;
			_Z[i-(37-10)] ^= temp;
			_Z[i-(37- 2)] ^= temp;
			_Z[i-(37- 0)] ^= temp;
		}
	}
	memcpy( outZ ,_Z,37);
}

inline void polyMul(byte * outZ , byte * x , byte * y)
{
   int i,j;
   memset(_Z,0,73);
   for( i = 0 ; i < 37 ; i++ )
   {
		int logX = logtab[x[i]];
		if( 0 == logX ) continue;
		for( j = 0 ; j < 37; j++ )
		{
			int logY = logtab[y[j]];
			if( logY ) _Z[i+j] ^= alogtab[logX+logY];
		}
	}
	
	byte temp;
	for(i=72;i>36;i--)
	{
		temp = _Z[i];
		if(temp)
		{
			_Z[i-(37-12)] ^= temp;
			_Z[i-(37-10)] ^= temp;
			_Z[i-(37- 2)] ^= temp;
			_Z[i-(37- 0)] ^= temp;
		}
	}
	memcpy( outZ ,_Z,37);
}


inline void polyPow8( byte * outZ , byte * x )
{
outZ[0]=x[0]^x[2]^x[5]^x[6]^x[10]^x[14]^x[15]^x[16]^x[18]^x[19]^x[22]^x[23]^x[26]^x[27]^x[28]^x[31]^x[34]^x[35]^x[36];
outZ[1]=x[1]^x[2]^x[4]^x[5]^x[6]^x[8]^x[10]^x[11]^x[12]^x[13]^x[14]^x[16]^x[17]^x[20]^x[21]^x[23]^x[24]^x[25]^x[27]^x[28]^x[31]^x[36];
outZ[2]=x[2]^x[3]^x[4]^x[5]^x[7]^x[13]^x[14]^x[17]^x[19]^x[21]^x[24]^x[29]^x[34]^x[35];
outZ[3]=x[2]^x[3]^x[4]^x[5]^x[6]^x[8]^x[10]^x[11]^x[12]^x[15]^x[19]^x[23]^x[24]^x[25]^x[26]^x[28]^x[29]^x[30]^x[31]^x[36];
outZ[4]=x[2]^x[4]^x[5]^x[6]^x[9]^x[10]^x[12]^x[14]^x[16]^x[17]^x[19]^x[21]^x[22]^x[26]^x[30]^x[33]^x[34]^x[36];
outZ[5]=x[2]^x[3]^x[5]^x[8]^x[9]^x[10]^x[13]^x[14]^x[16]^x[18]^x[19]^x[20]^x[22]^x[23]^x[27]^x[29]^x[30]^x[33];
outZ[6]=x[1]^x[5]^x[6]^x[8]^x[9]^x[11]^x[13]^x[23]^x[25]^x[27]^x[29]^x[32]^x[34]^x[35];
outZ[7]=x[1]^x[2]^x[3]^x[8]^x[9]^x[11]^x[15]^x[16]^x[24]^x[25]^x[26]^x[28]^x[29]^x[34]^x[35]^x[36];
outZ[8]=x[2]^x[3]^x[4]^x[5]^x[7]^x[9]^x[10]^x[11]^x[12]^x[17]^x[19]^x[20]^x[21]^x[24]^x[27]^x[28]^x[31]^x[32]^x[33];
outZ[9]=x[1]^x[2]^x[3]^x[5]^x[6]^x[8]^x[10]^x[12]^x[13]^x[16]^x[17]^x[19]^x[20]^x[22]^x[23]^x[26]^x[28]^x[29]^x[30]^x[33]^x[35]^x[36];
outZ[10]=x[1]^x[3]^x[5]^x[6]^x[8]^x[9]^x[10]^x[13]^x[14]^x[16]^x[17]^x[18]^x[22]^x[23]^x[27]^x[28]^x[29]^x[30]^x[34]^x[35];
outZ[11]=x[4]^x[7]^x[8]^x[10]^x[11]^x[15]^x[16]^x[17]^x[19]^x[22]^x[23]^x[26]^x[27]^x[30]^x[32]^x[33]^x[35];
outZ[12]=x[2]^x[7]^x[8]^x[9]^x[11]^x[12]^x[13]^x[14]^x[16]^x[17]^x[19]^x[24]^x[28]^x[30]^x[31]^x[34];
outZ[13]=x[3]^x[4]^x[6]^x[7]^x[8]^x[9]^x[10]^x[12]^x[13]^x[16]^x[22]^x[25]^x[27]^x[28]^x[29]^x[31]^x[32]^x[33];
outZ[14]=x[4]^x[6]^x[7]^x[8]^x[12]^x[13]^x[18]^x[21]^x[23]^x[25]^x[27]^x[28]^x[29]^x[31]^x[32]^x[33]^x[36];
outZ[15]=x[7]^x[10]^x[12]^x[13]^x[16]^x[17]^x[20]^x[21]^x[23]^x[30]^x[31]^x[32]^x[33]^x[36];
outZ[16]=x[5]^x[6]^x[7]^x[8]^x[9]^x[11]^x[12]^x[14]^x[16]^x[18]^x[19]^x[27]^x[29]^x[30]^x[31]^x[32]^x[33]^x[35];
outZ[17]=x[2]^x[3]^x[4]^x[5]^x[6]^x[8]^x[10]^x[12]^x[13]^x[14]^x[15]^x[19]^x[20]^x[21]^x[22]^x[23]^x[24]^x[26]^x[27]^x[30]^x[34];
outZ[18]=x[3]^x[4]^x[5]^x[6]^x[8]^x[10]^x[11]^x[12]^x[15]^x[16]^x[18]^x[20]^x[23]^x[24]^x[26]^x[27]^x[32]^x[33]^x[35];
outZ[19]=x[1]^x[2]^x[3]^x[4]^x[5]^x[6]^x[7]^x[8]^x[10]^x[12]^x[14]^x[18]^x[19]^x[22]^x[25]^x[26]^x[29]^x[31]^x[33]^x[36];
outZ[20]=x[7]^x[8]^x[10]^x[13]^x[15]^x[19]^x[20]^x[26]^x[28]^x[29]^x[31]^x[32]^x[34];
outZ[21]=x[1]^x[4]^x[5]^x[9]^x[11]^x[13]^x[19]^x[21]^x[27]^x[28]^x[29]^x[32]^x[33]^x[34]^x[36];
outZ[22]=x[5]^x[8]^x[9]^x[11]^x[12]^x[14]^x[17]^x[18]^x[22]^x[24]^x[29]^x[31];
outZ[23]=x[1]^x[3]^x[4]^x[6]^x[9]^x[10]^x[12]^x[13]^x[15]^x[17]^x[19]^x[20]^x[22]^x[23]^x[24]^x[26]^x[27]^x[29]^x[30]^x[31]^x[33]^x[36];
outZ[24]=x[4]^x[7]^x[9]^x[11]^x[14]^x[16]^x[17]^x[18]^x[19]^x[21]^x[22]^x[23]^x[24]^x[25]^x[26]^x[28]^x[31]^x[32]^x[33]^x[34];
outZ[25]=x[1]^x[4]^x[7]^x[8]^x[9]^x[12]^x[13]^x[14]^x[15]^x[17]^x[19]^x[20]^x[26]^x[31]^x[33];
outZ[26]=x[6]^x[8]^x[11]^x[12]^x[13]^x[14]^x[15]^x[16]^x[17]^x[18]^x[20]^x[21]^x[23]^x[24]^x[26]^x[32]^x[33];
outZ[27]=x[2]^x[3]^x[5]^x[7]^x[10]^x[12]^x[13]^x[16]^x[17]^x[19]^x[20]^x[23]^x[24]^x[25]^x[26]^x[27]^x[28]^x[30]^x[31]^x[33]^x[35]^x[36];
outZ[28]=x[3]^x[5]^x[8]^x[9]^x[12]^x[13]^x[14]^x[15]^x[16]^x[17]^x[23]^x[24]^x[26]^x[31]^x[33]^x[36];
outZ[29]=x[1]^x[2]^x[5]^x[6]^x[7]^x[8]^x[9]^x[10]^x[11]^x[14]^x[16]^x[19]^x[20]^x[23]^x[29]^x[34]^x[35];
outZ[30]=x[3]^x[7]^x[9]^x[11]^x[14]^x[20]^x[21]^x[24]^x[26]^x[31]^x[32]^x[34]^x[35];
outZ[31]=x[1]^x[2]^x[3]^x[7]^x[10]^x[13]^x[14]^x[16]^x[18]^x[19]^x[20]^x[21]^x[23]^x[24]^x[26]^x[30]^x[31]^x[35]^x[36];
outZ[32]=x[10]^x[11]^x[12]^x[14]^x[16]^x[18]^x[21]^x[22]^x[24]^x[28]^x[29]^x[32]^x[35]^x[36];
outZ[33]=x[1]^x[2]^x[3]^x[4]^x[5]^x[9]^x[10]^x[11]^x[12]^x[15]^x[16]^x[17]^x[18]^x[19]^x[21]^x[23]^x[24]^x[27]^x[30]^x[31]^x[35]^x[36];
outZ[34]=x[1]^x[4]^x[6]^x[8]^x[10]^x[12]^x[15]^x[16]^x[19]^x[20]^x[23]^x[24]^x[26]^x[28]^x[30]^x[31];
outZ[35]=x[1]^x[3]^x[4]^x[6]^x[8]^x[9]^x[18]^x[19]^x[22]^x[24]^x[26]^x[27]^x[30]^x[31]^x[34]^x[35];
outZ[36]=x[6]^x[8]^x[9]^x[10]^x[12]^x[15]^x[16]^x[17]^x[20]^x[22]^x[23]^x[24]^x[27]^x[29]^x[30]^x[31]^x[32]^x[35]^x[36];	
}


inline void polyPow24( byte * outZ , byte * x )
{
outZ[0]=x[0]^x[1]^x[3]^x[8]^x[12]^x[15]^x[17]^x[18]^x[19]^x[21]^x[22]^x[23]^x[25]^x[26]^x[28]^x[29]^x[30];
outZ[1]=x[2]^x[3]^x[4]^x[6]^x[9]^x[11]^x[12]^x[14]^x[15]^x[19]^x[22]^x[24]^x[25]^x[29]^x[30]^x[35]^x[36];
outZ[2]=x[2]^x[3]^x[4]^x[6]^x[8]^x[10]^x[11]^x[12]^x[13]^x[19]^x[20]^x[22]^x[23]^x[24]^x[26]^x[28]^x[29]^x[33]^x[35]^x[36];
outZ[3]=x[2]^x[4]^x[5]^x[6]^x[8]^x[11]^x[13]^x[15]^x[17]^x[18]^x[21]^x[24]^x[26]^x[27]^x[29]^x[30]^x[31]^x[32]^x[36];
outZ[4]=x[3]^x[4]^x[5]^x[9]^x[10]^x[11]^x[15]^x[16]^x[18]^x[21]^x[22]^x[23]^x[24]^x[26]^x[28]^x[29]^x[30]^x[31]^x[32]^x[33]^x[36];
outZ[5]=x[2]^x[4]^x[5]^x[6]^x[10]^x[11]^x[15]^x[16]^x[18]^x[20]^x[22]^x[25]^x[26]^x[27]^x[30]^x[31]^x[35];
outZ[6]=x[4]^x[5]^x[9]^x[11]^x[13]^x[15]^x[16]^x[17]^x[19]^x[20]^x[21]^x[22]^x[24]^x[27]^x[30]^x[34]^x[35];
outZ[7]=x[9]^x[10]^x[11]^x[12]^x[13]^x[14]^x[17]^x[18]^x[19]^x[20]^x[21]^x[22]^x[24]^x[25]^x[26]^x[28]^x[29]^x[32]^x[34]^x[35]^x[36];
outZ[8]=x[4]^x[5]^x[6]^x[7]^x[8]^x[9]^x[11]^x[14]^x[16]^x[17]^x[18]^x[19]^x[22]^x[23]^x[33]^x[34]^x[35];
outZ[9]=x[1]^x[6]^x[10]^x[12]^x[15]^x[16]^x[17]^x[19]^x[20]^x[23]^x[24]^x[25]^x[34]^x[36];
outZ[10]=x[1]^x[5]^x[7]^x[9]^x[14]^x[15]^x[17]^x[22]^x[24]^x[28]^x[31]^x[32]^x[33]^x[35];
outZ[11]=x[2]^x[3]^x[5]^x[7]^x[8]^x[9]^x[10]^x[14]^x[19]^x[22]^x[23]^x[29]^x[34]^x[35];
outZ[12]=x[5]^x[6]^x[7]^x[9]^x[10]^x[11]^x[12]^x[13]^x[16]^x[17]^x[21]^x[22]^x[23]^x[25]^x[27]^x[32];
outZ[13]=x[1]^x[2]^x[3]^x[4]^x[5]^x[6]^x[7]^x[8]^x[11]^x[13]^x[14]^x[16]^x[17]^x[18]^x[19]^x[21]^x[22]^x[25]^x[27]^x[28]^x[29]^x[30]^x[32];
outZ[14]=x[1]^x[5]^x[6]^x[8]^x[9]^x[10]^x[12]^x[13]^x[15]^x[17]^x[19]^x[22]^x[24]^x[25]^x[26]^x[29]^x[30]^x[31]^x[32]^x[33]^x[34];
outZ[15]=x[3]^x[5]^x[6]^x[7]^x[8]^x[10]^x[12]^x[13]^x[14]^x[16]^x[17]^x[20]^x[21]^x[24]^x[28]^x[30]^x[31]^x[32]^x[35];
outZ[16]=x[1]^x[5]^x[7]^x[9]^x[11]^x[13]^x[14]^x[15]^x[16]^x[17]^x[18]^x[19]^x[20]^x[22]^x[24]^x[26]^x[28]^x[31]^x[32]^x[33]^x[34]^x[35];
outZ[17]=x[1]^x[2]^x[3]^x[8]^x[11]^x[13]^x[14]^x[15]^x[17]^x[18]^x[22]^x[24]^x[25]^x[26]^x[27]^x[29]^x[31]^x[33]^x[34];
outZ[18]=x[2]^x[3]^x[4]^x[5]^x[9]^x[10]^x[12]^x[13]^x[14]^x[15]^x[16]^x[23]^x[24]^x[25]^x[36];
outZ[19]=x[1]^x[2]^x[3]^x[6]^x[7]^x[11]^x[12]^x[15]^x[18]^x[21]^x[23]^x[24]^x[32]^x[36];
outZ[20]=x[4]^x[6]^x[7]^x[9]^x[11]^x[13]^x[16]^x[20]^x[22]^x[23]^x[26]^x[27]^x[28]^x[35];
outZ[21]=x[1]^x[2]^x[3]^x[4]^x[5]^x[6]^x[7]^x[8]^x[10]^x[15]^x[16]^x[19]^x[20]^x[22]^x[24]^x[29]^x[31]^x[32]^x[34]^x[36];
outZ[22]=x[1]^x[2]^x[3]^x[4]^x[8]^x[9]^x[11]^x[12]^x[13]^x[14]^x[15]^x[17]^x[18]^x[19]^x[21]^x[27]^x[28]^x[31]^x[34]^x[35]^x[36];
outZ[23]=x[1]^x[2]^x[4]^x[5]^x[6]^x[9]^x[10]^x[11]^x[13]^x[14]^x[15]^x[20]^x[21]^x[23]^x[24]^x[25]^x[26]^x[27]^x[29]^x[32]^x[33]^x[34]^x[35];
outZ[24]=x[1]^x[3]^x[9]^x[10]^x[11]^x[12]^x[13]^x[14]^x[17]^x[18]^x[19]^x[20]^x[22]^x[24]^x[26]^x[27]^x[31]^x[32]^x[33]^x[34]^x[35];
outZ[25]=x[1]^x[3]^x[7]^x[8]^x[9]^x[10]^x[17]^x[19]^x[20]^x[22]^x[23]^x[24]^x[26]^x[29]^x[30]^x[33]^x[34]^x[36];
outZ[26]=x[2]^x[3]^x[4]^x[6]^x[8]^x[10]^x[11]^x[12]^x[13]^x[14]^x[16]^x[17]^x[21]^x[22]^x[23]^x[25]^x[26]^x[28]^x[32]^x[33]^x[34]^x[36];
outZ[27]=x[1]^x[2]^x[3]^x[5]^x[8]^x[11]^x[12]^x[13]^x[17]^x[18]^x[22]^x[24]^x[26]^x[29]^x[31]^x[32]^x[36];
outZ[28]=x[2]^x[3]^x[9]^x[10]^x[11]^x[12]^x[13]^x[16]^x[18]^x[20]^x[23]^x[24]^x[25]^x[26]^x[30]^x[34]^x[35];
outZ[29]=x[1]^x[2]^x[3]^x[6]^x[8]^x[9]^x[12]^x[13]^x[15]^x[18]^x[19]^x[22]^x[23]^x[24]^x[25]^x[28]^x[30]^x[31]^x[32]^x[33]^x[35];
outZ[30]=x[5]^x[6]^x[9]^x[10]^x[11]^x[12]^x[13]^x[14]^x[15]^x[16]^x[17]^x[20]^x[23]^x[24]^x[25]^x[26]^x[27]^x[28]^x[31]^x[32]^x[34]^x[35];
outZ[31]=x[1]^x[2]^x[7]^x[11]^x[12]^x[13]^x[18]^x[19]^x[20]^x[21]^x[23]^x[24]^x[25]^x[29]^x[30]^x[33]^x[34]^x[36];
outZ[32]=x[1]^x[2]^x[5]^x[7]^x[9]^x[10]^x[14]^x[18]^x[19]^x[22]^x[23]^x[25]^x[26]^x[28]^x[29]^x[30]^x[31]^x[32]^x[34]^x[36];
outZ[33]=x[1]^x[2]^x[3]^x[4]^x[6]^x[7]^x[8]^x[9]^x[11]^x[12]^x[15]^x[17]^x[18]^x[21]^x[26]^x[27]^x[28]^x[29]^x[36];
outZ[34]=x[1]^x[2]^x[3]^x[4]^x[6]^x[7]^x[15]^x[16]^x[22]^x[23]^x[25]^x[26]^x[27]^x[28]^x[30]^x[34]^x[36];
outZ[35]=x[1]^x[2]^x[3]^x[5]^x[7]^x[8]^x[9]^x[10]^x[11]^x[13]^x[15]^x[17]^x[20]^x[21]^x[22]^x[26]^x[27]^x[28]^x[29]^x[31]^x[32]^x[33]^x[34]^x[35];
outZ[36]=x[1]^x[3]^x[4]^x[5]^x[6]^x[8]^x[10]^x[17]^x[18]^x[19]^x[20]^x[23]^x[24]^x[26]^x[27]^x[28]^x[29]^x[30]^x[32]^x[33];	
}


inline void polyPow56( byte * outZ , byte * x )
{
outZ[0]=x[0]^x[1]^x[3]^x[4]^x[5]^x[6]^x[10]^x[11]^x[12]^x[13]^x[17]^x[18]^x[19]^x[26]^x[27]^x[28]^x[30]^x[31]^x[32]^x[34]^x[35]^x[36];
outZ[1]=x[2]^x[3]^x[5]^x[7]^x[9]^x[12]^x[13]^x[14]^x[15]^x[17]^x[18]^x[19]^x[20]^x[21]^x[24]^x[25]^x[28]^x[31]^x[33]^x[35]^x[36];
outZ[2]=x[2]^x[3]^x[7]^x[8]^x[10]^x[11]^x[15]^x[16]^x[18]^x[19]^x[23]^x[26]^x[27]^x[28]^x[29]^x[30]^x[31]^x[35];
outZ[3]=x[3]^x[6]^x[9]^x[12]^x[13]^x[14]^x[16]^x[18]^x[19]^x[22]^x[26];
outZ[4]=x[1]^x[3]^x[4]^x[5]^x[7]^x[10]^x[12]^x[14]^x[16]^x[19]^x[21]^x[25]^x[26]^x[27]^x[28]^x[30]^x[33]^x[34];
outZ[5]=x[2]^x[3]^x[4]^x[8]^x[10]^x[11]^x[13]^x[14]^x[15]^x[16]^x[17]^x[19]^x[20]^x[23]^x[24]^x[25]^x[28]^x[33]^x[34]^x[36];
outZ[6]=x[2]^x[3]^x[4]^x[10]^x[12]^x[14]^x[15]^x[16]^x[25]^x[26]^x[27]^x[31]^x[33]^x[34]^x[35]^x[36];
outZ[7]=x[2]^x[4]^x[9]^x[10]^x[11]^x[12]^x[15]^x[17]^x[18]^x[19]^x[20]^x[25]^x[26]^x[27]^x[28]^x[29]^x[30]^x[31]^x[33]^x[34];
outZ[8]=x[1]^x[2]^x[3]^x[4]^x[9]^x[11]^x[17]^x[18]^x[20]^x[25]^x[26]^x[29]^x[30]^x[31]^x[33]^x[34]^x[36];
outZ[9]=x[1]^x[2]^x[5]^x[8]^x[10]^x[13]^x[14]^x[15]^x[16]^x[17]^x[23]^x[25]^x[26]^x[27]^x[28]^x[29]^x[30]^x[31]^x[32]^x[33]^x[35]^x[36];
outZ[10]=x[2]^x[4]^x[6]^x[9]^x[10]^x[11]^x[14]^x[15]^x[16]^x[17]^x[18]^x[19]^x[20]^x[22]^x[23]^x[25]^x[27]^x[28]^x[32];
outZ[11]=x[1]^x[3]^x[4]^x[5]^x[6]^x[7]^x[9]^x[10]^x[13]^x[14]^x[17]^x[19]^x[23]^x[24]^x[25]^x[27]^x[28]^x[29]^x[30]^x[36];
outZ[12]=x[1]^x[4]^x[8]^x[9]^x[12]^x[14]^x[15]^x[16]^x[18]^x[19]^x[21]^x[22]^x[24]^x[25]^x[26]^x[27]^x[29]^x[30]^x[33]^x[35];
outZ[13]=x[2]^x[4]^x[6]^x[7]^x[9]^x[11]^x[13]^x[14]^x[22]^x[23]^x[24]^x[25]^x[26]^x[28]^x[29]^x[32]^x[34];
outZ[14]=x[1]^x[3]^x[4]^x[6]^x[8]^x[10]^x[12]^x[18]^x[21]^x[24]^x[26]^x[28]^x[30]^x[31]^x[32];
outZ[15]=x[2]^x[3]^x[4]^x[5]^x[6]^x[9]^x[10]^x[12]^x[13]^x[15]^x[17]^x[19]^x[20]^x[21]^x[22]^x[24]^x[27]^x[28]^x[29]^x[30]^x[33]^x[34]^x[35]^x[36];
outZ[16]=x[7]^x[8]^x[9]^x[10]^x[14]^x[15]^x[16]^x[17]^x[21]^x[22]^x[23]^x[24]^x[25]^x[28]^x[32]^x[33]^x[35]^x[36];
outZ[17]=x[3]^x[5]^x[10]^x[12]^x[13]^x[16]^x[18]^x[19]^x[20]^x[21]^x[22]^x[23]^x[26]^x[31]^x[32]^x[33]^x[34]^x[36];
outZ[18]=x[1]^x[2]^x[3]^x[5]^x[6]^x[7]^x[8]^x[9]^x[14]^x[15]^x[16]^x[17]^x[18]^x[19]^x[20]^x[21]^x[24]^x[36];
outZ[19]=x[1]^x[6]^x[7]^x[9]^x[10]^x[12]^x[14]^x[18]^x[23]^x[25]^x[26]^x[28]^x[29]^x[30]^x[32]^x[33]^x[35]^x[36];
outZ[20]=x[1]^x[3]^x[8]^x[10]^x[11]^x[12]^x[13]^x[14]^x[18]^x[23]^x[24]^x[26]^x[27]^x[29]^x[30]^x[31]^x[35]^x[36];
outZ[21]=x[2]^x[3]^x[4]^x[5]^x[7]^x[11]^x[13]^x[17]^x[19]^x[20]^x[23]^x[27]^x[28]^x[29]^x[32]^x[35];
outZ[22]=x[1]^x[3]^x[4]^x[6]^x[7]^x[9]^x[10]^x[11]^x[14]^x[15]^x[22]^x[24]^x[27]^x[29]^x[31]^x[32]^x[36];
outZ[23]=x[3]^x[5]^x[6]^x[8]^x[9]^x[10]^x[11]^x[13]^x[16]^x[18]^x[20]^x[21]^x[24]^x[32]^x[36];
outZ[24]=x[1]^x[2]^x[8]^x[11]^x[13]^x[15]^x[16]^x[18]^x[19]^x[23]^x[24]^x[28]^x[30]^x[31]^x[32]^x[33]^x[35]^x[36];
outZ[25]=x[6]^x[9]^x[11]^x[12]^x[13]^x[14]^x[15]^x[17]^x[18]^x[19]^x[20]^x[21]^x[23]^x[24]^x[26]^x[28]^x[29]^x[30]^x[32]^x[33]^x[36];
outZ[26]=x[3]^x[4]^x[7]^x[8]^x[9]^x[12]^x[13]^x[14]^x[17]^x[18]^x[22]^x[26]^x[28]^x[29]^x[33]^x[35];
outZ[27]=x[1]^x[2]^x[3]^x[4]^x[7]^x[9]^x[11]^x[12]^x[13]^x[15]^x[16]^x[17]^x[19]^x[21]^x[22]^x[23]^x[26]^x[27]^x[28]^x[29]^x[31]^x[32]^x[34];
outZ[28]=x[2]^x[6]^x[7]^x[8]^x[11]^x[12]^x[13]^x[15]^x[16]^x[17]^x[20]^x[21]^x[24]^x[27]^x[33]^x[36];
outZ[29]=x[2]^x[3]^x[5]^x[7]^x[8]^x[9]^x[11]^x[12]^x[13]^x[14]^x[17]^x[20]^x[21]^x[22]^x[23]^x[27]^x[28]^x[29]^x[30]^x[32]^x[34]^x[35]^x[36];
outZ[30]=x[1]^x[4]^x[6]^x[7]^x[8]^x[9]^x[10]^x[11]^x[12]^x[13]^x[18]^x[19]^x[20]^x[21]^x[24]^x[25]^x[26]^x[27]^x[29]^x[30]^x[34];
outZ[31]=x[1]^x[6]^x[7]^x[14]^x[16]^x[23]^x[26]^x[28]^x[31]^x[32]^x[33];
outZ[32]=x[1]^x[7]^x[13]^x[14]^x[15]^x[16]^x[17]^x[18]^x[19]^x[20]^x[25]^x[27]^x[28]^x[29]^x[30]^x[31]^x[32]^x[33]^x[34];
outZ[33]=x[1]^x[3]^x[8]^x[9]^x[10]^x[11]^x[13]^x[19]^x[20]^x[21]^x[24]^x[25]^x[26]^x[30]^x[32]^x[33]^x[36];
outZ[34]=x[1]^x[3]^x[5]^x[6]^x[7]^x[10]^x[11]^x[15]^x[20]^x[23]^x[24]^x[26]^x[32]^x[33]^x[36];
outZ[35]=x[2]^x[5]^x[7]^x[10]^x[11]^x[12]^x[13]^x[14]^x[16]^x[17]^x[19]^x[20]^x[22]^x[24]^x[25]^x[26]^x[27]^x[28]^x[32]^x[33];
outZ[36]=x[1]^x[2]^x[4]^x[6]^x[9]^x[10]^x[11]^x[12]^x[13]^x[14]^x[16]^x[17]^x[18]^x[23]^x[27]^x[28]^x[30]^x[32]^x[33]^x[34]^x[36];
}

inline void polyPow88( byte * outPoly , byte * x )
{
outPoly[0]=x[0]^x[4]^x[6]^x[7]^x[10]^x[13]^x[14]^x[16]^x[17]^x[19]^x[20]^x[21]^x[22]^x[28]^x[30]^x[32]^x[33]^x[36];
outPoly[1]=x[3]^x[4]^x[6]^x[7]^x[8]^x[9]^x[10]^x[13]^x[14]^x[15]^x[18]^x[25]^x[33]^x[34]^x[35];
outPoly[2]=x[1]^x[3]^x[5]^x[9]^x[16]^x[18]^x[22]^x[23]^x[24]^x[29]^x[35];
outPoly[3]=x[1]^x[3]^x[4]^x[6]^x[7]^x[8]^x[9]^x[10]^x[11]^x[13]^x[16]^x[17]^x[18]^x[21]^x[24]^x[28]^x[34]^x[35]^x[36];
outPoly[4]=x[1]^x[3]^x[4]^x[7]^x[9]^x[11]^x[14]^x[17]^x[18]^x[19]^x[21]^x[24]^x[26]^x[27]^x[29]^x[32]^x[34]^x[35]^x[36];
outPoly[5]=x[2]^x[3]^x[8]^x[12]^x[14]^x[16]^x[18]^x[20]^x[26]^x[27]^x[29]^x[31]^x[35];
outPoly[6]=x[5]^x[6]^x[7]^x[9]^x[10]^x[12]^x[20]^x[21]^x[22]^x[25]^x[27]^x[29]^x[30]^x[31]^x[34]^x[35];
outPoly[7]=x[1]^x[2]^x[6]^x[7]^x[9]^x[10]^x[11]^x[13]^x[14]^x[16]^x[18]^x[23]^x[24]^x[26]^x[27]^x[33]^x[35];
outPoly[8]=x[1]^x[4]^x[6]^x[7]^x[9]^x[11]^x[15]^x[16]^x[17]^x[18]^x[19]^x[21]^x[22]^x[25]^x[26]^x[31]^x[32]^x[33]^x[35]^x[36];
outPoly[9]=x[3]^x[5]^x[6]^x[7]^x[11]^x[12]^x[13]^x[14]^x[17]^x[18]^x[19]^x[21]^x[22]^x[23]^x[24]^x[25]^x[26]^x[28]^x[31]^x[32]^x[36];
outPoly[10]=x[3]^x[6]^x[9]^x[10]^x[12]^x[14]^x[17]^x[19]^x[21]^x[23]^x[25]^x[27]^x[28]^x[30]^x[31];
outPoly[11]=x[1]^x[3]^x[8]^x[13]^x[14]^x[15]^x[17]^x[18]^x[20]^x[21]^x[25]^x[27]^x[28]^x[30]^x[32]^x[33];
outPoly[12]=x[1]^x[2]^x[4]^x[6]^x[7]^x[11]^x[13]^x[14]^x[15]^x[16]^x[17]^x[19]^x[20]^x[24]^x[26]^x[28]^x[32]^x[33];
outPoly[13]=x[3]^x[10]^x[11]^x[14]^x[17]^x[18]^x[19]^x[22]^x[29]^x[30]^x[35]^x[36];
outPoly[14]=x[1]^x[6]^x[9]^x[10]^x[12]^x[15]^x[17]^x[18]^x[19]^x[20]^x[22]^x[24]^x[25]^x[28]^x[29]^x[31]^x[32]^x[33]^x[34]^x[35]^x[36];
outPoly[15]=x[1]^x[5]^x[6]^x[7]^x[8]^x[9]^x[10]^x[14]^x[16]^x[19]^x[20]^x[22]^x[29]^x[31];
outPoly[16]=x[1]^x[5]^x[7]^x[9]^x[10]^x[12]^x[16]^x[19]^x[22]^x[26]^x[27]^x[31]^x[33]^x[34]^x[35];
outPoly[17]=x[5]^x[6]^x[7]^x[9]^x[11]^x[14]^x[15]^x[16]^x[17]^x[20]^x[21]^x[23]^x[27]^x[28]^x[31]^x[33]^x[35];
outPoly[18]=x[1]^x[2]^x[4]^x[5]^x[6]^x[8]^x[9]^x[10]^x[11]^x[12]^x[16]^x[21]^x[22]^x[24]^x[25]^x[29]^x[32]^x[33]^x[35]^x[36];
outPoly[19]=x[2]^x[3]^x[4]^x[5]^x[7]^x[9]^x[17]^x[20]^x[23]^x[24]^x[25]^x[29]^x[30]^x[31]^x[32]^x[33]^x[34];
outPoly[20]=x[7]^x[8]^x[12]^x[14]^x[18]^x[20]^x[22]^x[26]^x[27]^x[28]^x[31]^x[32]^x[33];
outPoly[21]=x[1]^x[4]^x[6]^x[9]^x[10]^x[12]^x[13]^x[14]^x[18]^x[19]^x[22]^x[27]^x[28]^x[29]^x[30]^x[31]^x[35];
outPoly[22]=x[3]^x[4]^x[5]^x[6]^x[7]^x[8]^x[10]^x[14]^x[18]^x[20]^x[21]^x[22]^x[24]^x[26]^x[28]^x[29]^x[31]^x[32]^x[33]^x[34];
outPoly[23]=x[4]^x[5]^x[8]^x[9]^x[10]^x[11]^x[12]^x[13]^x[16]^x[19]^x[23]^x[24]^x[25]^x[26]^x[31]^x[35];
outPoly[24]=x[2]^x[3]^x[4]^x[8]^x[9]^x[11]^x[12]^x[13]^x[14]^x[15]^x[17]^x[22]^x[23]^x[25]^x[26]^x[28]^x[29]^x[30]^x[32]^x[33]^x[34]^x[35];
outPoly[25]=x[7]^x[9]^x[13]^x[20]^x[21]^x[26]^x[30]^x[35];
outPoly[26]=x[1]^x[3]^x[5]^x[6]^x[7]^x[11]^x[20]^x[21]^x[22]^x[28]^x[31]^x[33]^x[34]^x[36];
outPoly[27]=x[3]^x[6]^x[7]^x[9]^x[10]^x[11]^x[12]^x[13]^x[14]^x[23]^x[27]^x[28]^x[31]^x[32]^x[33]^x[34];
outPoly[28]=x[1]^x[2]^x[12]^x[15]^x[17]^x[18]^x[20]^x[23]^x[24]^x[25]^x[27]^x[30]^x[31]^x[33]^x[34]^x[35]^x[36];
outPoly[29]=x[1]^x[2]^x[6]^x[7]^x[9]^x[10]^x[11]^x[12]^x[15]^x[16]^x[18]^x[19]^x[22]^x[23]^x[24]^x[26]^x[30]^x[36];
outPoly[30]=x[1]^x[2]^x[3]^x[5]^x[9]^x[10]^x[12]^x[13]^x[14]^x[16]^x[17]^x[18]^x[19]^x[20]^x[23]^x[27]^x[28]^x[29]^x[32]^x[35];
outPoly[31]=x[2]^x[6]^x[11]^x[12]^x[13]^x[14]^x[15]^x[16]^x[17]^x[18]^x[21]^x[22]^x[23]^x[25]^x[26]^x[27]^x[28]^x[32]^x[33]^x[34];
outPoly[32]=x[1]^x[2]^x[3]^x[7]^x[9]^x[10]^x[11]^x[14]^x[15]^x[18]^x[19]^x[20]^x[21]^x[24]^x[25]^x[27]^x[29]^x[31]^x[32]^x[35];
outPoly[33]=x[1]^x[3]^x[5]^x[8]^x[9]^x[11]^x[12]^x[15]^x[17]^x[18]^x[19]^x[20]^x[21]^x[22]^x[23]^x[26]^x[32]^x[34]^x[35]^x[36];
outPoly[34]=x[5]^x[9]^x[10]^x[11]^x[12]^x[13]^x[14]^x[15]^x[17]^x[18]^x[21]^x[22]^x[23]^x[25]^x[27]^x[28]^x[29]^x[30]^x[31]^x[32]^x[33]^x[34]^x[35];
outPoly[35]=x[3]^x[4]^x[7]^x[11]^x[12]^x[13]^x[16]^x[17]^x[19]^x[22]^x[23]^x[25]^x[26]^x[31]^x[34]^x[36];
outPoly[36]=x[1]^x[2]^x[4]^x[5]^x[7]^x[8]^x[9]^x[10]^x[11]^x[12]^x[15]^x[16]^x[18]^x[19]^x[20]^x[21]^x[22]^x[23]^x[24]^x[25]^x[29]^x[31]^x[32];
}

/*
static byte Apow8[] = {
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,1,1,0,
1,1,1,1,1,1,0,1,1,1,0,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,
0,0,1,1,0,1,0,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,0,1,0,0,0,1,1,0,1,1,0,1,0,1,0,
0,1,1,1,1,0,0,0,1,0,0,1,0,1,1,0,0,1,1,1,0,1,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,
1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,1,1,1,1,0,1,1,0,0,0,0,1,1,1,0,0,0,1,0,0,0,
1,1,0,1,1,0,1,0,0,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,1,0,0,1,0,0,0,0,1,1,1,
0,0,1,0,0,0,0,0,1,0,0,1,1,1,1,1,1,0,0,1,1,0,0,0,1,1,0,1,0,1,1,1,0,0,0,0,0,
0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,1,1,1,
0,0,0,0,1,1,1,1,1,0,1,0,1,1,0,0,1,0,0,0,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,1,1,
1,1,0,1,1,1,0,0,1,1,1,1,0,1,0,1,0,1,1,1,1,0,0,1,0,0,0,1,0,1,0,1,1,1,1,0,1,
0,1,0,1,0,0,1,1,1,0,0,1,1,0,0,0,1,0,1,0,0,1,1,0,1,0,1,0,0,1,1,0,1,1,0,0,0,
0,1,0,1,1,0,0,0,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,1,1,0,1,
0,1,1,0,0,1,1,0,0,1,1,0,1,1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,1,0,0,0,0,0,
1,1,1,0,1,1,0,0,0,0,1,0,1,0,0,0,1,1,0,1,0,0,1,0,1,1,1,0,1,1,1,1,1,0,0,0,0,
1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,0,0,1,1,0,1,
1,1,0,0,1,1,0,1,0,1,1,1,1,1,0,1,1,0,1,0,0,0,0,0,1,0,1,1,1,1,0,1,1,1,1,0,1,
0,1,1,0,1,0,0,0,1,1,1,1,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,0,0,1,
1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,0,1,0,
1,0,1,1,1,1,0,0,1,1,0,1,1,0,0,0,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,
0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,1,0,0,1,0,1,1,1,0,1,1,1,0,0,1,0,1,
0,1,1,0,1,0,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,0,0,0,1,1,1,1,0,0,0,
1,0,0,0,1,1,0,0,0,1,1,1,0,1,0,0,0,1,0,1,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,1,1,
1,1,0,1,0,1,1,0,0,1,1,1,0,0,1,1,0,1,1,0,0,0,0,1,1,0,1,1,1,1,0,1,0,1,1,0,1,
0,1,1,1,0,0,0,1,1,0,0,0,1,0,0,0,0,1,1,0,0,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
0,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,
1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0,1,1,0,0,1,1,0,
1,1,0,0,0,1,1,0,1,0,1,1,0,1,1,0,1,1,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,1,
1,1,0,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,0,0,1,1,0,0,1,0,0,1,0,0,0,0,1,0,1,0,0,
0,0,1,1,0,1,1,1,0,1,1,0,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,1,
0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,1,1,1,
1,1,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,
0,0,0,0,0,0,1,0,1,0,0,1,0,1,1,1,1,0,1,0,1,1,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,
0,0,0,0,1,1,0,0,1,1,0,1,0,1,1,1,1,0,1,1,0,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
1,0,1,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,
1,0,1,0,0,0,1,1,0,1,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,0,1,1,
1,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,0,0,0,1,0,1,0,1,0,0,0,1,1,0,0,1,1,1,0,0,1 };

void matMul( byte* z , byte* x , byte * y )
{
	memset( z , 0 , 37*37 );
	int i,j,k;
	for( i = 0 ; i < 37 ; i++ )
		for( j = 0 ; j < 37 ; j++ )
			for( k = 0 ; k < 37 ; k++ )
					z[IDX2(i,j,37)] ^= x[IDX2(i,k,37)] * y[IDX2(k,j,37)];	
}

static byte ApowN[37*37];
static byte ApowNN[37*37];

void print_code()
{
	memcpy( ApowN , Apow8 , 37*37 );   //  8
	matMul( ApowNN , ApowN , Apow8 );  // 16
	matMul( ApowN , ApowNN , Apow8 );  // 24
	matMul( ApowNN , ApowN , Apow8 );  // 32
	matMul( ApowN , ApowNN , Apow8 );  // 40
	matMul( ApowNN , ApowN , Apow8 );  // 48
	matMul( ApowN , ApowNN , Apow8 );  // 56
	
	
	int i,j;
	for( i = 0 ; i < 37 ; i++ )
	{
		printf("outZ[%d]=",i);	
		for( j = 0 ; j < 37 ; j++ )
			if( ApowN[IDX2(j,i,37)] ) printf("x[%d]^",j);
		printf(";\n");
	}
}

inline void polyPow8( byte * outZ , byte * x )
{
	memset(outZ , 0 , 37 );
	int i,j;
	for( i = 0 ; i < 37 ; i++ )
		for( j = 0 ; j < 37 ; j ++ )
			if( Apow8[IDX2(j,i,37)] ) outZ[i] ^= x[j];
}


inline void polyPow24( byte * outZ , byte * x )
{
	polyPow8( outZ , x );
	polyPow8( _Z , outZ );
	polyPow8( outZ , _Z );
}
*/

