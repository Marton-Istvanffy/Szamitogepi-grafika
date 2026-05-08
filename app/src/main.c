#include "app.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv; 

    App app = {0}; 

    app_init(&app);
    app_run(&app);
    app_cleanup(&app);

    return 0;
}