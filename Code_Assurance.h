//
//	Debugging Aids
//	==============
//

#ifndef _CODE_ASSURANCE_H_
#define _CODE_ASSURANCE_H_

//
//	We must have the error codes.
//
#include "Errors.h"

//
//	Code Assurance macros.
//
#define ASSERT(v)	do{ if(!(v))errors.log_terminate( CODE_ASSURANCE_ERR_ASSERT, F( __FILE__ ), __LINE__); }while(false)
#define ABORT(e)	do{ errors.log_terminate((e), F( __FILE__ ), __LINE__ ); }while(false)

#endif

//
//	EOF
//

