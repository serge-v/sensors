/*

generic gpio helpers for Raspberry PI.

*/

int gpio_export(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
int gpio_set_active_low(unsigned int gpio, unsigned int alow_flag);
int gpio_set_edge(unsigned int gpio, char *edge);
int gpio_fd_open(unsigned int gpio);
int gpio_get_value(unsigned int gpio, unsigned int *value);
