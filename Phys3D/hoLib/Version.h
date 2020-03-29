//デバッグの有無を本体と共有するためのヘッダ
#pragma once
#include "../Version.h"

/*本体側の Version.h の中で、デバッグモード時は
#define __DEBUG
を定義しておく。

さらに、
//_DEBUG_TF でデバッグモード有無に応じた true false の値が得られる
#ifdef __DEBUG
#define __DEBUG_TF true
#else
#define __DEBUG_TF false
#endif
も書いておく。
*/
