#include "utcp/utcp_manager.h"

int main() {
    UTCP* mgr = new UTCP();
    
    // char* data = utcp_manager->recv_utcp();
    char* data = (char*)malloc(256);
    int sizebuffer = mgr->Recv(&data);
    printf("length: %d Data: %s\n", sizebuffer, data);
    delete mgr;
    free(data);

    return 0;
}