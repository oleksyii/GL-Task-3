#include "utcp/utcp_manager.h"

int main() {
    UTCP* mgr = new UTCP(12345, 54321);
    
    // char* data = utcp_manager->recv_utcp();
    char* data = (char*)malloc(1024);
    int sizebuffer = mgr->Recv(&data);
    printf("length: %d Data: %s\n", sizebuffer, data);
    delete mgr;
    free(data);

    return 0;
}
