//���ʒ�`�̃w�b�_
#pragma once
#include <stdio.h>

//����������p�}�N��
#define FREE(p) { if (p) { free(p); (p) = NULL; } }
#define SDELETE(p) { if (p) { delete (p); (p) = NULL; } }
#define SDELETES(p) { if (p) { delete [] (p); (p) = NULL; } }
#define RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

