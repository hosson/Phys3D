//共通定義のヘッダ
#pragma once
#include <stdio.h>

//メモリ解放用マクロ
#define FREE(p) { if (p) { free(p); (p) = NULL; } }
#define SDELETE(p) { if (p) { delete (p); (p) = NULL; } }
#define SDELETES(p) { if (p) { delete [] (p); (p) = NULL; } }
#define RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

