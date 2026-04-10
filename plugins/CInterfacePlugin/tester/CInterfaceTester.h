/*
 * CInterfaceTester.h
 *
 *  Created on: Jul 13, 2011
 *      Author: djc
 */

#ifndef CInterfaceTester_h
#define CInterfaceTester_h

void (*GetFunction(char* funName, void *libHandle))();
char *GetLastMessage(void *libHandle);

int StartGmat(void *libHandle);
int LoadScript(char *scriptName, int sandboxNumber, void *libHandle);
int RunScript(int sandboxNumber, void *libHandle);
int FindOdeModel(char *odeName, int sandboxNumber, void *libHandle);
int GetState(char *odeName, int sandboxNumber, void *libHandle);
int SetState(double *state, int size, char *odeName, int sandboxNumber, void *libHandle);
int GetDerivatives(char *odeName, int sandboxNumber, void *libHandle);

int main(int argc, char *argv[]);

#endif /* CInterfaceTester_h */
