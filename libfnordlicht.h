/* vim ts=4 noexpandtab sw=4 */
/* fnordlicht.h */

/* INCOMPLETE */

//file to include to use the fnordlicht funktions

extern void sync(void);
extern void powerdown(int addr);
extern void fade_rgb(int addr, int step, int delay, int red, int green, int blue);
extern void fade_hsv(int addr, int step, int delay, int hue, int saturation, int value);
extern void save_rgb(int addr, int slot, int step, int delay, int pause, int red, int green, int blue);
extern void save_hsv(int addr, int slot, int step, int delay, int pause, int hue, int saturation, int value);
extern void save_current(int addr, int slot, int step, int delay, int pause);
extern void config_offsets(int addr, int step, int delay, int hue, int saturation, int value);
extern void config_startup(int addr,int mode, int program, int parameters[10]);
extern void start_program(int addr, int program, int params[10]);
extern void stop(int addr,int fade);
