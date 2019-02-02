#ifndef MAIN_FUNC_H_
#define MAIN_FUNC_H_



#if __cplusplus
extern "C"{
#endif

void Init();

void Loop();

void Debug(char* data,int size);

//void SetSPIData(uint8_t *pData, int Size);


void TimerInterrupt();

#if __cplusplus
};
#endif


#endif //MAIN_FUNC_H_
