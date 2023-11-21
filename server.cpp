#include "utcp/utcp_manager.h"

int main() {
    UTCP* mgr = new UTCP(54321, 12345);
    mgr->Send("Hello UDP! Here Comes your saviour, UTCP! It is soooo cool, that it can check for broken packets and do acknowledgement!");
    delete mgr;

    return 0;
}
